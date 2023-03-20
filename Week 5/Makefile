OBJ     = main.o guest.o cleaner.o
HEADERS = Room.hpp Common.hpp Hotel.hpp
FLAGS   = -O6 -std=gnu++20 -lpthread -Wall -fdiagnostics-color -D_GNU_SOURCE -I.

build: main

main: $(OBJ) $(HEADERS)
	g++ -o main $(OBJ) $(FLAGS)

%.o: %.cpp $(HEADERS)
	g++ -c $< $(FLAGS)

run: main
	./main 30 50 40 > out.log

clean:
	rm main *.o 

clean_logs:
	rm *.log
