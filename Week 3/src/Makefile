run: main
	./main

main: main.cpp My_Allocator.inl My_Allocator.hpp
	g++ -o main main.cpp -Wall -std=c++20 -fdiagnostics-color -D_GNU_SOURCE -I.

clean:
	-rm main
