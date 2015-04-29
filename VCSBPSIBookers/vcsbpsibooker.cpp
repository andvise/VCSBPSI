#include "vcsbpsibooker.h"

VCSBPSIBooker::VCSBPSIBooker()
{
}
void VCSBPSIBooker::setVolumes(int newSmallBinVolume,int newMediumBinVolume, int newLargeBinVolume)
{
    smallBinVolume = newSmallBinVolume;
    mediumBinVolume = newMediumBinVolume;
    largeBinVolume = newLargeBinVolume;
}
void VCSBPSIBooker::setAlpha(double newAlpha)
{
    alpha = newAlpha;
}
