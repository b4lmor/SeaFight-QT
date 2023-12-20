#ifndef SEAFIGHT_H
#define SEAFIGHT_H

#include <vector>

#define TABLE_SQUARE_SIZE 10

#define BATTLESHIP_NUMBER 1
#define CRUISER_NUMBER 2
#define DESTROYER_NUMBER 3
#define TORPEDO_BOAT_NUMBER 4

#define BATTLESHIP_SIZE 4
#define CRUISER_SIZE 3
#define DESTROYER_SIZE 2
#define TORPEDO_BOAT_SIZE 1

#define TOTAL_SHIP_SIZE (BATTLESHIP_NUMBER * BATTLESHIP_SIZE + CRUISER_NUMBER * CRUISER_SIZE \
                        + DESTROYER_NUMBER * DESTROYER_SIZE + TORPEDO_BOAT_NUMBER * TORPEDO_BOAT_SIZE)

enum class CellState {
    EMPTY,
    SHIP,
    HIT,
    SHIPWRECK_AREA,
    CRUSHED_SHIP
};

enum class Ship {
    BATTLE_SHIP = 4,
    CRUISER = 3,
    DESTROYER = 2,
    TORPEDO_BOAT = 1
};

enum class Direction {
    VERTICAL,
    HORIZONTAL
};

enum class HitResult {
    MISS,
    HIT,
    BAD_CELL
};

struct Point {
    int x;
    int y;
};

class SeaGrid
{
private:
    CellState * cells;
    int hitCount;

    int battleShips;
    int cruisers;
    int destroyers;
    int torpedoBoats;

    std::vector<std::vector<Point>> ships;
public:
    SeaGrid();
    ~SeaGrid() { delete[] cells; }
    CellState getCell(int row, int col);

    bool isAbleToSet(Ship ship, Direction dir, int x, int y);
    void setShip(Ship ship, Direction dir, int x, int y);
    bool shipReachedLimit(Ship ship);

    HitResult hit(int x, int y);
    bool allShipsDestroyed();

    void clear();
private:
    void setCell(int row, int col, CellState state);
    bool hasShipNeighbours(int row, int col);

    void checkShipwreck();
    void fillShipwreck(std::vector<Point> coords);
    void markShipwreckCell(int x, int y);
};

class SeaFightBot {
public:
    static void init_gird(SeaGrid * grid);
    static HitResult hit(SeaGrid * grid);
};

#endif // SEAFIGHT_H
