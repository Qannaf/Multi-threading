# Multi-threading
## Table of Contents
1. [Chapter 1](#Ch1)
	1. [Multi-threading libraries c++](#1)  
	1. [C++ Standard Library Thread >= C++11](#2)
		1. [Qu'est-ce qu'un thread ?](#21)
		1. [L'API thread](#22)
		1. [Synchronisation : le mutex](#23)
	1. [C pthread ](#3)
		1. [Premier thread](#31)
		1. [Modification d'une variable](#32)
		1. [pthread_mutex_t](#33)
		1. [pthread_cond_t](#34)
		1. [return a value from pthread threads in C](#35)
	1. [QThread](#4)
		1. [QThread ](#41)
		1. [QThread "HighestPriority, NormalPriority and LowestPriority"](#42)
		1. [QThread with QMutex](#43)

1.  [Chapter 2](#Ch2)
	1. [Creating a std::thread](#s1)
	1. [Passing a reference to a thread](#s2)
	1. [Using std::async instead of std::thread](#s3)
	1. [Basic Synchronization](#s4)
	1. [Ensuring a thread is always joined](#s6)
	1. [Operations on the current thread](#s7)
	1. [Using Condition Variables](#s8)
	1. [Thread operations](#s9)
	1. [Thread-local storage](#s10)
	1. [Reassigning thread objects](#11)


<a name="Ch1"></a>
<a name="1"></a>
# Multi-threading libraries c++
* Rogue Wave SourcePro Threads Module
* Boost.Thread
* C++ Standard Library Thread
* Dlib
* OpenMP
* OpenThreads
* Parallel Patterns Library
* POCO C++ Libraries Threading
* POSIX Threads
* Qt QThread
* Stapl
* TBB
* IPP

<a name="2"></a>
## C++ Standard Library Thread >= C++11
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

<br><br><br>

<a name="Ch2"></a>
# Chapter 2

<a name="s1"></a>
## 1: Creating a std::thread
When creating a thread, you need to pass something to be executed on it. A few things that
you can pass to a thread:

1. Free functions : example 
```CPP
#include <iostream>
#include <thread>
 
void foo(int a)
{
    std::cout << a << '\n';
}
 
int main()
{
    // Create and execute the thread
    std::thread thread{ foo, 10 }; // Pass 10 to free function
 
    // The free function will be executed in a separate thread
 
    // Wait for the thread to finish, this is a blocking operation
    thread.join();
 
    return 0;
}
```

2. Member functions : example

```CPP
#include <iostream>
#include <thread>
class Bar
{
public:
 void foo(int a)
 {
 std::cout << a << '\n';
 }
};


int main()
{
 Bar bar;
 
 // Create and execute the thread
 std::thread thread(&Bar::foo, &bar, 10); // Pass 10 to member function
 // The member function will be executed in a separate thread
 // Wait for the thread to finish, this is a blocking operation
 thread.join();
 return 0;
}
```
3. Functor objects : example

```CPP
#include <iostream>
#include <thread>
class Bar
{
public:
 void operator()(int a)
 {
 std::cout << a << '\n';
 }
};
int main()
{
 Bar bar;
 
 // Create and execute the thread
 std::thread thread(bar, 10); // Pass 10 to functor object
 // The functor object will be executed in a separate thread
 // Wait for the thread to finish, this is a blocking operation
 thread.join();
 return 0;
}
```

4. Lambda expressions :example
```CPP
#include <iostream>
#include <thread>
int main()
{
 auto lambda = [](int a) { std::cout << a << '\n'; };
 // Create and execute the thread
 std::thread th1{lambda, 10}; // Pass 10 to the lambda expression
 std::thread th2([](int a) { std::cout << a << '\n'; }, 10);
 std::thread th3([](int a=10) { std::cout << a << '\n'; });
  //std::thread th2([](int a=10) { std::cout << a << '\n'; });
 // The lambda expression will be executed in a separate thread
 // Wait for the thread to finish, this is a blocking operation
 th1.join();
 th2.join();
 th3.join();
 return 0;
}
```
<br><br>

<a name="s2"></a>
## 2: Passing a reference to a thread
1. std::ref : passed as reference _&_
2. std::cref: passed as const reference _const&_
<p>You cannot pass a reference (or const reference) directly to a thread because std::thread will copy/move them.
Instead, use std::reference_wrapper:

```CPP
void foo(int& b)
{
 b = 10;
}
int a = 1;
std::thread thread{ foo, std::ref(a) }; //'a' is now really passed as reference
thread.join();
std::cout << a << '\n'; //Outputs 10
void bar(const ComplexObject& co)
{
 co.doCalculations();
}
ComplexObject object;
std::thread thread{ bar, std::cref(object) }; //'object' is passed as const&
thread.join();
std::cout << object.getResult() << '\n'; //Outputs the result
```
<br><br>

<a name="s3"></a>
## 3: Using std::async instead of std::thread
std::async is also _able to make threads_. Compared to std::thread it is considered _less powerful_ but easier to use
when you just want to run a function asynchronously.
1. Asynchronously calling a function

```CPP
#include <future>
#include <iostream>
unsigned int square(unsigned int i){
 return i*i;
}
int main() {
 auto f = std::async(std::launch::async, square, 8);
 std::cout << "square currently running\n"; //do something while square is running
 std::cout << "result is " << f.get() << '\n'; //getting the result from square
}
```

2. Common Pitfall
* std::async returns a std::future that holds the return value that will be calculated by the function. When
that future gets destroyed it waits until the thread completes, making your code effectively single threaded.
This is easily overlooked when you don't need the return value:
std::async(std::launch::async, square, 5);
//thread already completed at this point, because the returning future got destroyed

* std::async works without a launch policy, so std::async(square, 5); compiles. When you do that the
system gets to decide if it wants to create a thread or not. The idea was that the system chooses to make a
thread unless it is already running more threads than it can run efficiently. Unfortunately implementations
commonly just choose not to create a thread in that situation, ever, so you need to override that behavior
with std::launch::async which forces the system to create a thread.
<br><br>

<a name="s4"></a>
## 4: Basic Synchronization
Thread synchronization can be accomplished using mutexes, among other synchronization primitives. There are
several mutex types provided by the standard library, but the simplest is std::mutex. To lock a mutex, you
construct a lock on it. The simplest lock type is std::lock_guard:
```CPP
std::mutex m;
void worker() {
 std::lock_guard<std::mutex> guard(m); // Acquires a lock on the mutex
 // Synchronized code here
} // the mutex is automatically released when guard goes out of scope
```
With std::lock_guard the mutex is locked for the whole lifetime of the lock object. In cases where you need to
manually control the regions for locking, use std::unique_lock instead:
```CPP
std::mutex m;
void worker() {
 // by default, constructing a unique_lock from a mutex will lock the mutex
 // by passing the std::defer_lock as a second argument, we
 // can construct the guard in an unlocked state instead and
 // manually lock later.
 std::unique_lock<std::mutex> guard(m, std::defer_lock);
 // the mutex is not locked yet!
 guard.lock();
 // critical section
 guard.unlock();
 // mutex is again released
}
```
<br><br>

<a name="s6"></a>
## 6: Ensuring a thread is always joined
When the destructor for std::thread is invoked, a call to either `join() or detach()` must have been made. If a
thread has not been joined or detached, then by default ``std::terminate`` will be called. Using RAII, this is generally
simple enough to accomplish:
```CPP
#include <future>
#include <iostream>
#include <functional>

class thread_joiner
{
private:
    std::thread t_;

public:
    thread_joiner(std::thread t): t_(std::move(t)){ }
    ~thread_joiner()
    {
        if(t_.joinable())  t_.join();
    }
};

unsigned int square(unsigned int i){return i*i;}

void perform_work(const int& x){ 
    std::cout<<"square("<<x<<") = "<<square(x)<<"\n";
}

void t(){ 
    int param(5);
    thread_joiner j{std::thread{perform_work,std::cref( param)}};
}
  


int main() {
    t();
    std::cout<<"Do some other calculations while thread is running\n";
    return 0;
}
```
<br><br>

<a name="s7"></a>
## 7: Operations on the current thread
std::this_thread is a namespace which has functions to do interesting things on the current thread from function
it is called from.
| Function    | Description |
| ------      | ------      |
| get_id      | Returns the id of the thread|
| sleep_for   | Sleeps for a specified amount of time|
| sleep_until     | Sleeps until a specific time|
| yield       | Reschedule running threads, giving other threads priority|

* Getting the current threads id using 
> std::this_thread::get_id:
```cpp
void foo()
{
 //Print this threads id
 std::cout << std::this_thread::get_id() << '\n';
}
std::thread thread{ foo };
thread.join(); //'threads' id has now been printed, should be something like 12556
foo(); //The id of the main thread is printed, should be something like 2420
```

* Sleeping for 3 seconds using 
>std::this_thread::sleep_for:

```CPP
void foo()
{
 std::this_thread::sleep_for(std::chrono::seconds(3));
}
std::thread thread{ foo };
foo.join();
std::cout << "Waited for 3 seconds!\n";
```

* Sleeping until 3 hours in the future using 
> std::this_thread::sleep_until:
```CPP
void foo()
{
 std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::hours(3));
}
std::thread thread{ foo };
thread.join();
std::cout << "We are now loc


ated 3 hours after the thread has been called\n";
```

* Letting other threads take priority using 
> std::this_thread::yield:
```CPP
void foo(int a)
{
 for (int i = 0; i < al; ++i)
 std::this_thread::yield(); //Now other threads take priority, because this thread
 //isn't doing anything important
 std::cout << "Hello World!\n";
}
std::thread thread{ foo, 10 };
thread.join();
```
<br><br>

<a name="s8"></a>
## 8: Using Condition Variables
A condition variable is a primitive used in conjunction with a mutex to orchestrate communication between
threads. While it is neither the exclusive or most efficient way to accomplish this, it can be among the simplest to
those familiar with the pattern.
One waits on a std::condition_variable with a std::unique_lock<std::mutex>. This allows the code to safely
examine shared state before deciding whether or not to proceed with acquisition.
Below is a producer-consumer sketch that uses std::thread, std::condition_variable, std::mutex, and a few
others to make things interesting.

```CPP
#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <vector>
#include <random>
#include <thread>
int main()
{
    std::condition_variable cond;
    std::mutex mtx;
    std::vector<int> vec;
    bool stopped = false;

    auto lambda1 = [&]()
        {
            std::cout<<"Launch thread producer\n";
            // Choose a random mean between 1 and 8
            std::default_random_engine val;
            std::uniform_int_distribution<int> dist(1, 6);
            

            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::size_t count = 5; 
            while(count--)
            {
                std::lock_guard<std::mutex> L{mtx};
                vec.push_back(dist(val));
                cond.notify_one();
            }

            {
                std::lock_guard<std::mutex> L{mtx};
                std::cout << "\n\t\tthread producer create vector\tdone!\n";
                for(auto v:vec)std::cout<<"\t"<<v<<"\t";std::cout<<"\n";
                stopped = true;
                cond.notify_one();
            }
        };

    auto lambda2 = [&]()
        {   
            std::cout<<"Launch thread consumer \n ";
            do
            {
            
                std::unique_lock<std::mutex> L{mtx};
                std::cerr << "\n.................... thread consumer Waiting... thread producer until create vector.......>\n";
                cond.wait(L,[&]() {return stopped || !vec.empty();});
                std::cerr << "...................... finished waiting. i == 1 ...........................................>\n\n";
                while( ! vec.empty())
                {
                    const auto val = vec.back();
                    vec.pop_back();
                    std::cout << "Consumer popped: " << val << std::endl;
                }
                if(stopped)
                {
                    std::cout << "Consumer is done!" << std::endl;break;
                }
                std::cout << "test3\n";
            }while(true);
        };



    std::thread producer{lambda1};
    std::thread consumer{lambda2};

    consumer.join();
    producer.join();

    std::cout << "Example Completed!" << std::endl;
    return 0;
}
```

```
Launch thread producer
Launch thread consumer

.................... thread consumer Waiting... thread producer until create vector.......>

                thread producer create vector   done!
        1               1               5               3               4
...................... finished waiting. i == 1 ...........................................>

Consumer popped: 4
Consumer popped: 3
Consumer popped: 5
Consumer popped: 1
Consumer popped: 1
Consumer is done!
Example Completed!
```

<br><br>
<a name="s9"></a>
## 9: Thread operations
When you start a thread, it will execute until it is finished.
Often, at some point, you need to (possibly - the thread may already be done) wait for the thread to finish, because
you want to use the result for example.
```CPP
int n;
std::thread thread{ calculateSomething, std::ref(n) };
//Doing some other stuff
//We need 'n' now!
//Wait for the thread to finish - if it is not already done
thread.join();
//Now 'n' has the result of the calculation done in the seperate thread
std::cout << n << '\n';
```

You can also detach the thread, letting it execute freely:

```CPP
std::thread thread{ doSomething };
//Detaching the thread, we don't need it anymore (for whatever reason)
thread.detach();
//The thread will terminate when it is done, or when the main thread return
```
<br><br>

<a name="s10"></a>
## 10: Thread-local storage
Thread-local storage can be created using the thread_local keyword. A variable declared with the thread_local
specifier is said to have thread storage duration.
- Each thread in a program has its own copy of each thread-local variable.
- A thread-local variable with function (local) scope will be initialized the first time control passes through its
definition. Such a variable is implicitly static, unless declared extern.
- A thread-local variable with namespace or class (non-local) scope will be initialized as part of thread startup.
- Thread-local variables are destroyed upon thread termination.
- A member of a class can only be thread-local if it is static. There will therefore be one copy of that variable
per thread, rather than one copy per (thread, instance) pair.
<p>Example:

```CPP
#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>
 
std::mutex cv_m; 
 
void t()
{   
    thread_local int count = 0;
    std::unique_lock<std::mutex> lk(cv_m);
    std::cerr<<"This function has been called " <<++count<< " times by this thread\n";    
}
 
int main()
{
    std::thread t1(t), t2(t), t3(t);
    t1.join(); 
    t2.join(); 
    t3.join();

    return 0;
}
```
```
This function has been called 1 times by this thread
This function has been called 1 times by this thread
This function has been called 1 times by this thread
```

<a name="s11"></a>
## 11: Reassigning thread objects
We can create empty thread objects and assign work to them later.
If we assign a thread object to another active, joinable thread, std::terminate will automatically be called before
the thread is replaced.
```CPP
#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>
 

std::mutex cv_m; 
 
void foo()
{   
     std::cout << "foo() " << '\n';
}
 
void t()
{   
     std::cout << "t() " << '\n';
} 
 
int main()
{

    std::thread v[5];
    v[0]=std::thread(foo);
    v[1]=std::thread(foo);
    v[2]=std::thread(foo);
   //v[3]=std::thread(foo);
    v[4]=std::thread(foo);

    bool test=true;
    for (std::size_t i = 0;i < 5;i++)
        if (!v[i].joinable())
        {
            v[i] = std::thread(t);
            test = false;
            v[i].join();
        }
        else
        {
            v[i].join();
        }

    if(test)std::cerr<<"\nThis object doesn't have a thread assigned, Please create  new thread";

    return 0;
}
```





## license
<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="creative commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">creative commons Attribution-Noncommercial-ShareAlike 4.0 International License</a>.