OBJ     = main.o guest.o cleaner.o
HEADERS = Room.hpp Common.hpp Hotel.hpp
FLAGS   = -std=gnu++20 -pthread -Wall -fdiagnostics-color -D_GNU_SOURCE -I.

build: main

main: $(OBJ) $(HEADERS)
	g++ -o main $(OBJ) $(FLAGS)

%.o: %.cpp $(HEADERS)
	g++ -c $< $(FLAGS)

run: main
	./main > out.log

clean:
	rm main *.o 

clean_logs:
	rm *.log
