#ifndef EBM_H
#include "structDef.h"

#define EBM_H

class EBM
{
private:
    binSet data;
    int scenarioNr;
public:
    EBM();
    void addScenario(binSet newScenario);
    float getSmallPerc();
    float getMediumPerc();
    float getLargePerc();
    void setNew(int newScenarioNr, float smallPerc, float mediumPerc,float largePerc);
    int getScenario();

};

#endif // EBM_H
