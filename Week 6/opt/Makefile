OBJ     = main.o goodmalloc.o
HEADERS = goodmalloc.hpp
FLAGS   = -std=gnu++17 -O6 -Wall -fdiagnostics-color -D_GNU_SOURCE -I.

main: $(OBJ) $(HEADERS)
	g++ -o main $(OBJ) $(FLAGS)

%.o: %.cpp $(HEADERS)
	g++ -c $< $(FLAGS)

run: main
	./main > out.txt

time: main
	>times.txt; for i in {1..100}; do echo "Iter $$i"; ./main > /dev/null 2>> times.txt ; done
	gawk '{print $$3}' times.txt | gawk '{s+=$$1} END {print "Avg Time: " s/NR}'

clean:
	-rm main *.o

