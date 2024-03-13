#include <queue>
#include <chrono>
#include <random>
#include "QueueManager.hpp"

using ms = std::chrono::milliseconds;

std::random_device rand{};
std::mt19937 generator{rand()};
std::normal_distribution<unsigned int> nD{2000, 500};
std::uniform_int_distribution uD(0,9);

struct job
{
    unsigned int id = 0;
    ms pTime = ms(nD(generator));
    ms eTime = ms(0);
    ms iTime = ms(0);
    unsigned int priority = uD(generator);
    int pass = 0;
    Q currentQueue = FORE;

    bool operator<(const job& rhs) const
    {
        return priority < rhs.priority;
    }
};

int main()
{

}