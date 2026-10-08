/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
	Gets the current real-world time and converts it completely into milliseconds.
*/
long	get_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

/*
	Custom sleep function.
	Instead of blindly sleeping for the whole duration, it sleeps in tiny 500
	microsecond chunks and constantly checks if the simulation has ended.
*/
void	sleep_ms(long milliseconds, t_simulation *sim)
{
	long	start_time;

	start_time = get_time_ms();
	while ((get_time_ms() - start_time) < milliseconds)
	{
		if (sim != NULL && !is_simulation_running(sim))
			break ;
		usleep(500);
	}
}

/*
	A barrier that freezes the thread right after it is created.
*/
void	wait_for_start_signal(t_simulation *sim)
{
	pthread_mutex_lock(&sim->sim_start_lock);
	while (sim->start_time == 0)
	{
		pthread_cond_wait(&sim->sim_start_cond,
			&sim->sim_start_lock);
	}
	pthread_mutex_unlock(&sim->sim_start_lock);
}

/*
	Safely checks if the simulation is still running (1) or over (0).
*/
int	is_simulation_running(t_simulation *sim)
{
	int	is_running;

	pthread_mutex_lock(&sim->state_mutex);
	is_running = sim->sim_running;
	pthread_mutex_unlock(&sim->state_mutex);
	return (is_running);
}

void	log_action(t_coder *coder, char *message)
{
	long	timepassed;

	pthread_mutex_lock(&coder->simulation->log_mutex);
	if (!is_simulation_running(coder->simulation)
		&& strcmp(message, "burned out") != 0)
	{
		pthread_mutex_unlock(&coder->simulation->log_mutex);
		return ;
	}
	timepassed = get_time_ms() - coder->simulation->start_time;
	printf("%ld %d %s\n", timepassed, coder->id, message);
	pthread_mutex_unlock(&coder->simulation->log_mutex);
}
