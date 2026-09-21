/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 18:08:24 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 13:34:14 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wait_for_start_signal(t_data *data)
{
	pthread_mutex_lock(&data->sim_start_lock);
	while (data->sim_start_time == 0)
	{
		pthread_cond_wait(&data->sim_start_cond, &data->sim_start_lock);
	}
	pthread_mutex_unlock(&data->sim_start_lock);
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;

	coder = (t_coder *)arg;
	wait_for_start_signal(coder->data);
	if (coder->id % 2 == 0)
		ft_usleep(5, coder->data);
	while (is_simulation_running(coder->data))
	{
		if (take_dongles(coder))
		{
			do_compile(coder);
			drop_dongles(coder);
			if (coder->compiles_done == coder->data->args.compiles_req)
				break ;
			if (!is_simulation_running(coder->data))
				break ;
			do_debug(coder);
			if (!is_simulation_running(coder->data))
				break ;
			do_refactor(coder);
		}
		if (coder->compiles_done == coder->data->args.compiles_req)
			break ;
	}
	return (NULL);
}
