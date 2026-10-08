/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnout(t_simulation *sim, int i)
{
	long	time_now;

	time_now = get_time_ms();
	pthread_mutex_lock(&sim->state_mutex);
	if ((time_now - sim->coders_array[i].last_compile_start)
		>= sim->config.burnout_time)
	{
		sim->sim_running = 0;
		pthread_cond_broadcast(&sim->table_cond);
		pthread_mutex_unlock(&sim->state_mutex);
		log_action(&sim->coders_array[i], "burned out");
		return (1);
	}
	return (0);
}

static int	check_coders(t_simulation *sim)
{
	int	i;
	int	fully_compiled;

	i = 0;
	fully_compiled = 0;
	while (i < sim->config.coders_num)
	{
		if (check_burnout(sim, i))
			return (1);
		if (sim->coders_array[i].compile_count
			>= sim->config.compiles_number)
			fully_compiled++;
		pthread_mutex_unlock(&sim->state_mutex);
		i++;
	}
	if (fully_compiled == sim->config.coders_num)
	{
		pthread_mutex_lock(&sim->state_mutex);
		sim->sim_running = 0;
		pthread_cond_broadcast(&sim->table_cond);
		pthread_mutex_unlock(&sim->state_mutex);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	wait_for_start_signal(sim);
	while (is_simulation_running(sim))
	{
		if (check_coders(sim))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
