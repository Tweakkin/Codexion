#include "codexion.h"

#include <string.h>

void push_request(t_queue *queue, t_request req)
{
    t_request temp;

    queue->requests[queue->size] = req;

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
    if (get_time_ms() < coder->left_dongle->available_at || get_time_ms() < coder->right_dongle->available_at)
        return (0);
    return (1);
}

void pop_request(t_queue *queue)
{
    // If size is 2, shift the person in slot 1 up to slot 0
    if (queue->size == 2)
        queue->requests[0] = queue->requests[1];
    if (queue->size > 0)
        queue->size--;
}

void take_dongles(t_coder *coder)
{
    t_request req;

    pthread_mutex_lock(&coder->simulation->state_mutex);
    req.coder_id = coder->id;
    if (strcmp(coder->simulation->config.scheduler, "fifo") == 0)
    {
        req.priority_value = coder->simulation->next_ticket_number;
        coder->simulation->next_ticket_number++;
    }
    else
        req.priority_value = coder->last_compile_start + coder->simulation->config.burnout_time;
    req.compiles_done = coder->compile_count;
    push_request(&coder->left_dongle->wait_queue, req);
    push_request(&coder->right_dongle->wait_queue, req);
    while (coder->simulation->finished == 0)
    {
        if (can_take_donglsles(coder) == 1)
            break;
        pthread_cond_wait(&coder->simulation->table_cond, &coder->simulation->state_mutex);
    }
    
    // Phase 3: The Grab
    pop_request(&coder->left_dongle->wait_queue);
    pop_request(&coder->right_dongle->wait_queue);
    
    coder->left_dongle->is_available = 0;
    coder->right_dongle->is_available = 0;
    
    pthread_mutex_unlock(&coder->simulation->state_mutex);

    // Physically lock the dongles
    pthread_mutex_lock(&coder->left_dongle->mutex);
    log_action(coder, "has taken a dongle");
    
    pthread_mutex_lock(&coder->right_dongle->mutex);
    log_action(coder, "has taken a dongle");
}

void    *routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;

    while (coder->simulation->finished == 0)
    {
        // take_dongles(coder);

        log_action(coder, "is compiling");
        sleep_ms(coder->simulation->config.compile_time);

        // drop_dongles(coder);

        log_action(coder, "is debugging");
        sleep_ms(coder->simulation->config.debug_time);

        log_action(coder, "is refactoring");
        sleep_ms(coder->simulation->config.refac_time);
    }
    return NULL;
}