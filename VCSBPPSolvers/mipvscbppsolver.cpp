#include "mipvscbppsolver.h"

MIPVSCBPPSolver::MIPVSCBPPSolver()
{

}
MIPVSCBPPSolver::MIPVSCBPPSolver(double newTimeLimit)
{
    timeLimit = newTimeLimit;
}

void MIPVSCBPPSolver::setTimeLimit(double newTimeLimit)
{
    timeLimit = newTimeLimit;
}

void MIPVSCBPPSolver::setOutFile(const string & fileName)
{
    gapFile.open(fileName, ios_base::app);
    useOutFile = true;
}

binSet MIPVSCBPPSolver::solveModel(std::vector<item> items, std::vector<bin> bins)
{

    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    const int binSize = bins.size();
    const int itemSize = items.size();
    GRBVar binsVar[binSize];
    GRBVar itemVars[binSize][itemSize];
    int i, j;
    model.getEnv().set(GRB_DoubleParam_TimeLimit, timeLimit);
    model.getEnv().set(GRB_IntParam_LogToConsole, 0);

    // Create the bin selected variables
    for (i = 0; i < binSize ; i++)
        binsVar[i] = model.addVar(0.0, 1.0, bins.at(i).cost, GRB_BINARY);

    // Create the bin assignment variables
    for (i = 0; i < binSize ; i++)
        for(j = 0; j < itemSize ; j++)
            itemVars[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY);

    // Integrate variables into model
    model.update();

    // Add constraints
    // Each items has to be packed in a bin
    for(j = 0; j < itemSize ; j++)
    {
        GRBLinExpr expr = 0;

        for (i = 0; i < binSize ; i++)
            expr += itemVars[i][j];

        model.addConstr(expr, GRB_EQUAL, 1.0);
    }

    // The volume of the bin cannot be exceeded
    for (i = 0; i < binSize ; i++)
    {
        GRBLinExpr expr = 0;

        for(j = 0; j < itemSize ; j++)
            expr += items.at(j).weight * itemVars[i][j];

        expr -= bins.at(i).volume * binsVar[i];
        model.addConstr(expr <= 0.0);
    }

    // Optimize model
    model.optimize();
    binSet output;
    output.small = 0;
    output.medium = 0;
    output.large = 0;

    if (useOutFile) gapFile << std::fixed << std::setw( 4 )  << std::setprecision(4)  << std::setfill( '0' ) << model.get(GRB_DoubleAttr_MIPGap) <<"\n";

    for (i = 0; i < binSize ; i++)
    {
        if ((binsVar[i].get(GRB_DoubleAttr_X) > 0.5) && (bins.at(i).cost > 0))
        {
            if (bins.at(i).volume == smallBinVolume)
                output.small++;
            if (bins.at(i).volume == mediumBinVolume)
                output.medium++;
            if (bins.at(i).volume == largeBinVolume)
                output.large++;
        }
    }
    return output;
}
