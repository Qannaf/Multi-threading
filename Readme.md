# Multi-threading
## Table of Contents
1. [Multi-threading libraries c++](#1)  
1. [C++ Standard Library Thread >= C++11] (#2)
	1. [Qu'est-ce qu'un thread ?] (#21)
	1. [L'API thread] (#22)
	1. [Synchronisation : le mutex] (#23)
1. [C pthread ] (#3)
	1. [Premier thread](#31)
	1. [Modification d'une variable](#32)
	1. [pthread_mutex_t](#33)
	1. [pthread_cond_t](#34)
	1. [return a value from pthread threads in C](#35)
1. [QThread] (#4)
	1. [QThread ](#41)
	1. [QThread "HighestPriority, NormalPriority and LowestPriority"](#42)
	1. [QThread with QMutex](#43)

<a name="1"></a>
# Multi-threading libraries c++
*  Rogue Wave SourcePro Threads Module
*  Boost.Thread
*  C++ Standard Library Thread
*  Dlib
*  OpenMP
*  OpenThreads
*  Parallel Patterns Library
*  POCO C++ Libraries Threading
*  POSIX Threads
*  Qt QThread
*  Stapl
*  TBB
*  IPP

<a name="2"></a>
# C++ Standard Library Thread >= C++11
<a name="21"></a>
## 1. Qu'est-ce qu'un thread ?
Les threads ou processus légers sont utilisés pour paralléliser l'exécution dans un programme. Ils sont dits légers car ils s'exécutent dans le même contexte que le processus d'exécution principal qui les crée et consomment donc moins de mémoire / CPU.

<a name="22"></a>
## 2. L'API thread
Les thread et mutex ont maintenant une API disponible dans la std : std::thread et std::mutex. Leurs documentations sont disponibles en ligne 
aux adresses respectives suivantes http://www.cplusplus.com/reference/thread/thread/ et http://www.cplusplus.com/reference/mutex/

* thread std::thread 
possède un constructeur par défaut, surtout utile pour pouvoir gérer ses threads dans des collections 
comme std::vector, mais surtout un constructeur qui prend une fonction et des arguments en paramètres et lancera un thread l'exécutant. 
On pourra bien sûr l'utiliser avec une lambda également. 
* join
Dans l'exemple ci-dessus, vous avez pu constater l'appel à une méthode join. Cette fonction est bloquante jusqu'à ce que le thread ait terminé
son exécution, dans notre cas jusqu'à ce que la boucle et l'affichage de chaque valeur ait été faite. Quand la fonction d'un thread est terminée, 
le thread est automatiquement terminé.
```CPP
#include <thread>
#include <mutex>
#include <iostream>
void display(int start, int nb)
{
	for (int i = start; i < start + nb; ++i)
		std::cout << i << ",";
}
int main(){
	std::thread t1(display, 0, 5);
	std::thread t2([]() { display(5, 5); });
	t1.join();
	t2.join();
	return 0 ;
}
```
![alt text](images/1.PNG?raw=true "sortie de code")

* lambda fonction
```CPP
#include <thread>
#include <iostream>

void printFunction(const int& nbr)
{
	for (int i = 0; i < 10; ++i)
		std::cout << (i * 3) + nbr << " ";
}

int main() {

	std::thread threadFunction (printFunction,1);

	std::thread threadFunctionLambda([]() {		for (int i = 0; i < 10; ++i)
													std::cout << (i * 3) + 1 << " ";
										   });

	threadFunction.join();
	threadFunctionLambda.join();

	return 0;
}
```
![alt text](images/1a.PNG?raw=true "sortie de code")
<a name="23"></a>
## 3. Synchronisation : le mutex
Avec le multi-threading vient les soucis de synchronisation, plus communément appelés « race-condition ». En effet, chaque thread est exécuté en parallèle, 
mais nous n'avons aucune assurance de l'ordre des opérations de chaque thread.
Pour en prendre conscience, essayez le code suivant :

```CPP
#include <thread>
#include <iostream>
int main() { 
	std::thread t1([]() {
		for (int i = 0; i < 10; ++i)
		{
			std::cout << (i * 3) << " ";
		}
	});
	std::thread t2([]() {
		for (int i = 0; i < 10; ++i)
		{
			std::cout << (i * 3) + 1 << " ";
		}
	});
	std::thread t3([]() {
		for (int i = 0; i < 10; ++i)
		{
			std::cout << (i * 3) + 2 << " ";
		}
	});
	t1.join();
	t2.join();
	t3.join();
	return 0;
}
```
![alt text](images/2.PNG?raw=true "sortie de code")

![alt text](images/2a.PNG?raw=true "sortie de code")

L'accès à la console via std::cout est concurrentiel entre les threads. On ne maîtrise pas quand chacun y accède et y écrit, d'où les différents résultats ci-dessus. 
Dans certains cas, un accès concurrent n'est pas gênant, ici écrire dans la console dans un ordre à priori aléatoire, en tout cas non maîtrisé, ne dérange pas le programme. 
Mais dans certains cas, il peut s'agir d'une ressource critique qui nécessitera alors que les threads l'utilisant soient synchronisés, d'une ressource nécessitant d'être utilisée 
par un unique thread à la fois.

Pour synchroniser nos applications, nous pouvons avoir recours à un `mutex`. 
Les mutex sont maintenant disponibles dans la std avec 
```CPP
std::mutex 
std::recursive_mutex 
std::timed_mutex  
std::recursive_timed_mutex
```

De manière générale, il est préférable de construire son application de manière à ce que le code soit synchronisé par construction. 
En effet, un mutex est un point de contention, utilisé pour que deux tâches ne puissent s'exécuter en parallèle. On verrouillera un mutex pour l'acquérir, 
les appels successifs ne pourront alors pas le verrouiller et devront attendre qu'il soit libéré. Il s'agit bel et bien d'une attente, donc de bloquer le thread en question.

*  std::mutex
std::mutex est le plus simple d'entre eux. Il peut être verrouillé une seule et unique fois, chaque appel à lock supplémentaire attendra que le mutex soit déverrouillé via un appel à unlock avant de retourner. Il existe également la méthode try_lock qui permet, si le mutex est déjà verrouillé, et un appel à lock serait alors bloquant de retourner false directement. Si le mutex peut être acquis, il est verrouillé et try_lock retourne true.

On pourra ainsi modifier le premier code comme ceci :

```CPP
// avec fonction lambda
#include <thread>
#include <mutex>
#include <iostream>
int main() {
	std::mutex lock; 
	std::thread t1([&lock]() {
		lock.lock();
		std::cout << "Dans le thread t1" << std::endl;
		for (int i = 0; i < 10; ++i)
		{
			std::cout << (i * 3) << " ";
		}
		std::cout << std::endl;
		lock.unlock();
	});
	std::thread t2([&lock]() {
		lock.lock();
		std::cout << "Dans le thread t2" << std::endl;
		for (int i = 0; i < 10; ++i)
		{
			std::cout << (i * 3) + 1 << " ";
		}
		std::cout << std::endl;
		lock.unlock();
	});
	std::thread t3([&lock]() {
		lock.lock();
		std::cout << "Dans le thread t3" << std::endl;
		for (int i = 0; i < 10; ++i)
		{
			std::cout << (i * 3) + 2 << " ";
		}
		std::cout << std::endl;
		lock.unlock();
	});
	t1.join();
	t2.join();
	t3.join();
	return 0;
}
```
```CPP
#include <thread>
#include <mutex>
#include <iostream>
#include <string>

// pour éveter d'utiliser des variables globales :)
namespace mutex
{
	std::mutex lock;
}

void f1( const int& nbr, const std::string& t)
{
	mutex::lock.lock();

	std::cout << "Dans le thread "<<t << "\n";
	for (int i = 0; i < 10; ++i)
		std::cout << (i * 3) + nbr << " ";
	std::cout <<"\n";

	mutex::lock.unlock();
}


int main() {
	

	std::thread t1(f1, 0, "t1");
	std::thread t2(f1, 1, "t2");
	std::thread t3(f1, 2, "t3");
	
	t1.join();
	t2.join();
	t3.join();
	
	return 0;
}
```
![alt text](images/3.PNG?raw=true "sortie de code")


* std::lock_guard
Pour ce dernier cas, et pour une utilisation vraiment pratique et simplifiée des mutex, il existe un petit objet nommé 
```CPP
std::lock_guard.
```

Cet objet prend un mutex en paramètre et le verrouille à sa création, puis se charge de le libérer à sa destruction. Il est ainsi très simple de limiter un verrou à un bloc de code, et en particulier en cas d'exception, early-return ou toute sortie prématurée du bloc, le verrou est également libéré automatiquement.
```CPP
#include <thread>
#include <mutex>
#include <iostream>
#include <string>

// pour éveter d'utiliser des variables globales :)
namespace mutex
{
	std::mutex lock;
}

void f1(const int& nbr, const std::string& t, const int& size = 10 )
{
	const std::lock_guard<std::mutex> lock(mutex::lock);

	std::cout << "Dans le thread " << t << "\n";
	for (int i = 0; i < size; ++i)
		std::cout << (i * 3) + nbr << " ";
	std::cout << "\n";


}


int main() {


	std::thread t1([]() {	f1(0, "t1");	 });
	std::thread t2([]() {	f1(1, "t2", 10); });
	std::thread t3([]() {	f1(2, "t3", 10); });

	t1.join();
	t2.join();
	t3.join();

	return 0;
}
```
![alt text](images/3.PNG?raw=true "sortie de code")


<a name="3"></a>
# C pthread 
* Pré-requis
Pour pouvoir créer des threads il faut utiliser la librairie pthread (POSIX thread):
dans mon cas je l'ai mis en C:/Program Files/pthread/Pre-built.2  et j'ai utilisé CMakeLists.txt pour l'ajouter en utulisant CMake pour générer mon projet, voila le structure de mon programme:

![alt text](images/4.PNG?raw=true "sortie de code")

<a name="31"></a>
## Premier thread
Dans ce premier exemple, nous allons créer un thread qui affiche une message:
```CPP
cmake_minimum_required(VERSION 2.8.12)

project(test)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#+------------------------------
# Add the headers(/*.h)
set(hdr "include/")
include_directories(${hdr})

#+------------------------------
# Add the sources (/*.cpp)
file(GLOB USER_CPP "src/*.cpp"  )

<a name="3"></a>
# pthread
set(PTHREAD_DIR "C:/Program Files/pthread/Pre-built.2" CACHE PATH "Location of libraries" FORCE)
set(PTHREAD_LIBS "pthreadVC2.lib")
set(PTHREAD_LIBS_DYN "pthreadVC2.dll")

include_directories(${PTHREAD_DIR}/include)
link_directories(${PTHREAD_DIR}/lib/x64)
link_directories(${PTHREAD_DIR}/dll/x64)

add_executable(${PROJECT_NAME} ${USER_CPP})

target_link_libraries (${PROJECT_NAME} ${PTHREAD_LIBS} ${PTHREAD_LIBS_DYN})
						



set( PCL_LIB_DLL_DEBUG "${PTHREAD_DIR}/dll/x64/pthreadVC2.dll"  )
	message("Files .dll are ${PCL_LIB_DLL_DEBUG}" )	

	file(COPY ${PCL_LIB_DLL_DEBUG}
		 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	message("Done coping .dll files")
```
Pthread.cpp
```CPP
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
	//pthread_join(thread1, NULL);
	printf("Apres la creation du thread.\n");

	system("pause");
	return EXIT_SUCCESS;
}
```
Dans cet exemple, nous utilisons la fonction suivante:
```CPP
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
```
Lorsque l'on exécute cet exemple, on a le résultat suivant :

![alt text](images/5.PNG?raw=true "sortie de code")

On ne voit pas le message du thread e la fin de son exécution. Pour attendre la fin, il faut utiliser la fonction suivante :
```CPP
int pthread_join(pthread_t thread, void **retval);
```
Ajoutez la ligne suivante :
```CPP
pthread_join(thread1, NULL);
```
après l'appel de la fonction pthread_create pour avoir le résultat suivant:

![alt text](images/6.PNG?raw=true "sortie de code")



<a name="32"></a>
## Modification d'une variable
Dans cet exemple nous allons incrémenter dans le thread un entier qui est déclaré dans le processus principal:
```CPP
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
```

Cela donne le résultat suivant :

![alt text](images/7.PNG?raw=true "sortie de code")

On peut remarquer que:
* dans le processus principal, on passe une référence vers la variable i (&i) dans la fonction pthread_create;
* dans le thread on récupère le pointeur vers cette variable et on la déclare comme étant un entier :
```CPP
int *i = (int *) arg;
```
on incrémente la valeur et non l'adresse pointée :	```(*i)++;```


<a name="33"></a>
## pthread_mutex_t
```CPP
#include <stdio.h>
#include <stdlib.h>
#define HAVE_STRUCT_TIMESPEC  
#include <pthread.h>
#include<iostream>
#include<string.h>

pthread_mutex_t lock;

void* doSomeThing(void* arg)
{
    pthread_mutex_lock(&lock);

    unsigned long i = 0;
    int* counter = static_cast<int*>(arg);
    (*counter) ++;

    std::cout<<"\n debut du threadi = "<<*counter<<"\n";

    for (i = 0; i < (0xFFFFFFFF); i++);

    std::cout << "\n fin du threadi = " << *counter << "\n";

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
    pthread_t t1,t2;
    int i(0);
    
    
    chakError(pthread_mutex_init(&lock, NULL));
        
    // Création du thread
    chakError(pthread_create(&t1, NULL, &doSomeThing, &i));
    chakError(pthread_create(&t2, NULL, &doSomeThing, &i));
        

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);


    pthread_mutex_destroy(&lock);

    system("pause");
    return EXIT_SUCCESS;
}
```

![alt text](images/9.PNG?raw=true "sortie de code")


<a name="34"></a>
## pthread_cond_t
```CPP
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
```

![alt text](images/10.PNG?raw=true "sortie de code")

<a name="35"></a>
## return a value from pthread threads in C
```cpp
#include <stdio.h>
#include <stdlib.h>
#define HAVE_STRUCT_TIMESPEC  
#include <pthread.h>
#include<iostream>
#include<string.h>



typedef struct thread_data {
	int a;
	int b;
	int result;

} thread_data;

void* myThread(void* arg)
{
	thread_data* tdata = (thread_data*)arg;

	int a = tdata->a;
	int b = tdata->b;
	int result = a + b;

	tdata->result = result;
	pthread_exit(NULL);
	return 0;
}

int main()
{
	pthread_t tid;
	thread_data tdata;

	tdata.a = 10;
	tdata.b = 32;

	pthread_create(&tid, NULL, myThread, (void*)&tdata);
	pthread_join(tid, NULL);

	printf("%d + %d = %d\n", tdata.a, tdata.b, tdata.result);
	system("pause");
	return 0;
}
```

![alt text](images/11.PNG?raw=true "sortie de code")
<a name="4"></a>
# QThread 

<a name="41"></a>
* QThread
![alt text](images/28.PNG?raw=true "sortie de code")

<a name="42"></a>
* QThread "HighestPriority, NormalPriority and LowestPriority"
![alt text](images/29.PNG?raw=true "sortie de code")

<a name="43"></a>
* QThread with QMutex
![alt text](images/30.PNG?raw=true "sortie de code")