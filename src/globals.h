/*
    Simulador para Memória Virutal e Algoritmos de Substituição de Página
    Autores: Gabriela Paola Sereniski
             João Victor Salvi Silva
*/

#ifndef GLOBALS_H
// arquivo que define variáveis e estruturas compartilhadas entre os códigos
// facilita a implementação

#define GLOBALS_H

#include "mmu.h"
#include "queue.h"

// variáveis de entrada
extern int ram_size, process_size, page_size;
extern policy algorithm;

// tamanho das estruturas
extern int total_virtual_pages, total_physical_frames;

// estruturas de memória
extern int disk[DISK_SIZE];
extern int *ram;
extern table_entry *page_table;

// informações sobre o processo lido do trace
extern process_entry entries[DISK_SIZE];
extern int process_len;

// fila do algoritmo FIFO
extern queue fifo;

// contador de faltas de página
extern int page_fault_count;

// indica se passos da execução deverão ser exibidos ou não
extern int step_by_step;

#endif
