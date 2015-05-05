#include "basbooker.h"
#include <iostream>
#include <iomanip>      // std::setfill, std::setw

using namespace std;

BASBooker::BASBooker()
{
    binUt = new BinUtility();

}

binSet BASBooker::bookBins(std::vector<bin> bins)

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


void BASBooker::training(vector<vector<bin> > scenarioBins,vector<vector<item> > scenarioItems)
{
    struct binCostComparator {
        bool operator() (const bin& i, const bin& j) const {
            return (i.cost < j.cost);
        }
    } binCostComp;
    binUt->setAlpha(alpha);
    averageVolume = 0;
    for(int a = 0; a < scenarioItems.size() ; a++)
    {
        for (int b = 0; b < scenarioItems.at(a).size(); b++)
            averageVolume += scenarioItems.at(a).at(b).weight;
    }
    averageVolume /= scenarioItems.size();
    AverageBooker *avBooker= new AverageBooker();
    avBooker->setVolumes(smallBinVolume,mediumBinVolume,largeBinVolume);    
    avBooker->setAlpha(alpha);
    avBooker->setScenarios(scenarioItems);
    binSet bookSet, extraSet;
    for (int i = 0; i < iterationNr; i++)
    {
        double costUp = 0, costLow = 0;
        double lowPerc = minPerc + (maxPerc - minPerc)/4;
        double upPerc = maxPerc - (maxPerc - minPerc)/4;

        for (int j = 0; j < scenarioBins.size(); j++)
        {
            avBooker->setVolumePerc(lowPerc);
            bookSet = avBooker->bookBins(scenarioBins.at(j));
            vector <bin> smallBins = binUt->binsSplitter(scenarioBins.at(j),smallBinVolume);
            sort(smallBins.begin(), smallBins.end(), binCostComp);

            vector <bin> mediumBins = binUt->binsSplitter(scenarioBins.at(j),mediumBinVolume);
            sort(mediumBins.begin(), mediumBins.end(), binCostComp);

            vector <bin> largeBins = binUt->binsSplitter(scenarioBins.at(j),largeBinVolume);
            sort(largeBins.begin(), largeBins.end(), binCostComp);

            //devo salvarli per evitare di risolvere il modello
            extraSet =solver->solveModel(scenarioItems.at(j), binUt->binsBookerApplicator(bookSet, smallBins, mediumBins, largeBins));
            costLow += binUt->costEvaluator(bookSet, extraSet, smallBins, mediumBins, largeBins);

        }
        for (int j = 0; j < scenarioBins.size(); j++)
        {
            avBooker->setVolumePerc(upPerc);
            bookSet = avBooker->bookBins(scenarioBins.at(j));
            vector <bin> smallBins = binUt->binsSplitter(scenarioBins.at(j),smallBinVolume);
            sort(smallBins.begin(), smallBins.end(), binCostComp);

            vector <bin> mediumBins = binUt->binsSplitter(scenarioBins.at(j),mediumBinVolume);
            sort(mediumBins.begin(), mediumBins.end(), binCostComp);

            vector <bin> largeBins = binUt->binsSplitter(scenarioBins.at(j),largeBinVolume);
            sort(largeBins.begin(), largeBins.end(), binCostComp);

            //devo salvarli per evitare di risolvere il modello
            extraSet = solver->solveModel(scenarioItems.at(j), binUt->binsBookerApplicator(bookSet, smallBins, mediumBins, largeBins));
            costUp += binUt->costEvaluator(bookSet, extraSet, smallBins, mediumBins, largeBins);

        }

        (costUp > costLow)? maxPerc= upPerc : minPerc = lowPerc;

    }

    volumePerc = (maxPerc + minPerc)/2;
    cout << "la percentuale migliore è:"<< fixed << setw(5)  << setprecision(3)  << setfill( '0' )  <<volumePerc;

}

void BASBooker::setVCSBPPSolver(VCSBPPSolver* newSolver)
{
    solver = newSolver;
}
