#ifndef MMU_H
#define MMU_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// macros de limite máximo de tamanho das estruturas
// o algoritmo suporta valores maiores, mas isso não é necessário para realização de simulações simples
#define DISK_SIZE 8192
#define MAX_RAM_SIZE 4096
#define MAX_PROCESS_SIZE 8192
#define MAX_PAGE_SIZE 1024

// calculos de endereçamento
// calcula índice da tabela de páginas
#define PAGE_TABLE_INDEX(logical_address) ((logical_address) / page_size)
// calcula offset a partir do endereço lógico
#define OFFSET(logical_address) ((logical_address) % page_size)
// calcula endereço físico a partir do frame armazenado na tabela de páginas
#define RAM_ADDRESS(frame, offset) (frame * page_size + offset)
// calcula endereço no disco
/* 
    como os endereços lógicos são carregados de 0 ao tamanho máximo do processo, a partir do início do disco
    a macro retorna apenas o endereço base do bloco que deve ser carregado na ram
*/
#define DISK_ADDRESS(logical_address) (logical_address - (logical_address % page_size))

// possíveis algoritmos de substituição de página
typedef enum {FIFO, LRU, NRU} policy;
// possíveis operações lidas do trace
typedef enum {READ, WRITE} op_code;

typedef struct {
    int frame;  // ponteiro para quadro da memória ram
    bool v;     // valid, ativo quando o quadro está carregado na ram
    bool r;     // referenced, ativo quando a página é referenciada
    bool m;     // modified, ativo quando há operação de escrita
    int age;    // idade do processo
                /*
                    é usado para controle dos algoritmos LRU e NRU
                    decisão de projeto - campo r é desativado caso o processo tenha sido referenciado há muito tempo:
                        caso tenham sido processados mais endereços que o número de páginas da ram, campo é desativado
                    LRU: remove a página com maior idade
                    NRU: remove a página com menor categoria e maior idade entre as de mesma categoria
                         categoria é afetada pela mudança do campo r
                */
} table_entry;

// estrutura para armazenar endereço e operação realizada sobre o endereço
typedef struct {
    op_code op;
    int address;
} process_entry;

// aloca e inicializa estrutura da ram
int *init_ram(int ram_size);
// aloca e inicializa tabela de páginas
table_entry *new_page_table(int table_size);
// executa simulação
void simulation();

#endif
