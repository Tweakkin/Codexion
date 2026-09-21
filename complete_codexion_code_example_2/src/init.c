/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 10:40:37 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 15:53:02 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	init_dongles(t_data *data)
{
	int		i;

	data->dongles = (t_dongle *)malloc(sizeof(t_dongle) * data->args.nb_coders);
	if (!data->dongles)
		return (false);
	i = 0;
	while (i < data->args.nb_coders)
	{
		data->dongles[i].id = i;
		data->dongles[i].is_available = true;
		data->dongles[i].last_released_time = 0;
		data->dongles[i].pqueue = (t_heapq *)malloc(sizeof(t_heapq));
		if (!data->dongles[i].pqueue)
			return (false);
		if (!pqueue_create(data->dongles[i].pqueue))
			return (false);
		pthread_mutex_init(&data->dongles[i].lock, NULL);
		i++;
	}
	return (true);
}

static bool	init_coders(t_data *data)
{
	int		i;

	data->coders = (t_coder *)malloc(sizeof(t_coder) * data->args.nb_coders);
	if (!data->coders)
		return (false);
	i = 0;
	while (i < data->args.nb_coders)
	{
		data->coders[i].id = i + 1;
		data->coders[i].last_compiled_time = 0;
		data->coders[i].compiles_done = 0;
		data->coders[i].data = data;
		data->coders[i].left_dongle = &data->dongles[i];
		data->coders[i].right_dongle = &data->dongles[(i + 1)
			% data->args.nb_coders];
		i++;
	}
	return (true);
}

bool	init_data(t_data *data)
{
	if (!init_dongles(data))
		return (false);
	if (!init_coders(data))
		return (false);
	data->sim_running = false;
	pthread_mutex_init(&data->print_lock, NULL);
	pthread_mutex_init(&data->sim_start_lock, NULL);
	pthread_cond_init(&data->sim_start_cond, NULL);
	pthread_mutex_init(&data->state_lock, NULL);
	return (true);
}

bool	free_all(t_data *data)
{
	int		i;

	i = 0;
	if (data->coders)
		free(data->coders);
	if (data->dongles)
	{
		while (i < data->args.nb_coders)
		{
			free(data->dongles[i].pqueue->requests);
			free(data->dongles[i].pqueue);
			pthread_mutex_destroy(&data->dongles[i].lock);
			i++;
		}
		free(data->dongles);
	}
	pthread_mutex_destroy(&data->print_lock);
	pthread_mutex_destroy(&data->sim_start_lock);
	pthread_cond_destroy(&data->sim_start_cond);
	pthread_mutex_destroy(&data->state_lock);
	return (true);
}
