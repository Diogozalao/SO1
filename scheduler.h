// ----------------------------------------------------------------
//             Implementa os algoritmos de escalonamento

//             Gerencia a ordem de execução dos processos
// ----------------------------------------------------------------



#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

typedef enum {
    FCFS,
    SJF,
    PRIORITY_NP,    // Prioridade não-preemptivo
    PRIORITY_P,     // Prioridade preemptivo
    ROUND_ROBIN,
    RATE_MONOTONIC,
    EDF
} SchedulerType;


// Funções de escalonamento
void fcfs_scheduler(Process** processes, int count);
void sjf_scheduler(Process** processes, int count);
void priority_scheduler(Process** processes, int count, bool preemptive);
void rr_scheduler(Process** processes, int count, int quantum);
void rate_monotonic_scheduler(Process** processes, int count);
void edf_scheduler(Process** processes, int count);

// Função principal de escalonamento
void schedule(Process** processes, int count, SchedulerType type, int quantum, int max_time);

// Função para imprimir resultados
void print_schedule(Process** processes, int count);

#endif