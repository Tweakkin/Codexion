#include "codexion.h"

int main(int argc, char **argv)
{
    // Holds the rules
    t_config config;
    // Holds The prepared room
    t_simulation sim;

    // Parse commands line, and fills up config
    if (parse_args(argc, argv, &config) != 0)
		return (1);
    
    // build the room (Sim): allocate memory, link coders/dongles, init mutexes
    if (init_simulation(&sim, &config) != 0)
    {
        printf("Failed to initialize simulation\n");
        return (1);
    }
    
    printf("ALL GOOD! Room is built.\n");

    // Spawns all the threads (Coders and Monitor start, but hit the wait_for_start_signal wall)
    start_threads(&sim, &config);
    pthread_create(&sim.monitor_thread, NULL, monitor_routine, &sim);

    // The organizer sets everyone's starting clock
    pthread_mutex_lock(&sim.state_mutex);
    sim.start_time = get_time_ms();
    for (int j = 0; j < config.coders_num; j++)
    {
        sim.coders_array[j].last_compile_start = sim.start_time;
    }
    sim.sim_running = 1;
    pthread_mutex_unlock(&sim.state_mutex);

    // The organizer announces Go
    pthread_mutex_lock(&sim.sim_start_lock);
    pthread_cond_broadcast(&sim.sim_start_cond);
    pthread_mutex_unlock(&sim.sim_start_lock);

    // Wait for all threads to finish their work before closing the program
    int i = 0;
    while (i < config.coders_num)
    {
        pthread_join(sim.coders_array[i].thread, NULL);
        i++;
    }
    pthread_join(sim.monitor_thread, NULL);

    // Destory mutexes and free coder and dongle array
    free_simulation(&sim);
    return (0);
}