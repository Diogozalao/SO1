#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include "random_generator.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static bool initialized = false;

// Inicializa com seed aleatória (time(NULL))
void init_random() {
    if (!initialized) {
        srand(time(NULL));
        initialized = true;
    }
}

// Inicializa com seed específica (útil para testes)
void init_random_with_seed(unsigned int seed) {
    srand(seed);
    initialized = true;
}

// Distribuição exponencial para tempos entre chegadas
double exponential_random(double lambda) {
    if (!initialized) init_random();
    double u = rand() / (RAND_MAX + 1.0);
    return -log(1.0 - u) / lambda;
}

// Distribuição normal usando transformação Box-Muller
double normal_random(double mean, double stddev) {
    if (!initialized) init_random();
    double u1, u2, z0;
    
    // Gera dois números uniformes (0,1]
    do {
        u1 = rand() / (RAND_MAX + 1.0);
        u2 = rand() / (RAND_MAX + 1.0);
    } while (u1 <= 0.0 || u2 <= 0.0);  // Evita log(0)
    
    // Transformação Box-Muller
    z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return mean + stddev * z0;
}

// Distribuição uniforme contínua
double uniform_random_double(double min, double max) {
    if (!initialized) init_random();
    return min + (max - min) * (rand() / (RAND_MAX + 1.0));
}

// Distribuição de Poisson para modelar chegadas de processos
int poisson_random(double lambda) {
    if (!initialized) init_random();
    double L = exp(-lambda);
    double p = 1.0;
    int k = 0;
    
    do {
        k++;
        p *= rand() / (RAND_MAX + 1.0);
    } while (p > L);
    
    return k - 1;
}

// Distribuição uniforme discreta
int uniform_random_int(int min, int max) {
    if (!initialized) init_random();
    return min + rand() % (max - min + 1);
}

// Amostragem ponderada para prioridades
int weighted_random(int weights[], int n) {
    if (!initialized) init_random();
    int sum = 0;
    
    // Calcula soma total dos pesos
    for (int i = 0; i < n; i++) {
        sum += weights[i];
    }
    
    // Gera número aleatório no intervalo [0, sum)
    int r = rand() % sum;
    
    // Determina qual intervalo o número caiu
    for (int i = 0; i < n; i++) {
        if (r < weights[i]) {
            return i;
        }
        r -= weights[i];
    }
    
    return n - 1;  // Por segurança
}

// Gera burst times conforme especificado no projeto
int generate_burst_time(int dist_type, double param1, double param2) {
    double val;
    switch (dist_type) {
        case 0:  // Exponencial
            val = exponential_random(param1);
            break;
        case 1:  // Normal
            do {
                val = normal_random(param1, param2);
            } while (val <= 0);  // Garante valor positivo
            break;
        case 2:  // Uniforme
            val = uniform_random_double(param1, param2);
            break;
        default:
            val = uniform_random_double(1, 10);
    }
    return (int)(val + 0.5);  // Arredonda para inteiro
}

// Gera intervalos entre chegadas de processos
int generate_arrival_interval(int dist_type, double param1, double param2) {
    double val;
    switch (dist_type) {
        case 0:  // Poisson (equivalente a exponencial para intervalos)
            val = exponential_random(param1);
            break;
        case 1:  // Normal
            do {
                val = normal_random(param1, param2);
            } while (val <= 0);
            break;
        case 2:  // Uniforme
            val = uniform_random_double(param1, param2);
            break;
        default:
            val = uniform_random_double(1, 5);
    }
    return (int)(val + 0.5);
}

// Gera prioridades conforme especificado no projeto (1-10)
int generate_priority(bool weighted) {
    if (weighted) {
        // Pesos decrescentes (prioridade 1 tem peso 10, prioridade 10 tem peso 1)
        int weights[10] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        return weighted_random(weights, 10) + 1;
    } else {
        return uniform_random_int(1, 10);
    }
}