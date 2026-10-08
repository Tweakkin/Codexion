/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_coder(t_simulation *sim, t_config *config, int i)
{
	sim->coders_array[i].id = i + 1;
	sim->coders_array[i].compile_count = 0;
	sim->coders_array[i].simulation = sim;
	sim->coders_array[i].right_dongle = &sim->dongles_array[i];
	sim->coders_array[i].left_dongle
		= &sim->dongles_array[(i + 1) % config->coders_num];
	sim->dongles_array[i].id = i + 1;
	sim->dongles_array[i].is_available = 1;
	sim->dongles_array[i].last_released_time = 0;
	sim->dongles_array[i].wait_queue.size = 0;
}

static int	init_coders_dongles(t_simulation *sim, t_config *config)
{
	int	i;

	i = 0;
	while (i < config->coders_num)
	{
		init_coder(sim, config, i);
		if (pthread_mutex_init(&sim->dongles_array[i].mutex,
				NULL) != 0)
		{
			free(sim->coders_array);
			free(sim->dongles_array);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	init_mutexes(t_simulation *sim)
{
	sim->sim_running = 0;
	sim->start_time = 0;
	sim->next_ticket_number = 0;
	sim->coder_threads_created = 0;
	sim->monitor_created = 0;
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0
		|| pthread_mutex_init(&sim->log_mutex, NULL) != 0
		|| pthread_cond_init(&sim->table_cond, NULL) != 0
		|| pthread_mutex_init(&sim->sim_start_lock, NULL) != 0
		|| pthread_cond_init(&sim->sim_start_cond, NULL) != 0)
	{
		free(sim->coders_array);
		free(sim->dongles_array);
		return (1);
	}
	return (0);
}

int	init_simulation(t_simulation *sim, t_config *config)
{
	sim->config = *config;
	sim->coders_array = malloc(sizeof(t_coder)
			* config->coders_num);
	if (!sim->coders_array)
		return (1);
	sim->dongles_array = malloc(sizeof(t_dongle)
			* config->coders_num);
	if (!sim->dongles_array)
	{
		free(sim->coders_array);
		return (1);
	}
	if (init_coders_dongles(sim, config) != 0)
		return (1);
	if (init_mutexes(sim) != 0)
		return (1);
	return (0);
}
