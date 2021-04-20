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