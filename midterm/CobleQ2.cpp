//
// File:    inputgen.cpp
// Author:  Your Glorious Instructor
// Pupose:
// Starting code for thread question in exam 1
//
#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>
#include <mutex>

std::ofstream outputStream("output.txt");
std::random_device rd{};
std::mt19937 gen{rd()};
std::uniform_int_distribution<int> sampleUd(0, 100);
std::uniform_int_distribution<int> waitUd(0,10);

struct DataSample {
  int generator;
  int value;
};
std::queue<DataSample> sampleQueue;
bool twoMin = true;
std::mutex mx;

void simulateInput(int generatorNumber) {
  // TODO:
  // Add the appropriate code that uses a lock_guard and a mutex to
  // protect the critical section in this function.
  //

  while(twoMin) 
  {
    DataSample ds;
    ds.generator = generatorNumber;
    ds.value = sampleUd(gen);
    //Weird looking block so other threads don't have to wait for this one to wake up - HC
    {
      std::lock_guard<std::mutex> lock(mx);
      sampleQueue.push(ds);
    }
    std::this_thread::sleep_for(std::chrono::seconds(waitUd(gen)));
  }
}


int main(int argc, char *argv[])
{
  if (!outputStream.is_open()) 
  {
    std::cout << "Unable to create output file" << std::endl;
    exit(1);
  }
  // TODO:
  // Create a vector of threads
  std::vector<std::thread> tVec;

  // and then write a loop that adds ten
  // threads to that vector with each thread using simuateInput() as the
  // the thread function, and then detaching from the thread.
  for(int i = 0; i < 10; i++)
  {
    std::thread t(simulateInput, i);
    tVec.push_back(move(t));
    tVec[i].detach();
  }
  std::cout << "Sleeping for 2 minutes...\n";
  // Then make the main thread (here) wait for two seconds ("minutes" on canvas hence * 120 - HC)
  std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 120));
  std::cout << "Emptying Queue...\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  
  //and  write an 
  // infinite loop that checks to see if there is anything on the
  // sampleQueue queue and write to the output file and standard
  // output.
  while (true)
  {
    //lock
    std::lock_guard<std::mutex> lock(mx);
    if(!sampleQueue.empty())
    {
      outputStream << sampleQueue.front().value << ", " << sampleQueue.front().generator << "\n";
      std::cout << sampleQueue.front().value << ", " << sampleQueue.front().generator << "\n";
      sampleQueue.pop();
    }
    else
      {
        std:: cout << "Queue is empty, awaiting input...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
  }
  // Note: Don't forget to lock the critical section using lock_guard()
  // and a mutex.
  return 1;
}