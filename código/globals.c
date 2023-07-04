#include "globals.h"
#include "queue.h"

int ram_size, process_size, page_size;
policy algorithm;
int total_virtual_pages, total_physical_frames;

int disk[DISK_SIZE];
int *ram;
table_entry *page_table;

process_entry entries[DISK_SIZE];
int process_len = 0;

queue fifo;
int page_fault_count = 0;

int step_by_step = 0;