/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 17:10:23 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 15:55:44 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_data		data;

	memset(&data, 0, sizeof(t_data));
	if (!parse_args(argc, argv, &data.args))
	{
		printf("Error: Wrong arguments!\n");
		return (1);
	}
	if (data.args.burnout_time == 0 || data.args.compiles_req == 0)
		return (false);
	if (!init_data(&data))
	{
		printf("Error: Initialization error!");
		return (1);
	}
	start_simulation(&data);
	stop_simulation(&data);
	free_all(&data);
	return (0);
}
