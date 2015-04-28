#include "abdf.h"
#include <algorithm>    // std::sort
#include <vector>

using namespace std;


ABDF::ABDF()
{
}
ABDF::~ABDF()
{
    this->setVolumes(0,0,0);
}

ABDF::ABDF(int newSmallBinVolume,int newMediumBinVolume, int newLargeBinVolume)
{
    this->smallBinVolume = newSmallBinVolume;
    this->mediumBinVolume = newMediumBinVolume;
    this->largeBinVolume = newLargeBinVolume;
}

struct binComp {
    bool operator() (const bin& i, const bin& j) const {
        return (i.volumeRemaining < j.volumeRemaining);
    }
} upComp;


binSet ABDF::solveModel(std::vector<item> items, std::vector<bin> bins)
{
    vector<bin> bookedBins;
    binSet extra;

    bookedBins.push_back(bins.at(bins.size() - 1));
    if(bookedBins.at(bookedBins.size() - 1).cost != 0)
    {
        if (bookedBins.at(bookedBins.size() - 1).volume == smallBinVolume)
            extra.small++;
        if (bookedBins.at(bookedBins.size() - 1).volume == mediumBinVolume)
            extra.medium++;
        if (bookedBins.at(bookedBins.size() - 1).volume == largeBinVolume)
            extra.large++;
    }

    bins.pop_back();

    //  cout << "prenotato il  bin " << bookedBins.at(0).code << " con il volume " << bookedBins.at(0).volume << "\n" ;
    for (int i = items.size() - 1; i >= 0 ; i--)
    {
        if (!( items.at(i).weight <= bookedBins.at(bookedBins.size() - 1).volumeRemaining ))
        {
            bookedBins.push_back(bins.at(bins.size() - 1));
            bins.pop_back();

            if(bookedBins.at(bookedBins.size() - 1).cost != 0)
            {
                if (bookedBins.at(bookedBins.size() - 1).volume == smallBinVolume)
                    extra.small++;
                if (bookedBins.at(bookedBins.size() - 1).volume == mediumBinVolume)
                    extra.medium++;
                if (bookedBins.at(bookedBins.size() - 1).volume == largeBinVolume)
                    extra.large++;
            }

            //cout <<"prenotato il  bin " << bookedBins.at(bookedBins.size()-1).code << " con il volume " << bookedBins.at(bookedBins.size()-1).volume  << "\n" ;
        }

        bookedBins.at(bookedBins.size() - 1).volumeRemaining -= items.at(i).weight;
        //cout <<"item "<< items.at(i).code << " nel bin " << bookedBins.at(bookedBins.size()-1).code << " con il volume rimanente "<< bookedBins.at(bookedBins.size()-1).volumeRemaining<< "\n" ;
        sort(bookedBins.begin(), bookedBins.end(), upComp);
    }

    // cout<<"Il valore dell'bdf è  "<< extra.small<< ","<< extra.medium<< " "<< extra.large<< "\n";
    return extra;
}
