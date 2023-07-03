#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_RAM_SIZE 1024
#define MAX_PROCESS_SIZE 4096
#define MAX_PAGE_SIZE 256

typedef struct
{
    int physical_page_number;
    bool present;
} PageTableEntry;

PageTableEntry *virtual_page_table;
bool *physical_page_table;
int *fifo_queue;
int fifo_front = 0;
int fifo_rear = 0;
int *lru_counter;
int *lfu_counter;

int total_virtual_pages;
int total_physical_pages;
int page_size;
int process_size;
int ram_size;

int find_free_physical_page()
{
    for (int i = 0; i < total_physical_pages; i++)
    {
        if (!physical_page_table[i])
        {
            return i;
        }
    }
    return -1;
}

void update_fifo_queue(int physical_page_number)
{
    fifo_queue[fifo_rear] = physical_page_number;
    fifo_rear = (fifo_rear + 1) % total_physical_pages;
}

int replace_page_fifo()
{
    int physical_page_number = fifo_queue[fifo_front];
    fifo_front = (fifo_front + 1) % total_physical_pages;
    return physical_page_number;
}

int replace_page_lru()
{
    int min_counter = lru_counter[0];
    int min_counter_page = 0;

    for (int i = 1; i < total_physical_pages; i++)
    {
        if (lru_counter[i] < min_counter)
        {
            min_counter = lru_counter[i];
            min_counter_page = i;
        }
    }

    return min_counter_page;
}

int replace_page_lfu()
{
    int min_counter = lfu_counter[0];
    int min_counter_page = 0;

    for (int i = 1; i < total_physical_pages; i++)
    {
        if (lfu_counter[i] < min_counter)
        {
            min_counter = lfu_counter[i];
            min_counter_page = i;
        }
    }

    return min_counter_page;
}

int translate_virtual_to_physical(int virtual_address, char algorithm)
{
    int virtual_page_number = virtual_address / page_size;
    int offset = virtual_address % page_size;

    if (virtual_page_table[virtual_page_number].present)
    {
        int physical_page_number = virtual_page_table[virtual_page_number].physical_page_number;
        int physical_address = physical_page_number * page_size + offset;

        if (algorithm == 'L')
        {
            lru_counter[physical_page_number]++;
        }
        else if (algorithm == 'F')
        {
            lfu_counter[physical_page_number]++;
        }

        return physical_address;
    }
    else
    {
        printf("Page fault occurred for virtual address %d\n", virtual_address);
        int free_physical_page = find_free_physical_page();

        if (free_physical_page != -1)
        {
            virtual_page_table[virtual_page_number].physical_page_number = free_physical_page;
            virtual_page_table[virtual_page_number].present = true;
            physical_page_table[free_physical_page] = true;
            update_fifo_queue(free_physical_page);
            lru_counter[free_physical_page] = 0;
            lfu_counter[free_physical_page] = 1;

            int physical_address = free_physical_page * page_size + offset;
            return physical_address;
        }
        else
        {
            printf("No free physical pages available. Performing page replacement.\n");
            int replaced_physical_page;

            if (algorithm == 'F')
            {
                replaced_physical_page = replace_page_fifo();
            }
            else if (algorithm == 'L')
            {
                replaced_physical_page = replace_page_lru();
            }
            else if (algorithm == 'U')
            {
                replaced_physical_page = replace_page_lfu();
            }
            else
            {
                printf("Invalid algorithm specified.\n");
                return -1;
            }

            int replaced_virtual_page = -1;

            for (int i = 0; i < total_virtual_pages; i++)
            {
                if (virtual_page_table[i].physical_page_number == replaced_physical_page)
                {
                    virtual_page_table[i].present = false;
                    replaced_virtual_page = i;
                    break;
                }
            }

            virtual_page_table[virtual_page_number].physical_page_number = replaced_physical_page;
            virtual_page_table[virtual_page_number].present = true;
            update_fifo_queue(replaced_physical_page);
            lru_counter[replaced_physical_page] = 0;
            lfu_counter[replaced_physical_page] = 1;

            int physical_address = replaced_physical_page * page_size + offset;
            return physical_address;
        }
    }
}

void simulate(char algorithm, int *process_addresses, int process_length)
{
    // Initialize data structures
    for (int i = 0; i < total_virtual_pages; i++)
    {
        virtual_page_table[i].present = false;
    }

    for (int i = 0; i < total_physical_pages; i++)
    {
        physical_page_table[i] = false;
        fifo_queue[i] = -1;
        lru_counter[i] = 0;
        lfu_counter[i] = 0;
    }

    // Access virtual addresses
    for (int i = 0; i < process_length; i++)
    {
        int virtual_address = process_addresses[i];
        int physical_address = translate_virtual_to_physical(virtual_address, algorithm);
        printf("Virtual address %d maps to physical address %d\n", virtual_address, physical_address);
    }
}

int main()
{
    // Input parameters
    ram_size = 512;                                                                                 // Size of RAM in bytes
    page_size = 256;                                                                                // Size of each page in bytes
    process_size = 2048;                                                                            // Size of the process in bytes
    char algorithm = 'F';                                                                           // Algorithm to be simulated (F for FIFO, L for LRU, U for LFU)
    int process_addresses[] = {0x0, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800, 0x900}; // Sequence of addresses to be processed
    int process_length = sizeof(process_addresses) / sizeof(int);

    // Calculate total pages
    total_virtual_pages = MAX_PROCESS_SIZE / page_size;
    total_physical_pages = MAX_RAM_SIZE / page_size;

    // Allocate memory for data structures
    virtual_page_table = (PageTableEntry *)malloc(total_virtual_pages * sizeof(PageTableEntry));
    physical_page_table = (bool *)malloc(total_physical_pages * sizeof(bool));
    fifo_queue = (int *)malloc(total_physical_pages * sizeof(int));
    lru_counter = (int *)malloc(total_physical_pages * sizeof(int));
    lfu_counter = (int *)malloc(total_physical_pages * sizeof(int));

    printf("Simulation Parameters:\n");
    printf("RAM Size: %d bytes\n", ram_size);
    printf("Page Size: %d bytes\n", page_size);
    printf("Process Size: %d bytes\n", process_size);
    printf("Algorithm: %c\n", algorithm);
    printf("Process Addresses: ");
    for (int i = 0; i < process_length; i++)
    {
        printf("%#x ", process_addresses[i]);
    }
    printf("\n\n");

    printf("Simulation Results:\n");
    simulate(algorithm, process_addresses, process_length);

    // Free allocated memory
    free(virtual_page_table);
    free(physical_page_table);
    free(fifo_queue);
    free(lru_counter);
    free(lfu_counter);

    return 0;
}
