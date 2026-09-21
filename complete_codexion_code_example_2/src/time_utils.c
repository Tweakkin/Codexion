/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 17:43:18 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 13:31:26 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_current_time_ms(void)
{
	struct timeval	tv;
	long long		time_in_ms;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	time_in_ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (time_in_ms);
}

void	ft_usleep(long long wait_time, t_data *data)
{
	long long	start_time;

	start_time = get_current_time_ms();
	while ((get_current_time_ms() - start_time) < wait_time)
	{
		if (!is_simulation_running(data))
			break ;
		usleep(500);
	}
}
