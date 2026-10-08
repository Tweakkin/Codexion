/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_dongles.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	build_request(t_coder *coder, t_request *req)
{
	req->coder_id = coder->id;
	if (strcmp(coder->simulation->config.scheduler, "fifo") == 0)
	{
		req->priority_value
			= coder->simulation->next_ticket_number;
		coder->simulation->next_ticket_number++;
	}
	else
		req->priority_value = coder->last_compile_start
			+ coder->simulation->config.burnout_time;
	req->compiles_done = coder->compile_count;
}

static int	check_cooldown(t_coder *coder, long time_now, long cd)
{
	if (time_now - coder->left_dongle->last_released_time < cd
		|| time_now
		- coder->right_dongle->last_released_time < cd)
		return (1);
	return (0);
}

static int	wait_for_turn(t_coder *coder, long cooldown)
{
	long	time_now;

	while (1)
	{
		time_now = get_time_ms();
		if (coder->simulation->sim_running == 0)
		{
			pthread_mutex_unlock(&coder->simulation->state_mutex);
			return (0);
		}
		if (can_take_dongles(coder) == 0)
			pthread_cond_wait(&coder->simulation->table_cond,
				&coder->simulation->state_mutex);
		else if (check_cooldown(coder, time_now, cooldown))
		{
			pthread_mutex_unlock(&coder->simulation->state_mutex);
			usleep(500);
			pthread_mutex_lock(&coder->simulation->state_mutex);
		}
		else
			break ;
	}
	return (1);
}

static int	grab_dongles(t_coder *coder)
{
	pop_request(&coder->left_dongle->wait_queue);
	pop_request(&coder->right_dongle->wait_queue);
	coder->left_dongle->is_available = 0;
	coder->right_dongle->is_available = 0;
	pthread_mutex_unlock(&coder->simulation->state_mutex);
	if (coder->left_dongle->id == coder->right_dongle->id)
	{
		pthread_mutex_lock(&coder->left_dongle->mutex);
		log_action(coder, "has taken a dongle");
		pthread_mutex_unlock(&coder->left_dongle->mutex);
		return (0);
	}
	pthread_mutex_lock(&coder->left_dongle->mutex);
	log_action(coder, "has taken a dongle");
	pthread_mutex_lock(&coder->right_dongle->mutex);
	log_action(coder, "has taken a dongle");
	return (1);
}

int	take_dongles(t_coder *coder)
{
	t_request	req;
	long		cooldown;

	cooldown = coder->simulation->config.dongle_cooldown;
	if (!is_simulation_running(coder->simulation))
		return (0);
	pthread_mutex_lock(&coder->simulation->state_mutex);
	build_request(coder, &req);
	push_request(&coder->left_dongle->wait_queue, req);
	push_request(&coder->right_dongle->wait_queue, req);
	if (!wait_for_turn(coder, cooldown))
		return (0);
	return (grab_dongles(coder));
}
