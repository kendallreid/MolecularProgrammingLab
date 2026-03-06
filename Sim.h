/******************************************************************************
 * File: Sim.h
 * Author: Kendall Reid
 * Description:
 *     Declares the Sim class which runs the molecular simulation over time.
 *     Tracks concentrations, reaction times, and handles simulation steps.
 *     Provides functions to run the simulation, determine reaction occurrence,
 *     and create plots of concentration over time.
 ******************************************************************************/

#ifndef SIM
#define SIM
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <unordered_map>
#include <random>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include "Tile.h"

using std::string;
using std::vector;
using std::pair;
using std::unordered_map;
using std::ofstream;

class Sim
{
public:
    Sim() : gen(rd()), dis(0, 1.0), _timeTrack{ 0.0 } {}
    ~Sim() {}

    // Calculates next reaction time
    void nextReacTime(const double& totalProp);
    // Determines which pixel pair position reaction occurs at
    int getRandIndex(int size);
    // Determines which reaction to occur next
    int determineReaction(const double& totalProp, const vector<double>& reacProp);
    // Performs the reaction by updating pixels and concentrations
    void performReaction(const int& rxn, Tile& tile);

    // One cycle of picking time, reaction, position, and performing reaction
    void simStep(Tile& tile);
    // Runs entire simulation
    void runSim(double maxTime);

    // Prints all concentrations to file at each time step
    void printConcToFile(const string& filename);
    // Generates plot of reaction concentrations at the end of the simulation
    void createPlot();

private:
    // Keeps track of concentrations over time
    vector<vector<pair<string,int>>> _concOverTime;
    // Keep track of all reactions
    vector<int> _rxnTrack;
    // Keeps track of time for each reaction
    vector<double> _timeTrack;

    // Creates uniform distribution for generating random number
    // Used in determining which reaction occurs at each time step
    std::random_device rd;  // Obtain seed
    std::mt19937 gen; // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis;  // (0, 1.0)
    double getRand() { return dis(gen); }
};

#endif