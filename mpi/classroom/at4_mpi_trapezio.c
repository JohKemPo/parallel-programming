#include <stdio.h>
#include <math.h>
#include "mpi.h"

double f(double x)
{
    double return_val;
    return_val = x * x;
    return return_val;
}

int main(int argc, char *argv[])
{                                      /* mpi_trapezio.c  */
    int meu_ranque, num_procs;         /* respectivamente q e p */
    double a = 0.0, b = 1.0;           /* Limites da integral */
    double tempo_inicial, tempo_final; /* Tempo de execução */
    long int n = 100000000;            /* Número de trapezoides */
    double x, h;                       /* x e h, a base do trapezoide */
    double integral = 0.0, total;      /* Integral de cada processo e total */
    int origem, destino = 0;           /* Origem e destino das mensagens */
    int etiq = 3;                      /* Uma etiqueta qualquer */

    long int local_n;        /* Número de trapézios para o processo */
    double local_a, local_b; /* Limites de integração para o processo */
    double integral_local;   /* Integral calculada por o processo */
    long int i;             

    /* Inicia o MPI e determina o ranque e o número de processos ativos  */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Inicia a contagem de tempo no processo 0
    if (meu_ranque == 0)
    {
        tempo_inicial = MPI_Wtime();
    }

    /* h é o mesmo para todos os processos */
    h = (b - a) / n;

    /* Calcula o número de trapézios, o início e o fim do intervalo local */
    local_n = n / num_procs; // numero de divisoes
    local_a = a + meu_ranque * local_n * h; // limite sup do intervalo
    local_b = local_a + local_n * h; // limite inf do intervalo

    /* Cada processo calcula a sua integral local usando a regra do trapézio.*/
    integral_local = (f(local_a) + f(local_b)) / 2.0;
    for (i = 1; i < local_n; i++)
    {
        x = local_a + i * h;
        integral_local += f(x);
    }
    integral_local = integral_local * h;

    /* O processo 0 soma as integrais parciais recebidas */
    if (meu_ranque == 0)
    {
        total = integral_local;
        for (int origem = 1; origem < num_procs; origem++)
        {
            double integral_recebida;
            MPI_Recv(&integral_recebida, 1, MPI_DOUBLE, origem, etiq, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += integral_recebida;
        }
    }
    /* Os demais processos enviam suas integrais parciais para o processo 0 */
    else
    {
        MPI_Send(&integral_local, 1, MPI_DOUBLE, destino, etiq, MPI_COMM_WORLD);
    }

    /* Imprime o resultado */
    if (meu_ranque == 0)
    {
        tempo_final = MPI_Wtime();
        printf("Foram gastos %3.1f segundos\n", tempo_final - tempo_inicial);
        printf("Com n = %ld trapezoides e %d processos, a estimativa\n", n, num_procs);
        printf("da integral de %lf ate %lf = %lf \n", a, b, total);
    }

    MPI_Finalize();
    return (0);
}