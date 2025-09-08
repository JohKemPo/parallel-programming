#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Inicializa o ambiente MPI
    MPI_Init(&argc, &argv);

    // Descobre quantos processos foram iniciados
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Descobre o rank (ID) do processo atual
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Descobre o nome do processador
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    printf("Olá do processador %s, rank %d de %d processos!\n",
           processor_name, world_rank, world_size);

    // Finaliza o ambiente MPI
    MPI_Finalize();
    return 0;
}
