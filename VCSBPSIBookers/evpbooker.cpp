#include "evpbooker.h"

EVPBooker::EVPBooker()
{
    solver = new MIPVSCBPPSolver();  
}

void EVPBooker::setTimeLimit(double newTimeLimit)
{
    timeLimit = newTimeLimit;
    solver->setTimeLimit(timeLimit);    
}

void EVPBooker::setScenarios(vector< vector <item> > newScenarioItems)
{
    int smallItemNr = 0,smallItemVolume = 0;
    int mediumItemNr = 0, mediumItemVolume = 0;
    int largeItemNr = 0,largeItemVolume = 0;
    scenarioNr = newScenarioItems.size();
    averageItemSet.clear();
    for (int i = 0; i < scenarioNr;i++)
    {
        for(int j = 0; j < newScenarioItems.at(i).size(); j++)
        {
            if (newScenarioItems[i][j].code < 2000)
            {
                smallItemNr++;
                smallItemVolume+=newScenarioItems[i][j].weight;
            }else
            {
                if (newScenarioItems[i][j].weight <= 25)
                {
                    mediumItemNr++;
                    mediumItemVolume+=newScenarioItems[i][j].weight;
                }else 
                {
                    largeItemNr++;
                    largeItemVolume+=newScenarioItems[i][j].weight;
                }
            }
        }
    }
    item a;
    a.code = 1000;
    a.weight = (int)(smallItemVolume/smallItemNr+0.5);
    for (int i = 0; i < (int)(smallItemNr/scenarioNr + 0.5); i++)
        averageItemSet.push_back(a);
    
    a.code = 2000;
    a.code = (int)(mediumItemNr/scenarioNr + 0.5);
    a.weight = (int)(mediumItemVolume/mediumItemNr +0.5);
    for (int i = 0; i < (int)(mediumItemNr/scenarioNr + 0.5); i++)
        averageItemSet.push_back(a);
    
    a.code = 3000;
    int c = (int)(largeItemVolume/largeItemNr + 0.5);
    a.weight = (int)(largeItemVolume/largeItemNr + 0.5);
    for (int i = 0; i < (int)(largeItemNr/scenarioNr + 0.5); i++)
        averageItemSet.push_back(a);

}

binSet EVPBooker::bookBins(std::vector<bin> bins)
{
    solver->setVolumes(smallBinVolume,mediumBinVolume,largeBinVolume);
    return solver->solveModel(averageItemSet,bins);
}
