#include <thread>
#include <mutex>
//#include <queue>
#include <fstream>  //Reader class writes to a file.
#include <chrono>
#include <iostream>

//Only one mutex required:
//A reader can only read when no other reader is AND no writer is writing.
//A writer can only writer when no other writer is AND when no reader is reading.

//The last position in my array stores the number of items in the array
//If 0, then array is empty.
//If 4, then the array is full.

std::mutex mx;
int sharedArray[5] = {0,0,0,0,0};
bool keepGoing = true;

using ms = std::chrono::milliseconds;

bool writeEnforcement = false;
bool emptyRead = false;

class Reader
{
    private:
        int data = -1;
        unsigned int index=0;
        std::ofstream outFile;
    public:
        Reader()
        {
            outFile.open("readDataQ3.txt");
        }
        ~Reader()
        {
            outFile.close();
        }
        void read()
        {
            std::lock_guard<std::mutex> lg(mx);

            if(sharedArray[4] != 0)
            {
                //Get the item from index 0 - 3, increment index
                //This keeps the queue behavior to prevent stagnation
                data = sharedArray[index++ % 4];

                //Reset the number of items (pop from queue)
                sharedArray[4]--;
                outFile << data << std::endl;
            }
            else
            {
                outFile << "No data in queue"<< std::endl;
                emptyRead = true;
            }
        }

};

class Writer
{
    private:
        int data = 0;
        unsigned int index=0;
    public:
        void write()
        {
            std::lock_guard<std::mutex> lg(mx);
            if(sharedArray[4] != 4)
            {
                sharedArray[index++ % 4] = data++;
                sharedArray[4]++;
            }
            else
                writeEnforcement = true;
        }
};

void readThread()
{
    Reader readingObj;
    while(keepGoing)
    {
        readingObj.read();
        std::this_thread::sleep_for(ms(1000));
    }
}

void writeThread()
{
    Writer writingObj;
    while(keepGoing)
    {
        writingObj.write();
        std::this_thread::sleep_for(ms(2000));
    }
}

int main()
{
    int input;
    std::thread t1(readThread);
    t1.detach();
    std::thread t2(writeThread);
    t2.detach();

    std::this_thread::sleep_for(ms(15000));
    keepGoing = false;
    std::this_thread::sleep_for(ms(2000));

    if (emptyRead)
        std::cout << "Prevented reading from \"empty\" array\n";
    if (writeEnforcement)
        std::cout << "Prevented writing to full array\n";
    return 0;
}