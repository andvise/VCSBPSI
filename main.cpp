#include <QCoreApplication>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "include/structDef.h"
#include <cstring>
#include <iomanip>
#include <limits>
#include "gurobi_c++.h"
#include <stdlib.h>
#include <algorithm>
#include <unistd.h>
#include "include/EBM.h"
#include "abdf.h"
#include "mipvscbppsolver.h"
#include "tspbooker.h"
#include "vcsbppsolver.h"
#include "mppbooker.h"
#include "binutility.cpp"
#include "binutility.h"
#include "averagebooker.h"

using namespace std;

#define SMALLITEMMINVOLUME 5
#define SMALLITEMMAXVOLUME 10
#define MEDIUMITEMMINVOLUME 15
#define MEDIUMITEMMAXVOLUME 25
#define LARGEITEMMINVOLUME 20
#define LARGEITEMMAXVOLUME 40

#define SMALLBINVOLUME 50
#define MEDIUMBINVOLUME 70
#define LARGEBINVOLUME 90


int ITEMSMIN = 40;        //size of the problem
int ITEMSMAX = 50;
#define LOWERBOUNDEBM 0.7   //lower bound to evaluate the EBM position
float ALPHAINCREMENT = 1.3;  //alpha increment of the bins cost
float MAXLIMITAVERAGE = 0.7;   //maximum value of an EBM position for being a valid solution for the second phase
#define SMALLITEMPERC 30     //various percentage of the items
#define MEDIUMITEMPERC 40
#define LARGEBINPERC 30


#define MATRIXREADY false
#define STOCHASTICSCENARIONR 25
#define AVERAGESCENARIONR 50
#define TRAINNR 25
#define TESTNR 10

#define PERCVOLUME 0.85

#define BDFORGUROBI false
#define BDFORGUROBITRAIN true
#define USESTHOCASTIC true
#define NOLAZYCOMPARE false


ofstream inputWriter;
ofstream gurobiData;



struct binCompCheap {
    bool operator() (const bin& i, const bin& j) const {
        return (i.cost < j.cost);
    }
} cheapComp;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //initialization time for rand
    srand(time(NULL));

    //initialization bin utility
    BinUtility *binUt = new BinUtility();
    binUt->setItemVolumes(SMALLITEMMINVOLUME,SMALLITEMMAXVOLUME,MEDIUMITEMMINVOLUME,MEDIUMITEMMAXVOLUME,LARGEITEMMINVOLUME,LARGEITEMMAXVOLUME);
    binUt->setAlpha(ALPHAINCREMENT);

    bool commandOutput = true;
    struct binCostComparator {
        bool operator() (const bin& i, const bin& j) const {
            return (i.cost < j.cost);
        }
    } binCostComp;

    vector<VCSBPSIBooker*> bookers;
    vector<string> bookerNames;
    vector<binSet> extraSets;
    vector<binSet> totExtraBins;
    vector<binSet> bookedSets;
    vector<double> singleCost;
    vector<double> totalCost;

    if (commandOutput) cout <<"Items number:\t["<<ITEMSMIN<<";"<<ITEMSMAX<<"]\nTrain number:\t" << TRAINNR << "\nTest number:\t"<<TESTNR <<"\n\n";

    vector<bin> smallBins;
    vector<bin> mediumBins;
    vector<bin> largeBins;
    vector<item> items;
    binSet emptySet;

    //part to estimate the average volume... I have to remove it
    if (commandOutput) cout <<"Calculating average volume...\n" ;
    int averageVolume = 0;
    for(int a = 0; a < AVERAGESCENARIONR; a++)
    {
        items = binUt->itemsInstanceGenerator(ITEMSMIN, ITEMSMAX, SMALLITEMPERC, MEDIUMITEMPERC, LARGEBINPERC);
        for (int b = 0; b < items.size(); b++)
            averageVolume += items.at(b).weight;
    }
    averageVolume /= AVERAGESCENARIONR;

    if (commandOutput) cout <<"The average volume is "<< averageVolume <<"\n" ;

    //CREATING THE SCENARIOS
    if (commandOutput) cout <<"Creating the training scenarios...\n" ;
    vector< vector< item > > scenarioItemSets;
    vector< vector< bin > > scenarioBinSets;

    for (int i = 0; i < TRAINNR; i++)
    {

        scenarioItemSets.push_back(binUt->itemsInstanceGenerator(ITEMSMIN, ITEMSMAX, SMALLITEMPERC, MEDIUMITEMPERC, LARGEBINPERC));
        scenarioBinSets.push_back(binUt->binsBookerApplicator(emptySet, binUt->binsInstanceGenerator(averageVolume, SMALLBINVOLUME), binUt->binsInstanceGenerator(averageVolume, MEDIUMBINVOLUME), binUt->binsInstanceGenerator(averageVolume, LARGEBINVOLUME)));
    }

    //intialization VCSBPP solvers
#if BDFORGUROBI
    if (commandOutput) cout <<"Initialization of A-BDF, the VCSBPP solver...\n" ;

    ABDF *VCSBPP = new ABDF();
    VCSBPP->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
#else
    if (commandOutput) cout <<"Initialization of MIP VCSBPP solver...\n" ;
    MIPVSCBPPSolver *VCSBPP = new MIPVSCBPPSolver();
    VCSBPP->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
    VCSBPP->setTimeLimit(3.0);
#endif


    smallBins = binUt->binsInstanceGenerator(averageVolume, SMALLBINVOLUME);
    mediumBins = binUt->binsInstanceGenerator(averageVolume, MEDIUMBINVOLUME);
    largeBins = binUt->binsInstanceGenerator(averageVolume, LARGEBINVOLUME);

//initialization Booker

    //MPP A-BDF
    if (commandOutput) cout <<"Initialization of MPP A-BDF booker...\n" ;
#if BDFORGUROBITRAIN
    VCSBPPSolver *trainingSolver = new ABDF();
#else
    VCSBPPSolver *trainingSolver = new MIPVSCBPPSolver();
    trainingSolver->setTimeLimit(5.0);
#endif
    trainingSolver->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);

    MPPBooker *mppSolverABDF = new MPPBooker(smallBins.size(),mediumBins.size(),largeBins.size());
    mppSolverABDF->setVCSBPPSolver(trainingSolver);
    mppSolverABDF->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
    mppSolverABDF->setAlpha(ALPHAINCREMENT);

    if (MATRIXREADY)
    {
        if (commandOutput) cout <<"Using the EBM matrix in file X...\n" ;

    }else
    {
        if (commandOutput) cout << "Training of th EBM matrix...\n" ;
        mppSolverABDF->training(scenarioBinSets,scenarioItemSets);

    }
    bookers.push_back(mppSolverABDF);
    bookerNames.push_back("MPP-B");
    bookedSets.push_back(emptySet);
    extraSets.push_back(emptySet);
    totExtraBins.push_back(emptySet);
    totalCost.push_back(0);
    singleCost.push_back(0);

    if (commandOutput) cout << "Training of th EBM matrix complete!\n" ;


    //MPP MIP
    if (commandOutput) cout <<"Initialization of MPP MIP booker...\n" ;
    MIPVSCBPPSolver *trainingSolverMIP = new MIPVSCBPPSolver();
    trainingSolverMIP->setTimeLimit(2.0);
    trainingSolverMIP->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);

    MPPBooker *mppSolverMIP = new MPPBooker(smallBins.size(),mediumBins.size(),largeBins.size());
    mppSolverMIP->setVCSBPPSolver(trainingSolverMIP);
    mppSolverMIP->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
    mppSolverMIP->setAlpha(ALPHAINCREMENT);

    if (MATRIXREADY)
    {
        if (commandOutput) cout <<"Using the EBM matrix in file X...\n" ;

    }else
    {
        if (commandOutput) cout << "Training of th EBM matrix...\n" ;
        mppSolverMIP->training(scenarioBinSets,scenarioItemSets);

    }
    bookers.push_back(mppSolverMIP);
    bookerNames.push_back("MPP-M");
    bookedSets.push_back(emptySet);
    extraSets.push_back(emptySet);
    totExtraBins.push_back(emptySet);
    totalCost.push_back(0);
    singleCost.push_back(0);

    if (commandOutput) cout << "Training of th EBM matrix complete!\n" ;

    //2SP
    if (commandOutput) cout <<"Initialization of 2SP boker...\n" ;
    TSPBooker *stochasticBooker = new TSPBooker();
    stochasticBooker->setAlpha(ALPHAINCREMENT);
    stochasticBooker->setTimeLimit(20.0);
    stochasticBooker->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
    stochasticBooker->setScenarios(scenarioItemSets);
    bookers.push_back(stochasticBooker);
    bookerNames.push_back("2SP");
    bookedSets.push_back(emptySet);
    extraSets.push_back(emptySet);
    totExtraBins.push_back(emptySet);
    totalCost.push_back(0);
    singleCost.push_back(0);


    //average
    if (commandOutput) cout <<"Initialization of average boker...\n" ;
    AverageBooker *avBooker = new AverageBooker();
    avBooker->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
    avBooker->setScenarios(scenarioItemSets);
    bookers.push_back(avBooker);
    bookerNames.push_back("AVG");
    bookedSets.push_back(emptySet);
    extraSets.push_back(emptySet);
    totExtraBins.push_back(emptySet);
    totalCost.push_back(0);
    singleCost.push_back(0);


    //**************************************TEST PHASE*************************************************************//
    if (commandOutput) cout <<"\nStart the comparations. \n" ;

    for(int a = 0; a < TESTNR  ; a++)
    {
        if (commandOutput)cout << "\nTest nr:" << a + 1 << "\n";
        //----------------------PLANNING PHASE(FIRST STAGE)-------------------------------------//
        if (commandOutput) cout <<"Start of the planning phase. \n" ;

        //bin set creation
        smallBins = binUt->binsInstanceGenerator(averageVolume, SMALLBINVOLUME);
        mediumBins = binUt->binsInstanceGenerator(averageVolume, MEDIUMBINVOLUME);
        largeBins = binUt->binsInstanceGenerator(averageVolume, LARGEBINVOLUME);

        sort(smallBins.begin(), smallBins.end(), binCostComp);
        sort(mediumBins.begin(), mediumBins.end(), binCostComp);
        sort(largeBins.begin(), largeBins.end(), binCostComp);

        for(int i = 0; i < totalCost.size(); i++)
        {
            bookedSets.at(i) = bookers.at(i)->bookBins(binUt->binsBookerApplicator(emptySet, smallBins, mediumBins, largeBins));
            cout << "The best solution for the "<< bookerNames.at(i) <<" is : \t(" << bookedSets.at(i).small << "," << bookedSets.at(i).medium << "," << bookedSets.at(i).large << ")\n";

        }
        //Item Set Generator
        items = binUt->itemsInstanceGenerator(ITEMSMIN, ITEMSMAX, SMALLITEMPERC, MEDIUMITEMPERC, LARGEBINPERC);
        sort(items.begin(), items.end());

        //----------------------OPERARIVE PHASE(SECOND STAGE)-------------------------------------//
        if (commandOutput) cout <<"Start of the operative phase. \n" ;

        for(int i = 0; i < totalCost.size(); i++)
        {
            extraSets.at(i) = VCSBPP->solveModel(items, binUt->binsBookerApplicator(bookedSets.at(i), smallBins, mediumBins, largeBins));
            cout << "Extra bins for "<< bookerNames.at(i) <<": \t(" << extraSets.at(i).small << "," << extraSets.at(i).medium << "," << extraSets.at(i).large <<")\n";
            totExtraBins.at(i).small += extraSets.at(i).small;
            totExtraBins.at(i).medium += extraSets.at(i).medium;
            totExtraBins.at(i).large += extraSets.at(i).large;
            singleCost.at(i) = binUt->costEvaluator(bookedSets.at(i), extraSets.at(i), smallBins, mediumBins, largeBins);
            totalCost.at(i) += singleCost.at(i);
        }
        cout<<"Cost:\n";
        for(int i = 0; i < totalCost.size(); i++)
            cout<<    bookerNames.at(i) <<"\t\t";
        cout<<endl;
        for(int i = 0; i < totalCost.size(); i++)
        {
            cout << fixed << setw( 5 )  << setprecision(3)  << setfill( '0' ) << singleCost.at(i) << "\t\t";
        }
        cout<<"\nTotal:\n";
        for(int i = 0; i < totalCost.size(); i++)
            cout<<    bookerNames.at(i) <<"\t\t";
        cout<<endl;
        for(int i = 0; i < totalCost.size(); i++)
        {
            bookedSets.at(i) = bookers.at(i)->bookBins(binUt->binsBookerApplicator(emptySet, smallBins, mediumBins, largeBins));
            cout << fixed << setw( 5 )  << setprecision(3)  << setfill( '0' ) << totalCost.at(i) << "\t\t";
        }
        cout<<"\nExtra Bins:\n";
        for(int i = 0; i < totalCost.size(); i++)
            cout<<    bookerNames.at(i) <<"\t\t";
        cout<<endl;
        for(int i = 0; i < totalCost.size(); i++)
        {
            cout << "(" << totExtraBins.at(i).small << "," << totExtraBins.at(i).medium << "," << totExtraBins.at(i).large << ")\t\t";
        }
        cout<<endl;
        /*
        inputWriter     << std::fixed << std::setw( 5 )  << std::setprecision(3)  << std::setfill( '0' )
                    << ALPHAINCREMENT  << "\t" << (PERCVOLUME) << "\t" << EBMcost << "\t" << averageCost << "\t"<< stochasticCost << "\n";/*
            << EMBTotalExtra.small << "\t" << EMBTotalExtra.medium << "\t" << EMBTotalExtra.large << "\t"
            << averageTotalExtra.small << "\t" << averageTotalExtra.medium << "\t" << averageTotalExtra.large << "\n" ;*/


    }

    inputWriter.close();

/*
    double gurobiCost = 0, totGurobi = 0, EBMCost = 0;
    inputWriter.open("/home/andrea/VCSBPSI/performanceBDF_Gurobi.txt");

    for (int i = 0; i < 0; i++)
    {
        smallBins = binUt->binsInstanceGenerator(averageVolume, SMALLBINVOLUME);
        mediumBins = binUt->binsInstanceGenerator(averageVolume, MEDIUMBINVOLUME);
        largeBins = binUt->binsInstanceGenerator(averageVolume, LARGEBINVOLUME);
        sort(smallBins.begin(), smallBins.end(), cheapComp);
        sort(mediumBins.begin(), mediumBins.end(), cheapComp);
        sort(largeBins.begin(), largeBins.end(), cheapComp);

        items = binUt->itemsInstanceGenerator(ITEMSMIN, ITEMSMAX, SMALLITEMPERC, MEDIUMITEMPERC, LARGEBINPERC);

        binSet BDFBins = VCSBPP->solveModel(items, binUt->binsBookerApplicator(emptySet, smallBins, mediumBins, largeBins));
        binSet gurobiBins = VCSBPP->solveModel(items, binUt->binsBookerApplicator(emptySet, smallBins, mediumBins, largeBins));
        averageVolume = 0;
        for (int b = 0; b < items.size(); b++)
            averageVolume += items.at(b).weight;

        int volumeUsedGurobi = gurobiBins.small * SMALLBINVOLUME + gurobiBins.medium * MEDIUMBINVOLUME + gurobiBins.large * LARGEBINVOLUME;

        int volumeUsedBDF = BDFBins.small * SMALLBINVOLUME + BDFBins.medium * MEDIUMBINVOLUME + BDFBins.large * LARGEBINVOLUME;

        gurobiCost     = binUt->costEvaluator(gurobiBins, emptySet, smallBins, mediumBins, largeBins);
        EBMCost     = binUt->costEvaluator(BDFBins, emptySet, smallBins, mediumBins, largeBins);
        inputWriter     << std::fixed << std::setw( 6 )  << std::setprecision(2)  << std::setfill( '0' ) << gurobiCost <<"\t"
                        << gurobiBins.small <<"\t" << gurobiBins.medium <<"\t" << gurobiBins.large <<"\t" << volumeUsedGurobi << "\t"
                        << EBMCost<< "\t" << BDFBins.small <<"\t" << BDFBins.medium <<"\t" << BDFBins.large << " \t" << volumeUsedBDF <<" \t" << averageVolume << "\n";

        totEBM += EBMcost;
        totGurobi *= gurobiCost;

    }

    inputWriter.close();*/
    return 0;
}