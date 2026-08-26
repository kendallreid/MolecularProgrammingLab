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

        bool readMatrix = false;
        bool readReactions = false;

        while (getline(input, line))
        {
            // Skip empty lines
            if (line.empty())
                continue;

            // Skip comments
            if (line[0] == '#')
                continue;

            if (line == "!START_TRANSITION_RULES")
            {
                readReactions = true;
                readMatrix = false;
                continue;
            }
            else if (line == "!END_TRANSITION_RULES")
            {
                readReactions = false;
                continue;
            }
            else if (line == "!START_INIT_STATE")
            {
                readMatrix = true;
                readReactions = false;
                continue;
            }
            else if (line == "!END_INIT_STATE")
            {
                readMatrix = false;
                continue;
            }
            else if (line == "!START_COLORMAP")
            {
                continue;
            }
            else if (line == "!END_COLORMAP")
            {
                continue;
            }

            // Read reaction
            if (readReactions)
            {
                populateReactions(tile, line);
            }

            // Read matrix row
            else if (readMatrix)
            {
                populateMatrix(tile, line);
            }
        }
    }
    else
    {
        cout << "File not properly opened" << endl;
    }
    input.close();
}

// Parses a single line of the file as a row in the tile's pixel matrix
// Each line should contain reactants separated by commas.
// Adds the parsed row to the Tile's _pixelMatrix.
void TileIO::populateMatrix(Tile& tile, string& line)
{
    if (line.empty())
        return;

    vector<string> row;
    stringstream ss(line);
    string reactant;

    // Read each space-separated reactant from the row
    while (ss >> reactant)
    {
        row.push_back(reactant);
    }

    if (!row.empty())
    {
        tile._pixelMatrix.push_back(row);
    }
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

    // Example line:
    // A + B -> C + D (1)

    stringstream ss(line);

    string firstReactant;
    string plus;
    string secondReactant;
    string arrow;
    string firstProduct;
    string secondProduct;
    string rate;

    // Read each part of reaction
    ss >> firstReactant
       >> plus
       >> secondReactant
       >> arrow
       >> firstProduct
       >> plus
       >> secondProduct
       >> rate;

    // Remove parentheses from rate
    rate = rate.substr(1, rate.size() - 2);

    // Store reactants and products
    pair<string, string> reactants(firstReactant, secondReactant);
    pair<string, string> products(firstProduct, secondProduct);

    // Create and store reaction
    Reaction rxn(reactants, products, stod(rate));
    tile._reactions.push_back(rxn);
}

// Print current tile to file at each time step to allow for python visuals
void TileIO::printMatrixToFile(const Tile& tile, const string& filename)
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