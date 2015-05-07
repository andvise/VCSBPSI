#ifndef MIPVSCBPPSOLVER_H
#define MIPVSCBPPSOLVER_H
#include "vcsbppsolver.h"
#include "gurobi_c++.h"
#include <fstream>
#include <iomanip>

class MIPVSCBPPSolver : public VCSBPPSolver
{
private:
    double timeLimit;
    ofstream gapFile;
    bool useOutFile = false;
public:
    MIPVSCBPPSolver(double newTimeLimit);
    MIPVSCBPPSolver();
    void setTimeLimit(double newTimeLimit);
    void setOutFile(const string & fileName);
    binSet solveModel(std::vector<item> items, std::vector<bin> bins);

};

#endif // MIPVSCBPPSOLVER_H
