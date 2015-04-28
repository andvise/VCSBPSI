#include "vcsbppsolver.h"
#include "include/structDef.h"
#include <vector>

VCSBPPSolver::VCSBPPSolver()
{
}
VCSBPPSolver::~VCSBPPSolver()
{
}
void VCSBPPSolver::setVolumes(int newSmallBinVolume,int newMediumBinVolume, int newLargeBinVolume)
{
    this->smallBinVolume = newSmallBinVolume;
    this->mediumBinVolume = newMediumBinVolume;
    this->largeBinVolume = newLargeBinVolume;
}
