#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "globals.h"
#include "queue.h"
#include "mmu.h"

int main(int argc, char **argv)
{
    // TRATAMENTO DA ENTRADA ----------------------------------------------------------------------------------
    if(argc < 6)
    {
        printf("USE: ./$ <ram_size> <page_size> <process_size> <algorithm> <trace_file>\n");
        printf("MAX SIZES: RAM = %d; PAGE = %d, PROCESS = %d\n", MAX_RAM_SIZE, MAX_PAGE_SIZE, MAX_PROCESS_SIZE);
        printf("algorithm values: \n0 - FIFO; \n1 - LRU; \n2 - NRU\n");
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

    algorithm = atoi(argv[4]);
    if(algorithm < FIFO || algorithm > NRU)
    {
        printf("INVALID ALGORITHM, USING FIFO INSTEAD\n");
        algorithm = FIFO;
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
        if(op < READ || op > WRITE)
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

    // preenche o disco com o espaço de endereçamento do processo
    // preenche a partir do início do disco, para facilitar a implementação
    for(int i = 0; i < process_size; i++)
        disk[i] = i; 

    total_virtual_pages = process_size / page_size; // numero total de páginas
    total_physical_frames = ram_size / page_size;   // numero total de quadros

    // aloca o vetor que representa a memória ram
    ram = init_ram(ram_size);
    // aloca a estrutura da tabela de páginas
    page_table = new_page_table(total_virtual_pages);

    // inicializa a estrutura de fila, caso o algoritmo selecionado seja o FIFO
    if(algorithm == FIFO) queue_init(&fifo);

    // executa a simulação
    simulation();

    return 0;
}