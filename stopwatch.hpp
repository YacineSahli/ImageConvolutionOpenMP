#pragma once 
#include <chrono>

class stopwatch {
	std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;
public:
	void start();
	void stop(); 
	double elapsedTime();
};