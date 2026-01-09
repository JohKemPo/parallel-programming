#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define VETOR_SIZE 1000000

int main(int argc, char *argv[]) {
    int rank, size;
    int *vet;
    long long soma = 0, subtracao = 0;
    double start_time, end_time;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Alocar vetor
    vet = (int*)malloc(VETOR_SIZE * sizeof(int));
    
    start_time = MPI_Wtime();
    
    // Inicialização do vetor (todos os processos fazem para evitar broadcast muito grande)
    for (int i = 0; i < VETOR_SIZE; i++) {
        vet[i] = (i % 1000) + 1;  // Valores de 1 a 1000 (evita overflow rápido)
    }
    
    // Divisão do trabalho
    int elementos_por_processo = VETOR_SIZE / size;
    int inicio = rank * elementos_por_processo;
    int fim = (rank == size - 1) ? VETOR_SIZE : inicio + elementos_por_processo;
    
    // Cálculos locais
    long long soma_local = 0, subtracao_local = 0;
    
    printf("Processo %d: processando elementos %d a %d\n", rank, inicio, fim-1);
    
    for (int i = inicio; i < fim; i++) {
        soma_local += vet[i];
        subtracao_local -= vet[i];
    }
    
    // Redução dos resultados
    long long soma_total, subtracao_total;
    
    MPI_Reduce(&soma_local, &soma_total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&subtracao_local, &subtracao_total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
    end_time = MPI_Wtime();
    
    if (rank == 0) {
        printf("\n=== RESULTADOS PARA %d ELEMENTOS ===\n", VETOR_SIZE);
        printf("Soma = %lld\n", soma_total);
        printf("Subtracao = %lld\n", subtracao_total);
        printf("Multiplicacao = OMITIDA (overflow para %d elementos)\n", VETOR_SIZE);
        printf("Tempo paralelo (%d processos): %f segundos\n", size, end_time - start_time);
        
        
        // Cálculo de eficiência
        double tempo_sequencial_estimado = 0.008410; 
        double speed_up = tempo_sequencial_estimado / (end_time - start_time);
        double eficiencia = (speed_up / size) * 100;
        printf("Speed Up estimado: %.2f\n", speed_up);
        printf("Eficiencia estimada: %.1f%%\n", eficiencia);
    }
    
    free(vet);
    MPI_Finalize();
    return 0;
}