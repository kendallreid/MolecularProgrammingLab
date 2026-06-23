/******************************************************************************
 * File: Sim.cpp
 * Author: Kendall Reid
 * Description:
 *     Implements the Sim class functions. Handles simulation loop, reaction
 *     selection using stochastic methods, updating the Tile, and printing
 *     concentration data to CSV for plotting.
 ******************************************************************************/

#include "Sim.h"

// Calculates next reaction time
//   time = (-log(getRand())) / totalProp
// Tracks time
void Sim::nextReacTime(const double& totalProp)
{
	double dt = (-log(getRand())) / totalProp;  // Get next reaction time interval
	_timeTrack.push_back(dt + _timeTrack[_timeTrack.size() - 1]);  // Add current time plus dt to find next reaction time
}

// Provides a random number for where reaction takes place at
int Sim::getRandIndex(int size)
{
	std::uniform_int_distribution<> dist(0, size - 1);
	return dist(gen);
}

// Selects which reaction occurs based on propensities using a random threshold.
// Updates _rxnTrack with the selected reaction index.
// Returns the index of the chosen reaction, or -1 if none occurs.
int Sim::determineReaction(const double& totalProp, const vector<double>& reacProp)
{
	double stepReac = getRand() * totalProp;  // Random value (0 - 1.0) to determine reaction
	double cumulativeSum = 0.0;

	for (int j = 0; j < reacProp.size(); ++j)  // Size is number of reactions possible
	{
		cumulativeSum += reacProp[j];
		if (cumulativeSum >= stepReac)  // Reaction j occurred
		{
			_rxnTrack.push_back(j);  // Add reaction to rxn vector
			return j;
		}
	}
	return -1;
}

// Updates matrix by performing reaction at randomly generated position
void Sim::performReaction(const int& rxn, Tile& tile)
{
	int rxnIndex = getRandIndex(tile.getPixelPairPos()[rxn].size());  // Find a random index for specific reaction
	tile.updateMatrix(rxn, rxnIndex);  // Update matrix pixels
}

// Picks reaction time, reaction, and performs the reaction
void Sim::simStep(Tile& tile)
{
	tile.tileSimStep();  // Pixel pairs and propensities
	if (tile.getTotalProp() != 0)  // Check if any possible reactions left
	{
		nextReacTime(tile.getTotalProp());
		int reaction = determineReaction(tile.getTotalProp(), tile.getReacProp());
		performReaction(reaction, tile);
		_concOverTime.push_back(tile.concToVector());  // Add new concentrations to tracker
	}
}

// Prints current concentrations at time steps to output file
void Sim::printConcToFile(const string& filename)
{
	std::ofstream csvFileC(filename, std::ios::out | std::ios::app);
	if (csvFileC.good())
	{
		double currentTime = _timeTrack[_timeTrack.size() - 1];
		csvFileC << currentTime;
		if (!_concOverTime.empty())
		{
			for (const auto& conc : _concOverTime[_concOverTime.size() - 1])  // Print each reactants concentration
			{
				csvFileC << "," << conc.second;
			}
		}
		csvFileC << "\n";
	}
	else { cout << "File not properly opened" << endl; }
	csvFileC.close();
}

// Runs entire simulation
// Opens output and input files
// Runs until total propensity is 0 or specified time runs out
// Generates output concentrations, matrices, and visual plot
void Sim::runSim(double maxTime)
{
	Tile tile("input.csv");

	_timeTrack.clear();
	_timeTrack.push_back(0.0);

	_concOverTime.clear();
	ofstream csvFile("matrices.csv", std::ios::out | std::ios::trunc);
	if (csvFile.good())
	{
		csvFile.close();  // Close so all old data is gone
		_concOverTime.push_back(tile.concToVector());  // Starting concentrations
		tile.tileSimStep();  // Calculates starting propensities so sim can run

		// Open data file for plotting values
		ofstream csvFileC("concentrations.csv", std::ios::out | std::ios::trunc);
		if (csvFileC.good())
		{
			csvFileC << "Time";
			for (const auto& reactant : _concOverTime[0])  // Print all reactants in header
			{
				csvFileC << "," << reactant.first;
			}
			csvFileC << "\n";
			csvFileC.close();  // Finish flushing header to file before other data written

			while (_timeTrack[_timeTrack.size() - 1] < maxTime && tile.getTotalProp() > 0)  // Time not run out & reactions still possible
			{
				TileIO::printMatrixToFile(tile, "matrices.csv");
				printConcToFile("concentrations.csv");
				simStep(tile);  // Run reaction
			}
			createPlot();
		}
		else { cout << "File not properly opened" << endl; }
		csvFileC.close();
	}
	else{ cout << "File not properly opened" << endl; }
	csvFile.close();
}

// Plots concentrations of reactants over time
void Sim::createPlot()
{
	std::ofstream gp("plot.gp");
	gp << "set key autotitle columnhead\n";
	gp << "set xlabel 'Time'\n";
	gp << "set ylabel 'Concentration'\n";
	gp << "set datafile separator ','\n";
	gp << "plot for [col=2:*] 'concentrations.csv' using 1:col with lines lw 2\n";
	gp.close();

	std::system("gnuplot -p plot.gp");
}