#include <stdio.h>
#include <stdlib.h>
#define HAVE_STRUCT_TIMESPEC  
#include <pthread.h>

void* thread_1(void* arg) {
	int* i = static_cast<int*> ( arg);
	(*i) = 10;
	printf("Nous sommes dans le threadi = %d.\n", *i);
	
	// Arrêt propre du thread
	pthread_exit(EXIT_SUCCESS);

	return EXIT_SUCCESS;
}

int main(void) {
	int i(0);
;	// Création de la variable qui va contenir le thread
	pthread_t thread1;

	printf("Avant la creation du thread i = %d.\n",i);

	// Création du thread
	pthread_create(&thread1, NULL, thread_1, &i);
	pthread_join(thread1, NULL);

	printf("Apres la creation du threadi = %d.\n", i);

	system("pause");
	return EXIT_SUCCESS;
}