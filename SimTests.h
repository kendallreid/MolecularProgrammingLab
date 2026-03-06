/******************************************************************************
 * File: SimTests.h
 * Author: Kendall Reid
 * Description:
 *     Declares the SimTests class, which performs unit tests on the Tile
 *     and simulation functionality. Includes tests for matrix initialization,
 *     reaction loading, concentration calculations, pixel pair detection,
 *     and simulation steps to validate correct behavior.
 *****************************************************************************/

#ifndef SIM_TESTS
#define SIM_TESTS

#include "Tile.h"
#include <iostream>
#include <cassert>

// Tests simulation
class SimTests{
public:
    SimTests() {}

    // Runs all test cases
    void runAllTests();

private:
    // Tests different sized tiles
    // Emtpy, single, small, large
    void testEmptyTile();
    void testSinglePixelTile();
    void testSmallTileMatrix();
    void testLargeTile();

    // Tests initial setup
    void testReactionsLoaded();
    void testInitialConcentrations();

    // Test finding pixel pairs and propensity updates based on found pairs
    void testPixelPairs();
    void testCalcReacProp();
    void testTotalProp();

    // Tests updating
    void testUpdateMatrix();
    void testUpdateConc();

    // Tests reactions
    // Single reactant, invalid reactant
    void testSingleReactantReaction();
    void testNoReaction();

    // Other test cases
    void testTileSimStep();
    void testConcToVector();
    void testReactionParsing();
    void testEmptyRow();
    void testTwoProducts();
    void testIdenticalReactants();
    void testMultipleReactions();
};
#endif