#include <Common/getRandomASCIIString.h>
#include <Common/thread_local_rng.h>
#include <random>

namespace DB
{

String getRandomASCIIString(unsigned long len, char first, char last)
{
    std::uniform_int_distribution<int> distribution(first, last);
    String res(len, ' ');
    for (auto & c : res)
        c = distribution(thread_local_rng);
    return res;
}

}
