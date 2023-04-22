OBJ     = mergesort.o goodmalloc.o
HEADERS = goodmalloc.hpp
FLAGS   = -std=gnu++17 -O6 -Wall -fdiagnostics-color -D_GNU_SOURCE -I.

run: mergesort
	./mergesort > out.txt 2> err.txt

mergesort: $(OBJ) $(HEADERS)
	g++ -o mergesort $(OBJ) $(FLAGS)

%.o: %.cpp $(HEADERS)
	g++ -c $< $(FLAGS)

time: mergesort
	>times.txt; for i in {1..100}; do echo "Iter $$i"; ./mergesort > /dev/null 2>> times.txt ; done
	gawk '{print $$3}' times.txt | gawk '{s+=$$1} END {print "Avg Time: " s/NR " ms">>"all_times.txt"}'

clean:
	-rm mergesort *.o *.txt

