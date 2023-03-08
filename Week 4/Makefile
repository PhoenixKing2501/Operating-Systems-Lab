A: main

main: main.o userSimulator.o pushUpdate.o readPost.o Graph.hpp Node.hpp Action.hpp Common.hpp Queue.hpp
	g++ -o main main.o userSimulator.o pushUpdate.o readPost.o -lpthread -Wall -std=gnu++20 -fdiagnostics-color -D_GNU_SOURCE -I.

%.o: %.cpp Graph.hpp Node.hpp Action.hpp Common.hpp Queue.hpp
	g++ -c $< -std=c++20 -lpthread -Wall -std=gnu++20 -fdiagnostics-color -D_GNU_SOURCE -I.

run: main
	./main > out.log

clean:
	rm *.log main *.o 
