#include "averagebooker.h"

using namespace std;

AverageBooker::AverageBooker()
{
    binUt = new BinUtility();
}
binSet AverageBooker::bookBins(std::vector<bin> bins)
{
    struct binCostComparator {
        bool operator() (const bin& i, const bin& j) const {
            return (i.cost < j.cost);
        }
    } binCostComp;
    binSet bookedSet;
    int bookedVolume;
    vector <bin> smallBins = binUt->binsSplitter(bins,smallBinVolume);
    sort(smallBins.begin(), smallBins.end(), binCostComp);

    vector <bin> mediumBins = binUt->binsSplitter(bins,mediumBinVolume);
    sort(mediumBins.begin(), mediumBins.end(), binCostComp);

    vector <bin> largeBins = binUt->binsSplitter(bins,largeBinVolume);
    sort(largeBins.begin(), largeBins.end(), binCostComp);

    do {
        if(bookedSet.small < smallBins.size())
        {
            if ((bookedSet.medium < mediumBins.size()) &&
                    (smallBins.at(bookedSet.small).volume * 1.0 / smallBins.at(bookedSet.small).cost > mediumBins.at(bookedSet.medium).volume * 1.0 / mediumBins.at(bookedSet.medium).cost))
            {
                if ((bookedSet.large < largeBins.size()) &&
                        (smallBins.at(bookedSet.small).volume * 1.0 / smallBins.at(bookedSet.small).cost > largeBins.at(bookedSet.large).volume * 1.0 / largeBins.at(bookedSet.large).cost))
                {
                    bookedVolume += smallBins.at(bookedSet.small).volume;
                    bookedSet.small++;
                } else {
                    bookedVolume += largeBins.at(bookedSet.large).volume;
                    bookedSet.large++;
                }
            } else
            {
                if ((bookedSet.large < largeBins.size()) &&
                        (mediumBins.at(bookedSet.medium).volume * 1.0 / mediumBins.at(bookedSet.medium).cost > largeBins.at(bookedSet.large).volume * 1.0 / largeBins.at(bookedSet.large).cost))
                {
                    bookedVolume += mediumBins.at(bookedSet.medium).volume;
                    bookedSet.medium++;
                } else {
                    bookedVolume += largeBins.at(bookedSet.large).volume;
                    bookedSet.large++;
                }
            }
        } else
        {
            if ((bookedSet.medium < mediumBins.size()) &&
                    (mediumBins.at(bookedSet.medium).volume * 1.0 / mediumBins.at(bookedSet.medium).cost > largeBins.at(bookedSet.large).volume * 1.0 / largeBins.at(bookedSet.large).cost))
            {
                bookedVolume += mediumBins.at(bookedSet.medium).volume;
                bookedSet.medium++;
            } else {
                bookedVolume += largeBins.at(bookedSet.large).volume;
                bookedSet.large++;
            }
        }
    } while (bookedVolume <= volumePerc * averageVolume);
    return bookedSet;
}

void AverageBooker::setVolumePerc(double newVolumePerc)
{
    volumePerc = newVolumePerc;
}

void AverageBooker::setScenarios(vector <vector<item>> scenarioItemSets)
{
    binUt->setAlpha(alpha);
    averageVolume = 0;
    for(int a = 0; a < scenarioItemSets.size() ; a++)
    {
        for (int b = 0; b < scenarioItemSets.at(a).size(); b++)
            averageVolume += scenarioItemSets.at(a).at(b).weight;
    }
    averageVolume /= scenarioItemSets.size();

}
