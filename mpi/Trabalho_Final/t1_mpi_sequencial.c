#include <stdio.h>
#include <time.h>
#include <math.h>

#define VECTOR_SIZE 1000000

int main() {
    int x;
    long long soma = 0, subtracao = 0;
    double log_mult = 0.0;  // Usar logaritmo para multiplicação
    int vet[VECTOR_SIZE];
    double start_time, end_time;
    
    start_time = clock();
    
    // Inicializar vetor
    for (x = 0; x < VECTOR_SIZE; x++) {
        vet[x] = (x % 1000) + 1;  // Valores de 1 a 1000
    }
    
    // Calcular operações
    for (x = 0; x < VECTOR_SIZE; x++) {
        soma += vet[x];
        subtracao -= vet[x];
        log_mult += log(vet[x]);  // Soma de logaritmos
    }
    
    end_time = clock();
    
    double mult = exp(log_mult);  // Converter de volta
    
    printf("=== SEQUENCIAL COM %d ELEMENTOS ===\n", VECTOR_SIZE);
    printf("Soma = %lld\n", soma);
    printf("Subtracao = %lld\n", subtracao);
    printf("Multiplicacao = %e\n", mult);
    printf("Log10 da Multiplicacao = %f\n", log_mult / log(10));  // Tamanho em base 10
    printf("Tempo sequencial: %f segundos\n", (end_time - start_time) / CLOCKS_PER_SEC);
    
    return 0;
}