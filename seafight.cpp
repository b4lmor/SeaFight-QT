#include "seafight.h"
#include <QRandomGenerator>

SeaGrid::SeaGrid() {
    this->cells = new CellState[TABLE_SQUARE_SIZE * TABLE_SQUARE_SIZE];
    for (int row = 0; row < TABLE_SQUARE_SIZE; row++) {
        for (int col = 0; col < TABLE_SQUARE_SIZE; col++) {
            setCell(row, col, CellState::EMPTY);
        }
    }

    battleShips = 0;
    cruisers = 0;
    destroyers = 0;
    torpedoBoats = 0;
    hitCount = 0;
}

bool SeaGrid::shipReachedLimit(Ship ship) {
    switch(ship) {
    case Ship::BATTLE_SHIP:
        return battleShips >= BATTLESHIP_NUMBER;
    case Ship::CRUISER:
        return cruisers >= CRUISER_NUMBER;
    case Ship::DESTROYER:
        return destroyers >= DESTROYER_NUMBER;
    case Ship::TORPEDO_BOAT:
        return torpedoBoats >= TORPEDO_BOAT_NUMBER;
    }
    return false;
}

CellState SeaGrid::getCell(int row, int col) {
    if (0 > row || row >= TABLE_SQUARE_SIZE || 0 > col || col >= TABLE_SQUARE_SIZE) {
        return CellState::EMPTY;
    }
    return cells[row * TABLE_SQUARE_SIZE + col];
}

bool SeaGrid::hasShipNeighbours(int row, int col) {
    return getCell(row, col)         == CellState::SHIP
        || getCell(row + 1, col)     == CellState::SHIP
        || getCell(row - 1, col)     == CellState::SHIP
        || getCell(row, col + 1)     == CellState::SHIP
        || getCell(row, col - 1)     == CellState::SHIP
        || getCell(row + 1, col + 1) == CellState::SHIP
        || getCell(row + 1, col - 1) == CellState::SHIP
        || getCell(row - 1, col + 1) == CellState::SHIP
        || getCell(row - 1, col - 1) == CellState::SHIP;
}

bool SeaGrid::isAbleToSet(Ship ship, Direction dir, int x, int y) {
    for (int l = 0; l < static_cast<int>(ship); l++) {
        int new_x = dir == Direction::VERTICAL ? x + l : x;
        int new_y = dir == Direction::VERTICAL ? y : y + l;
        if (hasShipNeighbours(new_x, new_y)
            || (0 > new_x || new_x >= TABLE_SQUARE_SIZE || 0 > new_y || new_y >= TABLE_SQUARE_SIZE)) {
            return false;
        }
    }
    return true;
}

void SeaGrid::setCell(int row, int col, CellState state) {
    if (0 > row || row >= TABLE_SQUARE_SIZE || 0 > col || col >= TABLE_SQUARE_SIZE) {
        return;
    }
    cells[row * TABLE_SQUARE_SIZE + col] = state;
}

void SeaGrid::setShip(Ship ship, Direction dir, int x, int y) {
    switch(ship) {
    case Ship::BATTLE_SHIP:
        battleShips++;
        break;
    case Ship::CRUISER:
        cruisers++;
        break;
    case Ship::DESTROYER:
        destroyers++;
        break;
    case Ship::TORPEDO_BOAT:
        torpedoBoats++;
        break;
    }

    std::vector<Point> shipCoords;

    for (int l = 0; l < static_cast<int>(ship); l++) {
        int new_x = dir == Direction::VERTICAL ? x + l : x;
        int new_y = dir == Direction::VERTICAL ? y : y + l;
        setCell(new_x, new_y, CellState::SHIP);
        shipCoords.push_back({new_x, new_y});
    }
    ships.push_back(shipCoords);
}

void SeaGrid::checkShipwreck() {

    for (const auto& coords : ships) {
        int cnt = 0;
        for (const Point point : coords) {
            cnt += getCell(point.x, point.y) != CellState::CRUSHED_SHIP;
        }
        if (cnt == 0) {
            fillShipwreck(coords);
        }
    }
}

void SeaGrid::fillShipwreck(std::vector<Point> coords) {
    for (const auto point : coords) {
        int x = point.x;
        int y = point.y;
        markShipwreckCell(x - 1, y);
        markShipwreckCell(x + 1, y);
        markShipwreckCell(x, y - 1);
        markShipwreckCell(x, y + 1);
        markShipwreckCell(x - 1, y - 1);
        markShipwreckCell(x + 1, y + 1);
        markShipwreckCell(x + 1, y - 1);
        markShipwreckCell(x - 1, y + 1);
    }
}

void SeaGrid::markShipwreckCell(int x, int y) {
    auto cell = getCell(x, y);
    if (cell == CellState::EMPTY) {
        setCell(x, y, CellState::SHIPWRECK_AREA);
    }
}

bool SeaGrid::allShipsDestroyed() {
    return hitCount == TOTAL_SHIP_SIZE;
}

HitResult SeaGrid::hit(int x, int y) {
    auto cell = getCell(x, y);
    if (cell == CellState::CRUSHED_SHIP || cell == CellState::SHIPWRECK_AREA || cell == CellState::HIT) {
        return HitResult::BAD_CELL;
    } else if (cell == CellState::SHIP) {
        hitCount++;
        setCell(x, y, CellState::CRUSHED_SHIP);
        checkShipwreck();
        return HitResult::HIT;
    } else {
        setCell(x, y, CellState::HIT);
        return HitResult::MISS;
    }
}

void SeaFightBot::init_gird(SeaGrid * grid) {
    const std::vector<Ship> ships = {Ship::BATTLE_SHIP, Ship::CRUISER, Ship::DESTROYER, Ship::TORPEDO_BOAT};
    for (auto curShip : ships) {
        while (!grid->shipReachedLimit(curShip)) {
            int x =  QRandomGenerator::global()->bounded(TABLE_SQUARE_SIZE);
            int y =  QRandomGenerator::global()->bounded(TABLE_SQUARE_SIZE);
            Direction curDir = QRandomGenerator::global()->bounded(2) & 1
                                   ? Direction::HORIZONTAL
                                   : Direction::VERTICAL;
            if (grid->isAbleToSet(curShip, curDir, x, y)) {
                grid->setShip(curShip, curDir, x, y);
            }
        }
    }
}

HitResult SeaFightBot::hit(SeaGrid * grid) {
    int x;
    int y;
    CellState cell;
    do {
        x = QRandomGenerator::global()->bounded(TABLE_SQUARE_SIZE);
        y = QRandomGenerator::global()->bounded(TABLE_SQUARE_SIZE);
        cell = grid->getCell(x, y);
    } while (cell == CellState::HIT || cell == CellState::SHIPWRECK_AREA);
    return grid->hit(x, y);
}

void SeaGrid::clear() {
    for (int i = 0; i < TABLE_SQUARE_SIZE * TABLE_SQUARE_SIZE; i++) {
        this->cells[i] = CellState::EMPTY;
    }
    this->hitCount = 0;
    this->battleShips = 0;
    this->cruisers = 0;
    this->destroyers = 0;
    this->torpedoBoats = 0;
    this->ships.clear();
}
