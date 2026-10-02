#include "codexion.h"
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

/* Grabs the exact current computer time and converts it entirely into milliseconds. */
long get_time_ms(void)
{
    struct timeval time;

    gettimeofday(&time, NULL);
    return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

/* Pauses the calling thread, breaking early if the simulation ends (like Example 2's ft_usleep). */
void sleep_ms(long milliseconds, t_simulation *sim)
{
    long start_time;

    start_time = get_time_ms();
    while ((get_time_ms() - start_time) < milliseconds)
    {
        if (sim != NULL && !is_simulation_running(sim))
            break;
        usleep(500);
    }
}

/* Freezes a thread until the main thread fires the start signal. */
void wait_for_start_signal(t_simulation *sim)
{
    pthread_mutex_lock(&sim->sim_start_lock);
    while (sim->start_time == 0)
    {
        pthread_cond_wait(&sim->sim_start_cond, &sim->sim_start_lock);
    }
    pthread_mutex_unlock(&sim->sim_start_lock);
}

/* Returns 1 if simulation is running, 0 if it has ended. Thread-safe. */
int is_simulation_running(t_simulation *sim)
{
    int is_running;

    pthread_mutex_lock(&sim->state_mutex);
    is_running = sim->sim_running;
    pthread_mutex_unlock(&sim->state_mutex);
    return (is_running);
}

/* Prints a thread-safe message using the log_mutex so multiple threads printing at once don't overlap text. */
void log_action(t_coder *coder, char *message)
{
    long timepassed;

    timepassed = get_time_ms() - coder->simulation->start_time;
    pthread_mutex_lock(&coder->simulation->log_mutex);
    
    // Stop any ghost prints if the simulation ended (unless it's the death message!)
    if (coder->simulation->sim_running == 0 && strcmp(message, "is burned out") != 0)
    {
        pthread_mutex_unlock(&coder->simulation->log_mutex);
        return;
    }
    
    printf("%ld %d %s\n", timepassed, coder->id, message);
    pthread_mutex_unlock(&coder->simulation->log_mutex);
}