run: main producer
	./main

main: main.cpp Graph.inl Graph.hpp
	g++ -o main  main.cpp  -Wall -std=c++20 -fdiagnostics-color -D_GNU_SOURCE -I.

producer: producer.cpp Graph.inl Graph.hpp
	g++ -o producer producer.cpp -Wall -std=c++20 -fdiagnostics-color -D_GNU_SOURCE -I.

clean:
	-rm main producer
