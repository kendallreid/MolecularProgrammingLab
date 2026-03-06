/******************************************************************************
 * File: TileIO.cpp
 * Author: Kendall Reid
 * Description:
 *     Implements the TileIO class functions. Handles reading input files,
 *     parsing the reactant matrix and reactions, and printing the Tile matrix
 *     to CSV files for simulation visualization.
 ******************************************************************************/

#include "TileIO.h"
#include "Tile.h"

// Reads tile data from a file and populates the given Tile object.
// The file is expected to have two sections:
//   ---MATRIX--- : each line corresponds to a row of the pixel matrix
//   ---REACTIONS--- : each line specifies a reaction and its rate
// The function delegates parsing of matrix rows and reactions to helper methods.
void TileIO::readFromFile(Tile& tile, const string& filename)
{
    fstream input(filename);
    if (input.good())
    {
        string line;
        bool readMatrix = false, readReactions = false;

        while (getline(input, line))
        {
            if (line == "---MATRIX---")
            {
                readMatrix = true;
                continue;
            }
            else if (line == "---REACTIONS---")
            {
                readMatrix = false;
                readReactions = true;
                continue;
            }

            if (readMatrix)
            {
                populateMatrix(tile, line);
                continue;
            }
            else if (readReactions)
            {
                populateReactions(tile, line);
                continue;
            }
        }
    }
    else { cout << "File not properly opened" << endl; }
    input.close();
}

// Parses a single line of the file as a row in the tile's pixel matrix
// Each line should contain reactants separated by commas.
// Adds the parsed row to the Tile's _pixelMatrix.
void TileIO::populateMatrix(Tile& tile, string& line)
{
    // Check if reached end of matrix input within file
    if (line.empty())
        return;

    vector<string> row;
    stringstream ss(line);
    string reactant;

    // Add reactant to proper row in matrix
    while (getline(ss, reactant, ','))
        row.push_back(reactant);

    // Add rows to matrix
    tile._pixelMatrix.push_back(row);
}

// Parses a single line of the file as a reaction and adds it to the Tile.
// Expected line format: "Reactant1+Reactant2=Product1+Product2,Rate"
// Steps:
//   1. Split line into reaction part and rate part
//   2. Split reaction part into reactants and products
//   3. Store reactants in an unordered_set for quick lookup
//   4. Store products as a pair of strings
//   5. Convert rate string to double and create a Reaction object
//   6. Append the Reaction to the Tile's _reactions vector
void TileIO::populateReactions(Tile& tile, string& line)
{
    // Check if reached end of reaction input within file
    if (line.empty())
        return;

    // Split into reaction and rate
    stringstream ss(line);
    string reaction, rate;
    getline(ss, reaction, ',');
    getline(ss, rate);

    // Split into reactants and products
    string reactant, product;
    stringstream reactSs(reaction);
    getline(reactSs, reactant, '=');
    getline(reactSs, product);

    // Parse reactants
    unordered_set<string> reactants;  // Set of all reactants
    stringstream reacSs(reactant);
    string val;
    while (getline(reacSs, val, '+'))
        reactants.insert(val);

    // Parse products
    stringstream prodSs(product);
    string first, second;
    getline(prodSs, first, '+');
    getline(prodSs, second);
    pair<string, string> products(first, second);

    // Create and store reaction
    Reaction rxn(reactants, products, stod(rate));
    tile._reactions.push_back(rxn);
}

// Print current tile to file at each time step to allow for python visuals
void TileIO::printMatrixToFile(Tile& tile, const string& filename)
{
    std::ofstream csvFile(filename, std::ios::out | std::ios::app);
    if (csvFile.good())
    {
        for (const auto& row : tile._pixelMatrix)
        {
            for (int j = 0; j < row.size(); ++j)
            {
                csvFile << row[j];
                if (j + 1 < row.size()) csvFile << ",";
            }
            csvFile << "\n";
        }
        csvFile << "\n";
    }
    else { cout << "File not properly opened" << endl; }
    csvFile.close();
}