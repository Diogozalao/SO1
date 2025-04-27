#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <stdbool.h>

// Inicialização do gerador
void init_random();
void init_random_with_seed(unsigned int seed);

// Distribuições contínuas
double exponential_random(double lambda);    // Distribuição exponencial
double normal_random(double mean, double stddev);  // Distribuição normal (Gaussiana)
double uniform_random_double(double min, double max); // Uniforme contínua

// Distribuições discretas
int poisson_random(double lambda);          // Distribuição de Poisson
int uniform_random_int(int min, int max);   // Uniforme discreta
int weighted_random(int weights[], int n);  // Amostragem ponderada

// Geradores especializados para o projeto
int generate_burst_time(int dist_type, double param1, double param2);
int generate_arrival_interval(int dist_type, double param1, double param2);

// Função para gerar prioridades conforme especificado no projeto
int generate_priority(bool weighted);  // Se weighted=true, usa prioridades mais baixas com maior probabilidade

#endif