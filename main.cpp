#include "Sim.h"
#include "Tile.h"

int main()
{
    Tile t("input.csv");
    Sim simulation;
    simulation.runSim(100);
}