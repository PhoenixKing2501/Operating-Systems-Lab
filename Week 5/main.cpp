#include "Common.hpp"
#include <iostream>
using namespace std;
/*create a vector of priority*/
/*initialise its size and dummy value in main*/
/*need to keep it global to share between multiple threads*/
vector<int32_t> pr_guests;

/*create a pointer to a Hotel instance*/
/*initialise it in main*/
/*need to keep it global to share between multiple threads*/
Hotel *hotel{nullptr};

vector<pthread_mutex_t> guest_mutex{};
vector<pthread_cond_t> guest_cond{};

int32_t numGuests{};
int32_t numRooms{};
int32_t numCleaners{};

int cleaner_ctr{0};

bool operator<(const pair<int32_t, Room> &below, const pair<int32_t, Room> &above)
{
	if (below.second.guestPriority > above.second.guestPriority)
	{
		return true;
	}
	else if (below.second.guestPriority == above.second.guestPriority && below.second.occupancy > above.second.occupancy)
	{
		return true;
	}

	return false;
}

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

	/*initialise the vector of priorities*/
	pr_guests.resize(numGuests);
	for (int32_t i = 0; i < numGuests; i++)
	{
		pr_guests[i] = rand() % numGuests; /*priority is a random value between 0 and 100*/

		printf("Guest %d has priority %d\n", i, pr_guests[i]);
	}

	/*initialise guest mutex and cond*/
	guest_mutex.resize(numGuests);
	guest_cond.resize(numGuests);

	for (int32_t i = 0; i < numGuests; i++)
	{
		pthread_mutex_init(&guest_mutex[i], nullptr);
		pthread_cond_init(&guest_cond[i], nullptr);
	}

	printf("Initialised all guest's mutex and cond\n");

	/*initialise the instance of hotel pointer*/
	hotel = new Hotel(numCleaners, numRooms);
	hotel->startCleaners();

	printf("Initialised hotel instance\n");

	/*create the guest threads*/
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

	// join the threads here
	for (int32_t i = 0; i < numGuests; i++)
	{
		pthread_join(guests[i], nullptr);
	}
}
