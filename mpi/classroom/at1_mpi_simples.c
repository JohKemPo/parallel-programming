#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char *argv[])
{0
    int meu_ranque, num_procs;
    int origem, destino, etiq = 0;
    char mensagem[200];
    MPI_Status estado;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    if (meu_ranque != 0)
    {
        sprintf(mensagem, "Processo %d está vivo!", meu_ranque);
        destino = 0;
        MPI_Send(mensagem, strlen(mensagem) + 1, MPI_CHAR, destino, etiq, MPI_COMM_WORLD);
    }
    else
    {
        for (origem = 1; origem < num_procs; origem++)
        {
            MPI_Recv(mensagem, 200, MPI_CHAR, MPI_ANY_SOURCE, etiq, MPI_COMM_WORLD, &estado);
            printf("%s\n", mensagem);
            printf("Recebi %d do processo %d (tag %d)\n", *mensagem, estado.MPI_SOURCE, estado.MPI_TAG);
        }
    }
    MPI_Finalize();
    return (0);
}