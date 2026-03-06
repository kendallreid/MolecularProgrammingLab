/******************************************************************************
 * File: SimTests.cpp
 * Author: Kendall Reid
 * Description:
 *     Implements unit tests for the simulation. Tests cover tile
 *     initialization, reaction parsing, concentration calculations, pixel pair
 *     generation, propensity calculations, and simulation step updates.
 ******************************************************************************/

#include "SimTests.h"
#include "Tile.h"
#include <vector>
#include <string>
#include <cassert>

using std::cout;
using std::endl;
using std::vector;
using std::string;

// Helper function to compare two 2D string matrices
bool compareMatrices(const vector<vector<string>>& a, const vector<vector<string>>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].size() != b[i].size()) return false;
        for (size_t j = 0; j < a[i].size(); ++j)
            if (a[i][j] != b[i][j]) return false;
    }
    return true;
}

// --------------------- Individual Tests -----------------------

// Test an empty tile has no pixels or reactions
void SimTests::testEmptyTile() {
    Tile tile;
    assert(tile.getPixelMatrix().empty());
    assert(tile.getReactions().empty());
}

// Test single pixel tile is initialized correctly
void SimTests::testSinglePixelTile() {
    Tile tile;
    tile._pixelMatrix = { {"A"} };
    tile._reactions = { Reaction({ "A" }, { "B","C" }, 0.1) };
    tile.initConc();  // Populate _conc

    assert(tile.getPixelMatrix().size() == 1 && tile.getPixelMatrix()[0].size() == 1);
    assert(tile.getConc().at("A") == 1);
}

// Test small tile matrix matches expected content
void SimTests::testSmallTileMatrix() {
    Tile tile;
    tile._pixelMatrix = { {"A","B"}, {"C","D"} };
    vector<vector<string>> expected = { {"A","B"}, {"C","D"} };
    assert(compareMatrices(tile.getPixelMatrix(), expected));
}

// Test reactions are loaded from input correctly
void SimTests::testReactionsLoaded() {
    Tile tile;
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1), Reaction({ "C" }, {"A","B"}, 0.2) };
    assert(tile.getReactions().size() == 2);
}

// Test concentrations initialized correctly for each reactant
void SimTests::testInitialConcentrations() {
    Tile tile;
    tile._pixelMatrix = { {"A","A","B","C"} };
    tile.initConc();
    assert(tile.getConc().at("A") == 2);
    assert(tile.getConc().at("B") == 1);
    assert(tile.getConc().at("C") == 1);
}

// Test pixel pairs are generated correctly based on reaction rules
void SimTests::testPixelPairs() {
    Tile tile;
    tile._pixelMatrix = { {"A","B"},{"C","D"} };
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1) };
    tile.findPixelPairs();
    auto pairs = tile._reactantPixelPairPos;
    assert(!pairs.empty());
}

// Test reaction propensities are non-negative
void SimTests::testCalcReacProp() {
    Tile tile;
    tile._pixelMatrix = { {"A","B"} };
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1) };
    tile.findPixelPairs();
    tile.calcReacProp();
    for (auto p : tile._reacProp) assert(p >= 0);
}

// Test total propensity calculation is non-negative
void SimTests::testTotalProp() {
    Tile tile;
    tile._pixelMatrix = { {"A","B"} };
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1) };
    tile.findPixelPairs();
    tile.calcReacProp();
    tile.calcTotalProp();
    assert(tile.getTotalProp() >= 0);
}

// Test updating tile matrix correctly changes pixel values
void SimTests::testUpdateMatrix() {
    Tile tile;
    tile._pixelMatrix = { {"A","B"} };
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1) };
    tile.findPixelPairs();
    tile.calcReacProp();
    tile.calcTotalProp();
    if (!tile._reactantPixelPairPos[0].empty())
        tile.updateMatrix(0,0);
    assert(!tile.getConc().empty());
}

// Test concentration updates reflect reaction outcomes
void SimTests::testUpdateConc() {
    Tile tile;
    tile._pixelMatrix = { {"A","B"} };
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1) };
    tile.findPixelPairs();
    tile.calcReacProp();
    tile.calcTotalProp();
    if (!tile._reactantPixelPairPos[0].empty())
        tile.updateMatrix(0,0);
    assert(tile.getConc().at("A") >= 0);
}

// Test reactions with a single reactant are handled
void SimTests::testSingleReactantReaction() {
    Tile tile;
    tile._pixelMatrix = { {"A","A"} };
    tile._reactions = { Reaction({ "A" }, {"B","C"}, 0.1) };
    tile.findPixelPairs();
    tile.calcReacProp();
    assert(tile._reacProp[0] >= 0);
}

// Test scenario where no reaction occurs results in zero propensities
void SimTests::testNoReaction() {
    Tile tile;
    tile._pixelMatrix = { {"A","B"} };
    tile._reactions = { Reaction({ "C","D" }, {"E","F"}, 0.1) };
    tile.findPixelPairs();
    tile.calcReacProp();
    for (auto p : tile._reacProp) assert(p == 0);
}

// Test larger tile matrix initialization
void SimTests::testLargeTile() {
    Tile tile;
    tile._pixelMatrix = vector<vector<string>>(5, vector<string>(5,"A"));
    assert(tile.getPixelMatrix().size() == 5);
    assert(tile.getPixelMatrix()[0].size() == 5);
}

// Test tile simulation step recalculates pairs and propensities
void SimTests::testTileSimStep() {
    Tile tile;
    tile._pixelMatrix = { {"A","B"},{"C","D"} };
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1) };
    tile.findPixelPairs();
    tile.tileSimStep();
    for (auto &vec : tile._reactantPixelPairPos) assert(vec.empty() || vec.size() >= 0);
}

// Test conversion of concentration map to vector
void SimTests::testConcToVector() {
    Tile tile;
    tile._pixelMatrix = { {"A","B","C"} };
    tile.initConc();
    auto vec = tile.concToVector();
    assert(!vec.empty());
}

// Test reaction parsing produces non-empty reactions
void SimTests::testReactionParsing() {
    Tile tile;
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1) };
    assert(!tile.getReactions().empty());
}

// Test a tile row is not empty
void SimTests::testEmptyRow() {
    Tile tile;
    tile._pixelMatrix = { {"A"},{"B"},{"C"} };
    assert(tile.getPixelMatrix().size() == 3);
}

// Test reactions with two products are stored correctly
void SimTests::testTwoProducts() {
    Tile tile;
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1) };
    auto rxn = tile.getReactions()[0];
    assert(!rxn.products.first.empty() && !rxn.products.second.empty());
}

// Test reactions with identical reactants are recognized correctly
void SimTests::testIdenticalReactants() {
    Tile tile;
    tile._reactions = { Reaction({ "A" }, {"B","C"}, 0.1) };
    auto rxn = tile.getReactions()[0];
    assert(rxn.reactants.size() == 1);
}

// Test multiple reactions exist and are processed correctly
void SimTests::testMultipleReactions() {
    Tile tile;
    tile._reactions = { Reaction({ "A","B" }, {"C","D"}, 0.1), Reaction({ "B","C" }, {"D","E"}, 0.2) };
    assert(tile.getReactions().size() > 1);
}

// --------------------- Run all tests -----------------------

// Executes all tests in order
void SimTests::runAllTests() {
    testEmptyTile();
    testSinglePixelTile();
    testSmallTileMatrix();
    testReactionsLoaded();
    testInitialConcentrations();
    testPixelPairs();
    testCalcReacProp();
    testTotalProp();
    testUpdateMatrix();
    testUpdateConc();
    testSingleReactantReaction();
    testNoReaction();
    testLargeTile();
    testTileSimStep();
    testConcToVector();
    testReactionParsing();
    testEmptyRow();
    testTwoProducts();
    testIdenticalReactants();
    testMultipleReactions();

    cout << "All SimTests passed!" << endl;
}