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

// Finds all possible pixel pairs for original matrix by looping through tile and checking neighbors of each reactant based on reaction rules
void Tile::findInitialPixelPairs()
{
	for (int row = 0; row < _rowSize; ++row)
	{
		for (int col = 0; col < _colSize; ++col)
		{
			if (col < _colSize - 1)  // Check if in bounds (next column) right
				populateReacPosVec({ row, col }, { row, col + 1 });  // Inputs valid reaction pixel pair under correct reaction rule
			if (row < _rowSize - 1)  // Check if in bounds (lower row) down
				populateReacPosVec({ row, col }, { row + 1, col });
		}
	}
}

void Tile::removeLocalPairs(pair<int, int> pos) {
	/*string reactant = _pixelMatrix[pos.first][pos.second];

	for (int rxn = 0; rxn < _reactantPixelPairPos.size(); ++rxn) {
		if (reactant == _reactions[rxn].reactants.first || reactant == _reactions[rxn].reactants.second) {  // if position reactant is used in a reaction remove surrounding possible reaction pairs
			auto& reactionVec = _reactantPixelPairPos[rxn];
			for (int i = reactionVec.size() - 1; i >= 0; --i) {
				if (reactionVec[i].first == pos || reactionVec[i].second == pos) {
					reactionVec.erase(reactionVec.begin() + i);
				}
			}
		}
	}*/
	for (auto& reactionVec : _reactantPixelPairPos)
	{
		for (int i = static_cast<int>(reactionVec.size()) - 1; i >= 0; --i)
		{
			if (reactionVec[i].first == pos || reactionVec[i].second == pos)
			{
				reactionVec.erase(reactionVec.begin() + i);
			}
		}
	}
}

void Tile::addLocalPairsHelper(pair<int, int> pos, pair<int, int> otherChangedPos)
{
	int row = pos.first;
	int col = pos.second;
	pair<int, int> neighbor;

	// Right neighbor
	if (col < _colSize - 1)
	{
		neighbor = { row, col + 1 };
		if (neighbor != otherChangedPos)
		{
			populateReacPosVec({ row, col }, neighbor);
		}
	}

	// Left neighbor
	if (col > 0)
	{
		neighbor = { row, col - 1 };
		if (neighbor != otherChangedPos)
		{
			populateReacPosVec(neighbor, { row, col });
		}
	}

	// Down neighbor
	if (row < _rowSize - 1)
	{
		neighbor = { row + 1, col };
		if (neighbor != otherChangedPos)
		{
			populateReacPosVec({ row, col }, neighbor);
		}
	}

	// Up neighbor
	if (row > 0)
	{
		neighbor = { row - 1, col };
		if (neighbor != otherChangedPos)
		{
			populateReacPosVec(neighbor, { row, col });
		}
	}
}

void Tile::addLocalPairs(pair<int, int> pos1, pair<int,int> pos2) {
	addLocalPairsHelper(pos1, pos2);
	addLocalPairsHelper(pos2, pos1);
	populateReacPosVec(pos1, pos2);
}


// Inputs a valid pixel pair combination under proper reaction rule in _reactantPixelPairPos
void Tile::populateReacPosVec(pair<int, int> pos1, pair<int, int> pos2)
{
	string reac1 = _pixelMatrix[pos1.first][pos1.second];
	string reac2 = _pixelMatrix[pos2.first][pos2.second];

	for (int i = 0; i < _reactions.size(); ++i)
	{
		string r1 = _reactions[i].reactants.first;
		string r2 = _reactions[i].reactants.second;

		bool differentReactantsMatch = reac1 != reac2 && ((reac1 == r1 && reac2 == r2) || (reac1 == r2 && reac2 == r1));
		bool sameReactantsMatch = reac1 == reac2 && r1 == r2 && reac1 == r1;

		if (differentReactantsMatch || sameReactantsMatch)
		{
			_reactantPixelPairPos[i].emplace_back(pos1, pos2);
		}
	}
}

// After previous reaction, find new pixel pairs based on updated reactants then recalculates propensities for next reaction
void Tile::tileSimStepSetUp()
{
	for (auto& reactionPairs : _reactantPixelPairPos)
	{
		reactionPairs.clear();
	}
	findInitialPixelPairs();
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
	_totalProp = 0;

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
	pair<pair<int, int>, pair<int, int>> pixelPair = _reactantPixelPairPos[rxn][rxnIndex];
	pair<int, int> pos1 = pixelPair.first;
	pair<int, int> pos2 = pixelPair.second;

	string val1 = _pixelMatrix[pos1.first][pos1.second];
	string val2 = _pixelMatrix[pos2.first][pos2.second];

	string r1 = _reactions[rxn].reactants.first;
	string r2 = _reactions[rxn].reactants.second;

	bool validPair =
		(val1 == r1 && val2 == r2) ||
		(val1 == r2 && val2 == r1);

	if (!validPair)
	{
		cout << "STALE PAIR FOUND" << endl;
		return;
	}

	// Remove old possible pairs involving changed cells
	removeLocalPairs(pos1);
	removeLocalPairs(pos2);

	// Update matrix - ensures correct order
	if (_pixelMatrix[pos1.first][pos1.second] == _reactions[rxn].reactants.first) {
		_pixelMatrix[pos1.first][pos1.second] = _reactions[rxn].products.first;
		_pixelMatrix[pos2.first][pos2.second] = _reactions[rxn].products.second;
	}
	else {
		_pixelMatrix[pos1.first][pos1.second] = _reactions[rxn].products.second;
		_pixelMatrix[pos2.first][pos2.second] = _reactions[rxn].products.first;
	}
	updateConc(rxn);

	// add new possible pairs involving changed cells
	addLocalPairs(pos1,pos2);

	// recalculate propensities from updated possible pair lists
	calcReacProp();
	calcTotalProp();
}

// After a reaction concentrations are updated by subtracting from reactants used, and adding to new products
// Accounts for different or same reactants
void Tile::updateConc(int rxn)
{
	for (int i = 0; i < _reactions.size(); ++i)
	{
		if (rxn == i)
		{
			++_conc[_reactions[rxn].products.first];
			++_conc[_reactions[rxn].products.second];
			--_conc[_reactions[rxn].reactants.first];
			--_conc[_reactions[rxn].reactants.second];
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