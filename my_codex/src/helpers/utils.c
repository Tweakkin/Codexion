#include "codexion.h"
#include <sys/time.h>
#include <unistd.h>

long get_time_ms(void)
{
    struct timeval time;

    gettimeofday(&time, NULL);
    return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void sleep_ms(long milliseconds)
{
    usleep(milliseconds * 1000);
}

void log_action(t_coder *coder, char *message)
{
    long timepassed;

    timepassed = get_time_ms() - coder->simulation->start_time;
    pthread_mutex_lock(&coder->simulation->log_mutex);
    printf("%ld %d %s\n", timepassed, coder->id, message);
    pthread_mutex_unlock(&coder->simulation->log_mutex);
}