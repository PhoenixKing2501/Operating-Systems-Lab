OBJ     = main.o userSimulator.o pushUpdate.o readPost.o
HEADERS = Graph.hpp Node.hpp Action.hpp Common.hpp Queue.hpp
FLAGS   = -std=gnu++20 -lpthread -Wall -fdiagnostics-color -D_GNU_SOURCE -I.

A: main

main: $(OBJ) $(HEADERS)
	g++ -o main $(OBJ) $(FLAGS)

%.o: %.cpp $(HEADERS)
	g++ -c $< $(FLAGS)

run: main
	./main > out.log

clean:
	rm *.log main *.o 
