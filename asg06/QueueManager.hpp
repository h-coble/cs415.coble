#pragma once
#include <queue>
#include <iostream>
#include <fstream>
#include <stdexcept>
enum Q {FORE = 0, BACK, BATCH};

class QueueManager
{
    private:
        //RR q=1
        std::priority_queue<job> foreground;
        //FCFS q=2, q=3
        std::queue<job> background, 
                        batch;
        int maxPass[3] = {1, 2, 3};
    public:
        QueueManager()
        {
            //set default maxpass and quantum
        }
        void enter(job j)
        {
            foreground.push(j);
        }

        void reschedule(job j)
        {
            j.pass++;
            if(j.pass >= maxPass[j.currentQueue])
            {
                //RESET PASS TO 0
                //INC QUEUE IF < BATCH
            }
            switch(j.currentQueue)
            {
                //CASES
                //PUSH TO APPROPRIATE QUEUE
                default:
                    throw std::out_of_range("Queue enum is invalid");
            }
        }

        job* schedule()
        {
            job* out = nullptr;
            if(!foreground.empty())
            {
                *out = foreground.top();
                foreground.pop();
            }
            else if(!background.empty())
            {
                *out = background.front();
                background.pop();
            }
            else if(!batch.empty())
            {
                *out = batch.front();
                batch.pop();
            }
            else
            {
                delete out;
                return nullptr;
            }
            return out;
        }


};