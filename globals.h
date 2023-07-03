#ifndef GLOBALS_H
#define GLOBALS_H

#include "mmu.h"
#include "queue.h"

extern int ram_size, process_size, page_size;
extern policy algorithm;
extern int total_virtual_pages, total_physical_frames;

extern int disk[DISK_SIZE];
extern int *ram;
extern table_entry *page_table;

extern process_entry entries[DISK_SIZE];
extern int process_len;// numero de instrucoes do processo

extern queue fifo;
extern int page_fault_count;

#endif
