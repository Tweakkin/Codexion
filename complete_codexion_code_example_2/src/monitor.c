/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 18:48:41 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 13:35:03 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	check_death(t_data *data)
{
	int			i;
	long long	time_now;

	i = 0;
	while (i < data->args.nb_coders)
	{
		time_now = get_current_time_ms();
		pthread_mutex_lock(&data->state_lock);
		if ((time_now - data->coders[i].last_compiled_time)
			>= data->args.burnout_time)
		{
			data->sim_running = false;
			pthread_cond_broadcast(&data->table_cond);
			pthread_mutex_unlock(&data->state_lock);
			pthread_mutex_lock(&data->print_lock);
			printf("%lld %d burned out\n", time_now - data->sim_start_time,
				data->coders[i].id);
			pthread_mutex_unlock(&data->print_lock);
			return (true);
		}
		pthread_mutex_unlock(&data->state_lock);
		i++;
	}
	return (false);
}

static bool	check_success(t_data *data)
{
	int		i;
	int		finished;

	i = 0;
	finished = 0;
	while (i < data->args.nb_coders)
	{
		pthread_mutex_lock(&data->state_lock);
		if (data->coders[i].compiles_done >= data->args.compiles_req)
			finished++;
		pthread_mutex_unlock(&data->state_lock);
		i++;
	}
	if (finished == data->args.nb_coders)
	{
		pthread_mutex_lock(&data->state_lock);
		data->sim_running = false;
		pthread_cond_broadcast(&data->table_cond);
		pthread_mutex_unlock(&data->state_lock);
		return (true);
	}
	return (false);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	pthread_mutex_lock(&data->sim_start_lock);
	while (data->sim_start_time == 0)
	{
		pthread_cond_wait(&data->sim_start_cond, &data->sim_start_lock);
	}
	pthread_mutex_unlock(&data->sim_start_lock);
	while (is_simulation_running(data))
	{
		if (check_death(data) || check_success(data))
			break ;
		usleep(1000);
	}
	return (NULL);
}
