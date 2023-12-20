#include "mainwindow.h"
#include "./ui_mainwindow.h"

static void initTable(QTableWidget * table);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Tabs->tabBar()->hide();
    ui->Tabs->setCurrentWidget(ui->start_tab);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_PB_Quit_clicked()
{
    this->close();
}

void MainWindow::checkShips() {
    if (user_grid.shipReachedLimit(Ship::BATTLE_SHIP)) {
        ui->Checker_Battleship->setChecked(true);
    }
    if (user_grid.shipReachedLimit(Ship::CRUISER)) {
        ui->Checker_Cruiser->setChecked(true);
    }
    if (user_grid.shipReachedLimit(Ship::DESTROYER)) {
        ui->Checker_Destroyer->setChecked(true);
    }
    if (user_grid.shipReachedLimit(Ship::TORPEDO_BOAT)) {
        ui->Checker_TorpedoBoat->setChecked(true);
    }
}

void MainWindow::on_PB_Play_clicked()
{
    ui->Table_BattlePreparation->clear();
    user_grid.clear();
    bot_grid.clear();

    ui->Tabs->setCurrentWidget(ui->preparation_tab);

    initTable(ui->Table_BattlePreparation);

    ui->Checker_Battleship->setEnabled(false);
    ui->Checker_Cruiser->setEnabled(false);
    ui->Checker_Destroyer->setEnabled(false);
    ui->Checker_TorpedoBoat->setEnabled(false);

    ui->Checker_Battleship->setChecked(false);
    ui->Checker_Cruiser->setChecked(false);
    ui->Checker_Destroyer->setChecked(false);
    ui->Checker_TorpedoBoat->setChecked(false);

    curShip = Ship::TORPEDO_BOAT;
    ui->RB_TorpedoBoat->setChecked(true);

    curDirection = Direction::HORIZONTAL;
    ui->RB_Horizontal->setChecked(true);
}

void MainWindow::on_PB_resign_clicked()
{
    ui->Tabs->setCurrentWidget(ui->defeat_tab);
}

void MainWindow::on_PB_to_main_menu_after_defeat_clicked()
{
    ui->Tabs->setCurrentWidget(ui->start_tab);
}


void MainWindow::on_PB_to_main_menu_after_victory_clicked()
{
    ui->Tabs->setCurrentWidget(ui->start_tab);
}

void initTable(QTableWidget * table) {
    char column = 'A';
    int row = 1;
    QStringList columns;
    QStringList rows;
    for (int i = 0; i < TABLE_SQUARE_SIZE; i++) {
        for (int j = 0; j < TABLE_SQUARE_SIZE; j++) {
            columns << QString(column++);
            rows << QString::number(row++);
        }
    }

    table->setColumnCount(TABLE_SQUARE_SIZE);
    table->setRowCount(TABLE_SQUARE_SIZE);
    table->setHorizontalHeaderLabels(columns);
    table->setVerticalHeaderLabels(rows);
}

void MainWindow::on_Table_BattlePreparation_cellClicked(int row, int column)
{
    if (user_grid.isAbleToSet(curShip, curDirection, row, column)
        && !user_grid.shipReachedLimit(curShip)) {
        user_grid.setShip(curShip, curDirection, row, column);
        checkShips();

        ui->Table_BattlePreparation->clearContents();
        for (int row = 0; row < TABLE_SQUARE_SIZE; row++) {
            for (int col = 0; col < TABLE_SQUARE_SIZE; col++) {
                CellState state = user_grid.getCell(row, col);
                Qt::GlobalColor color = state == CellState::SHIP ? Qt::darkGray : Qt::white;

                auto * item = new QTableWidgetItem();
                item->setBackground(color);
                ui->Table_BattlePreparation->setItem(row, col, item);
            }
        }
    }
}


void MainWindow::on_RB_BattleShip_clicked()
{
    curShip = Ship::BATTLE_SHIP;
}


void MainWindow::on_RB_Cruiser_clicked()
{
    curShip = Ship::CRUISER;
}


void MainWindow::on_RB_Destroyer_clicked()
{
    curShip = Ship::DESTROYER;
}


void MainWindow::on_RB_TorpedoBoat_clicked()
{
    curShip = Ship::TORPEDO_BOAT;
}


void MainWindow::on_RB_Vertical_clicked()
{
    curDirection = Direction::VERTICAL;
}


void MainWindow::on_RB_Horizontal_clicked()
{
    curDirection = Direction::HORIZONTAL;
}


void MainWindow::on_PB_endPreparation_clicked()
{
    if (ui->Checker_Battleship->isChecked()
        && ui->Checker_Cruiser->isChecked()
        && ui->Checker_Destroyer->isChecked()
        && ui->Checker_TorpedoBoat->isChecked()) {

        ui->Table_bot->clear();
        ui->Table_user->clear();

        ui->Tabs->setCurrentWidget(ui->sea_fight_tab);

        initTable(ui->Table_user);
        initTable(ui->Table_bot);

        SeaFightBot::init_gird(&bot_grid);

        for (int row = 0; row < TABLE_SQUARE_SIZE; row++) {
            for (int col = 0; col < TABLE_SQUARE_SIZE; col++) {
                CellState user_state = user_grid.getCell(row, col);
                Qt::GlobalColor user_color = user_state == CellState::SHIP ? Qt::darkGray : Qt::white;
                auto * user_item = new QTableWidgetItem();
                user_item->setBackground(user_color);
                ui->Table_user->setItem(row, col, user_item);

                auto * bot_item = new QTableWidgetItem();
                bot_item->setBackground(Qt::white);
                ui->Table_bot->setItem(row, col, bot_item);
            }
        }

        this->isUserTurn = true;
    }
}


void MainWindow::on_Table_bot_cellClicked(int row, int column)
{
    if (!isUserTurn) {
        return;
    }

    auto res = bot_grid.hit(row, column);
    if (res == HitResult::BAD_CELL) {
        return;
    }
    if (bot_grid.allShipsDestroyed()) {
        ui->Tabs->setCurrentWidget(ui->victory_tab);
    }
    if (res != HitResult::HIT) {
        isUserTurn = false;
    }

    while (!isUserTurn) {
        res = SeaFightBot::hit(&user_grid);
        if (user_grid.allShipsDestroyed()) {
            ui->Tabs->setCurrentWidget(ui->defeat_tab);
        }
        if (res != HitResult::HIT) {
            isUserTurn = true;
        }
    }

    ui->Table_user->clearContents();
    ui->Table_bot->clearContents();
    for (int row = 0; row < TABLE_SQUARE_SIZE; row++) {
        for (int col = 0; col < TABLE_SQUARE_SIZE; col++) {
            CellState user_state = user_grid.getCell(row, col);
            Qt::GlobalColor user_color = Qt::white;
            switch(user_state) {
            case CellState::CRUSHED_SHIP:
                user_color = Qt::yellow;
                break;
            case CellState::SHIP:
                user_color = Qt::darkGray;
                break;
            case CellState::HIT:
                user_color = Qt::red;
                break;
            case CellState::EMPTY:
                user_color = Qt::white;
                break;
            case CellState::SHIPWRECK_AREA:
                user_color = Qt::darkBlue;
                break;
            }
            auto * user_item = new QTableWidgetItem();
            user_item->setBackground(user_color);
            ui->Table_user->setItem(row, col, user_item);


            CellState bot_state = bot_grid.getCell(row, col);
            Qt::GlobalColor bot_color = Qt::white;
            switch(bot_state) {
            case CellState::CRUSHED_SHIP:
                bot_color = Qt::yellow;
                break;
            case CellState::SHIP:
                bot_color = Qt::white;
                break;
            case CellState::HIT:
                bot_color = Qt::red;
                break;
            case CellState::EMPTY:
                bot_color = Qt::white;
                break;
            case CellState::SHIPWRECK_AREA:
                bot_color = Qt::darkBlue;
                break;
            }
            auto * bot_item = new QTableWidgetItem();
            bot_item->setBackground(bot_color);
            ui->Table_bot->setItem(row, col, bot_item);
        }
    }
}


void MainWindow::on_PB_RandomGridFiller_clicked()
{
    SeaFightBot::init_gird(&user_grid);
    ui->Checker_Battleship->setChecked(true);
    ui->Checker_Cruiser->setChecked(true);
    ui->Checker_Destroyer->setChecked(true);
    ui->Checker_TorpedoBoat->setChecked(true);
    ui->PB_endPreparation->click();
}

