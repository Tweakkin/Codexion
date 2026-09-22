#include "codexion.h"
#include <stdlib.h>

int start_threads(t_simulation *sim, t_config *config)
{
    int i;

    i = 0;
    while (i < config->coders_num)
    {
        pthread_create(&sim->coders_array[i].thread, NULL, routine, &sim->coders_array[i]);
        i++;
    }
    return (0);
}

int init_simulation(t_simulation *sim, t_config *config)
{
    int i;

    sim->config = *config;
    sim->coders_array = malloc(sizeof(t_coder) * config->coders_num);
    if (!sim->coders_array)
        return (1);
    sim->dongles_array = malloc(sizeof(t_dongle) * config->coders_num);
    if (!sim->dongles_array)
    {
        free(sim->coders_array);
        return (1);
    }
    
    i = 0;
    while (i < config->coders_num)
    {
        sim->coders_array[i].id = i + 1;
        sim->coders_array[i].compile_count = 0;
        sim->coders_array[i].simulation = sim;
        sim->coders_array[i].right_dongle = &sim->dongles_array[i];
        sim->coders_array[i].left_dongle = &sim->dongles_array[(i + 1) % config->coders_num];
        sim->dongles_array[i].id = i + 1;
        sim->dongles_array[i].is_available = 1;
        sim->dongles_array[i].wait_queue.size = 0;
        pthread_mutex_init(&sim->dongles_array[i].mutex, NULL);
        i++;
    }
    sim->finished = 0;
    sim->next_ticket_number = 0;
    sim->coder_threads_created = 0;
    sim->monitor_created = 0;
    pthread_mutex_init(&sim->state_mutex, NULL);
    pthread_mutex_init(&sim->log_mutex, NULL);
    pthread_cond_init(&sim->table_cond, NULL);
    return (0);
}