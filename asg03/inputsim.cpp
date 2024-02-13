/*
 * File:     inputsim.cpp
 * Author:   Adam.Lewis@athens.edu
 *
 * This code serves multiple purposes: (1) provide a more extensive example of
 * using threads, (2) demo how to use to C++ STL random classes for generating
 * random numbers, (3) provide more practice with the STL container and
 * iterator classes, and (4) provide code for use in a future programming
 * project.
 *
 * NOTES:
 * (a) Be aware that this code has some potential race conditions in how it's
 * reading from the deque.  You will address these conditions in the
 * aforementioned homework assignment.
 * (b) Most people are aware of the srand()/rand() pseudo-random number
 * generator that's in the cstdlib library.  It's preferred with Modern C++ to
 * use the C++ STL's random classes.
 * (c) We need to make this app sleep at multiple points in the
 * application. There are multiple ways of doing this but the preferred method
 * in Modern C++ is a combination of methods from the chrono and thread
 * libraries
 *
 */

#include <iostream>
#include <random>
#include <thread>
#include <string>
#include <sstream>
#include <deque>
#include <chrono>
#include <mutex>
#include <fstream>
// I am being lazy here... the common preference is to not do a using
// statement. 
using namespace std;

// We will be using this deque as a shared global between two threads: a thread
// that simulates the reading of a set of devices and the main thread that is
// extracting values from the queue and printing them.
deque<string> inputQueue;
mutex mx;
bool keepGoing = true;
bool keepGoing2 = true;

//
// void simulateInput(int rate)
//
// This function simulates a set of devices generating input to the
// program. Here's where you get to use some of those things that we learn from
// our Prob. and Stats. class.   The devices generate new input at a predefined
// rate.  For example, we expect input at a rate of 3 samples/second.
//
// Data from the devices is simulated using a normal distribution with a mean
// of 5.0 and standard deviation of 3.0.   We simulate device id by randomly
// selecting a uniformly distributed integer (this is what we
// commonly think of when we do random numbers in a program) between 0 and 10.
//
// We build the data for the queue using a string stream.  The randomly
// generated data is written to the stream and we then push the backing string
// onto the queue.
//

void simulateInput(int rate) 
{
  random_device rd{};
  mt19937 gen{rd()};
  int count = 0;

  poisson_distribution<> pD(rate);
  normal_distribution<> nD(5.0, 3.0);
  uniform_int_distribution<int> uD(0,10);

  while(keepGoing)
  {
    int numberEvents = pD(gen);
    if (numberEvents > 0)
    {
      for (int i = 0; i < numberEvents; ++i)
      {
        stringstream sampleStream;
        int deviceNumber = uD(gen);
        float sample = nD(gen);
        count++;
        sampleStream << count << " " << deviceNumber << " " << sample;
        mx.lock();
        inputQueue.push_front(sampleStream.str());
        mx.unlock();
      }
    }
    this_thread::sleep_for(chrono::milliseconds(1000));
  }
}

void sampleTail()
{
    ofstream out;
    stringstream fss;
    string file;
    int printed = 0;

    fss << this_thread::get_id();
    fss >> file;
    file += ".csv";
    while(keepGoing2)
    {
        out.open(file, ios::app);
        
      if(out.is_open())
      {  
        mx.lock();
        if(!inputQueue.empty())
        {
          if(printed == 9)
          {
            out << "\n"; 
            printed = 0;
          }
          else
            printed++;
          out << inputQueue.back() << ", ";
          inputQueue.pop_back();
        }
        //else
          //out << "Empty" << ", ";
        mx.unlock();
      }
      else
        std::cerr << "ERROR: " << file << " unable to open\n";
      out.close();;
    }

}
//
// int main(int argc, char *argv[])
//
// We test our input simulator by creating two threads: our main application
// thread (created when we start our program) and a thread that executes the
// simulateInput() program.   Then, we go into an infinite loop reading and
// printing the data values from the input queue.
//
// NOTES:
// You must manually terminate this program as both threads intentionally
// has infinite loops.
//
// TODO:
// For the moment, we use a constant to define the average sample rate passed
// to the simulateInput() function.  This needs to be changed to get that value
// from the command-line arguments.
//

const int SAMPLE_ARRIVAL_RATE = 10;
int main(int argc, char *argv[])
{
  inputQueue.push_front("0 START");
  // Build and lanuch the input simulator thread
  thread inputThread(simulateInput, SAMPLE_ARRIVAL_RATE);
  inputThread.detach();
  // Pause three seconds
  cout << "Pausing three seconds for station identification" << endl;
  this_thread::sleep_for(chrono::milliseconds(3000));

  thread t1(sampleTail);
  t1.detach();
  thread t2(sampleTail);
  t2.detach();
  thread t3(sampleTail);
  t3.detach();
  thread t4(sampleTail);
  t4.detach();
  thread t5(sampleTail);
  t5.detach();
  thread t6(sampleTail);
  t6.detach();
  thread t7(sampleTail);
  t7.detach();
  thread t8(sampleTail);
  t8.detach();
  thread t9(sampleTail);
  t9.detach();
  thread t10(sampleTail);
  t10.detach();
  // Now start printing stuff from the queue

  auto start = chrono::steady_clock::now();
  //5 minute end time
  auto end = chrono::milliseconds(5*60000);  

  //Loop for five minutes
  while(chrono::steady_clock::now() - start < end)
  {
    mx.lock();
    if (!inputQueue.empty())
    {
      
      string sample = inputQueue.back();
      inputQueue.pop_back();
      cout << inputQueue.size()<< endl;    
    }
    mx.unlock();
  }

  //Stop adding to queue after 5 min
  keepGoing = false;

  //Go until read all remaining into csv
  while(true)
  {
    if(inputQueue.empty())
    {
      keepGoing2 = false;
      break;
    }
  }
  cout << "Queue read, closing...\n";
  return 0;
}
