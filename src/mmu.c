/*
    Simulador para Memória Virutal e Algoritmos de Substituição de Página
    Autores: Gabriela Paola Sereniski
             João Victor Salvi Silva
*/

#include "globals.h"
#include "mmu.h"

// imprime dados armazenados na tabela de páginas
// assinala campos ativos, não imprime páginas inválidas para facilitar visualização
void page_table_report()
{
    printf(" --------------------------------\n");
    printf("|           PAGE TABLE           |\n");
    printf("|--------------------------------|\n");
    printf("| page | v | r | m | age | frame |\n");
    for (int i = 0; i < total_virtual_pages; i++)
    {
        table_entry te = page_table[i];
        printf("| %4d | ", i);
        printf("%s | ", te.v ? "#" : " ");
        printf("%s | ", te.r ? "#" : " ");
        printf("%s | ", te.m ? "#" : " ");
        te.v ? printf("%3d | ", te.age) : printf("    | ");
        te.v ? printf("%5d |\n", te.frame) : printf("      |\n");
    }
    printf(" --------------------------------\n");
}

// imprime dados armazenados na ram
void ram_report()
{
    printf(" ---------------\n");
    printf("|  RAM CONTENT  |\n");
    printf("|---------------|\n");
    printf("| frame | data  |\n");
    for (int i = 0; i < ram_size; i++)
    {
        printf("| %5d | ", i / page_size);
        ram[i] == -1 ? printf("      |\n") : printf("%5x |\n", ram[i]);
    }

    printf(" --------------\n");
}

// imprime relatório da simulação
void simulation_report()
{
    page_table_report();
    ram_report();
    printf("total page faults: %d\n", page_fault_count);

    printf("\n--------------------- press key to continue\n");
    getchar();
}

int *init_ram(int ram_size)
{
    // aloca o vetor que representa a memória ram
    int *ram_array = (int*)malloc(ram_size * sizeof(int));
    // seta todos os endereços como inválidos
    memset(ram_array, -1, ram_size * sizeof(int));
    
    return ram_array; 
}

// reinicializa entrada da tabela de páginas
void reset_table_entry(table_entry *e)
{
    e->frame = -1;
    e->v = false;
    e->r = false;
    e->m = false;
    e->age = -1;
}

// aloca e inicializa estrutura da tabela de páginas
table_entry *new_page_table(int table_size)
{  
    table_entry *page_table = (table_entry*)malloc(table_size * sizeof(table_entry));

    for(int i = 0; i < table_size; i++)
        reset_table_entry(&page_table[i]);

    return page_table;
}

// atualiza campos de controle (r, age)
void update_control_fields()
{
    for(int i = 0; i < total_virtual_pages; i++)
    {
        if(page_table[i].v)
        {
            if(page_table[i].age > total_physical_frames)
                page_table[i].r = false;
            page_table[i].age++;
        }
    }
}

// acessa memória ram, a partir do endereço lógico e do frame armazenado na tabela de páginas
void ram_access(int pti, int logical_address, op_code op)
{
    update_control_fields();    // atualiza campos de controle sempre que há acesso
    page_table[pti].r = true;   // assinala que houve acesso
    page_table[pti].age = 0;    // zera a idade da entrada, pois houve acesso recente
    int offset = OFFSET(logical_address);                   // calcula offset
    int ra = RAM_ADDRESS(page_table[pti].frame, offset);    // calcula endereço da ram
    if(op == WRITE)
    {
        // assinala que houve escrita (ativa bit m)
        if(step_by_step) printf("write operation on page %x\n", ra);
        page_table[pti].m = true;
    }
    // apresenta conversão de endereços
    if(step_by_step) printf("virtual address 0x%x -> physical address 0x%x\n\n", logical_address, ra);
}

// procura quadro vazio na ram, quadros vazios inicializam em -1
int find_free_slot()
{
    for(int i = 0; i < ram_size; i+= page_size)
        if(ram[i] == -1) return i;

    return -1;
}

// verifica se página está na ram
bool page_on_ram(int pti, int addr, op_code op)
{
    // se não está, precisa ser carregada
    if(page_table[pti].v == false) return false;

    // se está, indica que o endereço está carregado, realiza acesso e retorna verdadeiro
    if(step_by_step) printf("address %d is referenced by loaded page %d\n", addr, pti);  
    ram_access(pti, addr, op);

    return true;
}

// carrega dados do disco na ram
void load_frame(int logical_address, int physical_address)
{
    // acessa base do quadro e copia a partir dai
    int da = DISK_ADDRESS(logical_address);
    for(int i = da; i < da + page_size; i++)
        ram[physical_address++] = disk[i];
}

// atualiza entrada da tabela ao ser carregada
void update_table_entry(int pti, int logical_address, op_code op)
{
    if(step_by_step) printf("loaded page %d to ram\n", pti);
    page_table[pti].v = true; // assinala que agora a posicao eh valida
    ram_access(pti, logical_address, op);
}

// politica de substituição FIFO
int fifo_policy()
{
    if(step_by_step) printf("FIFO: ");
    int victim = queue_front(&fifo); // seleciona início página do início da fila como vítima
    queue_pop(&fifo);                // remove página da fila
    return victim;    
}

// politica de substituição LRU
int lru_policy()
{
    if(step_by_step) printf("LRU: ");
    int victim = -1;
    int max_age = -1;
    for(int i = 0; i < total_virtual_pages; i++)
    {
        // seleciona página mais "velha" entre as validas
        if(page_table[i].v && page_table[i].age > max_age)
            max_age = page_table[i].age, victim = i;
    }
    return victim;
}

// politica de substituição NRU
int nru_policy()
{
    if(step_by_step) printf("NRU: ");
    /*
                        CATEGORIAS [Maziero]: 

        (R = 0, M = 0): páginas que não foram referenciadas recentemente e cujo
                        conteúdo não foi modificado. São as melhores candidatas à substituição, pois
                        podem ser simplesmente retiradas da memória.

        (R = 0, M = 1): páginas que não foram referenciadas recentemente, mas cujo
                        conteúdo já foi modificado. Não são escolhas tão boas, porque terão de ser
                        gravadas na área de troca antes de serem substituídas.

        (R = 1, M = 0): páginas referenciadas recentemente, cujo conteúdo permanece
                        inalterado. São provavelmente páginas de código que estão sendo usadas
                        ativamente e serão referenciadas novamente em breve.

        (R = 1, M = 1): páginas referenciadas recentemente e cujo conteúdo foi
                        modificado. São a pior escolha, porque terão de ser gravadas na área de troca e
                        provavelmente serão necessárias em breve.
    */
    int victim = -1;
    int min_category = 0b100;
    int max_age = -1;
    for(int i = 0; i < total_virtual_pages; i++)
    {
        if(page_table[i].v)
        {
            int category = (page_table[i].r << 1) | page_table[i].m;    // calcula categoria com os campos r e m
            if(category <= min_category && page_table[i].age > max_age) // escolhe página de menor categoria e maior idade
                min_category = category, max_age = page_table[i].age, victim = i;
        }
    }
    return victim;
}

void simulation()
{
    for(int p = 0; p < process_len; p++)
    {
        op_code operation = entries[p].op;
        int addr = entries[p].address;
        int pti = PAGE_TABLE_INDEX(addr);   // calcula índice da tabela de páginas

        if(step_by_step) simulation_report();
        if(step_by_step) printf("op: %s, ", operation == READ ? "r" : "w");
        if(step_by_step) printf("address: 0x%x\n\n", addr);
        
        if(addr >= process_size)
        {
            printf("SEGMENTATION FAULT - invalid address [%x]\n", addr);
            exit(1);
        }

        // se índice for válido, RAM é acessada
        if(page_on_ram(pti, addr, operation)) continue;

        // se não, é necessário carregar um quadro
        if(step_by_step) printf("PAGE FAULT - address 0x%x is not on ram\n", addr);
        page_fault_count++;

        // atualiza a fila do FIFO se ele foi o algoritmo de substituição selecionado
        if(algorithm == FIFO) queue_push(&fifo, pti);
        
        // verifica se há espaco livre na RAM
        int slot = find_free_slot();
        // se sim, copia as informacoes do disco para a RAM
        if(slot != -1)
        {
            if(step_by_step) printf("free space available\n");
            // carrega dados do disco na RAM
            load_frame(addr, slot);
            // atualiza ponteiros da tabela de páginas para apontar para o espaço préviamente vazio
            page_table[pti].frame = slot / page_size;
            // atualiza informações de acesso e realiza acesso
            update_table_entry(pti, addr, operation);
                  
            continue;
        }

        if(step_by_step) printf("no free space available\n");
        // se nao há espaço livre, substituição de quadros é necessária
        // escolhe a página vítima de acordo com a politica de substituição selecionada
        int victim;
        switch(algorithm)
        {
            case FIFO: victim = fifo_policy(); break;
            case LRU:  victim = lru_policy();break;
            case NRU:  victim = nru_policy();break;
        }
        
        // aponta para o frame da vítima, pois ele será substituído
        page_table[pti].frame = page_table[victim].frame;
        // calcula endereço base do quadro da ram que será sobrescrito
        slot = page_table[victim].frame * page_size;

        if(step_by_step) printf("page %d was chosen to be removed\n", victim);
        // se o quadro que será removido fora modificado, indica que escrita no disco é necessária
        if(page_table[victim].m)
            if(step_by_step) printf("page %d was modified, write to disk\n", victim);
        
        // carrega dados do disco na ram
        load_frame(addr, slot); 
        // reinicializa informações da página vítima
        reset_table_entry(&page_table[victim]);
        // atualiza informações da página recem carregada
        update_table_entry(pti, addr, operation);
    }

    if(step_by_step) simulation_report();
    else             printf("total page faults: %d\n", page_fault_count);
}