/******************************************************************************
 * File: Tile.h
 * Author: Kendall Reid
 * Description:
 *     Declares the Tile class, which represents a grid of reactants for a
 *     molecular simulation. The class maintains the pixel matrix, reaction
 *     data, reactant concentrations, and provides functions to update the
 *     matrix during simulation steps.
 *     TileIO is declared as a friend class to handle file input/output.
 ******************************************************************************/

#ifndef TILE
#define TILE
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <random>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "TileIO.h"

using std::string;
using std::pair;
using std::unordered_set;
using std::unordered_map;
using std::vector;
using std::fstream;
using std::stringstream;
using std::cout;
using std::endl;


// Struct to store reactants and products for possible reactions per tile
struct Reaction {
	pair<string, string> reactants;
	pair<string, string> products;
	double rate;

	Reaction(const pair<string, string>& r, const pair<string, string>& p, const double k)
		: reactants(r), products(p), rate(k) {}
};

// Grid where each pixel represents a reactant for specified Reactions
class Tile
{
	// Allows TileIO to see private members for initializing tile and reactions
	friend class TileIO;

public:
	Tile(const string& filename);
	~Tile() {};

	// Calculates size of matrix
	void updateSizeParams();
	// Initializes concentration values based on each pixel's reactant
	void initConc();

	// Finds and updates possible pixel pairs that adhere to reaction rules
	void findInitialPixelPairs();
	void removeLocalPairs(pair<int, int> pos);
	void addLocalPairsHelper(pair<int, int> pos, pair<int, int> otherChangedPos);
	void addLocalPairs(pair<int,int> pos, pair<int,int> pos2);

	// Fills _reactantPixelPairPos with possible pairs - sorted by Reactions possible
	void populateReacPosVec(pair<int, int> pos1, pair<int, int> pos2);
	// Finds new pixel pairs and recalculates propensities after each reaction
	void tileSimStepSetUp();

	// Calculates propensities based on possible pixel pairs
	void calcReacProp();
	void calcTotalProp();

	// Getters for Tile private members
	const double& getTotalProp() const { return _totalProp; }
	const vector<double>& getReacProp() const { return _reacProp; }
	const vector<vector<pair<pair<int, int>, pair<int, int>>>>& getPixelPairPos() const { return _reactantPixelPairPos; }
	const vector<Reaction>& getReactions() const { return _reactions; }
	const vector<vector<string>>& getPixelMatrix() const { return _pixelMatrix; }
	const unordered_map<string, int>& getConc() const { return _conc; }
	static double getMaxDuration() { return _maxDuration; }

	// Updates tile pixels after each reaction
	void updateMatrix(int rxn, int rxnIndex);
	// Updates reactant concentrations after reaction
	void updateConc(int rxn);
	// Place concentrations in vector to later print to file
	const vector<pair<string, int>> concToVector();

public:
	static double _maxDuration;

private:
	int _rowSize, _colSize, _numReactions;

	// Gird of pixels representing a tile - string representation of reactant
	vector<vector<string>> _pixelMatrix;
	// Vector holding position of reactants for each possible pixel pair (pair of coordinate pairs)
	vector<vector<pair<pair<int, int>, pair<int, int>>>> _reactantPixelPairPos;

	// Propensities
	double _totalProp;
	vector<double> _reacProp;

	// List of possible reactions
	vector<Reaction> _reactions;
	// Concentration count for each reactant
	unordered_map<string, int> _conc;
};
#endif