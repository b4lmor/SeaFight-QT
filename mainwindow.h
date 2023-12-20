#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "seafight.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void checkShips();
    static void renderTable(QTableWidget * table, SeaGrid grid);

private slots:
    void on_PB_Play_clicked();

    void on_PB_to_main_menu_after_defeat_clicked();

    void on_PB_to_main_menu_after_victory_clicked();

    void on_Table_BattlePreparation_cellClicked(int row, int column);

    void on_RB_BattleShip_clicked();

    void on_RB_Cruiser_clicked();

    void on_RB_Destroyer_clicked();

    void on_RB_TorpedoBoat_clicked();

    void on_RB_Vertical_clicked();

    void on_RB_Horizontal_clicked();

    void on_PB_endPreparation_clicked();

    void on_PB_resign_clicked();

    void on_PB_Quit_clicked();

    void on_Table_bot_cellClicked(int row, int column);

    void on_PB_RandomGridFiller_clicked();

private:
    Ui::MainWindow *ui;
    SeaGrid user_grid;
    SeaGrid bot_grid;
    Ship curShip;
    Direction curDirection;
    bool isUserTurn;
};
#endif // MAINWINDOW_H



