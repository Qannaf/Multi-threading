# Multi-threading et mutex C++11
## 1. Qu'est-ce qu'un thread ?
Un thread permet de paralléliser du code, son exécution, au sein d'un programme.


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
```ruby
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
```ruby
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

# 3. Synchronisation : le mutex
Avec le multi-threading vient les soucis de synchronisation, plus communément appelés « race-condition ». En effet, chaque thread est exécuté en parallèle, 
mais nous n'avons aucune assurance de l'ordre des opérations de chaque thread.
Pour en prendre conscience, essayez le code suivant :

```ruby
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
```ruby
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

```ruby
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
```ruby
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
```ruby
std::lock_guard.
```

Cet objet prend un mutex en paramètre et le verrouille à sa création, puis se charge de le libérer à sa destruction. Il est ainsi très simple de limiter un verrou à un bloc de code, et en particulier en cas d'exception, early-return ou toute sortie prématurée du bloc, le verrou est également libéré automatiquement.
```ruby
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
	const std::lock_guard<std::mutex> lock(mutex::lock);

	std::cout << "Dans le thread "<<t << "\n";
	for (int i = 0; i < 10; ++i)
		std::cout << (i * 3) + nbr << " ";
	std::cout <<"\n";


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
