# Simulador de memória virtual e algoritmos de substituição.

### Alunos

| RA      | Nome                     |
| ------- | ------------------------ |
| 2349345 | Gabriela Paola Sereniski |
| 2304350 | João Victor Salvi Silva |

Data de finalização: 03/06/2023

### Execução

Para compilar, basta executar `make`.
O simulador recebe os seguintes argumentos para execução:
`<ram_size> <page_size> <process_size> <algorithm> <trace_file>`

Os algoritmos válidos são:

| Código | Algoritmo |
| ------- | --------- |
| 0       | FIFO      |
| 1       | LRU       |
| 2       | NRU       |

O usuário escolhe se deseja mostrar os passos da simulação ou não.

O simulador conta o número de faltas de página dado uma sequencia de acessos à RAM. 

A sequência de acessos pode ser gerada pelo programa auxiliar `input_generator`, que recebe como argumento o número de instruções e o tamanho do processo.
