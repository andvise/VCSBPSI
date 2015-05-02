#ifndef BINUTILITY_H
#define BINUTILITY_H
#include <time.h>
#include <vector>
#include "include/structDef.h"

class BinUtility
{
private:
    int smallItemMinVolume, smallItemMaxVolume,
        mediumItemMinVolume, mediumItemMaxVolume,
        largeItemMinVolume, largeItemMaxVolume;
    double alpha;
public:
    BinUtility();
    std::vector<bin> binsBookerApplicator(binSet bookedBins, std::vector<bin> smallBins, std::vector<bin> mediumBins, std::vector<bin> largeBins);
    std::vector<bin> binsSplitter(std::vector<bin> bins, int binVolume);  
    std::vector<item> itemsInstanceGenerator(int minNrItems, int maxNrItems, int percSmall, int percMedium, int percBig);
    std::vector<bin> binsInstanceGenerator(int totalVolume, int binVolume);
    void setItemVolumes(int newSmallItemMinVolume, int newSmallItemMaxVolume,int newMediumItemMinVolume, int newMediumItemMaxVolume,int newLargeItemMinVolume, int newLargeItemMaxVolume);
    void setAlpha(double newAlpha);
    float costEvaluator(binSet bookedBins, binSet extraBins, std::vector<bin> smallBins, std::vector<bin> mediumBins, std::vector<bin> largeBins);
};

#endif // BINUTILITY_H
