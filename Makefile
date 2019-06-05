all: main.exe

CC=g++
override CFLAGS += -Wall -std=c++11 -O3 -Wextra -fopenmp

OBJS=stopwatch.o convolution.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@  -ljpeg -lpng -lz -lboost_system -lboost_filesystem

main.exe: $(OBJS) main.cpp
	$(CC) $(CFLAGS) main.cpp $(OBJS) -o $@ -ljpeg -lpng -lz -lboost_system -lboost_filesystem

valgrind: main.exe
	valgrind --leak-check=full ./main.exe images/dragon.png kernels/box_blur3 outFolder/dragon.jpg
cachegrind: main .exe
	valgrind --tool=cachegrind ./main.exe images/dragon.png kernels/box_blur3 outFolder/dragon.jpg
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

	OMP_NUM_THREADS=1  ./main.exe images/veryBig.jpg kernels/box_blur15 outFolder/tmp.jpg >  tmp_bigpic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=2  ./main.exe images/veryBig.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=4  ./main.exe images/veryBig.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=8  ./main.exe images/veryBig.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=16 ./main.exe images/veryBig.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=32 ./main.exe images/veryBig.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel.txt

	OMP_NUM_THREADS=1  ./main.exe images/veryBig.jpg kernels/box_blur15_not_separable outFolder/tmp.jpg >  tmp_bigpic_different_thread_number_large_kernel_not_separable.txt
	OMP_NUM_THREADS=2  ./main.exe images/veryBig.jpg kernels/box_blur15_not_separable outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel_not_separable.txt
	OMP_NUM_THREADS=4  ./main.exe images/veryBig.jpg kernels/box_blur15_not_separable outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel_not_separable.txt
	OMP_NUM_THREADS=8  ./main.exe images/veryBig.jpg kernels/box_blur15_not_separable outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel_not_separable.txt
	OMP_NUM_THREADS=16 ./main.exe images/veryBig.jpg kernels/box_blur15_not_separable outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel_not_separable.txt
	OMP_NUM_THREADS=32 ./main.exe images/veryBig.jpg kernels/box_blur15_not_separable outFolder/tmp.jpg >> tmp_bigpic_different_thread_number_large_kernel_not_separable.txt

	OMP_NUM_THREADS=1  ./main.exe images/extraLarge.jpg kernels/box_blur15 outFolder/tmp.jpg >  tmp_extraLargePic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=2  ./main.exe images/extraLarge.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_extraLargePic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=4  ./main.exe images/extraLarge.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_extraLargePic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=8  ./main.exe images/extraLarge.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_extraLargePic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=16 ./main.exe images/extraLarge.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_extraLargePic_different_thread_number_large_kernel.txt
	OMP_NUM_THREADS=32 ./main.exe images/extraLarge.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_extraLargePic_different_thread_number_large_kernel.txt


	OMP_NUM_THREADS=1  ./main.exe images/dragon.jpg kernels/box_blur3 outFolder/tmp.jpg >  tmp_smallpic_different_kernel_size_separable.txt
	OMP_NUM_THREADS=1  ./main.exe images/dragon.jpg kernels/box_blur5 outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_separable.txt
	OMP_NUM_THREADS=1  ./main.exe images/dragon.jpg kernels/box_blur7 outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_separable.txt
	OMP_NUM_THREADS=1  ./main.exe images/dragon.jpg kernels/box_blur9 outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_separable.txt
	OMP_NUM_THREADS=1 ./main.exe images/dragon.jpg kernels/box_blur11 outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_separable.txt
	OMP_NUM_THREADS=1 ./main.exe images/dragon.jpg kernels/box_blur13 outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_separable.txt
	OMP_NUM_THREADS=1 ./main.exe images/dragon.jpg kernels/box_blur15 outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_separable.txt

	OMP_NUM_THREADS=1  ./main.exe images/dragon.jpg kernels/box_blur3_not_separable outFolder/tmp.jpg >  tmp_smallpic_different_kernel_size_not_separable.txt
	OMP_NUM_THREADS=1  ./main.exe images/dragon.jpg kernels/box_blur5_not_separable outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_not_separable.txt
	OMP_NUM_THREADS=1  ./main.exe images/dragon.jpg kernels/box_blur7_not_separable outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_not_separable.txt
	OMP_NUM_THREADS=1  ./main.exe images/dragon.jpg kernels/box_blur9_not_separable outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_not_separable.txt
	OMP_NUM_THREADS=1 ./main.exe images/dragon.jpg kernels/box_blur11_not_separable outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_not_separable.txt
	OMP_NUM_THREADS=1 ./main.exe images/dragon.jpg kernels/box_blur13_not_separable outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_not_separable.txt
	OMP_NUM_THREADS=1 ./main.exe images/dragon.jpg kernels/box_blur15_not_separable outFolder/tmp.jpg >> tmp_smallpic_different_kernel_size_not_separable.txt
folder: main.exe
	./main.exe images kernels/box_blur3 outFolder
# This cleans up the project
clean:
	rm -rf *.exe *.o outFolder
