#include "codexion.h"
#include <stdlib.h>

/* Spawns a thread for each coder to run the 'routine' function. */
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

/* Allocates memory for the coders and dongles, links them in a circle, and sets starting values. */
int init_simulation(t_simulation *sim, t_config *config)
{
    int i;

    sim->config = *config;
    // Malloc the arrays safely
    sim->coders_array = malloc(sizeof(t_coder) * config->coders_num);
    if (!sim->coders_array)
        return (1);
    sim->dongles_array = malloc(sizeof(t_dongle) * config->coders_num);
    if (!sim->dongles_array)
    {
        free(sim->coders_array);
        return (1);
    }
    
    // Build the room: Configure each coder and dongle, and link them together
    i = 0;
    while (i < config->coders_num)
    {
        sim->coders_array[i].id = i + 1;
        sim->coders_array[i].compile_count = 0;
        sim->coders_array[i].simulation = sim;
        
        // Circular linking: Every coder grabs the dongle at their index, and the one to their right
        sim->coders_array[i].right_dongle = &sim->dongles_array[i];
        sim->coders_array[i].left_dongle = &sim->dongles_array[(i + 1) % config->coders_num];
        
        sim->dongles_array[i].id = i + 1;
        sim->dongles_array[i].is_available = 1;
        sim->dongles_array[i].last_released_time = 0;
        sim->dongles_array[i].wait_queue.size = 0;
        pthread_mutex_init(&sim->dongles_array[i].mutex, NULL);
        i++;
    }
    
    // Set global starting states and initialize master locks
    sim->sim_running = 0; // Starts at 0, main thread sets to 1
    sim->start_time = 0;
    sim->next_ticket_number = 0;
    sim->coder_threads_created = 0;
    sim->monitor_created = 0;
    pthread_mutex_init(&sim->state_mutex, NULL);
    pthread_mutex_init(&sim->log_mutex, NULL);
    pthread_cond_init(&sim->table_cond, NULL);
    pthread_mutex_init(&sim->sim_start_lock, NULL);
    pthread_cond_init(&sim->sim_start_cond, NULL);
    return (0);
}

/* Cleans up all the memory and destroys all locks before closing the program. */
void free_simulation(t_simulation *sim)
{
    int i;

    i = 0;
    while (i < sim->config.coders_num)
    {
        pthread_mutex_destroy(&sim->dongles_array[i].mutex);
        i++;
    }
    pthread_mutex_destroy(&sim->state_mutex);
    pthread_mutex_destroy(&sim->log_mutex);
    pthread_cond_destroy(&sim->table_cond);
    pthread_mutex_destroy(&sim->sim_start_lock);
    pthread_cond_destroy(&sim->sim_start_cond);
    free(sim->coders_array);
    free(sim->dongles_array);
}