/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
    Spawns coders and monitor
    Sets exact starting time for everyone
    Broadcast starting signal for everyone to wakeup
*/
static int	start_and_sync(t_simulation *sim, t_config *config)
{
	int	j;

	if (start_threads(sim, config) != 0)
		return (1);
	if (pthread_create(&sim->monitor_thread, NULL,
			monitor_routine, sim) != 0)
	{
		free_simulation(sim, config->coders_num);
		return (1);
	}
	pthread_mutex_lock(&sim->sim_start_lock);
	pthread_mutex_lock(&sim->state_mutex);
	sim->start_time = get_time_ms();
	j = 0;
	while (j < config->coders_num)
	{
		sim->coders_array[j].last_compile_start = sim->start_time;
		j++;
	}
	sim->sim_running = 1;
	pthread_mutex_unlock(&sim->state_mutex);
	pthread_cond_broadcast(&sim->sim_start_cond);
	pthread_mutex_unlock(&sim->sim_start_lock);
	return (0);
}

/*
    Waits for all coder threads and the monitor thread to successfully
    finish and join back into the main program before cleaning up.
*/
static int	cleanup_threads(t_simulation *sim, t_config *config)
{
	if (join_threads(sim, config->coders_num) != 0)
	{
		free_simulation(sim, 0);
		return (1);
	}
	if (pthread_join(sim->monitor_thread, NULL) != 0)
	{
		free_simulation(sim, 0);
		return (1);
	}
	return (0);
}

/*
    Parse args, init mem and mutexes
    start threads, wait for them, free and exit
*/
int	main(int argc, char **argv)
{
	t_config		config;
	t_simulation	sim;

	if (parse_args(argc, argv, &config) != 0)
		return (1);
	if (config.compiles_number == 0)
		return (0);
	if (init_simulation(&sim, &config) != 0)
		return (1);
	if (start_and_sync(&sim, &config) != 0)
		return (1);
	if (cleanup_threads(&sim, &config) != 0)
		return (1);
	free_simulation(&sim, 0);
	return (0);
}
