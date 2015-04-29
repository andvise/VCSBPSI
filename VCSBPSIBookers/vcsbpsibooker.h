#ifndef VCSBPSIBOOKER_H
#define VCSBPSIBOOKER_H
#include "include/structDef.h"
#include <vector>
#include "include/binutility.h"
#include "VCSBPSIBookers/vcsbpsibooker.h"


class VCSBPSIBooker
{
protected:
    int smallBinVolume, mediumBinVolume, largeBinVolume;
    double alpha;
public:
    VCSBPSIBooker();
    virtual binSet bookBins(std::vector<bin> bins) = 0;
    void setVolumes(int newSmallBinVolume,int newMediumBinVolume, int newLargeBinVolume);
    void setAlpha(double newAlpha);
};

#endif // VCSBPSIBOOKER_H
