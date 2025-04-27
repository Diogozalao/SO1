#ifndef STATS_H
#define STATS_H

#include "process.h"

typedef struct {
    double avg_waiting_time;        // Tempo médio de espera
    double avg_turnaround_time;     // Tempo médio de retorno (turnaround)
    double cpu_utilization;         // Percentagem de utilização da CPU
    double throughput;             // Número de processos completados por unidade de tempo
    int deadline_misses;           // Número de deadlines perdidas (para algoritmos de tempo real)
    double avg_response_time;      // Tempo médio de resposta (opcional)
} SimulationStats;

// Cálculo das estatísticas de simulação
SimulationStats calculate_stats(Process** processes, int count, int total_time);

// Exibição das estatísticas
void print_stats(SimulationStats stats);

// Cálculo de estatísticas individuais por processo (para relatório detalhado)
void calculate_process_stats(Process* process, int total_time);

// Função para gerar relatório comparativo entre algoritmos
void print_comparative_stats(SimulationStats stats[], char* algorithm_names[], int count);

#endif