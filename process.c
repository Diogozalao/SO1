#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "process.h"
#include "random_generator.h"

Process* create_process(int pid, int arrival, int burst, int priority) {
    Process* p = (Process*)malloc(sizeof(Process));
    if (p == NULL) {
        perror("Erro ao alocar memória para processo");
        exit(EXIT_FAILURE);
    }

    p->pid = pid;
    p->arrival_time = arrival;
    p->burst_time = burst;
    p->remaining_time = burst;
    p->priority = priority;
    p->period = 0;
    p->deadline = 0;
    p->completion_time = 0;
    p->first_run_time = -1;
    p->missed_deadline = false;
    p->deadline_miss_count = 0;
    p->original_period = 0;
    p->original_deadline = 0;

    return p;
}

void generate_processes(Process** processes, int count, 
                       DistributionType arrival_dist, 
                       DistributionType burst_dist, 
                       int max_time) {
    if (count <= 0 || processes == NULL) return;

    int arrival_time = 0;
    const int real_time_probability = 20; // 20% chance de ser processo de tempo real

    for (int i = 0; i < count; i++) {
        
        // Gera tempo de chegada
        switch(arrival_dist) {
            case DIST_EXPONENTIAL:
                arrival_time += (int)(exponential_random(0.5) + 1);
                break;
            case DIST_NORMAL:
                arrival_time += (int)(normal_random(5, 2) + 1);
                break;
            case DIST_UNIFORM:
                arrival_time += uniform_random_int(1, 10);
                break;
            case DIST_POISSON:
                arrival_time += poisson_random(3);
                break;
        }

        // Gera tempo de execução
        int burst_time;
        switch(burst_dist) {
            case DIST_EXPONENTIAL:
                burst_time = (int)(exponential_random(0.3) + 1);
                break;
            case DIST_NORMAL:
                burst_time = (int)(normal_random(8, 3) + 1);
                break;
            case DIST_UNIFORM:
                burst_time = uniform_random_int(1, 15);
                break;
            case DIST_POISSON:
                burst_time = poisson_random(5) + 1;
                break;
        }

        // Cria o processo
        processes[i] = create_process(i+1, arrival_time, burst_time, generate_priority(true));

        // Configura como tempo real (com probabilidade)
        if (uniform_random_int(1, 100) <= real_time_probability) {
            int period = uniform_random_int(20, 50);
            setup_real_time_attributes(processes[i], period, period);
        }

        // Verifica limite máximo de tempo
        if (arrival_time > max_time) {
            // Ajusta o count para os processos gerados
            count = i + 1;
            break;
        }
    }
}

Process* clone_process(const Process* source) {
    if (source == NULL) return NULL;

    Process* clone = (Process*)malloc(sizeof(Process));
    if (clone == NULL) {
        perror("Erro ao alocar memória para clone de processo");
        exit(EXIT_FAILURE);
    }

    // Copia todos os campos
    memcpy(clone, source, sizeof(Process));
    return clone;
}


void setup_real_time_attributes(Process* process, int period, int deadline) {
    if (process == NULL) return;
    
    process->period = period;
    process->deadline = process->arrival_time + deadline; // Deadline absoluto
    process->original_period = period;
    process->original_deadline = deadline;
    process->priority = 0; // Máxima prioridade para tempo real
}

void free_process(Process* process) {
    if (process != NULL) {
        free(process);
    }
}