#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void *myturn(void *b)
{
    int *o = (int *)b;
    int i = 0;
    while (i < 10){
        sleep(1);
        printf("My turn!\n");
        i++;
        (*o)++;
    }
    return NULL;
}

void yourturn()
{
    int i = 0;
    while (i < 2){
        sleep(2);
        printf("Your turn!\n");
        i++;
    }
}

int main()
{
    pthread_t second_thread;
    int a = 1;

    pthread_create(&second_thread, NULL, myturn, &a);
    yourturn();
    pthread_join(second_thread, NULL);
    printf("WE DONE! a: %d", a);
    return 0;
}