/******************************************************************************
 * File: Tile.cpp
 * Author: Kendall Reid
 * Description:
 *     Implements Tile class functions. Includes matrix initialization,
 *     updating concentrations, finding neighboring pixel pairs for reactions,
 *     and calculating reaction propensities.
 ******************************************************************************/

#include "Tile.h"

// Initializes tile through helper TileIO reading input file and updates initial concentrations and size
Tile::Tile(const string& filename) : _rowSize(0), _colSize(0), _totalProp(0)
{
	TileIO::readFromFile(*this, filename);
	updateSizeParams();
	initConc();
}

// Determines size of tile and number of reaction rules as unknown till input file is read
void Tile::updateSizeParams()
{
	if (!_pixelMatrix.empty())  // check if empty
	{
		_rowSize = _pixelMatrix.size();
		_colSize = _pixelMatrix[0].size();  // How many elements in row
	}
	if (!_reactions.empty())
	{
		_numReactions = _reactions.size();
		_reactantPixelPairPos.resize(_numReactions);  // Creates enough rows to track pixel pairs for each specified reaction
		_reacProp.resize(_numReactions);  // Creates enough rows to track reaction propensities
	}
}

// Determines initial concentrations for each reactant by looping through tile matrix after reading input file
void Tile::initConc()
{
	for (const auto& row : _pixelMatrix)
		for (const auto& reactant : row)
			++_conc[reactant];
}

// Finds all possible pixel pairs by looping through tile and checking neighbors of each reactant based on reaction rules
void Tile::findPixelPairs()
{
	for (int row = 0; row < _rowSize; ++row)
	{
		for (int col = 0; col < _colSize; ++col)
		{
			if (col < _colSize - 1)  // Check if in bounds (next column) right
				populateReacPosVec({ row, col }, { row, col + 1 });  // Inputs valid reaction pixel pair under correct reaction rule

			if (col > 0)  // Check in bounds (previous column) left
				populateReacPosVec({ row, col }, { row, col - 1 });

			if (row < _rowSize - 1)  // Check if in bounds (lower row) down
				populateReacPosVec({ row, col }, { row + 1, col });

			if (row > 0)  // Check in bounds (above row) up
				populateReacPosVec({ row, col }, { row - 1, col });
		}
	}
}

// Inputs a valid pixel pair combination under proper reaction rule in _reactantPixelPairPos
void Tile::populateReacPosVec(pair<int, int> pos1, pair<int, int> pos2)
{
	string reac1 = _pixelMatrix[pos1.first][pos1.second];
	string reac2 = _pixelMatrix[pos2.first][pos2.second];

	// Validates both reactants present for reaction rules
	for (int i = 0; i < _reactions.size(); ++i)
	{
		if (reac1 != reac2 && bothInSet(reac1, reac2, _reactions[i]))  // Accounts for different reactants
			_reactantPixelPairPos[i].emplace_back(pos1, pos2);

		else if (reac1 == reac2 && _reactions[i].reactants.size() == 1 && _reactions[i].reactants.count(reac1))  // Accounts for reactants being the same
			_reactantPixelPairPos[i].emplace_back(pos1, pos2);
	}
}

// Checks that both reactants are listed under a reactions possible reactants
// .count should return 1 specifying valid
bool Tile::bothInSet(const string& reac1, const string& reac2, const Reaction& rxn)
{
	return rxn.reactants.count(reac1) && rxn.reactants.count(reac2);
}

// After previous reaction, find new pixel pairs based on updated reactants then recalculates propensities for next reaction
void Tile::tileSimStep()
{
	for (int i = 0; i < _reactantPixelPairPos.size(); ++i)  // Clear for next possible reactions
	{
		_reactantPixelPairPos[i].clear();
	}
	findPixelPairs();
	calcReacProp();
	calcTotalProp();
}

// Calculates reaction propensities by (# of possible reactions) * (specified reaction rate)
void Tile::calcReacProp()
{
	for (int i = 0; i < _reactions.size(); ++i)  // For each possible reaction, calculate propensity
	{
		_reacProp[i] = _reactantPixelPairPos[i].size() * _reactions[i].rate;
	}
}

// Calculates total propensity of tile by adding all reaction propensities
void Tile::calcTotalProp()
{
	for (int i = 0; i < _reacProp.size(); ++i)
	{
		_totalProp += _reacProp[i];
	}
	// Ex. for three reactants _totalProp = _reacProp[0] + _reacProp[1] + _reacProp[2];
}

// After reaction, update affected pixel pairs
// Previous reactants are updated to products from reaction that occurred
void Tile::updateMatrix(int rxn, int rxnIndex)
{
	pair<pair<int, int>, pair<int, int>> pair = _reactantPixelPairPos[rxn][rxnIndex];  // Get position of pair to update. Gives pair<pair, pair>

	_pixelMatrix[pair.first.first][pair.first.second] = _reactions[rxn].products.first;  // Update first pixel in matrix with new product
	_pixelMatrix[pair.second.first][pair.second.second] = _reactions[rxn].products.second;  // Update second pixel in matrix with new product

	updateConc(rxn);  // Update concentrations of matrix since matrix was changed
}

// After a reaction concentrations are updated by subtracting from reactants used, and adding to new products
// Accounts for different or same reactants
void Tile::updateConc(int rxn)
{
	for (int i = 0; i < _reactions.size(); ++i)
	{
		if (rxn == i)
		{
			++_conc[_reactions[rxn].products.first];  // Increase product conc
			++_conc[_reactions[rxn].products.second];  // Increase product conc

			for (const auto& reactant : _reactions[rxn].reactants)  // Decrease each reactant conc
			{
				if (_reactions[rxn].reactants.size() == 1)  // If reactants are the same
				{
					_conc[reactant] -= 2;
				}
				else  // Reactants are different
				{
					--_conc[reactant];
				}
			}
		}
	}
}

// Formats concentrations for vector to print to file at each time step
const vector<pair<string, int>> Tile::concToVector()
{
	vector<pair<string, int>> concs;
	for (const auto& conc : _conc)  // Add current conc to tracker
	{
		concs.push_back({ conc.first, conc.second });
	}
	return concs;
}