/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	drop_dongles(t_coder *coder)
{
	long	time_now;

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

static void	compile_cycle(t_coder *coder)
{
	pthread_mutex_lock(&coder->simulation->state_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->simulation->state_mutex);
	log_action(coder, "is compiling");
	sleep_ms(coder->simulation->config.compile_time,
		coder->simulation);
	drop_dongles(coder);
	if (!is_simulation_running(coder->simulation))
		return ;
	log_action(coder, "is debugging");
	sleep_ms(coder->simulation->config.debug_time,
		coder->simulation);
	if (!is_simulation_running(coder->simulation))
		return ;
	log_action(coder, "is refactoring");
	sleep_ms(coder->simulation->config.refac_time,
		coder->simulation);
}

void	*routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	wait_for_start_signal(coder->simulation);
	if (coder->id % 2 == 0)
		sleep_ms(5, coder->simulation);
	while (is_simulation_running(coder->simulation))
	{
		if (take_dongles(coder))
			compile_cycle(coder);
		if (coder->compile_count
			== coder->simulation->config.compiles_number)
			break ;
	}
	return (NULL);
}
