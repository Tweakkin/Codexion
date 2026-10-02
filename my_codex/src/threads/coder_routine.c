#include "codexion.h"
#include <string.h>

/* Adds a coder's request to a dongle's tiny waiting line, swapping to keep the highest priority at index [0]. */
void push_request(t_queue *queue, t_request req)
{
    t_request temp;

    // 1. Put the request in the next available slot
    queue->requests[queue->size] = req;

    // 2. If there are now 2 people in line, check who wins and swap them if necessary
    if (queue->size == 1)
    {
        if (queue->requests[1].priority_value < queue->requests[0].priority_value)
        {
            temp = queue->requests[0];
            queue->requests[0] = queue->requests[1];
            queue->requests[1] = temp;
        }
        else if (queue->requests[1].priority_value == queue->requests[0].priority_value)
        {
            if (queue->requests[1].compiles_done < queue->requests[0].compiles_done)
            {
                temp = queue->requests[0];
                queue->requests[0] = queue->requests[1];
                queue->requests[1] = temp;
            }
            else if (queue->requests[1].compiles_done == queue->requests[0].compiles_done && queue->requests[1].coder_id < queue->requests[0].coder_id)
            {
                temp = queue->requests[0];
                queue->requests[0] = queue->requests[1];
                queue->requests[1] = temp;
            }
        }
    }
    queue->size++;
}

/* Checks if a coder is allowed to grab the dongles. Must be #1 in line, available, and cooldown finished. */
int can_take_dongles(t_coder *coder)
{
    if (!coder->left_dongle->is_available)
        return (0);
    if (!coder->right_dongle->is_available)
        return (0);
    if (coder->left_dongle->wait_queue.requests[0].coder_id != coder->id)
        return (0);
    if (coder->right_dongle->wait_queue.requests[0].coder_id != coder->id)
        return (0);
    return (1);
}

/* Removes a request from the queue (usually index [0]) after a coder successfully grabs the dongle. */
void pop_request(t_queue *queue)
{
    // If size is 2, shift the person in slot 1 up to slot 0
    if (queue->size == 2)
        queue->requests[0] = queue->requests[1];
    if (queue->size > 0)
        queue->size--;
}

void drop_dongles(t_coder *coder)
{
    long time_now;

    pthread_mutex_unlock(&coder->left_dongle->mutex);
    pthread_mutex_unlock(&coder->right_dongle->mutex);

    pthread_mutex_lock(&coder->simulation->state_mutex);
    time_now = get_time_ms();
    coder->left_dongle->is_available = 1;
    coder->left_dongle->last_released_time = time_now;
    coder->right_dongle->is_available = 1;
    coder->right_dongle->last_released_time = time_now;
    coder->compile_count += 1;
    pthread_cond_broadcast(&coder->simulation->table_cond);
    pthread_mutex_unlock(&coder->simulation->state_mutex);
}

/* The full process: Submit a request, sleep until allowed, and physically lock the dongles. */
int take_dongles(t_coder *coder)
{
    t_request req;
    long cooldown = coder->simulation->config.dongle_cooldown;

    if (!is_simulation_running(coder->simulation))
        return (0);

    // Phase 1: Build the request and lock the master table
    pthread_mutex_lock(&coder->simulation->state_mutex);

    // Construct the request
    req.coder_id = coder->id;
    if (strcmp(coder->simulation->config.scheduler, "fifo") == 0)
    {
        req.priority_value = coder->simulation->next_ticket_number;
        coder->simulation->next_ticket_number++;
    }
    else
        req.priority_value = coder->last_compile_start + coder->simulation->config.burnout_time;
    req.compiles_done = coder->compile_count;
    
    // Put our name on the clipboards of dongles
    push_request(&coder->left_dongle->wait_queue, req);
    push_request(&coder->right_dongle->wait_queue, req);
    
    // Phase 2: Wait until it's our turn
    while (1)
    {
        long time_now = get_time_ms();
        
        if (coder->simulation->sim_running == 0)
        {
            pthread_mutex_unlock(&coder->simulation->state_mutex);
            return (0);
        }

        if (can_take_dongles(coder) == 0)
        {
            pthread_cond_wait(&coder->simulation->table_cond, &coder->simulation->state_mutex);
        }
        else if (time_now - coder->left_dongle->last_released_time < cooldown || 
                 time_now - coder->right_dongle->last_released_time < cooldown)
        {
            pthread_mutex_unlock(&coder->simulation->state_mutex);
            usleep(500);
            pthread_mutex_lock(&coder->simulation->state_mutex);
        }
        else
        {
            break;
        }
    }
    
    // Phase 3: The Grab
    pop_request(&coder->left_dongle->wait_queue);
    pop_request(&coder->right_dongle->wait_queue);
    
    coder->left_dongle->is_available = 0;
    coder->right_dongle->is_available = 0;
    
    pthread_mutex_unlock(&coder->simulation->state_mutex);

    // Example 2 Edge Case: If there is only 1 coder, they only have 1 dongle.
    if (coder->left_dongle->id == coder->right_dongle->id)
    {
        pthread_mutex_lock(&coder->left_dongle->mutex);
        log_action(coder, "has taken a dongle");
        pthread_mutex_unlock(&coder->left_dongle->mutex);
        return (0);
    }

    // Physically lock the dongles (Lowest ID first to prevent deadlocks)
    if (coder->left_dongle->id < coder->right_dongle->id)
    {
        pthread_mutex_lock(&coder->left_dongle->mutex);
        pthread_mutex_lock(&coder->right_dongle->mutex);
    }
    else
    {
        pthread_mutex_lock(&coder->right_dongle->mutex);
        pthread_mutex_lock(&coder->left_dongle->mutex);
    }
    log_action(coder, "has taken a dongle");
    log_action(coder, "has taken a dongle");
    return (1);
}

/* The infinite daily cycle for every coder: grab tools, compile, drop tools, debug, refactor. */
void    *routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;

    // Wait for the main thread to yell GO!
    wait_for_start_signal(coder->simulation);

    // Stagger even coders by 5ms to prevent starvation (Example 2)
    if (coder->id % 2 == 0)
        sleep_ms(5, coder->simulation);
        
    // Loop continuously until the global alarm rings
    while (is_simulation_running(coder->simulation))
    {
        if (take_dongles(coder))
        {
            coder->last_compile_start = get_time_ms();
            log_action(coder, "is compiling");
            sleep_ms(coder->simulation->config.compile_time, coder->simulation);
            drop_dongles(coder);
            
            if (coder->compile_count == coder->simulation->config.compiles_number)
                break;
            if (!is_simulation_running(coder->simulation))
                break;
                
            log_action(coder, "is debugging");
            sleep_ms(coder->simulation->config.debug_time, coder->simulation);
            
            if (!is_simulation_running(coder->simulation))
                break;
                
            log_action(coder, "is refactoring");
            sleep_ms(coder->simulation->config.refac_time, coder->simulation);
        }
        if (coder->compile_count == coder->simulation->config.compiles_number)
            break;
    }
    return (NULL);
}