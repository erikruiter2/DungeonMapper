#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTabBar>
#include "dungeonmap.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    DungeonMap * scene;
    int xpos;

    int ypos;

    QString wallText(int walltype);
    short int wallIndex(QString walltype);
    QString floorText(int walltype);
    short int floorIndex(QString walltype);

    void setupWallMenu();
    void setupFloorMenu();
    QMenu * wallMenu;
    QMenu * floorMenu;
    QTabBar * MapSelectorTab;
    QLineEdit * MapSelectorEdit;
    QAction * no_wall;
    QAction * wall;
    QAction * wall_button;
    QAction * wall_hole;
    QAction * wall_keyhole;

    QAction * no_floor;
    QAction * floor;
    QAction * floor_button;
    QAction * floor_hole;
    QAction * Floor_trap;
    QAction * floor_doorhor;
    QAction * floor_doorver;
    QAction * floor_rubble;

public slots:

    void updateGridLabel();
    void editMapSelectorName(int index);
    void updateMapSelectorName();
    void updateSelectorLabel();
    void tabChanged(int index);
    void updateTileProperties();
    void removeTile();
    void show_mapPreferences();
    void load_mapfiledialog();
    void saveas_mapfiledialog();
    void updateSelectedFloor(QAction * action);
    void updateSelectedWall_left(QAction * action);
    void updateSelectedWall_right(QAction * action);
    void updateSelectedWall_up(QAction * action);
    void updateSelectedWall_down(QAction * action);
    void updateMapBookName(QString name);
    void showWallMenu();
    void addElement();
    void actionTextChecked(bool checked);
    void actionTileChecked(bool checked);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
