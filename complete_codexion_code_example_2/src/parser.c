/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 17:08:30 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 15:54:49 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	is_valid_number(char *arg)
{
	int		i;

	i = 0;
	while (arg[i])
	{
		if (!(arg[i] >= '0' && arg[i] <= '9'))
			return (false);
		else
			i++;
	}
	return (true);
}

bool	set_scheduler(char *str, t_scheduler *policy)
{
	if (strcmp(str, "edf") == 0)
		*policy = EDF;
	else if (strcmp(str, "fifo") == 0)
		*policy = FIFO;
	else
		return (false);
	return (true);
}

bool	parse_args(int argc, char **argv, t_sim_args *args)
{
	int			i;

	if (argc != 9)
		return (false);
	i = 1;
	while (i < 8)
	{
		if (!is_valid_number(argv[i]))
			return (false);
		i++;
	}
	if (!set_scheduler(argv[8], &args->scheduler))
		return (false);
	args->nb_coders = atoi(argv[1]);
	args->burnout_time = atoi(argv[2]);
	args->compile_time = atoi(argv[3]);
	args->debug_time = atoi(argv[4]);
	args->refactor_time = atoi(argv[5]);
	args->compiles_req = atoi(argv[6]);
	args->cooldown_time = atoi(argv[7]);
	return (true);
}
