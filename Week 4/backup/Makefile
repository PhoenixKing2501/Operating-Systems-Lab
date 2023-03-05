run: main
	./main > out.txt

main: main.cpp Graph.hpp Node.hpp Action.hpp
	g++ -o main main.cpp -Wall -std=gnu++20 -fdiagnostics-color -D_GNU_SOURCE -I.

clean:
	rm main
	> out.txt
