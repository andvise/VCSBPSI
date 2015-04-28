#include "mppbooker.h"
//vise remove
#include <iostream>


MPPBooker::MPPBooker(int newSmallBinsNr, int newMediumBinsNr, int newLargeBinsNr)
{
    binUt = new BinUtility();
    smallBinsNr = newSmallBinsNr;
    mediumBinsNr = newMediumBinsNr;
    largeBinsNr = newLargeBinsNr;
    matrixEBM.resize(smallBinsNr + 1);
    averageVolume = 0;

    int i,j,k;
    for (i = 0; i < smallBinsNr + 1; ++i) {
        matrixEBM[i].resize(mediumBinsNr + 1);

        for (j = 0; j < mediumBinsNr + 1; ++j)
            matrixEBM[i][j].resize(largeBinsNr + 1);
    }

    flag.resize(smallBinsNr + 1);

    for (i = 0; i < smallBinsNr + 1; ++i)
    {
        flag[i].resize(mediumBinsNr + 1);

        for (j = 0; j < mediumBinsNr + 1; ++j)
            flag[i][j].resize(largeBinsNr + 1);
    }



}
void MPPBooker::training(vector<vector<bin>> scenarioBins, vector<vector<item>> scenarioItems)
{
    trainingScenarioNr = scenarioItems.size();
    for(int a = 0; a < scenarioNr; a++)
    {
        for (int b = 0; b < scenarioItems.at(a).size(); b++)
            averageVolume += scenarioItems.at(a).at(b).weight;
    }

    averageVolume/=scenarioNr;
    cout << "Training on scenario:" ;

    for (int i = 0; i < scenarioNr; i++)
    {
        cout << i +1 << "\t" ;
        trainingScenario(scenarioBins.at(i),scenarioItems.at(i));
    }
    cout << "\n" ;

}

void MPPBooker::trainingScenario(vector<bin> bins, vector<item> items)
{
    struct binCostComparator {
        bool operator() (const bin& i, const bin& j) const {
            return (i.cost < j.cost);
        }
    } binCostComp;
    int i,j,k;

    vector <bin> smallBins = binUt->binsSplitter(bins,smallBinVolume);
    sort(smallBins.begin(), smallBins.end(), binCostComp);

    vector <bin> mediumBins = binUt->binsSplitter(bins,mediumBinVolume);
    sort(mediumBins.begin(), mediumBins.end(), binCostComp);

    vector <bin> largeBins = binUt->binsSplitter(bins,largeBinVolume);
    sort(largeBins.begin(), largeBins.end(), binCostComp);

    //clean the flag matrix
    for ( i = 0; i < smallBinsNr + 1 ; i++)
    {
        for (j = 0; j < mediumBinsNr + 1; j++)
        {
            for ( k = 0; k < largeBinsNr + 1 ; k++)
            {
                flag[i][j][k] = 0;
            }
        }
    }


    binSet provv;
    int countLazy = 0;
    for ( i = 0; i < smallBinsNr + 1 ; i++)
    {
        for (j = 0; j < mediumBinsNr + 1; j++)
        {
            for ( k = 0; k < largeBinsNr + 1 ; k++)
            {
                if ( (flag[i][j][k] != 1) && ( i * smallBinVolume + j * mediumBinVolume + k * largeBinVolume > lowerBoundMPP * averageVolume) )
                {
                    provv.small = i;
                    provv.medium = j;
                    provv.large = k;
                    provv = solver->solveModel(items, binUt->binsBookerApplicator(provv, smallBins, mediumBins, largeBins));
                    countLazy++;
                  //  std::cout<<"\nApplicato per la " << countLazy <<" volta il BDF alla posizione ("<< i << " , " << j << " , " << k << ")  con i seguenti bin extra  ("<< provv.small << " , " << provv.medium << " , " << provv.large << ")";
                    lazyEvaluation(i, j, k, provv.small, provv.medium, provv.large);
                }
            }
        }
    }




}

void MPPBooker::setVCSBPPSolver(VCSBPPSolver* newSolver)
{
    solver = newSolver;
}
void MPPBooker::lazyEvaluation(int i, int j, int k, int s, int m, int l)
{
    binSet provv;

    if ((i <= smallBinsNr) && (j <= mediumBinsNr) && (k <= largeBinsNr) && (flag[i][j][k] != 1))
    {
        provv.small = s;
        provv.medium = m;
        provv.large = l;

        matrixEBM[i][j][k].addScenario(provv);
        flag[i][j][k] = 1;

        if (s > 0)
            lazyEvaluation(i + 1 , j , k , s - 1 , m , l);

        if (m > 0)
            lazyEvaluation(i , j + 1 , k , s , m - 1 , l);

        if (l > 0)
            lazyEvaluation(i , j , k + 1 , s , m , l - 1);

        if (( s == 0 ) && ( m == 0 ) && ( l == 0 ))
        {
            lazyEvaluation(i + 1 , j , k , 0, 0, 0);
            lazyEvaluation(i , j + 1 , k , 0, 0, 0);
            lazyEvaluation(i , j , k + 1 , 0, 0, 0);
        }
    }
}

binSet MPPBooker::bookBins(vector<bin> bins)
{
    struct binCostComparator {
        bool operator() (const bin& i, const bin& j) const {
            return (i.cost < j.cost);
        }
    } binCostComp;
    vector <bin> smallBins = binUt->binsSplitter(bins,smallBinVolume);
    sort(smallBins.begin(), smallBins.end(), binCostComp);

    vector <bin> mediumBins = binUt->binsSplitter(bins,mediumBinVolume);
    sort(mediumBins.begin(), mediumBins.end(), binCostComp);

    vector <bin> largeBins = binUt->binsSplitter(bins,largeBinVolume);
    sort(largeBins.begin(), largeBins.end(), binCostComp);

    //evaluation of the cost matrix
    float binCost[smallBinsNr + 1][mediumBinsNr + 1][largeBinsNr + 1];
    int i, j, k;
    binCost[0][0][0] = 0;
    for (j = 1; j < mediumBinsNr + 1; j++)\
    {
        binCost[0][j][0] = binCost[0][j - 1][0] + mediumBins.at(j - 1).cost;
    }

    for ( k = 1; k < largeBinsNr + 1 ; k++)
    {
        binCost[0][0][k] = binCost[0][0][k - 1] + largeBins.at(k - 1).cost;

        for (j = 1; j < mediumBinsNr + 1; j++)
        {
            binCost[0][j][k] = binCost[0][j - 1][k] + mediumBins.at(j - 1).cost;
        }
    }

    for ( i = 1; i < smallBinsNr + 1 ; i++)
    {
        binCost[i][0][0] = binCost[i - 1][0][0] + smallBins.at(i - 1).cost;

        for (j = 0  ; j < mediumBinsNr + 1; j++)
        {
            if (j != 0)
                binCost[i][j][0] = binCost[i][j - 1][0] + mediumBins.at(j - 1).cost;

            for ( k = 1; k < largeBinsNr + 1 ; k++)
            {
                binCost[i][j][k] = binCost[i][j][k - 1] + largeBins.at(k - 1).cost;
            }
        }
    }
    float minTotalCost = std::numeric_limits<float>::max();
    binSet EBMMinimumCostSet;

    for ( i = 0; i < smallBinsNr + 1 ; i++)
    {
        for (j = 0  ; j < mediumBinsNr + 1; j++)
        {
            for ( k = 0; k < largeBinsNr + 1; k++)
            {
                if (    ( i * smallBinVolume + j * mediumBinVolume + k * largeBinVolume > lowerBoundMPP * averageVolume) &&
                        (matrixEBM[i][j][k].getSmallPerc() < limitExpectedExtraBins) &&
                        (matrixEBM[i][j][k].getMediumPerc() < limitExpectedExtraBins) &&
                        (matrixEBM[i][j][k].getLargePerc() < limitExpectedExtraBins) &&
                        (matrixEBM[i][j][k].getSmallPerc() + matrixEBM[i][j][k].getMediumPerc() + matrixEBM[i][j][k].getLargePerc() < 10) )
                {
                    //cout << i << " , " << j << " , " << k << " .. " << smallBins.size() <<"\n";
                    /*inputWriter << "(" << i << " , " << j << " , " << k << ")  EBM Matrix ("
                            << matrixEBM[i][j][k].getSmallPerc() << " , " << matrixEBM[i][j][k].getMediumPerc() << " , " << matrixEBM[i][j][k].getLargePerc()  << ") prec bin Cost"
                            << binCost[i][j][k]  <<"\nNext bin cost: s=" << ((i < smallBins.size())  ? (matrixEBM[i][j][k].getSmallPerc() * smallBins.at(i).cost)    : 0 ) <<
                                                                    "m=" << ((j < mediumBinsNr) ? (matrixEBM[i][j][k].getMediumPerc() * mediumBins.at(j).cost)  : 0 ) <<
                                                                    "l=" << ((k < largeBinsNr)  ? (matrixEBM[i][j][k].getLargePerc() * largeBins.at(k).cost)    : 0 ) <<"\n";
                                                                    */
                    binCost[i][j][k] = binCost[i][j][k] +
                                       alpha * (
                                           ((i < smallBinsNr)  ? (matrixEBM[i][j][k].getSmallPerc() * smallBins.at(i).cost)    : 0 ) +
                                           ((j < mediumBinsNr) ? (matrixEBM[i][j][k].getMediumPerc() * mediumBins.at(j).cost)  : 0 ) +
                                           ((k < largeBinsNr)  ? (matrixEBM[i][j][k].getLargePerc() * largeBins.at(k).cost)    : 0 ));

                    if (binCost[i][j][k] < minTotalCost)
                    {
                        EBMMinimumCostSet.small    = i;
                        EBMMinimumCostSet.medium   = j;
                        EBMMinimumCostSet.large    = k;
                        minTotalCost = binCost[i][j][k];
                    }
                }
            }
        }
    }
    return EBMMinimumCostSet;
}


void MPPBooker::matrixPrint(const string & fileName)
{
    ofstream inputWriter;

    inputWriter.open(fileName);
    if (!inputWriter.is_open())
    {
        cout << "Cannot find file   " << fileName << endl;
    }else{
        inputWriter << trainingScenarioNr << "\n";
        for (int i = 0; i < matrixEBM.size() ; i++)
        {
            for (int j = 0; j < matrixEBM[0].size(); j++)
            {
                for (int k = 0; k < matrixEBM[0][0].size() ; k++)
                {
                    inputWriter << std::fixed << std::setw( 4 )  << std::setprecision(4)  << std::setfill( '0' )  << matrixEBM[i][j][k].getSmallPerc() << ";" << matrixEBM[i][j][k].getMediumPerc() << ";" << matrixEBM[i][j][k].getLargePerc() << "\n";
                }
            }
        }
    }
    inputWriter.close();
}

void MPPBooker::matrixRead(const string & fileName)
{
    ifstream instream;
    instream.open(fileName);
    string line;
    float smallPerc, mediumPerc,largePerc;

    if (!instream.is_open())
    {
        cout << "Cannot find file   " << fileName << endl;
    }
    else
    {
        getline(instream, line);
        trainingScenarioNr = stoi(line);

        string delimiter = ";";
        size_t pos = 0;
        string token;
        char conv[6];


        for (int i = 0; i < matrixEBM.size() ; i++)
        {
            for (int j = 0; j < matrixEBM[0].size(); j++)
            {
                for (int k = 0; k < matrixEBM[0][0].size() ; k++)
                {
                    getline(instream, line);
                    pos = line.find(delimiter);
                    token = line.substr(0, pos);
                    strcpy(conv, token.c_str());
                    smallPerc = conv[0]- '0' + (conv[2]- '0') *0.1 + (conv[3] - '0')*0.01 + (conv[4] - '0')*0.001;
                    line.erase(0, pos + delimiter.length());

                    pos = line.find(delimiter);
                    token = line.substr(0, pos);
                    strcpy(conv, token.c_str());
                    mediumPerc = conv[0]- '0' + (conv[2]- '0') *0.1 + (conv[3] - '0')*0.01 + (conv[4] - '0')*0.001;
                    line.erase(0, pos + delimiter.length());

                    strcpy(conv, line.c_str());
                    largePerc = conv[0]- '0' + (conv[2]- '0') *0.1 + (conv[3] - '0')*0.01 + (conv[4] - '0')*0.001;

                    matrixEBM[i][j][k].setNew(trainingScenarioNr, smallPerc, mediumPerc,largePerc);
                }
            }
        }
    }
    instream.close();

}
