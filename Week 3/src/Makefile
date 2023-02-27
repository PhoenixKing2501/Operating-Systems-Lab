A3: main producer consumer

main: main.cpp Graph.inl Graph.hpp
	g++ -o main  main.cpp  -Wall -std=c++20 -fdiagnostics-color -D_GNU_SOURCE -I.

producer: producer.cpp Graph.inl Graph.hpp
	g++ -o producer producer.cpp -Wall -std=c++20 -fdiagnostics-color -D_GNU_SOURCE -I.

consumer: consumer.cpp Graph.inl Graph.hpp
	g++ -o consumer consumer.cpp -Wall -std=c++20 -fdiagnostics-color -D_GNU_SOURCE -I.

run_optimize: main producer consumer
	./main -optimize

run: main producer consumer
	./main 

clean:
	-rm main producer consumer consumer[0-9].txt consumer[0-9]time.txt adjacency_matrix.txt
