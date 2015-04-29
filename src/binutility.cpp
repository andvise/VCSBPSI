#include "include/binutility.h"
#include <iostream>
using namespace std;

BinUtility::BinUtility()
{
}

std::vector<bin> BinUtility::binsBookerApplicator(binSet bookedBins, std::vector<bin> smallBins, std::vector<bin> mediumBins, std::vector<bin> largeBins)
{
    vector<bin> instance;

    for (int i = 0; i < smallBins.size(); i++)
    {
        instance.push_back(smallBins.at(i));

        if (i < bookedBins.small)
            instance.at(instance.size() - 1).cost = 0;
    }

    for (int i = 0; i < mediumBins.size(); i++)
    {
        instance.push_back(mediumBins.at(i));

        if (i < bookedBins.medium)
            instance.at(instance.size() - 1).cost = 0;
    }

    for (int i = 0; i < largeBins.size(); i++)
    {
        instance.push_back(largeBins.at(i));

        if (i < bookedBins.large)
            instance.at(instance.size() - 1).cost = 0;
    }
    sort(instance.begin(), instance.end());
    return instance;
}

std::vector<bin> BinUtility::binsSplitter(std::vector<bin> bins, int binVolume)
{
    vector<bin> instance;

    for (int i = 0; i < bins.size(); i++)
    {
        if (bins.at(i).volume == binVolume)
            instance.push_back(bins.at(i));
    }

    return instance;
}
void BinUtility::setAlpha(double newAlpha)
{
    alpha = newAlpha;
}
float BinUtility::costEvaluator(binSet bookedBins, binSet extraBins, std::vector<bin> smallBins, std::vector<bin> mediumBins, std::vector<bin> largeBins)
{
    int i;
    float cost = 0;

    for ( i = 0; i < bookedBins.small; i++)
        cost += smallBins.at(i).cost;

    for ( i = 0; i < bookedBins.medium; i++)
        cost += mediumBins.at(i).cost;

    for ( i = 0; i < bookedBins.large; i++)
        cost += largeBins.at(i).cost;

    for ( i = bookedBins.small; i < bookedBins.small + extraBins.small; i++)
        cost += alpha * smallBins.at(i).cost;

    for ( i = bookedBins.medium; i < bookedBins.medium + extraBins.medium; i++)
        cost += alpha * mediumBins.at(i).cost;

    for ( i = bookedBins.large; i < bookedBins.large + extraBins.large; i++)
        cost += alpha * largeBins.at(i).cost;

    return cost;
}


//generate an set of items
vector<item> BinUtility::itemsInstanceGenerator(int minNrItems, int maxNrItems, int percSmall, int percMedium, int percBig)
{
    vector<item> instance;
    item temp;

    if (percSmall + percMedium + percBig != 100)
    {
        cout << "Error, mistake in percantage on instance generator";
        return instance;
    }

    int itemsNr = minNrItems + rand() % (maxNrItems - minNrItems) + 1;

    for (int i = 0; i < itemsNr * percSmall / 100.0; i++)
    {
        temp.code = 1000 + i;
        temp.weight = smallItemMinVolume + rand() % (smallItemMaxVolume - smallItemMinVolume);
        instance.push_back(temp);
    }

    for (int i = 0; i < itemsNr * percMedium / 100.0; i++)
    {
        temp.code = 2000 + i;
        temp.weight = mediumItemMinVolume + rand() % (mediumItemMaxVolume - mediumItemMinVolume);
        instance.push_back(temp);
    }

    for (int i = 0; i < itemsNr * percBig / 100.0; i++)
    {
        temp.code = 3000 + i;
        temp.weight = largeItemMinVolume + rand() % (largeItemMaxVolume - largeItemMinVolume);
        instance.push_back(temp);
    }

    return instance;
}
void BinUtility::setItemVolumes(int newSmallItemMinVolume, int newSmallItemMaxVolume,int newMediumItemMinVolume, int newMediumItemMaxVolume,int newLargeItemMinVolume, int newLargeItemMaxVolume)
{
    smallItemMinVolume = newSmallItemMinVolume;
    smallItemMaxVolume = newSmallItemMaxVolume;
    mediumItemMinVolume = newMediumItemMinVolume;
    mediumItemMaxVolume = newMediumItemMaxVolume;
    largeItemMinVolume = newLargeItemMinVolume;
    largeItemMaxVolume = newLargeItemMaxVolume;
}

//generate an set of bins
vector<bin> BinUtility::binsInstanceGenerator(int totalVolume, int binVolume)
{
    vector<bin> instance;
    bin temp;
    totalVolume *= 1.4;
    temp.volume = binVolume;
    temp.volumeRemaining = binVolume;

    for (int i = 0; i < totalVolume / binVolume; i++)
    {
        temp.cost = (sqrt(binVolume) * (1.0 + ( (rand() % 60 - 30.0) / 100)));
        temp.code = binVolume * 10 + i;
        instance.push_back(temp);
    }

    return instance;
}
