#include "Common.hpp"
#include <iostream>
using namespace std;

vector<int32_t> pr_guests;

Hotel *hotel{nullptr};

vector<pthread_mutex_t> guest_mutex{};
vector<pthread_cond_t> guest_cond{};

int32_t numGuests{};
int32_t numRooms{};
int32_t numCleaners{};

int main(int argc, char const *argv[])
{
	srand(time(nullptr));

	if (argc != 4)
	{
		cerr << "Usage: " << argv[0] << "<numCleaners> <numGuests> <numRooms>" << endl;
		return EXIT_FAILURE;
	}

	numGuests = stoi(argv[2]);
	numRooms = stoi(argv[3]);
	numCleaners = stoi(argv[1]);

	if (not(numGuests >= numRooms and
			numRooms >= numCleaners and
			numCleaners > 0))
	{
		cerr << "Invalid input" << endl;
		return EXIT_FAILURE;
	}

	pr_guests.resize(numGuests);
	for (int32_t i = 0; i < numGuests; i++)
	{
		pr_guests[i] = rand() % numGuests;
		printf("Guest %d has priority %d\n", i, pr_guests[i]);
	}

	guest_mutex.resize(numGuests);
	guest_cond.resize(numGuests);

	for (int32_t i = 0; i < numGuests; i++)
	{
		pthread_mutex_init(&guest_mutex[i], nullptr);
		pthread_cond_init(&guest_cond[i], nullptr);
	}

	printf("Initialised all guest's mutex and cond\n");

	hotel = new Hotel(numCleaners, numRooms);
	hotel->startCleaners();

	printf("Initialised hotel instance\n");

	vector<pthread_t> guests(numGuests);
	for (int32_t i = 0; i < numGuests; i++)
	{
		auto ptr = new int32_t{i};
		pthread_create(&guests[i], nullptr, guestThread, ptr);
	}

	printf("Created all guest threads\n");

	for (;;)
	{
		sleep(5);
		fflush(stdout);
	}

	for (int32_t i = 0; i < numGuests; i++)
	{
		pthread_join(guests[i], nullptr);
	}

	for(int32_t i = 0; i < numGuests; i++)
	{
		pthread_mutex_destroy(&guest_mutex[i]);
		pthread_cond_destroy(&guest_cond[i]);
	}
	
	delete hotel;
}
