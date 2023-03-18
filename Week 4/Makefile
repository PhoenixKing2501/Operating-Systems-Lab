OBJ     = sns.o userSimulator.o pushUpdate.o readPost.o
HEADERS = Graph.hpp Node.hpp Action.hpp Common.hpp Queue.hpp
FLAGS   = -std=gnu++20 -pthread -Wall -fdiagnostics-color -D_GNU_SOURCE -I.

build: sns

sns: $(OBJ) $(HEADERS)
	g++ -o sns $(OBJ) $(FLAGS)

%.o: %.cpp $(HEADERS)
	g++ -c $< $(FLAGS)

run: sns
	./sns > out.log

clean:
	rm sns *.o 

clean_logs:
	rm *.log
