#ifndef MPPBOOKER_H
#define MPPBOOKER_H
#include"include/EBM.h"
#include <vector>
#include "VCSBPPSolvers/vcsbppsolver.h"
#include "VCSBPSIBookers/vcsbpsibooker.h"
#include <fstream>
#include <cstring>
#include <iomanip>      // std::setfill, std::setw

using namespace std;

class MPPBooker : public VCSBPSIBooker
{
private:
    BinUtility *binUt;
    vector< vector< vector <int> > > flag;
    vector< vector < vector <EBM> > > matrixEBM;
    int smallBinsNr, mediumBinsNr, largeBinsNr;
    int trainingScenarioNr = 0;
    VCSBPPSolver *solver;
    double lowerBoundMPP = 0.7;
    double limitExpectedExtraBins = 1.3;
    int averageVolume;
    void trainingScenario(vector<bin> bins, vector<item> items);

public:
    void matrixRead(const string & fileName);
    void matrixPrint(const string & fileName);
    MPPBooker(int newSmallBinsNr, int newMediumBinsNr, int newLargeBinsNr);
    binSet bookBins(vector<bin> bins);
    void setVCSBPPSolver(VCSBPPSolver* newSolver);
    void lazyEvaluation(int i, int j, int k, int s, int m, int l);
    void training(vector<vector<bin> > scenarioBins,vector<vector<item> > scenarioItems);


};

#endif // MPPBOOKER_H
