/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 18:34:39 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 13:32:19 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	start_simulation(t_data *data)
{
	int			i;
	long long	start_time;

	i = 0;
	while (i < data->args.nb_coders)
	{
		pthread_create(&data->coders[i].thread_id, NULL,
			coder_routine, &data->coders[i]);
		i++;
	}
	pthread_create(&data->monitor_thread, NULL, monitor_routine, data);
	pthread_mutex_lock(&data->state_lock);
	start_time = get_current_time_ms();
	i = 0;
	while (i < data->args.nb_coders)
	{
		data->coders[i].last_compiled_time = start_time;
		i++;
	}
	data->sim_running = true;
	pthread_mutex_unlock(&data->state_lock);
	pthread_mutex_lock(&data->sim_start_lock);
	data->sim_start_time = start_time;
	pthread_cond_broadcast(&data->sim_start_cond);
	pthread_mutex_unlock(&data->sim_start_lock);
}

bool	is_simulation_running(t_data *data)
{
	bool	is_running;

	pthread_mutex_lock(&data->state_lock);
	is_running = data->sim_running;
	pthread_mutex_unlock(&data->state_lock);
	return (is_running);
}

void	stop_simulation(t_data *data)
{
	int		i;

	pthread_join(data->monitor_thread, NULL);
	i = 0;
	while (i < data->args.nb_coders)
	{
		pthread_join(data->coders[i].thread_id, NULL);
		i++;
	}
}
