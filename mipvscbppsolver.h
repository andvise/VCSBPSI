#ifndef MIPVSCBPPSOLVER_H
#define MIPVSCBPPSOLVER_H
#include "vcsbppsolver.h"
#include "gurobi_c++.h"

class MIPVSCBPPSolver : public VCSBPPSolver
{
private:
    double timeLimit;
public:
    MIPVSCBPPSolver(int newSmallBinVolume,int newMediumBinVolume, int newLargeBinVolume);
    MIPVSCBPPSolver();
    void setTimeLimit(double newTimeLimit);

    binSet solveModel(std::vector<item> items, std::vector<bin> bins);

};

#endif // MIPVSCBPPSOLVER_H
