#ifndef BASBOOKER_H
#define BASBOOKER_H
#include <vector>
#include "VCSBPPSolvers/vcsbppsolver.h"
#include "VCSBPSIBookers/vcsbpsibooker.h"
#include "VCSBPSIBookers/averagebooker.h"
class BASBooker: public VCSBPSIBooker
{
private:
    int iterationNr = 10;
    int averageVolume;
    double volumePerc = 1.0;
    BinUtility *binUt;
    VCSBPPSolver *solver;
    double minPerc = 0.6 , maxPerc = 1;

public:
    BASBooker();
    binSet bookBins(std::vector<bin> bins);
    void training(vector<vector<bin> > scenarioBins,vector<vector<item> > scenarioItems);
    void setVCSBPPSolver(VCSBPPSolver* newSolver);
};
#endif // BASBOOKER_H
