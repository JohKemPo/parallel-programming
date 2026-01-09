#include <stdio.h>
#include <math.h>
#include <time.h>

double f(double x) {
    return x * x;
}

int main(int argc, char *argv[]) {
    double a = 0.0, b = 1.0;
    long int n = 100000000; 
    double h, integral, x;
    clock_t inicio, fim;

    h = (b - a) / n;
    integral = (f(a) + f(b)) / 2.0;

    inicio = clock();
    for (long int i = 1; i < n; i++) {
        x = a + i * h;
        integral += f(x);
    }
    integral = integral * h;
    fim = clock();

    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("Tempo gasto: %3.3f segundos\n", tempo);
    printf("Com n = %ld trapezoides\n", n);
    printf("Integral de %lf ate %lf = %lf\n", a, b, integral);

    return 0;
}
