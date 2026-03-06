/******************************************************************************
 * File: TileIO.h
 * Author: Kendall Reid
 * Description:
 *     Declares the TileIO static helper class for file input/output.
 *     Provides functions to read a Tile from a CSV file, populate its matrix
 *     and reactions, and write the current matrix to a file for visualization.
 ******************************************************************************/

#ifndef TILEIO
#define TILEIO
#include "Tile.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using std::fstream;
using std::stringstream;
using std::getline;
using std::cout;
using std::endl;
using std::string;

class Tile;

class TileIO
{
public:
    // Reads input file for tile and reactions
    static void readFromFile(Tile& tile, const string& filename);
    // When in matrix section of input file, populates each tile pixel with reactant values
    static void populateMatrix(Tile& tile, string& line);
    // When in reaction section of input file, populates reaction rules
    static void populateReactions(Tile& tile, string& line);
    // Prints matrix at each step to output file
    static void printMatrixToFile(Tile& tile, const string& filename);
};
#endif