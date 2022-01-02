# Threading

## Section 1: Creating a std::thread
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


## Section 2: Passing a reference to a thread
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


## Section 3: Using std::async instead of std::thread
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


## Section 4: Basic Synchronization
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

## Section 6: Ensuring a thread is always joined
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


## Section 7: Operations on the current thread
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


## Section 8: Using Condition Variables
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

## Section 9: Thread operations
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

## Section 10: Thread-local storage
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


## Section 11: Reassigning thread objects
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