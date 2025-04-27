#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stats.h"

SimulationStats calculate_stats(Process** processes, int count, int total_time) {
    SimulationStats stats = {0};
    if (count == 0 || total_time == 0) {
        return stats;
    }

    double total_waiting = 0;
    double total_turnaround = 0;
    double total_response = 0;  // Declaração adicionada aqui
    int total_burst = 0;
    stats.deadline_misses = 0;

    for (int i = 0; i < count; i++) {
        int turnaround = processes[i]->completion_time - processes[i]->arrival_time;
        int waiting = turnaround - processes[i]->burst_time;
        
        total_waiting += waiting;
        total_turnaround += turnaround;
        total_burst += processes[i]->burst_time;

        // Cálculo do tempo de resposta
        if (processes[i]->first_run_time != -1) {
            total_response += processes[i]->first_run_time - processes[i]->arrival_time;
        }

        if (processes[i]->deadline > 0 && 
            processes[i]->completion_time > processes[i]->arrival_time + processes[i]->deadline) {
            stats.deadline_misses++;
        }
    }

    stats.avg_waiting_time = total_waiting / count;
    stats.avg_turnaround_time = total_turnaround / count;
    stats.avg_response_time = total_response / count;  // Agora usando a variável declarada
    stats.cpu_utilization = (total_burst * 100.0) / total_time;
    stats.throughput = count / (double)total_time;

    return stats;
}



void print_stats(SimulationStats stats) {
    printf("\n=== Estatísticas da Simulação ===\n");
    printf("Tempo médio de espera:    %.2f unidades de tempo\n", stats.avg_waiting_time);
    printf("Tempo médio de retorno:   %.2f unidades de tempo\n", stats.avg_turnaround_time);
    printf("Tempo médio de resposta:  %.2f unidades de tempo\n", stats.avg_response_time);
    printf("Utilização da CPU:        %.2f%%\n", stats.cpu_utilization);
    printf("Throughput:               %.2f processos/unidade de tempo\n", stats.throughput);
    
    if (stats.deadline_misses > 0) {
        printf("Deadlines perdidas:       %d\n", stats.deadline_misses);
    }
    
    printf("================================\n");
}



void calculate_process_stats(Process* process, int total_time) {
    // Variáveis usadas apenas para cálculo temporário
    int turnaround = process->completion_time - process->arrival_time;
    int waiting = turnaround - process->burst_time;
    double response_ratio = (double)turnaround / process->burst_time;
    
    // Aqui você pode usar essas variáveis se necessário
    (void)waiting;          // Silencia warning se não for usar
    (void)response_ratio;   // Silencia warning se não for usar
    (void)total_time;       // Silencia warning do parâmetro
    
    if (process->deadline > 0) {
        process->missed_deadline = (process->completion_time > process->arrival_time + process->deadline);
    }
}



void print_comparative_stats(SimulationStats stats[], char* algorithm_names[], int count) {
    printf("\n=== Comparação entre Algoritmos ===\n");
    printf("%-20s %-15s %-15s %-15s %-10s\n", 
           "Algoritmo", "Espera Média", "Retorno Médio", "Throughput", "Deadlines");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-20s %-15.2f %-15.2f %-15.2f", 
               algorithm_names[i],
               stats[i].avg_waiting_time,
               stats[i].avg_turnaround_time,
               stats[i].throughput);
        
        if (stats[i].deadline_misses > 0) {
            printf("%-10d\n", stats[i].deadline_misses);
        } else {
            printf("%-10s\n", "N/A");
        }
    }
    printf("============================================================\n");
}