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
} t_config;

/* Represents a single request from a coder to compile */
typedef struct s_request
{
	int				coder_id;		// Who is asking for the dongles
	long			priority_value; // The deadline (EDF) or ticket number (FIFO)
	int				compiles_done;	// Used for tie-breaking if priorities are identical
}	t_request;

/* A tiny queue attached to EACH dongle (max 2 people can wait for one dongle) */
typedef struct s_queue
{
	t_request		requests[2];	// The waiting line (only ever holds up to 2 requests)
	int				size;			// How many coders are currently waiting (0, 1, or 2)
}	t_queue;

/* Represents a shared USB dongle on the table */
typedef struct s_dongle
{
	int				id;				// Dongle ID
	int				is_available;	// 1 if on table, 0 if held by a coder
	long			available_at;	// Timestamp when cooldown finishes
	t_queue			wait_queue;		// The personal waiting line for THIS specific dongle
	pthread_mutex_t	mutex;			// Lock to protect physical access to this dongle
}	t_dongle;

/* Represents a coder sitting at the table */
typedef struct s_coder
{
	int					id;					// Coder ID
	int					compile_count;		// How many times they have successfully compiled
	long				last_compile_start; // Timestamp of last compile (for burnout check)
	pthread_t			thread;				// The actual thread handle
	t_dongle			*left_dongle;		// Pointer to the shared dongle on their left
	t_dongle			*right_dongle;		// Pointer to the shared dongle on their right
	struct s_simulation	*simulation;		// Pointer to the master simulation struct
}	t_coder;

/* The Master Struct: Holds everything together to avoid global variables */
typedef struct s_simulation
{
	int				finished;				// Flag (1) if someone burned out or everyone is done
	t_config		config;					// The parsed command-line arguments
	t_coder			*coders_array;			// Array of all coders
	t_dongle		*dongles_array;			// Array of all dongles
	
	long			start_time;				// When the simulation began
	unsigned long	next_ticket_number;		// Global counter for FIFO (who arrived first)
	
	pthread_t		monitor_thread;			// Thread that checks for burnout/completion
	int				monitor_created;		// 1 if monitor successfully started
	int				coder_threads_created;	// How many coder threads successfully started
	
	pthread_mutex_t	state_mutex;			// Global lock for modifying queues, checking finishes, etc.
	pthread_mutex_t	log_mutex;				// Global lock so printfs don't overlap
	pthread_cond_t	table_cond;				// Where coders sleep until it's their turn for dongles
}	t_simulation;

int parse_args(int argc, char **argv, t_config *config);
long get_time_ms(void);
void sleep_ms(long milliseconds);
int init_simulation(t_simulation *sim, t_config *config);
void    *routine(void *arg);
int start_threads(t_simulation *sim, t_config *config);
void log_action(t_coder *coder, char *message);

#endif
