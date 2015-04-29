#ifndef TPSBOOKER_H
#define TPSBOOKER_H
#include "vcsbpsibooker.h"
#include "gurobi_c++.h"

class TSPBooker : public VCSBPSIBooker
{
private:
    double timeLimit;
    std::vector< std::vector <item> > scenarioItems;
    int scenarioNr;
public:
    TSPBooker();

    void setScenarios(std::vector< std::vector <item> > scenarioItems);
    binSet bookBins(std::vector<bin> bins);
    void setTimeLimit(double newTimeLimit);

};

#endif // TPSBOOKER_H
