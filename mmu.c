#include "globals.h"
#include "mmu.h"

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

void mmu_report()
{
    page_table_report();
    ram_report();
    printf("total page faults: %d\n", page_fault_count);
}

void reset_table_entry(table_entry *e)
{
    e->frame = -1;
    e->v = false;
    e->r = false;
    e->m = false;
    e->age = -1;
}

table_entry *new_page_table(int table_size)
{  
    table_entry *page_table = (table_entry*)malloc(table_size * sizeof(table_entry));

    for(int i = 0; i < table_size; i++)
        reset_table_entry(&page_table[i]);

    return page_table;
}

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

void ram_access(int pti, int logical_address, op_code op)
{
    update_control_fields();
    page_table[pti].r = true; // indico que houve acesso
    page_table[pti].age = 0;  // zero a idade da entrada
    int offset = OFFSET(logical_address);
    int ra = RAM_ADDRESS(page_table[pti].frame, offset);
    if(op == WRITE)
    {
        printf("write operation on page %x\n", ra);
        page_table[pti].m = true;
    }
    printf("virtual address 0x%x -> physical address 0x%x\n\n", logical_address, ra);
}

int find_free_slot()
{
    for(int i = 0; i < ram_size; i+= page_size)
        if(ram[i] == -1) return i;

    return -1;
}

bool page_on_ram(int pti, int addr, op_code op)
{
    if(page_table[pti].v == false) return false;

    printf("address %d is referenced by loaded page %d\n", addr, pti);
    
    ram_access(pti, addr, op);

    return true;
}

void update_ram(int logical_address, int physical_address)
{
    // acesso base do quadro e copio a partir dai
    int da = DISK_ADDRESS(logical_address);
    for(int i = da; i < da+page_size; i++)
        ram[physical_address++] = disk[i];
}

void update_table(int pti, int logical_address, op_code op)
{
    printf("loaded page %d to ram\n", pti);
    page_table[pti].v = true; // indico que agora a posicao eh valida
    ram_access(pti, logical_address, op);
}

int fifo_policy()
{
    printf("FIFO: ");
    int victim = queue_front(&fifo);
    queue_pop(&fifo);
    return victim;    
}

int lru_policy()
{
    printf("LRU: ");
    int victim = -1;
    int max_age = -1;
    for(int i = 0; i < total_virtual_pages; i++)
    {
        if(page_table[i].v && page_table[i].age > max_age)
            max_age = page_table[i].age, victim = i;
    }
    return victim;
}

int nru_policy()
{
    int victim = -1;
    int min_category = 4;
    int max_age = -1;
    for(int i = 0; i < total_virtual_pages; i++)
    {
        if(page_table[i].v)
        {
            int category = (page_table[i].r << 1) | page_table[i].m;
            if(category <= min_category && page_table[i].age > max_age) 
                min_category = category, max_age = page_table[i].age, victim = i;
        }
    }
    return victim;
}

void simulation()
{
    for(int p = 0; p < process_len; p++)
    {
        // acesso tabela de paginas e verifico a validade
        op_code operation = entries[p].op;
        int addr = entries[p].address;
        int pti = PAGE_TABLE_INDEX(addr);

        mmu_report();
        printf("\n--------------------- press key to continue\n");
        getchar();
        printf("op: %s, ", operation == READ ? "r" : "w");
        printf("address: 0x%x\n\n", addr);


        // se for verdadeira, posso acessar a ram
        if(page_on_ram(pti, addr, operation)) continue;

        // atualizo a fila do fifo
        if(algorithm == FIFO) queue_push(&fifo, pti);

        printf("PAGE FAULT - address 0x%x is not on ram\n", addr);
        page_fault_count++;

        // verifico se ha espaco livre
        int slot = find_free_slot();
        // se encontro, copio as informacoes do disco para a ram
        if(slot != -1)
        {
            printf("free space available\n");
            // indico que apartir de agora o frame se encontra no slot anteriormente vazio
            page_table[pti].frame = slot / page_size;
            update_table(pti, addr, operation);            
            update_ram(addr, slot);
                  
            continue;
        }

        printf("no free space available\n");
        // se nao ha free slot, preciso substituir um deles
        // escolhe a pagina vitima de acordo com a politica de substituicao
        int victim;
        switch(algorithm)
        {
            case FIFO: victim = fifo_policy(); break;
            case LRU:  victim = lru_policy();break;
            case NRU:  victim = nru_policy();break;
        }
        
        page_table[pti].frame = page_table[victim].frame;
        slot = page_table[victim].frame * page_size;

        printf("page %d was chosen to be removed\n", victim);
        if(page_table[victim].m)
            printf("page %d was modified, write to disk\n", victim);
        reset_table_entry(&page_table[victim]);
        update_table(pti, addr, operation);
        update_ram(addr, slot); 
    }
    mmu_report();
}