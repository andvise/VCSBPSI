#ifndef AVERAGEBOOKER_H
#define AVERAGEBOOKER_H
#include "vcsbpsibooker.h" 

class AverageBooker : public VCSBPSIBooker
{
private:
    int averageVolume;
    double volumePerc = 1.0;
    BinUtility *binUt;
public:
    AverageBooker();
    binSet bookBins(std::vector<bin> bins);
    void setVolumePerc(double newVolumePerc);
    void setScenarios(std::vector <std::vector<item>> scenarioItemSets);
};

#endif // AVERAGEBOOKER_H
