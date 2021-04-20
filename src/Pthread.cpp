#include <stdio.h>
#include <stdlib.h>
#define HAVE_STRUCT_TIMESPEC  
#include <pthread.h>
#include<iostream>
#include<string.h>




pthread_mutex_t lock;
pthread_cond_t cv;

void* doSomeThing(void* arg)
{
    pthread_mutex_lock(&lock);

    

    unsigned long i = 0;
    int* counter = static_cast<int*>(arg);
    (*counter)++;

    std::cout << "\n debut du threadi = " << *counter << "\n";
    for (i = 0; i < (0xFFFFFFFF); i++);
    std::cout << "\n fin du threadi = " << *counter << "\n";


    pthread_cond_wait(&cv, &lock);
    printf("unlocked.\n");
    pthread_mutex_unlock(&lock);

    // Arrêt propre du thread
    pthread_exit(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

void chakError(const bool& err)
{
    if (err != EXIT_SUCCESS)
        std::cerr << "\ncan't create thread : " << strerror(err) << "\n";
}

int main(void)
{
    // Création de la variable qui va contenir le thread
    pthread_t t1, t2;
    int i(0);


    chakError(pthread_mutex_init(&lock, NULL));

    // Création du thread
    chakError(pthread_create(&t1, NULL, &doSomeThing, &i));
    chakError(pthread_create(&t2, NULL, &doSomeThing, &i));


    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_cond_signal(&cv);
    pthread_mutex_destroy(&lock);

    system("pause");
    return EXIT_SUCCESS;
}