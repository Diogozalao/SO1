#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "process.h"
#include "scheduler.h"
#include "stats.h"
#include "random_generator.h"

void print_menu() {
    printf("\nProbSched - Simulador de Escalonamento de CPU\n");
    printf("1. Gerar processos aleatórios\n");
    printf("2. Executar FCFS (First-Come, First-Served)\n");
    printf("3. Executar SJF (Shortest Job First)\n");
    printf("4. Executar Prioridade não-preemptivo\n");
    printf("5. Executar Prioridade preemptivo\n");
    printf("6. Executar Round Robin\n");
    printf("7. Executar Rate Monotonic (tempo real)\n");
    printf("8. Executar EDF (Earliest Deadline First)\n");
    printf("9. Mostrar estatísticas\n");
    printf("10. Sair\n");
    printf("Escolha uma opção: ");
}

void setup_real_time_processes(Process** processes, int count) {
    // Configura períodos e deadlines para algoritmos de tempo real
    for (int i = 0; i < count; i++) {
        processes[i]->period = uniform_random_int(20, 50);    // Período entre 20 e 50
        processes[i]->deadline = processes[i]->period;    // Deadline igual ao período inicialmente
    }
}

int main() {
    init_random();
    
    Process** processes = NULL;
    int process_count = 0;
    int max_time = 100;
    int quantum = 4;
    SimulationStats stats = {0};
    bool processes_generated = false;
    
    int choice;
    do {
        print_menu();
        scanf("%d", &choice);
        
        switch(choice) {
            case 1: {
                printf("Número de processos a gerar: ");
                scanf("%d", &process_count);
                
                printf("Tempo máximo de simulação: ");
                scanf("%d", &max_time);
                
                printf("Quantum para Round Robin: ");
                scanf("%d", &quantum);
                
                // Libera processos anteriores se existirem
                if (processes != NULL) {
                    for (int i = 0; i < process_count; i++) {
                        free(processes[i]);
                    }
                    free(processes);
                }
                
                processes = (Process**)malloc(process_count * sizeof(Process*));
                if (processes == NULL) {
                    perror("Erro ao alocar memória para processos");
                    exit(EXIT_FAILURE);
                }
                
                // Gera processos com distribuição exponencial para chegada e normal para burst
                generate_processes(processes, process_count, DIST_EXPONENTIAL, DIST_NORMAL, max_time);
                
                // Configura processos de tempo real
                setup_real_time_processes(processes, process_count);
                
                processes_generated = true;
                printf("%d processos gerados com sucesso!\n", process_count);
                break;
            }
            case 2:
                if (!processes_generated) {
                    printf("Gere processos primeiro!\n");
                    break;
                }
                schedule(processes, process_count, FCFS, quantum, max_time);
                stats = calculate_stats(processes, process_count, max_time);
                break;
            case 3:
                if (!processes_generated) {
                    printf("Gere processos primeiro!\n");
                    break;
                }
                schedule(processes, process_count, SJF, quantum, max_time);
                stats = calculate_stats(processes, process_count, max_time);
                break;
            case 4:
                if (!processes_generated) {
                    printf("Gere processos primeiro!\n");
                    break;
                }
                schedule(processes, process_count, PRIORITY_NP, quantum, max_time);
                stats = calculate_stats(processes, process_count, max_time);
                break;
            case 5:
                if (!processes_generated) {
                    printf("Gere processos primeiro!\n");
                    break;
                }
                schedule(processes, process_count, PRIORITY_P, quantum, max_time);
                stats = calculate_stats(processes, process_count, max_time);
                break;
            case 6:
                if (!processes_generated) {
                    printf("Gere processos primeiro!\n");
                    break;
                }
                schedule(processes, process_count, ROUND_ROBIN, quantum, max_time);
                stats = calculate_stats(processes, process_count, max_time);
                break;
            case 7:
                if (!processes_generated) {
                    printf("Gere processos primeiro!\n");
                    break;
                }
                schedule(processes, process_count, RATE_MONOTONIC, quantum, max_time);
                stats = calculate_stats(processes, process_count, max_time);
                break;
            case 8:
                if (!processes_generated) {
                    printf("Gere processos primeiro!\n");
                    break;
                }
                schedule(processes, process_count, EDF, quantum, max_time);
                stats = calculate_stats(processes, process_count, max_time);
                break;
            case 9:
                if (!processes_generated) {
                    printf("Gere e execute processos primeiro!\n");
                    break;
                }
                print_stats(stats);
                break;
            case 10:
                printf("A sair...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (choice != 10);
    
    // Libertação de memória
    if (processes != NULL) {
        for (int i = 0; i < process_count; i++) {
            free(processes[i]);
        }
        free(processes);
    }
    
    return 0;
}