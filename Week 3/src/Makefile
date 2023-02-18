run: main child
	./main

main: main.cpp Graph.inl Graph.hpp
	g++ -o main  main.cpp  -Wall -std=c++20 -fdiagnostics-color -D_GNU_SOURCE -I.

child: child.cpp Graph.inl Graph.hpp
	g++ -o child child.cpp -Wall -std=c++20 -fdiagnostics-color -D_GNU_SOURCE -I.

clean:
	-rm main child
