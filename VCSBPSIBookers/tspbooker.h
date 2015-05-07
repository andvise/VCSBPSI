#ifndef TPSBOOKER_H
#define TPSBOOKER_H
#include "vcsbpsibooker.h"
#include "gurobi_c++.h"
#include <fstream>
#include <iomanip>      // std::setfill, std::setw

class TSPBooker : public VCSBPSIBooker
{
private:
    double timeLimit;
    std::vector< std::vector <item> > scenarioItems;
    int scenarioNr;
    ofstream gapFile;
    bool useOutFile = false;
public:
    TSPBooker();
    void setOutFile(const string & fileName);
    void setScenarios(std::vector< std::vector <item> > scenarioItems);
    binSet bookBins(std::vector<bin> bins);
    void setTimeLimit(double newTimeLimit);

};

#endif // TPSBOOKER_H
