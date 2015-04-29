#ifndef ABDF_H
#define ABDF_H
#include "vcsbppsolver.h"

class ABDF: public VCSBPPSolver
{
public:
    ABDF();
    ~ABDF();
    ABDF(int newSmallBinVolume,int newMediumBinVolume, int newLargeBinVolume);

    binSet solveModel(std::vector<item> items, std::vector<bin> bins);

};

#endif // ABDF_H
