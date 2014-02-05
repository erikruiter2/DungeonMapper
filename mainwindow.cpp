#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_preferences.h"
#include "dungeonmap.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QTabBar>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setAttribute(Qt::WA_TranslucentBackground, false);

    ui->setupUi(this);
    setupWallMenu();
    setupFloorMenu();
    QPushButton * testbutton;
    testbutton = new QPushButton();
    testbutton->setText("+");
    scene = new DungeonMap;
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    MapSelectorEdit = NULL;
    MapSelectorTab = new QTabBar();
    MapSelectorTab->setMovable(true);
    MapSelectorTab->setExpanding(false);
    ui->horizontalLayout->addWidget(MapSelectorTab, Qt::AlignLeft);
    MapSelectorTab->addTab(scene->currentMap->name);

    MapSelectorTab->addTab(" + ");

    // Graphicsview performance optimization
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, false);
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setScene(scene);

    ui->floorButton->setMenu(floorMenu);
    ui->wallLeftButton->setMenu(wallMenu);
    ui->wallRightButton->setMenu(wallMenu);
    ui->wallUpButton->setMenu(wallMenu);
    ui->wallDownButton->setMenu(wallMenu);
    ui->mapbookLineEdit->setText(scene->mapBook.name);


    QObject::connect(MapSelectorTab,SIGNAL(tabBarDoubleClicked(int)),SLOT(editMapSelectorName(int)));
    QObject::connect(scene,SIGNAL(mouseMoved()),SLOT(updateGridLabel()));
    QObject::connect(scene,SIGNAL(clicked()),SLOT(updateSelectorLabel()));
    QObject::connect(scene,SIGNAL(clicked()),SLOT(updateTileProperties()));
    QObject::connect(scene,SIGNAL(clicked()),SLOT(addElement()));

    QObject::connect(ui->tileDeleteButton,SIGNAL(clicked()),SLOT(removeTile()));

    QObject::connect(ui->actionMap_Preferences,SIGNAL(triggered()),SLOT(show_mapPreferences()));
    QObject::connect(ui->actionLoad,SIGNAL(triggered()),SLOT(load_mapfiledialog()));
    QObject::connect(ui->actionSaveAs,SIGNAL(triggered()),SLOT(saveas_mapfiledialog()));
    //QObject::connect(ui->actionSave,SIGNAL(triggered()),SLOT(save_map(QString )));
    QObject::connect(ui->actionQuit,SIGNAL(triggered()),SLOT(close()));
    QObject::connect(ui->wallLeftButton,SIGNAL(triggered(QAction *)),SLOT(updateSelectedWall_left(QAction *)));
    QObject::connect(ui->wallRightButton,SIGNAL(triggered(QAction *)),SLOT(updateSelectedWall_right(QAction *)));
    QObject::connect(ui->wallUpButton,SIGNAL(triggered(QAction *)),SLOT(updateSelectedWall_up(QAction *)));
    QObject::connect(ui->wallDownButton,SIGNAL(triggered(QAction *)),SLOT(updateSelectedWall_down(QAction *)));
    QObject::connect(ui->floorButton,SIGNAL(triggered(QAction *)),SLOT(updateSelectedFloor(QAction *)));
    QObject::connect(ui->mapbookLineEdit,SIGNAL(textEdited(QString)),SLOT(updateMapBookName(QString)));
    QObject::connect(MapSelectorTab,SIGNAL(currentChanged(int)),SLOT(tabChanged(int )));
    QObject::connect(MapSelectorTab,SIGNAL(currentChanged(int)),SLOT(updateGridLabel()));
    QObject::connect(ui->actionText,SIGNAL(toggled(bool)),SLOT(actionTextChecked(bool)));
    QObject::connect(ui->actionTile,SIGNAL(toggled(bool)),SLOT(actionTileChecked(bool)));

}

void MainWindow::actionTextChecked(bool checked) {
    if (checked) ui->actionTile->setChecked(false);
}

void MainWindow::actionTileChecked(bool checked) {
    if (checked) ui->actionText->setChecked(false);
}

void MainWindow::addElement() {
    if (ui->actionTile->isChecked()) {
        scene->placeSelector();
        scene->addTile();
    }
    if (ui->actionText->isChecked()) {
        ui->actionText->setChecked(false);
        scene->addTextItem();
    }
}


void MainWindow::editMapSelectorName(int index) {
    MapSelectorEdit = new QLineEdit(this);
    MapSelectorTab->setTabButton(index, QTabBar::LeftSide, MapSelectorEdit);
    MapSelectorEdit->setFocus();
    MapSelectorEdit->setText(MapSelectorTab->tabText(index));
    MapSelectorTab->setTabText(index,"");
    QObject::connect(MapSelectorEdit,SIGNAL(editingFinished()),SLOT(updateMapSelectorName()));
}

void MainWindow::tabChanged(int index) {
    if (MapSelectorTab->count()-1 == index) {
        Map * newmap = scene->addMap();
        scene->empty_map();
        scene->currentMap = newmap;

        scene->draw_map();
        MapSelectorTab->insertTab(index,newmap->name);
        updateGridLabel();
    }
    else {
        foreach(Map * map, scene->mapBook.map) {
            if (map->name == MapSelectorTab->tabText(MapSelectorTab->currentIndex())) {
                    //QMessageBox::warning(this,"",map->name);
                    scene->currentMap = map;
                    updateGridLabel();
                    scene->empty_map();
                    scene->draw_map();

            }

        }

        // scene->currentMap->name = MapSelectorTab->tabText(MapSelectorTab->currentIndex());
        // updateGridLabel();
    }
    if (MapSelectorEdit != NULL) {
       MapSelectorEdit->resize(0,0);
        MapSelectorEdit->hide();
    }
}

void MainWindow::updateMapSelectorName() {
    MapSelectorTab->setTabText(MapSelectorTab->currentIndex(), MapSelectorEdit->text());
    MapSelectorEdit->resize(0,0);
    MapSelectorEdit->hide();
    scene->currentMap->name = MapSelectorTab->tabText(MapSelectorTab->currentIndex());
    updateGridLabel();

}

void MainWindow::updateMapBookName(QString name) {
    scene->mapBook.name = name;
}

void MainWindow::showWallMenu() {
    ui->wallLeftButton->showMenu();
}

void MainWindow::show_mapPreferences() {
    QDialog * pref;
    pref = new QDialog(0,0);
    Ui::map_preferences * Ui_pref;
    Ui_pref = new Ui::map_preferences();
    Ui_pref->setupUi(pref);
    pref->show();
}

void MainWindow::load_mapfiledialog() {
    QString map = QFileDialog::getOpenFileName(this, tr("Load File"),"",tr("Map files (*.map)"));

    scene->load_map(map);
    ui->mapbookLineEdit->setText(scene->mapBook.name);

    qDebug() << scene->mapBook.map.size();
    foreach(Map * map, scene->mapBook.map) {
        MapSelectorTab->insertTab(MapSelectorTab->currentIndex(),map->name);
    }
    MapSelectorTab->removeTab(MapSelectorTab->count()-2);
}

void MainWindow::saveas_mapfiledialog() {
    QString map = QFileDialog::getSaveFileName(this, tr("Save File"),"untitled.map",tr("Map files (*.map)"));
    scene->save_map(map);
}

void MainWindow::updateGridLabel() {
    QString pos = scene->currentMap->name + QString().sprintf(" x: %d y: %d", scene->xcoord,scene->ycoord);
    ui->gridLabel->setText(pos);
}

void MainWindow::updateSelectorLabel() {
    QString pos = QString().sprintf("Selected: x: %d y: %d",scene->selectedXcoord,scene->selectedYcoord);
    ui->selectorLabel->setText(pos);
}

void MainWindow::updateTileProperties() {
    Tile * tile = scene->getTile(scene->selectedXcoord,scene->selectedYcoord);
    if (tile != NULL) {
        ui->floorButton->setText(floorText(tile->floortype)+"   ");
        ui->wallDownButton->setText(wallText(tile->down)+"   ");
        ui->wallUpButton->setText(wallText(tile->up)+"   ");
        ui->wallLeftButton->setText(wallText(tile->left)+"   ");
        ui->wallRightButton->setText(wallText(tile->right)+"   ");
   }
   else {
        ui->floorButton->setText("     ");
        ui->wallDownButton->setText("     ");
        ui->wallUpButton->setText("     ");
        ui->wallLeftButton->setText("     ");
        ui->wallRightButton->setText("     ");
   }
}

void MainWindow::removeTile() {
    scene->removeTile();
    updateTileProperties();
    ui->graphicsView->setFocus();
}

void MainWindow::updateSelectedFloor(QAction * action) {
    QString floortype = action->iconText();
    if (scene->getTile(scene->selectedXcoord,scene->selectedYcoord) != NULL) scene->updateSelectedFloor(floorIndex(floortype));
    ui->floorButton->setText(floortype+"   ");
}

void MainWindow::updateSelectedWall_left(QAction * action) {
    QString walltype = action->iconText();
    if (scene->getTile(scene->selectedXcoord,scene->selectedYcoord) != NULL) scene->updateSelectedWall_left(wallIndex(walltype));
    ui->wallLeftButton->setText(walltype+"   ");
}
void MainWindow::updateSelectedWall_right(QAction * action) {
    QString walltype = action->iconText();
    if (scene->getTile(scene->selectedXcoord,scene->selectedYcoord) != NULL) scene->updateSelectedWall_right(wallIndex(walltype));
    ui->wallRightButton->setText(walltype+"   ");
}
void MainWindow::updateSelectedWall_up(QAction * action) {
    QString walltype = action->iconText();
    if (scene->getTile(scene->selectedXcoord,scene->selectedYcoord) != NULL) scene->updateSelectedWall_up(wallIndex(walltype));
    ui->wallUpButton->setText(walltype+"   ");
}
void MainWindow::updateSelectedWall_down(QAction * action) {
    QString walltype = action->iconText();
    if (scene->getTile(scene->selectedXcoord,scene->selectedYcoord) != NULL) scene->updateSelectedWall_down(wallIndex(walltype));
    ui->wallDownButton->setText(walltype+"   ");
}

MainWindow::~MainWindow() {
    delete ui;
}

QString MainWindow::wallText(int walltype) {
    switch(walltype) {
    case 0: return "None";
    case 1: return "Wall";
    case 2: return "Button";
    case 3: return "Hole";
    case 4: return "Keyhole";
    case 5: return "Door";
    }
    return "";
}

QString MainWindow::floorText(int floortype) {
    switch(floortype) {
    case 0: return "None";
    case 1: return "Floor";
    case 2: return "Button";
    case 3: return "Hole";
    case 4: return "Trap";
    case 5: return "Horizontal Door";
    case 6: return "Vertical Door";
    }
    return "";
}

short int MainWindow::wallIndex(QString walltype) {
    if (walltype == "None") return 0;
    else if (walltype == "Wall") return 1;
    else if (walltype == "Button") return 2;
    else if (walltype == "Hole") return 3;
    else if (walltype == "Keyhole") return 4;
    else if (walltype == "Door") return 5;
    else return -1;
}

short int MainWindow::floorIndex(QString floortype) {
    if (floortype == "None") return 0;
    else if (floortype == "Floor") return 1;
    else if (floortype == "Button") return 2;
    else if (floortype == "Hole") return 3;
    else if (floortype == "Trap") return 4;
    else if (floortype == "Horizontal Door") return 5;
    else if (floortype == "Vertical Door") return 6;
    else return -1;
}

void MainWindow::setupWallMenu() {
    wallMenu = new QMenu();
    no_wall = new QAction("None", this);
    wallMenu->addAction(no_wall);
    wall = new QAction("Wall", this);
    wallMenu->addAction(wall);
    wall_button = new QAction("Button", this);
    wallMenu->addAction(wall_button);
    wall_hole = new QAction("Hole", this);
    wallMenu->addAction(wall_hole);
    wall_keyhole = new QAction("Keyhole", this);
    wallMenu->addAction(wall_keyhole);
    wall_keyhole = new QAction("Door", this);
    wallMenu->addAction(wall_keyhole);
}

void MainWindow::setupFloorMenu() {
    floorMenu = new QMenu();
    no_floor = new QAction("None", this);
    floorMenu->addAction(no_floor);
    floor = new QAction("Floor", this);
    floorMenu->addAction(floor);
    floor_button = new QAction("Button", this);
    floorMenu->addAction(floor_button);
    floor_hole = new QAction("Hole", this);
    floorMenu->addAction(floor_hole);
    Floor_trap = new QAction("Trap", this);
    floorMenu->addAction(Floor_trap);
    floor_doorhor = new QAction("Horizontal Door", this);
    floorMenu->addAction(floor_doorhor);
    floor_doorver = new QAction("Vertical Door", this);
    floorMenu->addAction(floor_doorver);
}
