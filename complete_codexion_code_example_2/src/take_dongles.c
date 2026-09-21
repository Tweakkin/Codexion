/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 08:50:51 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 16:53:41 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	enqueue_coder(t_coder *coder, t_dongle *dongle)
{
	t_request	coder_request;

	coder_request.coder_id = coder->id;
	coder_request.compiles_done = coder->compiles_done;
	if (coder->data->args.scheduler == EDF)
	{
		coder_request.priority = (coder->last_compiled_time
				+ coder->data->args.burnout_time);
	}
	else
		coder_request.priority = get_current_time_ms();
	pqueue_push(dongle->pqueue, coder_request);
}

static int	get_top_coder_id(t_heapq *pqueue)
{
	t_request	top;

	if (pqueue_peek(pqueue, &top) == true)
		return (top.coder_id);
	return (-1);
}

static bool	can_take_dongles(t_coder *coder)
{
	if (!coder->left_dongle->is_available || !coder->right_dongle->is_available)
		return (false);
	if (get_top_coder_id(coder->left_dongle->pqueue) != coder->id)
		return (false);
	if (get_top_coder_id(coder->right_dongle->pqueue) != coder->id)
		return (false);
	return (true);
}

static bool	wait_for_dongles(t_coder *coder)
{
	long long	now;
	long long	cooldown;

	cooldown = coder->data->args.cooldown_time;
	while (1)
	{
		if (!coder->data->sim_running)
			return (false);
		now = get_current_time_ms();
		if (!can_take_dongles(coder))
		{
			pthread_cond_wait(&coder->data->table_cond,
				&coder->data->state_lock);
		}
		else if (now - coder->left_dongle->last_released_time < cooldown
			|| now - coder->right_dongle->last_released_time < cooldown)
		{
			pthread_mutex_unlock(&coder->data->state_lock);
			usleep(500);
			pthread_mutex_lock(&coder->data->state_lock);
		}
		else
			break ;
	}
	return (true);
}

bool	take_dongles(t_coder *coder)
{
	if (!is_simulation_running(coder->data))
		return (false);
	pthread_mutex_lock(&coder->data->state_lock);
	enqueue_coder(coder, coder->left_dongle);
	enqueue_coder(coder, coder->right_dongle);
	if (!wait_for_dongles(coder))
	{
		pthread_mutex_unlock(&coder->data->state_lock);
		return (false);
	}
	pqueue_pop(coder->left_dongle->pqueue, NULL);
	pqueue_pop(coder->right_dongle->pqueue, NULL);
	coder->left_dongle->is_available = false;
	coder->right_dongle->is_available = false;
	pthread_mutex_unlock(&coder->data->state_lock);
	if (coder->left_dongle->id == coder->right_dongle->id)
	{
		pthread_mutex_lock(&coder->left_dongle->lock);
		print_action(coder, "has taken a dongle");
		pthread_mutex_unlock(&coder->left_dongle->lock);
		return (false);
	}
	lock_physical_dongles(coder);
	return (true);
}
