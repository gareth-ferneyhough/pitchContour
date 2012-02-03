all: proj

clean:
	rm *.o

pitch.o: pitch.h pitch.cpp
	g++ -c pitch.cpp

pitchContour.o: pitchContour.cpp pitchContour.h
	g++ -c pitchContour.cpp

main.o: main.cpp
	g++ -c main.cpp

proj: pitch.o pitchContour.o main.o
	g++ -o pitchy pitch.o pitchContour.o main.o -lsamplerate -ljackcpp -ljack -laubio