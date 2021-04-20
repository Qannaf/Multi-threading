# Multi-threading et mutex C++11
## I. Qu'est-ce qu'un thread ?
Un thread permet de paralléliser du code, son exécution, au sein d'un programme.


## II. L'API thread
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


# III. Synchronisation : le mutex
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

