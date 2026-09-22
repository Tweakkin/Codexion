#include "codexion.h"


int main(int argc, char **argv)
{
    t_config config;
    t_simulation sim;

    if (parse_args(argc, argv, &config) != 0)
		return (1);
    
    if (init_simulation(&sim, &config) != 0)
    {
        printf("Failed to initialize simulation\n");
        return (1);
    }
    
    printf("ALL GOOD! Room is built.\n");
    printf("Coder 1's left dongle ID is: %d\n", sim.coders_array[0].left_dongle->id);
    printf("Coder 1's right dongle ID is: %d\n", sim.coders_array[0].right_dongle->id);

    sim.start_time = get_time_ms();
    start_threads(&sim, &config);
    // We should free memory here later, but this is just a quick test
    int i = 0;
    while (i < config.coders_num)
    {
        pthread_join(sim.coders_array[i].thread, NULL);
        i++;
    }
    return (0);
}