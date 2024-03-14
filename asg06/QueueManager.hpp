#pragma once
#include <queue>
#include <iostream>
#include <fstream>
#include <stdexcept>    //Plan to throw exc if queue enum gets set to invalid value


enum Q {FORE = 0, BACK, BATCH};

class QueueManager
{
    private:
        //RR q=1
        std::priority_queue<job> foreground;

        //FCFS q=2, q=3
        std::queue<job> background;
        std::queue<job> batch;

        //MaxPass and Quantum members
        int maxPass= 2;
        ms quantum = ms(200);

    public:
        QueueManager()
        {
            //set default maxpass and quantum
            maxPass = 2;
            quantum = ms(200);
        }
        ~QueueManager()
        {

        }
        void enter(job j)
        {
            foreground.push(j);
        }

        void reschedule(job j)
        {
            j.pass++;
            if(j.pass >= maxPass)
            {
                //RESET PASS TO 0
                j.pass = 0;

                //INC QUEUE IF < BATCH
                if(j.currentQueue == FORE)
                    j.currentQueue = BACK;
                else if (j.currentQueue == BACK)
                    j.currentQueue = BATCH;
            }
            switch(j.currentQueue)
            {
                //CASES
                //PUSH TO APPROPRIATE QUEUE
                case FORE:
                    enter(j);
                    break;
                case BACK:
                    background.push(j);
                    break;
                case BATCH:
                    batch.push(j);
                    break;

                //Only three cases should be possible; default is very bad news
                default:
                    throw std::out_of_range("Queue enum is invalid");
            }
        }

        
        job* schedule()
        {
            //Processor thread handles deletion of ptr
            job* out = new job;

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