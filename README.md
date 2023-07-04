# Simulador de memória virtual e algoritmos de substituição.

### Alunos

| RA      | Nome                     |
| ------- | ------------------------ |
| 2349345 | Gabriela Paola Sereniski |
| 2304350 | João Victor Salvi Silva  |

Data de finalização: 03/06/2023

### Sobre
Aplicativo de console que simula as estruturas de uma memória virtual e o que acontece quando ocorre falta de página, de acordo com a política de substituição selecionada. O usuário escolhe se deseja mostrar os passos da simulação ou não. Se ele optar por exibí-las, o programa apresenta o estado da tabela de páginas e da RAM quando uma instrução é executada. O simulador conta e exibe o número de faltas de página dado a sequencia de acessos à RAM.

### Execução
Para compilar, basta executar `make` no diretório do código fonte.
O simulador recebe os seguintes argumentos para execução:
```console
$./sim <ram_size> <page_size> <process_size> <algorithm> <trace_file>
```
Os algoritmos válidos são:

| Código  | Algoritmo |
| ------- | --------- |
| 0       | FIFO      |
| 1       | LRU       |
| 2       | NRU       |

A sequência de acessos pode ser gerada pelo programa auxiliar `input_generator`, que é compilado quando o `make` é invocado e pode ser executado como:

```console
$./input_generator <num_instruções> <tam_processo>
```
