all: main.exe

CC=g++
override CFLAGS += -Wall -Wextra -g -fopenmp

OBJS=stopwatch.o convolution.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@  -ljpeg -lpng -lz -lboost_system -lboost_filesystem

main.exe: $(OBJS) main.cpp
	$(CC) $(CFLAGS) main.cpp $(OBJS) -o $@ -ljpeg -lpng -lz -lboost_system -lboost_filesystem

valgrind: main.exe
	valgrind ./main.exe images/dragon.png kernels/box_blur3 outFolder/dragon.jpg
test: main.exe
	./main.exe images/dragon.jpg kernels/box_blur3 outFolder/dragon.jpg
speed: main.exe
	./main.exe images/dragon.jpg kernels/box_blur3 outFolder/tmp.jpg > tmp.txt
	./main.exe images/dragon.jpg kernels/box_blur5 outFolder/tmp1.jpg >> tmp.txt
	./main.exe images/dragon.jpg kernels/box_blur7 outFolder/tmp2.jpg >>tmp.txt
	./main.exe images/dragon.jpg kernels/box_blur9 outFolder/tmp3.jpg >> tmp.txt
	./main.exe images/veryBig.jpg kernels/box_blur3 outFolder/tmp4.jpg >> tmp.txt
	./main.exe images/veryBig.jpg kernels/box_blur5 outFolder/tmp5.jpg >> tmp.txt
	./main.exe images/veryBig.jpg kernels/box_blur7 outFolder/tmp6.jpg >> tmp.txt
	./main.exe images/veryBig.jpg kernels/box_blur9 outFolder/tmp7.jpg >> tmp.txt
stats: main.exe

	OMP_NUM_THREADS=1  ./main.exe images/dragon.jpg kernels/box_blur3 outFolder/tmp.jpg >  tmp_smallpic_different_thread_number.txt
	OMP_NUM_THREADS=2  ./main.exe images/dragon.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_smallpic_different_thread_number.txt
	OMP_NUM_THREADS=4  ./main.exe images/dragon.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_smallpic_different_thread_number.txt
	OMP_NUM_THREADS=8  ./main.exe images/dragon.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_smallpic_different_thread_number.txt
	OMP_NUM_THREADS=16 ./main.exe images/dragon.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_smallpic_different_thread_number.txt
	OMP_NUM_THREADS=32 ./main.exe images/dragon.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_smallpic_different_thread_number.txt

	OMP_NUM_THREADS=1  ./main.exe images/veryBig.jpg kernels/box_blur3 outFolder/tmp.jpg >  tmp_bigpic_different_thread_number.txt
	OMP_NUM_THREADS=2  ./main.exe images/veryBig.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number.txt
	OMP_NUM_THREADS=4  ./main.exe images/veryBig.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number.txt
	OMP_NUM_THREADS=8  ./main.exe images/veryBig.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number.txt
	OMP_NUM_THREADS=16 ./main.exe images/veryBig.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number.txt
	OMP_NUM_THREADS=32 ./main.exe images/veryBig.jpg kernels/box_blur3 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number.txt
folder: main.exe
	./main.exe images kernels/box_blur3 outFolder
# This cleans up the project
clean:
	rm -rf *.exe *.o outFolder
