// TinyBenchmarkCpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <windows.h>

std::mutex fileLock;
void writeToCsv(const std::string& fileName, const std::string& data) {
	std::lock_guard<std::mutex> lock(fileLock);
	std::ofstream file(fileName, std::ios::app);
	if (file.is_open()) {
		file << data << std::endl;
		file.close();
	}
	else {
		std::cerr << "open file failed." << std::endl;
	}
}

void threadFunction(int group, int sn, int counter, int Interation)
{
	std::string fileName = std::format("threads_{}.csv", GetCurrentProcessId());
	std::thread::id threadId = std::this_thread::get_id();
	std::ostringstream oss;
	oss << threadId;
	std::string threadIdStr = oss.str();
	writeToCsv(fileName, std::format("{}, {}, {}, {}", GetCurrentProcessId(), threadIdStr, group, sn));

	std::cout << std::format("{},", sn);
	for (int i = 0; i < Interation; i++)
	{
		int c = counter;
		while (c-- > 0)
			;
	}
}

int main(int argc, char* argv[])
{
	int Group = std::stoi(argv[1]);
	int Counter = INT_MAX / 2;
	int Interation = std::stoi(argv[2]);
	int Affinity = std::stoi(argv[3]);

	HANDLE hProcess = GetCurrentProcess();
	DWORD_PTR processAffinityMask = Affinity;

	if (!SetProcessAffinityMask(hProcess, processAffinityMask)) {
		std::cerr << "SetProcessAffinityMask failed." << std::endl;
		return 1;
	}

	std::string fileName = std::format("threads_{}.csv", GetCurrentProcessId());
	writeToCsv(fileName, "Process ID, Thread ID, Group, Serial Number");

	for (int g = 0; g < Group; g++)
	{
		int thd_cound = 1 << g;
		std::vector<std::thread> threads;

		// create all threads
		for (int sn = 1; sn <= thd_cound; sn++)
		{
			// create a new thread
			std::thread t(threadFunction, g, sn, Counter, Interation);
			threads.push_back(std::move(t));
		}

		// wait for all threads
		for (auto& t : threads)
		{
			if (t.joinable()) {
				t.join();
			}
		}

		std::cout << std::endl;
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
