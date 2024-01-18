CFLAGS=-Wall -g

install: main.o
	g++ main.o -o sfml-app -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio
	rm main.o
	./sfml-app & 

main.o: main.cpp
	# make clean
	g++ -c main.cpp

clean:
	rm -f main.o sfml-app
