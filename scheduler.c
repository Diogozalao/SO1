#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "scheduler.h"
#include "stats.h"
#include "process.h"

// Helper functions
int compare_arrival(const void* a, const void* b) {
    Process* p1 = *(Process**)a;
    Process* p2 = *(Process**)b;
    return p1->arrival_time - p2->arrival_time;
}

int compare_burst(const void* a, const void* b) {
    Process* p1 = *(Process**)a;
    Process* p2 = *(Process**)b;
    return p1->burst_time - p2->burst_time;
}

int compare_priority(const void* a, const void* b) {
    Process* p1 = *(Process**)a;
    Process* p2 = *(Process**)b;
    return p1->priority - p2->priority;
}

int compare_period(const void* a, const void* b) {
    Process* p1 = *(Process**)a;
    Process* p2 = *(Process**)b;
    return p1->period - p2->period;
}

int compare_deadline(const void* a, const void* b) {
    Process* p1 = *(Process**)a;
    Process* p2 = *(Process**)b;
    return (p1->arrival_time + p1->deadline) - (p2->arrival_time + p2->deadline);
}



//-----------------------------------------------------------------
//                IMPLEMENTAÇÃO DOS ALGORITMOS
//-----------------------------------------------------------------

void fcfs_scheduler(Process** processes, int count) {
    qsort(processes, count, sizeof(Process*), compare_arrival);
    
    int current_time = 0;
    for (int i = 0; i < count; i++) {
        if (current_time < processes[i]->arrival_time) {
            current_time = processes[i]->arrival_time;
        }
        
        processes[i]->remaining_time = 0;
        processes[i]->completion_time = current_time + processes[i]->burst_time;
        current_time = processes[i]->completion_time;
    }
}



void sjf_scheduler(Process** processes, int count) {
    qsort(processes, count, sizeof(Process*), compare_arrival);
    
    int current_time = 0;
    int completed = 0;
    
    while (completed < count) {
        int shortest = -1;
        int shortest_burst = INT_MAX;
        
        for (int i = 0; i < count; i++) {
            if (processes[i]->arrival_time <= current_time && 
                processes[i]->remaining_time > 0 && 
                processes[i]->burst_time < shortest_burst) {
                shortest = i;
                shortest_burst = processes[i]->burst_time;
            }
        }
        
        if (shortest == -1) {
            current_time++;
            continue;
        }
        
        processes[shortest]->remaining_time = 0;
        current_time += processes[shortest]->burst_time;
        processes[shortest]->completion_time = current_time;
        completed++;
    }
}



void priority_scheduler(Process** processes, int count, bool preemptive) {
    qsort(processes, count, sizeof(Process*), compare_arrival);
    
    int current_time = 0;
    int completed = 0;
    int* remaining_time = (int*)malloc(count * sizeof(int));
    
    // Inicializa remaining_time
    for (int i = 0; i < count; i++) {
        remaining_time[i] = processes[i]->burst_time;
    }

    while (completed < count) {
        // 1. Verificar deadlines perdidas
        for (int i = 0; i < count; i++) {
            if (processes[i]->deadline > 0 && 
                current_time > processes[i]->arrival_time + processes[i]->deadline && 
                remaining_time[i] > 0) {
                processes[i]->missed_deadline = true;
                remaining_time[i] = 0;
                processes[i]->completion_time = current_time;
                completed++;
                printf(" Deadline perdida para PID %d (tempo atual: %d)\n", 
                      processes[i]->pid, current_time);
            }
        }

        // 2. Selecionar processo - prioridade 0 tem absoluta preferência
        int selected = -1;
        int highest_priority = INT_MAX;
        
        // Primeiro verifica se há processos RT (prioridade 0) prontos
        for (int i = 0; i < count; i++) {
            if (processes[i]->priority == 0 && 
                processes[i]->arrival_time <= current_time && 
                remaining_time[i] > 0) {
                selected = i;
                highest_priority = 0;
                break; // Prioridade 0 tem máxima urgência
            }
        }

        // Se não encontrou RT, busca o de maior prioridade normal
        if (selected == -1) {
            for (int i = 0; i < count; i++) {
                if (processes[i]->arrival_time <= current_time && 
                    remaining_time[i] > 0 && 
                    processes[i]->priority < highest_priority) {
                    highest_priority = processes[i]->priority;
                    selected = i;
                }
            }
        }

        if (selected == -1) {
            current_time++; // Nenhum processo pronto
            continue;
        }

        // 3. Lógica de execução
        if (preemptive) {
            // Versão PREEMPTIVA (executa 1 unidade de tempo)
            remaining_time[selected]--;
            current_time++;
            
            if (processes[selected]->first_run_time == -1) {
                processes[selected]->first_run_time = current_time - 1;
            }
            
            if (remaining_time[selected] == 0) {
                processes[selected]->completion_time = current_time;
                completed++;
            }
        } 
        else {
            // Versão NÃO-PREEMPTIVA (executa até completar)
            if (processes[selected]->first_run_time == -1) {
                processes[selected]->first_run_time = current_time;
            }
            
            int execution_time = remaining_time[selected];
            remaining_time[selected] = 0;
            current_time += execution_time;
            processes[selected]->completion_time = current_time; //+ execution_time;
            completed++;
            
            printf(" Executando PID %d (Prio %d) por %d unidades (t=%d a %d)\n",
                  processes[selected]->pid,
                  processes[selected]->priority,
                  execution_time,
                  processes[selected]->completion_time - execution_time,
                  processes[selected]->completion_time);    // Usa completion_time para precisão
        }
    }
    
    free(remaining_time);
}


void rr_scheduler(Process** processes, int count, int quantum) {
    qsort(processes, count, sizeof(Process*), compare_arrival);
    
    int* remaining_time = (int*)malloc(count * sizeof(int));
    for (int i = 0; i < count; i++) {
        remaining_time[i] = processes[i]->burst_time;
    }
    
    int current_time = processes[0]->arrival_time;
    int completed = 0;
    
    while (completed < count) {
        bool any_executed = false;
        
        for (int i = 0; i < count; i++) {
            if (processes[i]->arrival_time <= current_time && remaining_time[i] > 0) {
                any_executed = true;
                
                int exec_time = (remaining_time[i] > quantum) ? quantum : remaining_time[i];
                remaining_time[i] -= exec_time;
                current_time += exec_time;
                
                if (remaining_time[i] == 0) {
                    processes[i]->completion_time = current_time;
                    completed++;
                }
                
                // Verifica se novos processos chegaram durante a execução
                for (int j = 0; j < count; j++) {
                    if (processes[j]->arrival_time > current_time - exec_time && 
                        processes[j]->arrival_time <= current_time && 
                        remaining_time[j] > 0) {
                        // Novo processo chegou, volta ao início da fila
                        i = -1; // Reset para começar do início no próximo loop
                        break;
                    }
                }
            }
        }
        
        if (!any_executed) {
            current_time++;
        }
    }
    
    free(remaining_time);
}



void rate_monotonic_scheduler(Process** processes, int count) {
    // Para Rate Monotonic, assumimos que period está definido
    qsort(processes, count, sizeof(Process*), compare_period);
    
    int current_time = 0;
    int completed = 0;
    int max_simulation_time = 1000; // Limite de segurança

    int* next_release = (int*)malloc(count * sizeof(int));
    int* remaining_time = (int*)malloc(count * sizeof(int));
    
    for (int i = 0; i < count; i++) {
        next_release[i] = processes[i]->arrival_time;
        remaining_time[i] = processes[i]->burst_time;
    }
    
    while (completed < count && current_time < max_simulation_time) {
        for (int i = 0; i < count; i++) {
            if (processes[i]->period > 0 && current_time >= next_release[i] + processes[i]->period) {
                if (remaining_time[i] > 0) {
                    processes[i]->deadline_miss_count++;
                }
                next_release[i] += processes[i]->period;
                remaining_time[i] = processes[i]->burst_time;
            }
        }

        int selected = -1;
        int min_period = INT_MAX;
        
        for (int i = 0; i < count; i++) {
            if (current_time >= next_release[i] && 
                remaining_time[i] > 0 && 
                processes[i]->period < min_period) {
                min_period = processes[i]->period;
                selected = i;
            }
        }

        if (selected == -1) {
            current_time++;
            continue;
        }

        remaining_time[selected]--;
        current_time++;

        if (processes[selected]->first_run_time == -1) {
            processes[selected]->first_run_time = current_time - 1;
        }

        if (remaining_time[selected] == 0) {
            processes[selected]->completion_time = current_time;
            completed++;
        }
    }
    
    free(next_release);
    free(remaining_time);
}



void edf_scheduler(Process** processes, int count) {
    // Para EDF, assumimos que deadline está definido
    qsort(processes, count, sizeof(Process*), compare_arrival);
    
    int current_time = 0;
    int completed = 0;
    int* remaining_time = (int*)malloc(count * sizeof(int));
    int* absolute_deadline = (int*)malloc(count * sizeof(int));
    
    for (int i = 0; i < count; i++) {
        remaining_time[i] = processes[i]->burst_time;
        absolute_deadline[i] = processes[i]->arrival_time + processes[i]->deadline;
    }
    
    while (completed < count) {
        // Verifica deadlines perdidas
        for (int i = 0; i < count; i++) {
            if (processes[i]->arrival_time <= current_time && 
                remaining_time[i] > 0 && 
                current_time > absolute_deadline[i]) {
                printf("Deadline perdida para processo %d no tempo %d\n", 
                       processes[i]->pid, current_time);
                remaining_time[i] = 0;
                processes[i]->completion_time = current_time;
                completed++;
            }
        }
        
        int selected = -1;
        int earliest_deadline = INT_MAX;
        
        for (int i = 0; i < count; i++) {
            if (processes[i]->arrival_time <= current_time && 
                remaining_time[i] > 0 && 
                absolute_deadline[i] < earliest_deadline) {
                earliest_deadline = absolute_deadline[i];
                selected = i;
            }
        }
        
        if (selected == -1) {
            current_time++;
            continue;
        }
        
        remaining_time[selected]--;
        current_time++;
        
        if (remaining_time[selected] == 0) {
            processes[selected]->completion_time = current_time;
            completed++;
        }
    }
    
    free(remaining_time);
    free(absolute_deadline);
}



void check_missed_deadlines(Process** processes, int count, int current_time) {
    for (int i = 0; i < count; i++) {
        if (processes[i]->deadline > 0 && 
            processes[i]->remaining_time > 0 &&
            current_time > processes[i]->arrival_time + processes[i]->deadline) {
            processes[i]->missed_deadline = true;
            processes[i]->deadline_miss_count++;
            processes[i]->remaining_time = 0; // Marca como concluído (com deadline perdida)
            processes[i]->completion_time = current_time;
        }
    }
}



void schedule(Process** processes, int count, SchedulerType type, int quantum, int max_time) {
    
    (void)max_time;

    // Reset dos processos
    for (int i = 0; i < count; i++) {
        processes[i]->remaining_time = processes[i]->burst_time;
        processes[i]->completion_time = 0;
        processes[i]->first_run_time = -1;
        processes[i]->deadline_miss_count = 0;
        processes[i]->deadline_miss_count = 0;
    }
    
    switch(type) {
        
        case FCFS:
            fcfs_scheduler(processes, count);
            break;
        case SJF:
            sjf_scheduler(processes, count);
            break;
        case PRIORITY_NP:
        printf("\n Iniciando Prioridade Não-Preemptivo\n");
            priority_scheduler(processes, count, false);
            break;
        case PRIORITY_P:
        printf("\n Iniciando Prioridade Preemptivo\n");
            priority_scheduler(processes, count, true);
            break;
        case ROUND_ROBIN:
            rr_scheduler(processes, count, quantum);
            break;
        case RATE_MONOTONIC:
            rate_monotonic_scheduler(processes, count);
            break;
        case EDF:
            edf_scheduler(processes, count);
            break;
        default:
            fprintf(stderr, "Algoritmo não implementado ainda!\n");
    }
    
    print_schedule(processes, count);
}



void print_schedule(Process** processos, int count) {
    printf("\nResultado do Escalonamento:\n");
    printf("PID\tChegada\tExecução\tPrioridade\tConclusão\tDeadline\n");
    for (int i = 0; i < count; i++) {
        printf("%d\t%d\t%d\t\t%d\t\t%d\t\t%d\n", 
               processos[i]->pid,
               processos[i]->arrival_time,
               processos[i]->burst_time,
               processos[i]->priority,
               processos[i]->completion_time,
               processos[i]->deadline);
    }
}