#include <thread>
#include <mutex>
#include <queue>
#include <fstream>  //Reader class writes to a file.
#include <chrono>
#include <iostream>
//Only one mutex required:
//A reader can only read when no other reader is AND no writer is writing.
//A writer can only writer when no other writer is AND when no reader is reading.

std::mutex mx;
std::queue<int> sharedQueue;
bool keepGoing = true;

using ms = std::chrono::milliseconds;

class Reader
{
    private:
        int data = -1;
        std::ofstream outFile;
    public:
        Reader()
        {
            outFile.open("readData.txt");
        }
        ~Reader()
        {
            outFile.close();
        }
        void read()
        {
            std::lock_guard<std::mutex> lg(mx);
            if(!sharedQueue.empty())
            {
                data = sharedQueue.front();
                sharedQueue.pop();
                outFile << data << std::endl;
            }
            else
                outFile << "No data in queue"<< std::endl;
        }

};

class Writer
{
    private:
        int data = 0;
    public:
        void write()
        {
            std::lock_guard<std::mutex> lg(mx);
            sharedQueue.push(data++);
        }
};

void readThread()
{
    Reader readingObj;
    while(keepGoing)
    {
        readingObj.read();
        std::this_thread::sleep_for(ms(5000));
    }
}

void writeThread()
{
    Writer writingObj;
    while(keepGoing)
    {
        writingObj.write();
        std::this_thread::sleep_for(ms(1000));
    }
}

int main()
{
    int input;
    std::thread t1(readThread);
    t1.detach();
    std::thread t2(writeThread);
    t2.detach();

    std::cout << "Enter a number to stop the read/write threads: ";
    std::cin >> input;

    keepGoing = false;
    return 0;
}