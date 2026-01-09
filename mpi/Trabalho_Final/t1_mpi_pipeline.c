#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define VETOR_SIZE 1000000

int main(int argc, char *argv[]) {
    int rank, size;
    int vet[VETOR_SIZE];
    long long soma = 0, subtracao = 0;
    double mult = 1.0;
    double start_time, end_time;
    double tempo_sequencial_estimado = 0.008410; // Tempo sequencial estimado para 1M elementos
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    start_time = MPI_Wtime();
    
    if (rank == 0) {
        // Processo 0: Inicializa o vetor
        for (int i = 0; i < VETOR_SIZE; i++) {
            vet[i] = (i % 100) + 1;  // Valores de 1 a 100 para evitar overflow rápido
        }
    }
    
    // Broadcast do vetor para todos os processos
    MPI_Bcast(vet, VETOR_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Pipeline: cada processo processa uma parte do vetor
    int elementos_por_processo = VETOR_SIZE / size;
    int resto = VETOR_SIZE % size;
    
    // Calcula início e fim para cada processo
    int inicio = 0;
    for (int i = 0; i < rank; i++) {
        inicio += elementos_por_processo + (i < resto ? 1 : 0);
    }
    int fim = inicio + elementos_por_processo + (rank < resto ? 1 : 0);
    
    // Cada processo calcula suas operações locais
    long long soma_local = 0, subtracao_local = 0;
    double mult_local = 1.0;
    
    MPI_Barrier(MPI_COMM_WORLD);

    // Impressão ordenada dos processos
    for (int i = 0; i < size; i++) {
        if (rank == i) {
            printf("Processo %d: processando elementos %d a %d\n", rank, inicio, fim-1);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
    for (int i = inicio; i < fim; i++) {
        soma_local += vet[i];
        subtracao_local -= vet[i];
        mult_local *= vet[i];
        
        // Prevenir overflow na multiplicação
        if (mult_local > 1e100 || mult_local < 1e-100) {
            mult_local = 1.0;
        }
    }
    
    // Pipeline de redução: processo i envia para processo i+1
    if (rank == 0) {
        // Processo 0 inicia com seus valores
        soma = soma_local;
        subtracao = subtracao_local;
        mult = mult_local;
        
        if (size > 1) {
            // Envia para o próximo processo
            MPI_Send(&soma, 1, MPI_LONG_LONG, 1, 0, MPI_COMM_WORLD);
            MPI_Send(&subtracao, 1, MPI_LONG_LONG, 1, 1, MPI_COMM_WORLD);
            MPI_Send(&mult, 1, MPI_DOUBLE, 1, 2, MPI_COMM_WORLD);
        }
    } else {
        // Processos intermediários recebem, somam e repassam
        long long soma_anterior, subtracao_anterior;
        double mult_anterior;
        
        MPI_Recv(&soma_anterior, 1, MPI_LONG_LONG, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&subtracao_anterior, 1, MPI_LONG_LONG, rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mult_anterior, 1, MPI_DOUBLE, rank-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // Combina com resultados locais
        soma = soma_anterior + soma_local;
        subtracao = subtracao_anterior + subtracao_local;
        mult = mult_anterior * mult_local;
        
        // Se não for o último, repassa adiante
        if (rank < size - 1) {
            MPI_Send(&soma, 1, MPI_LONG_LONG, rank+1, 0, MPI_COMM_WORLD);
            MPI_Send(&subtracao, 1, MPI_LONG_LONG, rank+1, 1, MPI_COMM_WORLD);
            MPI_Send(&mult, 1, MPI_DOUBLE, rank+1, 2, MPI_COMM_WORLD);
        }
    }
    
    // Último processo tem os resultados finais
    if (rank == size - 1) {
        end_time = MPI_Wtime();
        double tempo_paralelo = end_time - start_time;
        double speed_up = tempo_sequencial_estimado / tempo_paralelo;
        double eficiencia = (speed_up / size) * 100;
        
        printf("\n=== RESULTADOS FINAIS - PIPELINE ===\n");
        printf("Soma = %lld\n", soma);
        printf("Subtracao = %lld\n", subtracao);
        printf("Multiplicacao = %e\n", mult);
        printf("Tempo de execucao: %f segundos\n", tempo_paralelo);
        printf("Numero de processos: %d\n", size);
        printf("Speed Up: %.2f\n", speed_up);
        printf("Eficiencia: %.1f%%\n", eficiencia);
        printf("Tempo sequencial estimado: %f segundos\n", tempo_sequencial_estimado);
    }
    
    MPI_Finalize();
    return 0;
}