#ifndef MMU_H
#define MMU_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK_SIZE 8192
#define MAX_RAM_SIZE 4096
#define MAX_PROCESS_SIZE 8192
#define MAX_PAGE_SIZE 1024

#define PAGE_TABLE_INDEX(logical_address) ((logical_address) / page_size)
#define OFFSET(logical_address) ((logical_address) % page_size)
#define RAM_ADDRESS(frame, offset) (frame * page_size + offset)
#define DISK_ADDRESS(logical_address) (logical_address - (logical_address % page_size))

typedef enum {FIFO, LRU, NRU} policy;
typedef enum {READ, WRITE} op_code;

typedef struct {
    int frame;
    bool v; // valid
    bool r; // referenced
    bool m; // modified
    int age;
} table_entry;

typedef struct {
    op_code op;
    int address;
} process_entry;

table_entry *new_page_table(int table_size);
void simulation();

#endif
