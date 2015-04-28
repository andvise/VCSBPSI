#ifndef EVPBOOKER_H
#define EVPBOOKER_H

#include "vcsbpsibooker.h"
#include "gurobi_c++.h"

class EVPBooker: public VCSBPSIBooker
{
private:
    double timeLimit;
    std::vector <item> averageItemSet;
    int scenarioNr;
public:
    EVPBooker();

    void setScenarios(std::vector< std::vector <item> > scenarioItems);
    binSet bookBins(std::vector<bin> bins);
    void setTimeLimit(double newTimeLimit);
};

#endif // EVPBOOKER_H
