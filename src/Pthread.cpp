#include <stdio.h>
#include <stdlib.h>
#define HAVE_STRUCT_TIMESPEC  
#include <pthread.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN  //!include winsock2.h  before windows.h, or define WIN32_LEAN_AND_MEAN before including windows.h
#include <winsock2.h>   
#include <windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd

#include<chrono>
#include<thread>

// Nombre total de thread
#define NB_THREAD 2

// Limite de l'incrément
#define INCREMENT_LIMIT 10

// Tableau contenant les threads
pthread_t threads[NB_THREAD];

// Structure de données contenant le mutex
typedef struct mutex_data {
	int data;
	pthread_mutex_t mutex;
} mutex_data;

// Fonction exécutée dans le thread
void* job(void* arg) {
	mutex_data* md = (mutex_data*)arg;
	pthread_t tid = pthread_self();
	while ((*md).data < INCREMENT_LIMIT) {
		// Vérouillage du mutex
		pthread_mutex_lock(&(*md).mutex);
		(*md).data++;
		// Dévérouillage du mutex
		pthread_mutex_unlock(&(*md).mutex);
		printf("thread [ %ld ] data [ %i ]\n", tid, (*md).data);
		// Pause l'exécution du thread pendant 1 seconde
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	printf("Fin du thread %ld\n", tid);
	pthread_exit(NULL);

	return EXIT_SUCCESS;
}

// Fonction principale
int main() {
	// Création du mutex
	mutex_data md;

	// Initialisation de la donnée
	md.data = 0;
	// Initialisation du mutex
	if (pthread_mutex_init(&md.mutex, NULL) != 0) {
		printf("\n mutex init failed\n");
		return EXIT_FAILURE;
	}
	// Boucle de création des threads
	for (int i = 0; i < NB_THREAD; i++) {
		// Création du thread et passage de la structure par référence
		int err = pthread_create(&threads[i], NULL, job, &md);
		if (err != 0) {
			printf("Echec de la creation du thread: [%s]", strerror(err));
			break;
		}
		printf("Creation du thread numero %ld\n", threads[i]);
	}
	// En attente des threads
	for (int i = 0; i < NB_THREAD; i++) {
		pthread_join(threads[i], NULL);
	}
	// Destruction du mutex
	pthread_mutex_destroy(&md.mutex);

	system("pause");
	return EXIT_SUCCESS;
}