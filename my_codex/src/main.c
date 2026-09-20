#include "codexion.h"


int main(int argc, char **argv)
{
    t_config config;

    if (parse_args(argc, argv, &config) != 0)
		return (1);
    printf("ALL GOOD");


    return (0);
}