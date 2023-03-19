#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
	if (argc != 4)
	{
		cerr << "Usage: " << argv[0] << " <numGuests> <numCleaners> <numRooms>" << endl;
		return EXIT_FAILURE;
	}

	int32_t numCleaners{stoi(argv[1])};
	int32_t numGuests{stoi(argv[2])};
	int32_t numRooms{stoi(argv[3])};

	if (not(numGuests >= numRooms and
			numRooms >= numCleaners and
			numCleaners > 0))
	{
		cerr << "Invalid input" << endl;
		return EXIT_FAILURE;
	}
}
