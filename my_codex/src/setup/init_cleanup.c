/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_cleanup.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	join_threads(t_simulation *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		if (pthread_join(sim->coders_array[i].thread, NULL) != 0)
			return (1);
		i++;
	}
	return (0);
}

int	start_threads(t_simulation *sim, t_config *config)
{
	int	i;

	i = 0;
	while (i < config->coders_num)
	{
		if (pthread_create(&sim->coders_array[i].thread, NULL,
				routine, &sim->coders_array[i]) != 0)
		{
			free_simulation(sim, i);
			return (1);
		}
		i++;
	}
	return (0);
}

void	free_simulation(t_simulation *sim, int count)
{
	int	i;

	if (count > 0)
	{
		pthread_mutex_lock(&sim->sim_start_lock);
		sim->start_time = -1;
		pthread_cond_broadcast(&sim->sim_start_cond);
		pthread_mutex_unlock(&sim->sim_start_lock);
		if (join_threads(sim, count) != 0)
			return ;
	}
	i = 0;
	while (i < sim->config.coders_num)
	{
		pthread_mutex_destroy(&sim->dongles_array[i].mutex);
		i++;
	}
	pthread_mutex_destroy(&sim->state_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_cond_destroy(&sim->table_cond);
	pthread_mutex_destroy(&sim->sim_start_lock);
	pthread_cond_destroy(&sim->sim_start_cond);
	free(sim->coders_array);
	free(sim->dongles_array);
}
