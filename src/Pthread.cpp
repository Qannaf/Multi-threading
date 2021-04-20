#include <stdio.h>
#include <stdlib.h>
#define HAVE_STRUCT_TIMESPEC  
#include <pthread.h>

void* thread_1(void* arg) {
	printf("Nous sommes dans le thread.\n");
	
	// Arrêt propre du thread
	pthread_exit(EXIT_SUCCESS);

	return EXIT_SUCCESS;
}

int main(void) {
	// Création de la variable qui va contenir le thread
	pthread_t thread1;

	printf("Avant la creation du thread.\n");

	// Création du thread
	pthread_create(&thread1, NULL, thread_1, NULL);
	pthread_join(thread1, NULL);

	printf("Apres la creation du thread.\n");

	system("pause");
	return EXIT_SUCCESS;
}