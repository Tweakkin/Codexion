/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>

static int	is_number(char *s)
{
	int	i;
	int	value;
	int	digit;

	if (!s || !s[0])
		return (0);
	i = 0;
	value = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		digit = s[i] - '0';
		if (value > (INT_MAX - digit) / 10)
			return (0);
		value = value * 10 + digit;
		i++;
	}
	return (1);
}

static int	is_args_valid(int argc, char **argv)
{
	if (argc != 9 || !is_number(argv[1]) || !is_number(argv[2])
		|| !is_number(argv[3]) || !is_number(argv[4])
		|| !is_number(argv[5]) || !is_number(argv[6])
		|| !is_number(argv[7]))
		return (0);
	if (strcmp(argv[8], "fifo") != 0
		&& strcmp(argv[8], "edf") != 0)
		return (0);
	return (1);
}

static int	is_config_valid(t_config *config)
{
	if (config->coders_num <= 0 || config->burnout_time < 0
		|| config->compile_time < 0 || config->debug_time < 0
		|| config->refac_time < 0
		|| config->compiles_number < 0
		|| config->dongle_cooldown < 0)
		return (0);
	return (1);
}

static void	print_error(void)
{
	fprintf(stderr, "Error: invalid arguments\n");
}

int	parse_args(int argc, char **argv, t_config *config)
{
	if (!is_args_valid(argc, argv))
	{
		print_error();
		return (1);
	}
	config->coders_num = atoi(argv[1]);
	config->burnout_time = atoi(argv[2]);
	config->compile_time = atoi(argv[3]);
	config->debug_time = atoi(argv[4]);
	config->refac_time = atoi(argv[5]);
	config->compiles_number = atoi(argv[6]);
	config->dongle_cooldown = atoi(argv[7]);
	config->scheduler = argv[8];
	if (!is_config_valid(config))
	{
		print_error();
		return (1);
	}
	return (0);
}
