/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>
# include <string.h>

typedef struct s_config
{
	int		coders_num;
	long	burnout_time;
	long	compile_time;
	long	debug_time;
	long	refac_time;
	int		compiles_number;
	long	dongle_cooldown;
	char	*scheduler;
}	t_config;

/* Represents a single request from a coder to compile */
typedef struct s_request
{
	int		coder_id;
	long	priority_value;
	int		compiles_done;
}	t_request;

/* A tiny queue attached to EACH dongle */
typedef struct s_queue
{
	t_request	requests[2];
	int			size;
}	t_queue;

/* Represents a shared USB dongle on the table */
typedef struct s_dongle
{
	int				id;
	int				is_available;
	long			last_released_time;
	t_queue			wait_queue;
	pthread_mutex_t	mutex;
}	t_dongle;

/* Represents a coder sitting at the table */
typedef struct s_coder
{
	int					id;
	int					compile_count;
	long				last_compile_start;
	pthread_t			thread;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	struct s_simulation	*simulation;
}	t_coder;

/* The Master Struct */
typedef struct s_simulation
{
	int				sim_running;
	t_config		config;
	t_coder			*coders_array;
	t_dongle		*dongles_array;
	long			start_time;
	unsigned long	next_ticket_number;
	pthread_t		monitor_thread;
	int				monitor_created;
	int				coder_threads_created;
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	log_mutex;
	pthread_cond_t	table_cond;
	pthread_mutex_t	sim_start_lock;
	pthread_cond_t	sim_start_cond;
}	t_simulation;

int		parse_args(int argc, char **argv, t_config *config);
long	get_time_ms(void);
void	sleep_ms(long milliseconds, t_simulation *sim);
int		init_simulation(t_simulation *sim, t_config *config);
void	*routine(void *arg);
int		start_threads(t_simulation *sim, t_config *config);
int		join_threads(t_simulation *sim, int count);
void	log_action(t_coder *coder, char *message);
void	*monitor_routine(void *arg);
void	free_simulation(t_simulation *sim, int count);
int		is_simulation_running(t_simulation *sim);
void	wait_for_start_signal(t_simulation *sim);
void	push_request(t_queue *queue, t_request req);
int		can_take_dongles(t_coder *coder);
void	pop_request(t_queue *queue);
void	drop_dongles(t_coder *coder);
int		take_dongles(t_coder *coder);

#endif
