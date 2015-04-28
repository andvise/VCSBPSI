#include "../include/EBM.h"
#include "../include/structDef.h"
#include <iostream>


EBM::EBM()
{
    scenarioNr = 0;
}
void EBM::addScenario(binSet newScenario)
{
    scenarioNr++;
    data.small += newScenario.small;
    data.medium += newScenario.medium;
    data.large += newScenario.large;
}

float EBM::getSmallPerc()
{
    if (scenarioNr!=0)
        return data.small*1.0/scenarioNr;
    return 0;
}

float EBM::getMediumPerc()
{
    if (scenarioNr!=0)
        return data.medium*1.0/scenarioNr;
    return 0;
}

float EBM::getLargePerc()
{
    if (scenarioNr!=0)
        return data.large*1.0/scenarioNr;
    return 0;
}
