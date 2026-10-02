#include "codexion.h"
#include <unistd.h>

void *monitor_routine(void *arg)
{
    t_simulation *sim = (t_simulation *)arg;
    
    wait_for_start_signal(sim);
    
    while (is_simulation_running(sim))
    {
        int i = 0;
        int fully_compiled_coders = 0;

        while (i < sim->config.coders_num)
        {
            long time_now = get_time_ms();
            
            // Check for Burnout
            pthread_mutex_lock(&sim->state_mutex);
            if ((time_now - sim->coders_array[i].last_compile_start) >= sim->config.burnout_time)
            {
                sim->sim_running = 0;
                pthread_cond_broadcast(&sim->table_cond);
                pthread_mutex_unlock(&sim->state_mutex);
                
                log_action(&sim->coders_array[i], "is burned out");
                return (NULL);
            }
            
            // Check Success
            if (sim->coders_array[i].compile_count >= sim->config.compiles_number)
            {
                fully_compiled_coders++;
            }
            pthread_mutex_unlock(&sim->state_mutex);
            i++;
        }

        // Did EVERYONE hit their compile target?
        if (fully_compiled_coders == sim->config.coders_num)
        {
            pthread_mutex_lock(&sim->state_mutex);
            sim->sim_running = 0;
            pthread_cond_broadcast(&sim->table_cond);
            pthread_mutex_unlock(&sim->state_mutex);
            return (NULL);
        }
        
        usleep(1000); 
    }
    return (NULL);
}