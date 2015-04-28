#ifndef STRUCTDEF_H
#define STRUCTDEF_H

#include <vector>
#include <algorithm>

using namespace std;
struct pairs
{
    int itemNr;
    int binNr;
};

struct bin
{
    int code;
    int volume;
    float cost;
    int volumeRemaining;
    bool operator<(const bin& i) const
    {
        return (volume/cost) < (i.volume/i.cost);

    }
};
struct binSet
{
    int small = 0;
    int medium = 0;
    int large = 0;
};


struct item
{
    int code;

    int weight;
    bool operator<(const item& i) const
    {
        return weight < i.weight;

    }
};




#endif // STRUCTDEF_H
