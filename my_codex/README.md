*This project has been created as part of the 42 curriculum by yboukhmi.*

## Description
Codexion is a concurrency and multi-threading project. It simulates a group of coders sitting at a table, sharing USB dongles to complete their daily routine: compiling, debugging, and refactoring. The goal of the project is to manage threads safely without deadlocks or data races, ensuring that no coder "burns out" (starves) while waiting for resources.

## Instructions
To compile the project, use the provided Makefile at the root of the repository:

```bash
# Compile the project
make

# Clean object files
make clean

# Clean everything including the executable
make fclean

# Recompile from scratch
make re
```

To run the program, use the following format:
```bash
./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler]
```
*Note: The scheduler argument must be either `fifo` or `edf`.*

## Resources
- **Documentation & Tutorials**: 
  - UNIX pthread documentation (`man pthread_create`, `man pthread_mutex_init`, etc.)
  - General guides on multi-threading and mutexes in C.
- **AI Usage**: AI was used primarily as a source of informations.

## Blocking cases handled
The project handles several concurrency issues to prevent crashes and deadlocks:
- **Data Races**: Handled by protecting shared variables (like the simulation state, print logs, and dongle statuses) with mutexes.
- **Deadlocks**: Handled by implementing a strict queue/request system and ensuring that coders only grab dongles when both are available and it is their turn.
- **Starvation**: Prevented by utilizing a priority queue (FIFO or EDF scheduler) so every coder gets a fair chance to acquire dongles before they burn out.
- **Output mixing**: Handled by locking a specific log mutex before printing to standard output, ensuring messages don't overlap.

## Thread synchronization mechanisms

- **Mutexes (`pthread_mutex_t`)** protect shared data. The state mutex lets coders update their compile times and counters while the monitor reads them safely. Each dongle has a mutex, and the log mutex protects printed messages.
- **Condition variables (`pthread_cond_t`)** let coders sleep while waiting for dongles. When dongles are released or the simulation stops, waiting coders are woken up to check the shared state again. A separate condition variable makes all threads wait for the simulation's start signal.
