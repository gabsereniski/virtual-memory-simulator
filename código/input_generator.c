#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void generate_trace(int n, int max) 
{
    char filename[20];
    sprintf(filename, "trace_%d_%d.txt", n, max);
    FILE *file = fopen(filename, "w");
    if (file == NULL) 
    {
        printf("erro ao abrir o arquivo.\n");
        return;
    }

    srand(time(NULL));

    for (int i = 0; i < n; i++)
    {
        int op = rand() % 2;
        int value = rand() % max;
        fprintf(file, "%d %x\n", op, value);
    }

    fclose(file);
    printf("trace gerado com sucesso!\n");
}

int main(int argc, char *argv[])
{
    if (argc != 3) 
    {
        printf("USO: ./input_generator <num_entradas> <tam_processo>\n");
        exit(0);
    }

    int n = atoi(argv[1]);
    int max = atoi(argv[2]);

    generate_trace(n, max);

    return 0;
}