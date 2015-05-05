#include <QCoreApplication>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <cstring>
#include <iomanip>
#include <limits>
#include "gurobi_c++.h"
#include <stdlib.h>
#include <algorithm>
#include <unistd.h>
#include "include/structDef.h"
#include "include/binutility.h"
#include "VCSBPPSolvers/abdf.h"
#include "VCSBPPSolvers/mipvscbppsolver.h"
#include "VCSBPSIBookers/tspbooker.h"
#include "VCSBPPSolvers/vcsbppsolver.h"
#include "VCSBPSIBookers/mppbooker.h"
#include "VCSBPSIBookers/averagebooker.h"
#include "VCSBPSIBookers/evpbooker.h"
#include "VCSBPSIBookers/basbooker.h"

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


#define PERCVOLUME 0.85


//Problem definition
int itemMinNr = 25;        //size of the problem
int itemMaxNr = 50;
int smallItemPerc = 30;     //various percentage of the items
int mediumItemPerc = 40;
int largeItemPerc = 30;
float alphaIncrement = 1.3;  //alpha increment of the bins cost

//scenarios number
#define AVERAGESCENARIONR 50
int scenarioNr = 25;
int testNr = 10;

//files strem
ofstream outStream;
string outFileName;
bool useFileOut = false, outFileExist = false;


//solver configuration
bool solveBDF = false, solveMIP = false;

//bookers configuration
bool useMPPBDF = false, useMPPMIP = false, useTSP = false, useAV = false, useEVP = false;
bool matrixReadyMPPBDF = false, matrixReadyMPPMIP = false;

//output config
bool commandOutput = true;



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    cout << "Command Line config\n";
    for (int i = 0; i < argc; i++)
    {
        if ((string)argv[i] == "-mppbdf")
        {
            useMPPBDF = true;
            cout <<"MPP with A-BDF will be used as booker.\n" ;
        }
        if ((string)argv[i] == "-mppmip")
        {
            useMPPMIP = true;
            cout <<"MPP with MIP will be used as booker.\n" ;
        }
        if ((string)argv[i] == "-tsp")
        {
            useTSP = true;
            cout <<"2SP will be used as booker.\n" ;
        }
        if ((string)argv[i] == "-av")
        {
            useAV = true;
            cout <<"AV will be used as booker.\n" ;
        }
        if ((string)argv[i] == "-evp")
        {
            useEVP = true;
            cout <<"EVP will be used as booker.\n" ;
        }
        if ((string)argv[i] == "-bdf")
        {
            solveBDF = true;
            solveMIP = false;
            cout <<"The VCSBPP solver is A-BDF.\n" ;
        }
        if ((string)argv[i] == "-mip")
        {
            solveBDF = false;
            solveMIP = true;
            cout <<"The VCSBPP solver is MIP.\n" ;
        }
        if ((string)argv[i] == "-snr")
        {
            scenarioNr = atoi(argv[++i]);
            cout <<"Number of the training scenarios = " << scenarioNr <<".\n" ;
        }
        if ((string)argv[i] == "-tnr")
        {
            testNr = atoi(argv[++i]);
            cout <<"Number of the test intances = " << scenarioNr <<".\n" ;
        }
        if ((string)argv[i] == "-alp")
        {
            alphaIncrement = atof(argv[++i]);
            cout <<"Alpha value = " << alphaIncrement  <<".\n" ;
        }
        if ((string)argv[i] == "-itnr")
        {
            itemMinNr = atoi(argv[++i]);
            itemMaxNr = atoi(argv[++i]);
            cout <<"Dimension of itemset in [" << itemMinNr << ";" << itemMaxNr <<"].\n" ;
        }
        if ((string)argv[i] == "-itvperc")
        {
            smallItemPerc = atoi(argv[++i]);
            mediumItemPerc = atoi(argv[++i]);
            largeItemPerc  = atoi(argv[++i]);
            if (smallItemPerc + mediumItemPerc + largeItemPerc != 100)
            {
                cout <<"Errore nelle percentuali\n";
                return 0;
            }
            cout <<"Percentage of items: " << itemMinNr << ";" << itemMaxNr <<"].\n" ;
        }
        if ((string)argv[i] == "-ofile")
        {
            outFileName = (string)argv[++i];
            if (ifstream(outFileName))
                outFileExist = true;
            outStream.open(outFileName,ios_base::app);
            useFileOut = true;
            if (!outStream.is_open())
            {
                cout << "Cannot find file " << outFileName << endl;
                return -1;
            }
            cout <<"Opened the file  \"" << outFileName <<"\" in output.\n" ;
        }

        
    }
    //initialization bin utility
    BinUtility *binUt = new BinUtility();
    binUt->setItemVolumes(SMALLITEMMINVOLUME,SMALLITEMMAXVOLUME,MEDIUMITEMMINVOLUME,MEDIUMITEMMAXVOLUME,LARGEITEMMINVOLUME,LARGEITEMMAXVOLUME);
    binUt->setAlpha(alphaIncrement);

    //bin comparator
    struct binCostComparator {
        bool operator() (const bin& i, const bin& j) const {
            return (i.cost < j.cost);
        }
    } binCostComp;

    //initialization of the structures that will collect bookers and the statistics
    vector<VCSBPSIBooker*> bookers;
    vector<string> bookerNames;
    vector<binSet> extraSets;
    vector<binSet> totExtraBins;
    vector<binSet> bookedSets;
    vector<double> singleCost;
    vector<double> totalCost;

    cout <<"Items number:\t["<<itemMinNr<<";"<<itemMaxNr<<"]\nTrain number:\t" << scenarioNr << "\nTest number:\t"<<testNr <<"\n\n";

    vector<bin> smallBins;
    vector<bin> mediumBins;
    vector<bin> largeBins;
    vector<item> items;
    binSet emptySet;

    //part to estimate the average volume... I have to remove it
    cout <<"Calculating average volume...\n" ;
    int averageVolume = 0;
    for(int a = 0; a < AVERAGESCENARIONR; a++)
    {
        items = binUt->itemsInstanceGenerator(itemMinNr, itemMaxNr, smallItemPerc, mediumItemPerc, largeItemPerc);
        for (int b = 0; b < items.size(); b++)
            averageVolume += items.at(b).weight;
    }
    averageVolume /= AVERAGESCENARIONR;

    cout <<"The average volume is "<< averageVolume <<"\n" ;

    //CREATING THE SCENARIOS
    cout <<"Creating the training scenarios...\n" ;
    vector< vector< item > > scenarioItemSets;
    vector< vector< bin > > scenarioBinSets;

    for (int i = 0; i < scenarioNr; i++)
    {

        scenarioItemSets.push_back(binUt->itemsInstanceGenerator(itemMinNr, itemMaxNr, smallItemPerc, mediumItemPerc, largeItemPerc));
        scenarioBinSets.push_back(binUt->binsBookerApplicator(emptySet, binUt->binsInstanceGenerator(averageVolume, SMALLBINVOLUME), binUt->binsInstanceGenerator(averageVolume, MEDIUMBINVOLUME), binUt->binsInstanceGenerator(averageVolume, LARGEBINVOLUME)));
    }
    VCSBPPSolver *VCSBPP;
    //intialization VCSBPP solvers
    if (solveBDF)
    {
        cout <<"Initialization of A-BDF, the VCSBPP solver...\n" ;
        VCSBPP = new ABDF();
        VCSBPP->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
    }else
    {
        cout <<"Initialization of MIP VCSBPP solver...\n" ;
        VCSBPP = new MIPVSCBPPSolver(3.0);
        VCSBPP->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
    }

    smallBins = binUt->binsInstanceGenerator(averageVolume, SMALLBINVOLUME);
    mediumBins = binUt->binsInstanceGenerator(averageVolume, MEDIUMBINVOLUME);
    largeBins = binUt->binsInstanceGenerator(averageVolume, LARGEBINVOLUME);

//initialization Booker

    //MPP A-BDF
    if(useMPPBDF)
    {
        cout <<"Initialization of MPP A-BDF booker...\n" ;
        VCSBPPSolver *trainingSolver = new ABDF();
        trainingSolver->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
        MPPBooker *mppSolverABDF = new MPPBooker(smallBins.size(),mediumBins.size(),largeBins.size());
        mppSolverABDF->setVCSBPPSolver(trainingSolver);
        mppSolverABDF->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
        mppSolverABDF->setAlpha(alphaIncrement);

        if (matrixReadyMPPBDF)
        {
            cout <<"Using the EBM matrix in file X...\n" ;

        }else
        {
            cout << "Training of th EBM matrix...\n" ;
            mppSolverABDF->training(scenarioBinSets,scenarioItemSets);

        }
        bookers.push_back(mppSolverABDF);
        bookerNames.push_back("MPP-B");
        bookedSets.push_back(emptySet);
        extraSets.push_back(emptySet);
        totExtraBins.push_back(emptySet);
        totalCost.push_back(0);
        singleCost.push_back(0);

        cout << "Training of th EBM matrix complete!\n" ;

    }

    //MPP MIP
    if(useMPPMIP)
    {
        cout <<"Initialization of MPP MIP booker...\n" ;
        MIPVSCBPPSolver *trainingSolverMIP = new MIPVSCBPPSolver();
        trainingSolverMIP->setTimeLimit(2.0);
        trainingSolverMIP->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);

        MPPBooker *mppSolverMIP = new MPPBooker(smallBins.size(),mediumBins.size(),largeBins.size());
        mppSolverMIP->setVCSBPPSolver(trainingSolverMIP);
        mppSolverMIP->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
        mppSolverMIP->setAlpha(alphaIncrement);

        if (matrixReadyMPPMIP)
        {
            cout <<"Using the EBM matrix in file X...\n" ;

        }else
        {
            cout << "Training of th EBM matrix...\n" ;
            mppSolverMIP->training(scenarioBinSets,scenarioItemSets);

        }
        bookers.push_back(mppSolverMIP);
        bookerNames.push_back("MPP-M");
        bookedSets.push_back(emptySet);
        extraSets.push_back(emptySet);
        totExtraBins.push_back(emptySet);
        totalCost.push_back(0);
        singleCost.push_back(0);
        cout << "Training of th EBM matrix complete!\n" ;
    }

    //2SP
    if (useTSP)
    {
        cout <<"Initialization of 2SP boker...\n" ;
        TSPBooker *stochasticBooker = new TSPBooker();
        stochasticBooker->setAlpha(alphaIncrement);
        stochasticBooker->setTimeLimit(5.0);
        stochasticBooker->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
        stochasticBooker->setScenarios(scenarioItemSets);
        bookers.push_back(stochasticBooker);
        bookerNames.push_back("2SP");
        bookedSets.push_back(emptySet);
        extraSets.push_back(emptySet);
        totExtraBins.push_back(emptySet);
        totalCost.push_back(0);
        singleCost.push_back(0);
    }

    //average
    if(useAV)
    {
        cout <<"Initialization of average boker...\n" ;
        AverageBooker *avBooker = new AverageBooker();
        avBooker->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
        avBooker->setAlpha(alphaIncrement);
        avBooker->setScenarios(scenarioItemSets);
        bookers.push_back(avBooker);
        bookerNames.push_back("AVG");
        bookedSets.push_back(emptySet);
        extraSets.push_back(emptySet);
        totExtraBins.push_back(emptySet);
        totalCost.push_back(0);
        singleCost.push_back(0);
    }

    //EVG
    if(useEVP)
    {
        cout <<"Initialization of EVP boker...\n" ;
        EVPBooker *evpSolution = new EVPBooker();
        evpSolution->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
        evpSolution->setAlpha(alphaIncrement);
        evpSolution->setScenarios(scenarioItemSets);
        evpSolution->setTimeLimit(10.0);
        bookers.push_back(evpSolution);
        bookerNames.push_back("EVP");
        bookedSets.push_back(emptySet);
        extraSets.push_back(emptySet);
        totExtraBins.push_back(emptySet);
        totalCost.push_back(0);
        singleCost.push_back(0);
    }

    if(useAV)
    {
        cout <<"Initialization of BAS boker...\n" ;
        BASBooker *basBooker = new BASBooker();
        basBooker->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
        basBooker->setAlpha(alphaIncrement);
        VCSBPPSolver *trainingSolver = new ABDF();
        trainingSolver->setVolumes(SMALLBINVOLUME,MEDIUMBINVOLUME,LARGEBINVOLUME);
        basBooker->setVCSBPPSolver(trainingSolver);
        basBooker->training(scenarioBinSets,scenarioItemSets);
        bookers.push_back(basBooker);
        bookerNames.push_back("BAS");
        bookedSets.push_back(emptySet);
        extraSets.push_back(emptySet);
        totExtraBins.push_back(emptySet);
        totalCost.push_back(0);
        singleCost.push_back(0);
    }
    //**************************************TEST PHASE*************************************************************//
    cout <<"\nStart the comparations. \n" ;
    if(!outFileExist)
    {
        if (useFileOut)
        {
            outStream << "ItemSet Volume;Alpha;Optimal Book;";
            for(int i = 0; i < totalCost.size(); i++)
            {
                outStream << bookerNames.at(i) << " Cost;" << bookerNames.at(i) << " S Book;" << bookerNames.at(i) << " M  Book;" << bookerNames.at(i) << " L Book;"
                          << bookerNames.at(i) << " S Extra;" << bookerNames.at(i) << " M Extra;" << bookerNames.at(i) << " L Extra;" << bookerNames.at(i) << " Quality;";
            }
            outStream << "\n";
        }
    }
    for(int a = 0; a < testNr  ; a++)
    {
        if (commandOutput)cout << "\nTest nr:" << a + 1 << "\n";
        //----------------------PLANNING PHASE(FIRST STAGE)-------------------------------------//
        cout <<"Start of the planning phase. \n" ;

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
        items = binUt->itemsInstanceGenerator(itemMinNr, itemMaxNr, smallItemPerc, mediumItemPerc, largeItemPerc);
        sort(items.begin(), items.end());

        //----------------------OPERARIVE PHASE(SECOND STAGE)-------------------------------------//
        cout <<"Start of the operative phase. \n" ;

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
        binSet totalBinsNeed;
        totalBinsNeed.small = extraSets.at(0).small + bookedSets.at(0).small;
        totalBinsNeed.medium = extraSets.at(0).medium + bookedSets.at(0).medium;
        totalBinsNeed.large = extraSets.at(0).large + bookedSets.at(0).large;

        int perfectBook = binUt->costEvaluator(totalBinsNeed, emptySet, smallBins, mediumBins, largeBins);

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

        if (useFileOut)
        {
            int itemsetVolume = 0;
            for (int b = 0; b < items.size(); b++)
                itemsetVolume += items.at(b).weight;
            outStream << itemsetVolume << ";" << fixed << setw(5)  << setprecision(3)  << setfill( '0' ) << alphaIncrement << ";" << perfectBook <<";";
            for(int i = 0; i < totalCost.size(); i++)
            {
                outStream << fixed << setw(5)  << setprecision(3)  << setfill( '0' ) << singleCost.at(i) << ";";
                outStream << bookedSets.at(i).small << ";" << bookedSets.at(i).medium << ";" << bookedSets.at(i).large << ";"
                          << extraSets.at(i).small << ";" << extraSets.at(i).medium << ";" << extraSets.at(i).large << ";" << singleCost.at(i) / perfectBook << ";";
            }
            outStream << "\n";
        }
    }

    outStream.close();
    return 0;
}
