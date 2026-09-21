*This project has been created as part of the 42 curriculum by abchahid.*

# Codexion

## Description
Codexion is a concurrency and resource management simulation where multiple coders share a circular workspace and compete for limited USB dongles to compile their code. The goal of the project is to safely orchestrate POSIX threads and mutexes to ensure coders can alternate between compiling, debugging, and refactoring without starving or causing system deadlocks. The simulation enforces a strict dongle cooldown period and utilizes a custom Priority Queue to grant resources fairly based on either a FIFO (First In, First Out) or EDF (Earliest Deadline First) scheduling policy. The program terminates when all coders have completed their required compilations, or when a coder starves and burns out.

## Instructions
**Compilation:**
To compile the project, a `Makefile` is provided at the root of the repository. Simply run the following command in your terminal:
`make`
This will compile the source files using `cc` with the mandatory flags `-Wall -Wextra -Werror -pthread` and generate the `codexion` executable.

**Execution:**
The program takes exactly 8 mandatory arguments. Run the simulation using the following format:
`./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler]`

*Example:*
`./codexion 5 800 200 200 200 7 60 fifo`

## Resources
During the development of this project, the following resources and tools were utilized:
*   **CodeVault - "Unix Threads in C" (YouTube):** An essential video series that visually breaks down POSIX threads. It was highly valuable for understanding the practical C implementation of thread creation, mutex locks, and condition variables.
*   **Artificial Intelligence (LLM):** AI was used strictly as a debugging assistant and conceptual sounding board. It helped decipher complex Helgrind lock-order violation reports, suggested strategies for mitigating data races during thread initialization, and assisted in refining the logic for the priority queue sorting algorithm.

## Blocking Cases Handled
This simulation successfully addresses several critical concurrency challenges:
*   **Deadlock Prevention (Coffman's Conditions):** Deadlocks are prevented by breaking the circular wait condition. When a coder acquires their physical dongles, a helper function (`lock_physical_dongles`) ensures they always lock the dongle with the lowest ID first, breaking the infinite lock cycle.
*   **Starvation Prevention & Scheduling:** A custom Min-Heap Priority Queue guarantees that resources are distributed fairly. In FIFO mode, dongles are granted based on arrival time. In EDF mode, dongles are granted to the coder closest to their burnout deadline, guaranteeing liveness.
*   **Dongle Cooldown:** The engine enforces a strict hardware cooldown. A condition variable checks the time elapsed since the `last_released_time` and forces threads to sleep if the cooldown period has not been met, preventing instant re-acquisition.
*   **Precise Burnout Detection:** A dedicated, detached monitor thread constantly evaluates the table. It is heavily optimized to detect a missed deadline and print the burnout log within the required 10ms window.
*   **Log Serialization:** A dedicated `print_lock` mutex wraps all `printf` calls, guaranteeing that state messages never interleave or mix up on the standard output.

## Thread Synchronization Mechanisms
To ensure safe memory access and coordination, the following synchronization primitives are heavily utilized:
*   **`pthread_mutex_t` (Mutexes):**
    *   `state_lock`: A global logical shield. It protects the shared logical table (e.g., updating timestamps, checking `is_available` booleans, and modifying queue sizes).
    *   `dongle[i].lock`: Physical locks that ensure only one thread can physically interact with a specific dongle at a time.
    *   `print_lock`: Ensures terminal outputs are atomic and serialized.
*   **`pthread_cond_t` (Condition Variables):**
    *   `table_cond`: Used to manage the waiting room. If a coder's required dongles are taken, or if they do not have priority, they call `pthread_cond_wait` to sleep. When dongles are dropped, `pthread_cond_broadcast` wakes all sleeping coders so they can re-evaluate the queue.
    *   `sim_start_cond`: Prevents initialization data races. All coder threads wait on this condition until the main thread has fully populated the starting timestamps, ensuring a perfectly synchronized starting line.