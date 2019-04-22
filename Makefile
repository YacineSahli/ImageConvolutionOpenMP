all: main.exe

CC=g++
CFLAGS=-Wall -g 

OBJS=stopwatch.o convolution.o

%.o: %.cpp 
	$(CC) $(CFLAGS) -c $< -o $@  -ljpeg 

main.exe: $(OBJS) main.cpp
	$(CC) $(CFLAGS) main.cpp $(OBJS) -o $@ -ljpeg 

test: main.exe
	valgrind ./main.exe cutie6.jpg kernel tmp.test
# This cleans up the project
clean: 
	rm *.exe *.o
