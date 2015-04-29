#ifndef VCSBPPSOLVER_H
#define VCSBPPSOLVER_H
#include "include/structDef.h"
#include <vector>

class VCSBPPSolver
{
protected:
    int smallBinVolume, mediumBinVolume, largeBinVolume;
public:
    VCSBPPSolver();
    ~VCSBPPSolver();
    virtual binSet solveModel(std::vector<item> items, std::vector<bin> bins) = 0;
    void setVolumes(int newSmallBinVolume,int newMediumBinVolume, int newLargeBinVolume);

};

#endif // VCSBPPSOLVER_H
