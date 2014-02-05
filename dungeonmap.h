#ifndef DUNGEONMAP_H
#define DUNGEONMAP_H

#include <QtWidgets/QGraphicsScene>
#include <QPoint>
#include <QObject>
#include <QList>
#include <QtWidgets/QGraphicsPixmapItem>
#include "dungeontextitem.h"


typedef enum Walltype {NO_WALL,WALL,WALL_BUTTON,WALL_HOLE,WALL_KEYHOLE} Walltype;

struct Tile {
    Tile() : xcoord(0),ycoord(0), up(WALL), down(WALL), left(WALL), right(WALL), floortype(1) {}
    short int xcoord;
    short int ycoord;
    short int up;
    short int down;
    short int left;
    short int right;
    short int floortype;
};

struct Text {
    int id;
    float xpos;
    float ypos;
    QString textString;
};

struct Map {
    short int id;
    QString name;
    QList<Tile *> tiles;

    QList<Text *> text;
};

struct MapBook {
    QString name;
    QList<Map *> map;
};


class DungeonMap : public QGraphicsScene
{
Q_OBJECT

public:
  DungeonMap();
  ~DungeonMap();
  QPointF movepos;
  QPointF presspos;
  int selectedXcoord;
  int selectedYcoord;
  short int maxMapId;
  short int maxTextId;
  MapBook mapBook;
  Map * currentMap;
  QList<Tile *> tiles;

  Tile * getTile(int xcoord, int ycoord);
  Text * getText(int id);
  void deleteWall(QGraphicsLineItem * wall);
  void drawTile (Tile * tile);
  void drawTextItem(Text * textItem, bool focus);
  void updateTile();
  QGraphicsRectItem * selector;
  int xcoord;
  int ycoord;
  float xpos;
  float ypos;
  void save_map(QString map);
  void writeXML();
  bool load_map(QString map);
  void draw_map();
  void empty_map();
  Map * addMap();

  void debugTile();
  void debugTile(Tile * t);
  bool updateSelectedFloor(int index);
  bool updateFloor(Tile * t, int index=-2);
  bool updateSelectedWall_left(int index);
  bool updateSelectedWall_right(int index);
  bool updateSelectedWall_up(int index);
  bool updateSelectedWall_down(int index);
  bool updateWall(Tile * t, QString wall, int walltype =-2);
  void listItems();
  QGraphicsItem * getItem(int xcoord, int ycoord, QString itemtype);
  void removeInvalidWalls(Tile * t);
  QPixmap floorpng;
  QPixmap floor_buttonpng;
  QPixmap floor_holepng;
  QPixmap floor_trappng;
  QPixmap floor_doorhorpng;
  QPixmap floor_doorverpng;
  QPixmap wallpng;
  QPixmap wall_holepng;
  QPixmap wall_buttonpng;
  QPixmap wall_keyholepng;
  QPixmap wall_doorpng;


protected:
void mousePressEvent(QGraphicsSceneMouseEvent *event);
void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

public slots:
 bool addTile();
 void addTextItem();
 bool removeTile();
 void placeSelector();
 void updateTextString(DungeonTextItem * t);

signals:
  void clicked();
  void mouseMoved();
  void coordChanged();

private:

};

#endif // DUNGEONMAP_H
