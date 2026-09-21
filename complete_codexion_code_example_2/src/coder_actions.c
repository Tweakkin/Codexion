/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 18:43:33 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 13:33:59 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_action(t_coder *coder, char *action)
{
	long long	time;

	pthread_mutex_lock(&coder->data->print_lock);
	if (is_simulation_running(coder->data))
	{
		time = get_current_time_ms() - coder->data->sim_start_time;
		printf("%lld %d %s\n", time, coder->id, action);
	}
	pthread_mutex_unlock(&coder->data->print_lock);
}

void	do_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->data->state_lock);
	coder->last_compiled_time = get_current_time_ms();
	pthread_mutex_unlock(&coder->data->state_lock);
	print_action(coder, "is compiling");
	ft_usleep(coder->data->args.compile_time, coder->data);
	pthread_mutex_lock(&coder->data->state_lock);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->data->state_lock);
}

void	do_debug(t_coder *coder)
{
	print_action(coder, "is debugging");
	ft_usleep(coder->data->args.debug_time, coder->data);
}

void	do_refactor(t_coder *coder)
{
	print_action(coder, "is refactoring");
	ft_usleep(coder->data->args.refactor_time, coder->data);
}
