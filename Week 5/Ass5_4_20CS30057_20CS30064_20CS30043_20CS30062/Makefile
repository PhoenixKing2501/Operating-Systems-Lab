OBJ     = main.o guest.o cleaner.o
HEADERS = Room.hpp Common.hpp Hotel.hpp
FLAGS   = -std=gnu++2a -lpthread -Wall -fdiagnostics-color -D_GNU_SOURCE -I.

build: main

main: $(OBJ) $(HEADERS)
	g++ -o main $(OBJ) $(FLAGS)

%.o: %.cpp $(HEADERS)
	g++ -c $< $(FLAGS)

clean:
	rm main *.o 

