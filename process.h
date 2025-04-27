// ----------------------------------------------------------------
//  Define a estrutura de dados Process que representa um processo
// ----------------------------------------------------------------

#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>

// Tipos de distribuição para geração de processos
typedef enum {
    DIST_EXPONENTIAL,  // Distribuição exponencial
    DIST_NORMAL,       // Distribuição normal (Gaussiana)
    DIST_UNIFORM,      // Distribuição uniforme
    DIST_POISSON       // Distribuição de Poisson
} DistributionType;

// Estrutura que representa um processo
typedef struct {
    int pid;               // Identificador único do processo
    int arrival_time;      // Tempo de chegada do processo
    int burst_time;        // Tempo total de execução necessário
    int remaining_time;    // Tempo restante de execução
    int priority;          // Prioridade do processo (1-10)
    int period;            // Período para processos periódicos (tempo real)
    int deadline;          // Deadline absoluta (tempo real)
    int completion_time;   // Tempo de conclusão do processo
    int first_run_time;    // Tempo da primeira execução (para cálculo de tempo de resposta)
    bool missed_deadline;  // Indica se o processo perdeu o deadline
    int deadline_miss_count; // Contador de deadlines perdidos
    int original_period;   // Período original (para reinicialização)
    int original_deadline; // Deadline original 
} Process;

// Cria um novo processo com os parâmetros especificados
Process* create_process(int pid, int arrival, int burst, int priority);

// Gera um conjunto de processos com propriedades aleatórias
void generate_processes(Process** processes, int count, 
                       DistributionType arrival_dist, 
                       DistributionType burst_dist, 
                       int max_time);

// Configura parâmetros de tempo real para um processo
void setup_real_time_attributes(Process* process, int period, int deadline);

// Função para clonar um processo (útil para comparação de algoritmos)
Process* clone_process(const Process* source);

// Libera a memória alocada para um processo
void free_process(Process* process);

#endif