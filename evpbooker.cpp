#include "evpbooker.h"

EVPBooker::EVPBooker()
{
}

void EVPBooker::setTimeLimit(double newTimeLimit)
{
    timeLimit = newTimeLimit;

}

void EVPBooker::setScenarios(vector< vector <item> > newScenarioItems)
{
    int smallItemNr = 0,smallItemVolume = 0;
    int mediumItemNr = 0, mediumItemVolume = 0;
    int largeItemNr = 0,largeItemVolume = 0;
    scenarioNr = newScenarioItems.size();
    for (int i = 0; i < newScenarioItems.size();i++)
    {
        for(int j = 0; j < newScenarioItems.at(i).size(); j++)
        {
            if (newScenarioItems[i][j].weight <= 10)
            {
                smallItemNr++;
                smallItemVolume+=newScenarioItems[i][j].weight;
                break;
            }
            if (newScenarioItems[i][j].weight <= 25)
            {
                mediumItemNr++;
                mediumItemVolume+=newScenarioItems[i][j].weight;
                break;
            }
            largeItemNr++;
            largeItemVolume+=newScenarioItems[i][j].weight;
            break;
        }
    }
    //vise manca la parte da aggiungere per mediare gli item

}

binSet EVPBooker::bookBins(std::vector<bin> bins)
{
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    const int binSize = bins.size();
    int i, j;
    int itemSize = scenarioItems.at(0).size();
    for (i = 1; i < scenarioNr; i++)
    {
        if (scenarioItems.at(i).size() > itemSize)
            itemSize = scenarioItems.at(i).size();
    }
    GRBVar binsVar[binSize];
    GRBVar extraBinsVar[scenarioNr][binSize];
    GRBVar itemVars[scenarioNr][binSize][itemSize];
    model.getEnv().set(GRB_DoubleParam_TimeLimit, timeLimit);
    model.getEnv().set(GRB_IntParam_LogToConsole, 0);

    // Create the bin selected variables
    for (i = 0; i < binSize ; i++)
        binsVar[i] = model.addVar(0.0, 1.0, bins.at(i).cost, GRB_BINARY);

    // Create the bin extra variable
    for (i = 0; i < scenarioNr; i++)
        for (j = 0; j < binSize ; j++)
            extraBinsVar[i][j] = model.addVar(0.0, 1.0, this->alpha * bins.at(j).cost / scenarioNr, GRB_BINARY);


    // Create the bin assignment variables
    for (int k = 0; k < scenarioNr; k++)
        for (i = 0; i < binSize ; i++)
            for(j = 0; j < scenarioItems[k].size() ; j++)
                itemVars[k][i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);

    // Integrate variables into model
    model.update();

    // Add constraints
    // Each items has to be packed in a bin
    for (int k = 0; k < scenarioNr; k++)
        for(j = 0; j < scenarioItems[k].size() ; j++)
        {
            GRBLinExpr expr = 0;
            for (i = 0; i < binSize ; i++)
                expr += itemVars[k][i][j];

            model.addConstr(expr, GRB_EQUAL, 1.0);
        }

    // The volume of the bin cannot be exceeded
    for (int k = 0; k < scenarioNr; k++)
        for (i = 0; i < binSize ; i++)
        {
            GRBLinExpr expr = 0;

            for(j = 0; j < scenarioItems[k].size() ; j++)
                expr += scenarioItems[k].at(j).weight * itemVars[k][i][j];

            expr -= bins.at(i).volume * (binsVar[i] + extraBinsVar[k][i]);
            model.addConstr(expr <= 0.0);
        }

    // Add constraint to avoid that a bin is taken twice
    for (i = 0; i < binSize ; i++)
    {
        GRBLinExpr expr = 0;
        expr += scenarioNr * binsVar[i];
        for (int k = 0; k < scenarioNr; k++)
            expr += extraBinsVar[k][i];
        model.addConstr(expr <= scenarioNr * 1.0);
    }

    // Optimize model

    model.update();
    model.optimize();
    binSet output;
    output.small = 0;
    output.medium = 0;
    output.large = 0;

    for (i = 0; i < binSize ; i++)
    {
        if (binsVar[i].get(GRB_DoubleAttr_X) > 0.5)
        {
            if (bins.at(i).volume == smallBinVolume)
                output.small++;
            if (bins.at(i).volume == mediumBinVolume)
                output.medium++;
            if (bins.at(i).volume == largeBinVolume)
                output.large++;
        }
    }
    //inputWriter << model.get(GRB_DoubleAttr_MIPGap) <<"\t";
    return output;
}
