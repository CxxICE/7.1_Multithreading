#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

class Clients
{
public:
	Clients(int max_clients)
	{
		_max_clients = max_clients;
		_clients_counter = 0;
	};
	void add_client()
	{
		while (_clients_counter < _max_clients)
		{
			++_clients_counter;
			++_clients_queue;
			std::cout << "\x1B[31m" << "Clients: Client number " << _clients_counter << " added." << std::endl << "\x1B[0m";				
			std::this_thread::sleep_for(1s);
		}		
	}
	friend class Teller;
private:
	int _max_clients;	
	int _clients_counter;
	static int _clients_queue;
};

int Clients::_clients_queue = 0;

class Teller
{
public:
	Teller() = default;
	void take_clients()
	{
		
		do//ожидание первого клиента
		{
			std::this_thread::sleep_for(50ms);
		} while (Clients::_clients_queue == 0);
		
		while (Clients::_clients_queue > 0)
		{
			++_clients_counter;
			--Clients::_clients_queue;
			std::cout << "\x1B[32m" << "Teller: Client number " << _clients_counter << " is taken." << std::endl << "\x1B[0m";			
			std::this_thread::sleep_for(2s);
		}
	}
private:
	int _clients_counter = 0;
};

int main()
{
	Clients clients(10);
	Teller teller;
	std::thread t1(&Clients::add_client, clients);
	std::thread t2(&Teller::take_clients, teller);
	if (t1.joinable())
	{
		t1.join();
	}
	
	if (t2.joinable())
	{
		t2.join();
	}
	
}


