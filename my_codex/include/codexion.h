#ifndef CODEXION_H
# define CODEXION_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

typedef struct  s_config{
    int coders_num;
    long burnout_time;
    long compile_time;
    long debug_time;
    long refac_time;
    int compiles_number;
    long dongle_cooldown;
    char *scheduler;
}t_config;

typedef struct s_coder
{
	int					id;
	int					compile_count;
	long				last_compile_start;
	pthread_t			thread;
	struct s_dongle		*left_dongle;
	struct s_dongle		*right_dongle;
	struct s_simulation	*simulation;
}	t_coder;

typedef struct s_dongle
{
	int				id;
	int				available;
	long			available_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
}	t_dongle;

typedef struct s_request
{
	t_coder			*coder;
	unsigned long	arrival_order;
	long			deadline;
}	t_request;

typedef struct s_heap
{
	t_request		**requests_arr;
	int				num_requests_waiting;
	int				max_requests;
}	t_heap;

typedef struct s_simulation
{
	int				finished;
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_mutex_t	state_mutex;
	pthread_t		monitor_thread;
	long			start_time;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	scheduler_cond;
	t_heap			scheduler_heap;
	unsigned long	next_arrival_order;
	int				coder_threads_created;
	int				monitor_created;
}	t_simulation;

int parse_args(int argc, char **argv, t_config *config);
long get_time_ms(void);
void sleep_ms(long milliseconds);

#endif
