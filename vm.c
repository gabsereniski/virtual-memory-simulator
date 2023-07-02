#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DISK_SIZE 512
#define MAX_RAM_SIZE 8
#define MAX_PROCESS_SIZE 32
#define MAX_PAGE_SIZE 2

typedef enum {FIFO, LRU, SECOND_CHANCE} policy;
typedef enum {READ, WRITE, SEARCH} op_code;

typedef struct {
    int frame;
    bool v, r, m; //valid, read, modified
} table_entry;

typedef struct {
    op_code op;
    int address;
} process_entry;

int ram_size, process_size, page_size;
int total_virtual_pages, total_physical_frames;

int disk[DISK_SIZE];
int *ram;
table_entry *page_table;

process_entry entries[DISK_SIZE];
int process_len = 0; // numero de instrucoes do processo

void print_process_entry(process_entry pe)
{
    printf("%d %d\n", pe.op, pe.address);
}

table_entry *new_page_table(int table_size)
{  
    table_entry *page_table = (table_entry*)malloc(table_size * sizeof(table_entry));

    for(int i = 0; i < table_size; i++)
    {
        page_table[i].frame = 0;
        page_table[i].v = false;
        page_table[i].r = false;
        page_table[i].m = false;
    }

    return page_table;
}

void print_table_entry(table_entry te)
{
    printf("[v: %d][r: %d][m: %d][frame: %d]\n", te.v, te.r, te.m, te.frame);
}

#define PAGE_TABLE_INDEX(logical_address) ((logical_address) / page_size)
#define OFFSET(logical_address) ((logical_address) % page_size)
#define RAM_ADDRESS(frame, offset) (frame * page_size + offset)
#define DISK_ADDRESS(logical_address) (logical_address - (logical_address % page_size))

int find_free_slot()
{
    for(int i = 0; i < ram_size; i+= page_size)
    {
        if(ram[i] == -1) return i;
    }

    return -1;
}

void simulation()
{
    for(int p = 0; p < process_len; p++)
    {
        // acesso tabela de paginas e verifico a validade
        int addr = entries[p].address;
        int pti = PAGE_TABLE_INDEX(addr);
        print_table_entry(page_table[pti]);
        // se for verdadeira, posso acessar a ram
        if(page_table[pti].v == true)
        {
            printf("on ram\n");
            int offset = OFFSET(pti);
            int ra = RAM_ADDRESS(page_table[pti].frame, offset);
            printf("virtual 0x%x >> physical 0x%x\n", addr, ra);
            page_table[pti].r = true;
            if(entries[p].op == WRITE)
            {
                printf("write to %x\n", ra);
                page_table[pti].m = true;
            }

            continue;
        }

        // se nao, preciso carregar a pagina
        else
        {
            printf("not on ram\n");
            for(int i = 0; i < ram_size; i++)
            {
                printf("%d ", ram[i]);
            }
            printf("\n");
            // verifico se ha espaco livre
            int slot = find_free_slot();
            printf("free slot = %d\n", slot);
            // se encontro, copio as informacoes do disco para a ram
            if(slot != -1)
            {
                printf("espaco livre\n");
                // indico que apartir de agora o frame se encontra no slot anteriormente vazio
                page_table[pti].frame = slot / page_size;
                page_table[pti].v = true; // indico que agora a posicao eh valida
                page_table[pti].r = true; // indico que houve acesso
                
                // acesso base do quadro e copio a partir dai
                int da = DISK_ADDRESS(addr);
                printf("da = %d\n", da);
                for(int i = da; i < da+page_size; i++)
                {
                    ram[slot++] = disk[i];
                    printf("disco[%d] = %d\n", i,disk[i]);
                }
                
                int offset = OFFSET(pti);
                int ra = RAM_ADDRESS(page_table[pti].frame, offset);
                printf("virtual 0x%x >> physical 0x%x\n", addr, ra);

                if(entries[p].op == WRITE)
                {
                    printf("write to %x\n", ra);
                    page_table[pti].m = true;
                }

                continue;
            }

            // se nao ha free slot, preciso substituir um deles
            else
            {
                // faz algo
                continue;
            }


        }

        print_table_entry(page_table[pti]);
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    // TRATAMENTO DA ENTRADA ----------------------------------------------------------------------------------
    if(argc < 6)
    {
        printf("USE: ./$ <ram_size> <page_size> <process_size> <algorithm> <trace_file>\n");
        printf("MAX SIZES: RAM = %d; PAGE = %d, PROCESS = %d\n", MAX_RAM_SIZE, MAX_PAGE_SIZE, MAX_PROCESS_SIZE);
        printf("<algorithm> values: 0 - FIFO; 1 - LRU; 2 - second chance\n");
        exit(0);
    }

    ram_size = atoi(argv[1]);
    if(ram_size > MAX_RAM_SIZE)
    {
        printf("INVALID RAM SIZE, USING MAX INSTEAD\n");
        ram_size = MAX_RAM_SIZE;
    }

    page_size = atoi(argv[2]);
    if(page_size > MAX_PAGE_SIZE)
    {
        printf("INVALID PAGE SIZE, USING MAX INSTEAD\n");
        ram_size = MAX_PAGE_SIZE;
    }

    process_size = atoi(argv[3]);
    if(process_size > MAX_PROCESS_SIZE)
    {
        printf("INVALID PROCESS SIZE, USING MAX INSTEAD\n");
        ram_size = MAX_PROCESS_SIZE;
    }

    FILE *trace_file = fopen(argv[5], "r");
    if(trace_file == NULL)
    {
        printf("Error opening trace file.\n");
        exit(1);
    }

    // ---------------------------------------------------------------------------------- TRATAMENTO DA ENTRADA

    // LEITURA DO TRACE ---------------------------------------------------------------------------------------
    int op;
    int address;
    while(fscanf(trace_file, "%d %x", &op, &address) == 2)
    {
        if(op < READ || op > SEARCH)
        {
            printf("INVALID OPERATION [%d], ENTRY WILL BE IGNORED\n", op);
            continue;
        }

        if(address < 0 || address >= process_size)
        {
            printf("INVALID ADDRESS [%d], ENTRY WILL BE IGNORED\n", address);
            continue;
        }

        entries[process_len].op = op;
        entries[process_len++].address = address;
    }

    fclose(trace_file);

    // --------------------------------------------------------------------------------------- LEITURA DO TRACE

    // preenche o disco com os enderecos do processo
    for(int i = 0; i < process_size; i++)
        disk[i] = i; 

    // numero total de paginas
    total_virtual_pages = process_size / page_size;
    // numero total de frames
    total_physical_frames = ram_size / page_size;

    // aloca memoria ram
    ram = (int*)malloc(ram_size * sizeof(int));
    memset(ram, -1, ram_size * sizeof(int)); // enderecos invalidos
    // aloca tabela de paginas
    page_table = new_page_table(total_virtual_pages);

    simulation();

    return 0;
}