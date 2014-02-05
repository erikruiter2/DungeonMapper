#include "dungeonmap.h"
#include <QDebug>
#include <QPointF>
#include <QMouseEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsLineItem>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QGraphicsTextItem>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QString>
#include <QPainter>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QXmlStreamWriter>
#include <QDir>
#include <QtWidgets/QMessageBox>

DungeonMap::DungeonMap()  : QGraphicsScene() {

    selector = NULL;
    maxMapId = 0;
    maxTextId = 0;
    // Add the vertical lines first
    for (int x=-2100; x<=2100; x+=30)
        addLine(x,-2100,x,2100, QPen(QColor(200, 200, 255, 125)));

    // Now add the horizontal lines
    for (int y=-2100; y<=2100; y+=30)
        addLine(-2100,y,2100,y, QPen(QColor(200, 200, 255, 125)));

    // pre load png's
    floorpng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/floor.png");
    wallpng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/wall.png");
    wall_holepng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/wall_hole.png");
    wall_buttonpng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/wall_button.png");
    wall_keyholepng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/wall_keyhole.png");
    wall_doorpng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/wall_door.png");
    floor_buttonpng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/floor_button.png");
    floor_holepng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/floor_hole.png");
    floor_trappng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/floor_trap.png");
    floor_doorhorpng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/floor_doorhor.png");
    floor_doorverpng.load("//Users/erikruiter/Google Drive/Erik/projects/DungeonMapper/images/floor_doorver.png");


    // Initialize empty map
    mapBook.name="untitled Mapbook";
    addMap();
}

void DungeonMap::updateTextString(DungeonTextItem * t) {
    Text * text = getText(t->data(1).toInt());
    text->xpos = t->x();
    text->ypos = t->y();
    text->textString = t->toPlainText();
}

DungeonMap::~DungeonMap(){}

bool DungeonMap::addTile() {
    if (getTile(xcoord,ycoord) != NULL) { // tile already exits; end function
       return 0;
    }

    Tile * tile = new Tile;
    tile->xcoord = xcoord;
    tile->ycoord = ycoord;

    Tile * tile_left = getTile(xcoord-1,ycoord);
    if (tile_left != NULL) { tile->left=NO_WALL; tile_left->right = NO_WALL;    }
    Tile * tile_right = getTile(xcoord+1,ycoord);
    if (tile_right != NULL) { tile->right=NO_WALL; tile_right->left = NO_WALL;  }
    Tile * tile_up = getTile(xcoord,ycoord+1);
    if (tile_up != NULL) { tile->up=NO_WALL; tile_up->down = NO_WALL;    }
    Tile * tile_down= getTile(xcoord,ycoord-1);
    if (tile_down != NULL) { tile->down=NO_WALL; tile_down->up = NO_WALL;   }

    currentMap->tiles.append(tile);
    drawTile(tile);
    removeInvalidWalls(tile);

    return true;
}

void DungeonMap::addTextItem() {

    Text * textItem = new Text();
    textItem->xpos = xpos;
    textItem->ypos = ypos;
    maxTextId ++;
    textItem->id = maxTextId;
    currentMap->text.append(textItem);
    drawTextItem(textItem,true);
    qDebug() << "added textItem";
}

void DungeonMap::drawTextItem(Text * t, bool focus=true) {
    DungeonTextItem * textItem = new DungeonTextItem();

    this->addItem(textItem);
    QObject::connect(textItem,SIGNAL(lostFocus(DungeonTextItem * )),SLOT(updateTextString(DungeonTextItem *)));

    textItem->setPos(t->xpos,t->ypos);
    textItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable);
    textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    textItem->setPlainText(t->textString);
    textItem->setData(0,"textitem");
    textItem->setData(1,t->id);
    textItem->show();
    if (focus) textItem->setFocus();
}

Tile * DungeonMap::getTile(int xcoord, int ycoord) {
    for (int i = 0; i < currentMap->tiles.size(); ++i) {
        if (currentMap->tiles.at(i)->xcoord == xcoord & currentMap->tiles.at(i)->ycoord == ycoord) {
            return currentMap->tiles.at(i);
        }
     }
    return NULL;
}

Text * DungeonMap::getText(int id) {
    foreach(Text * t, currentMap->text) {
        if (t->id == id) return t;
    }
    return NULL;
}


bool DungeonMap::updateSelectedFloor(int floortype) {
    Tile * t=NULL;
    t = getTile(selectedXcoord,selectedYcoord);
    if (t == NULL) return 0; else updateFloor(t, floortype);
    return 1;
}


bool DungeonMap::updateFloor(Tile *t, int floortype ) {
    qreal xpos,ypos;
    ypos=-(t->ycoord*30);
    xpos=t->xcoord*30;

    t->floortype= floortype;
    QGraphicsPixmapItem * newFloor = NULL;
    QGraphicsItem * floor = NULL;

    QPixmap pixmap;
    //if (walltype == 0) { pixmap = no_wallpng; }
    if (floortype == 1) { pixmap = floorpng;  }
    if (floortype == 2) { pixmap = floor_buttonpng;   }
    if (floortype == 3) { pixmap = floor_holepng;    }
    if (floortype == 4) { pixmap = floor_trappng;    }
    if (floortype == 5) { pixmap = floor_doorhorpng;    }
    if (floortype == 6) { pixmap = floor_doorverpng;    }


    floor = getItem(t->xcoord,t->ycoord,"floor");
    if (floor != NULL) { this->removeItem(floor); floor=NULL; }
    if (floor == NULL) {
        newFloor= this->addPixmap(pixmap); newFloor->setPos(xpos,ypos);
        newFloor->setData(0, t->xcoord); newFloor->setData(1, t->ycoord); newFloor->setData(2, "floor");
        newFloor->setZValue(-1);
        qDebug() << "Floor texture added, type: " << QString::number(floortype) << " x: " << QString::number(t->xcoord) <<  "y: " << QString::number(t->ycoord);
    }
    return 1;
}

bool DungeonMap::updateWall(Tile * t, QString wall, int walltype) {
    qreal xpos,ypos;
    ypos=-(t->ycoord*30);
    xpos=t->xcoord*30;

    QPixmap pixmap;
    //if (walltype == 0) { pixmap = no_wallpng; }
    if (walltype == 1) { pixmap = wallpng;  }
    if (walltype == 2) { pixmap = wall_buttonpng;   }
    if (walltype == 3) { pixmap = wall_holepng;    }
    if (walltype == 4) { pixmap = wall_keyholepng;    }
    if (walltype == 5) { pixmap = wall_doorpng;    }

    QGraphicsPixmapItem * newWall;
    if (wall == "left" ) t->left  = walltype;
    if (wall == "right") t->right = walltype;
    if (wall == "up"   ) t->up    = walltype;
    if (wall == "down" ) t->down  = walltype;

    QGraphicsItem * wallItem = NULL;
    wallItem = getItem(t->xcoord, t->ycoord, "wall_"+wall);
    if (wallItem != NULL) { this->removeItem(wallItem); wallItem=NULL; }
    if (wallItem == NULL) {
     if ((wall == "left") & (walltype != 0)) { newWall = this->addPixmap(pixmap); newWall->setPos(xpos-3,ypos);  }
     if ((wall == "right") & (walltype != 0)) { newWall = this->addPixmap(pixmap); newWall->setPos(xpos+27,ypos); }
     if ((wall == "up") & (walltype != 0)) { newWall = this->addPixmap(pixmap); newWall->setRotation(90); newWall->setPos(xpos+30,ypos-3); }
     if ((wall == "down") & (walltype != 0)) { newWall = this->addPixmap(pixmap); newWall->setRotation(90); newWall->setPos(xpos+30,ypos+27); }
     if (walltype != 0) { newWall->setData(0, t->xcoord); newWall->setData(1, t->ycoord); newWall->setData(2, "wall_"+wall); }

    }
    return 1;

}

void DungeonMap::removeInvalidWalls(Tile * t) {

    QGraphicsItem * temp;
    QGraphicsItem * current;

    current = getItem(t->xcoord,t->ycoord,"wall_left");
    temp = getItem(t->xcoord-1,t->ycoord,"wall_right");
    if (temp !=NULL)  this->removeItem(temp);

    current = getItem(t->xcoord,t->ycoord,"wall_right");
    temp = getItem(t->xcoord+1,t->ycoord,"wall_left");
    if (temp !=NULL)  this->removeItem(temp);

    current = getItem(t->xcoord,t->ycoord,"wall_up");
    temp = getItem(t->xcoord,t->ycoord+1,"wall_down");
    if (temp !=NULL)  this->removeItem(temp);

    current = getItem(t->xcoord,t->ycoord,"wall_down");
    temp = getItem(t->xcoord,t->ycoord-1,"wall_up");
    if (temp !=NULL)  this->removeItem(temp);

    this->update();
}

bool DungeonMap::updateSelectedWall_left(int walltype) {
    Tile * t=NULL;
    t = getTile(selectedXcoord,selectedYcoord);
    if (t == NULL) return 0; else updateWall(t, "left", walltype);
    return 1;
}
bool DungeonMap::updateSelectedWall_right(int walltype) {
    Tile * t=NULL;
    t = getTile(selectedXcoord,selectedYcoord);
    if (t == NULL) return 0; else updateWall(t, "right", walltype);
    return 1;
}
bool DungeonMap::updateSelectedWall_up(int walltype) {
    Tile * t=NULL;
    t = getTile(selectedXcoord,selectedYcoord);
    if (t == NULL) return 0; else updateWall(t, "up", walltype);
    return 1;
}
bool DungeonMap::updateSelectedWall_down(int walltype) {
    Tile * t=NULL;
    t = getTile(selectedXcoord,selectedYcoord);
    if (t == NULL) return 0; else updateWall(t, "down", walltype);
    return 1;
}

bool DungeonMap::removeTile() {
    QGraphicsItem * g;
    g = getItem(selectedXcoord,selectedYcoord,"floor");
    if (g != NULL) delete g;
    g = getItem(selectedXcoord,selectedYcoord,"wall_left");
    if (g != NULL) delete g;
    g = getItem(selectedXcoord,selectedYcoord,"wall_right");
    if (g != NULL) delete g;
    g = getItem(selectedXcoord,selectedYcoord,"wall_up");
    if (g != NULL) delete g;
    g = getItem(selectedXcoord,selectedYcoord,"wall_down");
    if (g != NULL) delete g;

    foreach( Tile * tile, currentMap->tiles ) {
        if (tile->xcoord == selectedXcoord & tile->ycoord == selectedYcoord) {
            currentMap->tiles.removeOne(tile);
        }
    }

    Tile * tile_left = getTile(selectedXcoord-1,selectedYcoord);
    if (tile_left != NULL) tile_left->right = WALL;

    Tile * tile_right = getTile(selectedXcoord+1,selectedYcoord);
    if (tile_right != NULL) tile_right->left = WALL;

    Tile * tile_up = getTile(selectedXcoord,selectedYcoord+1);
    if (tile_up != NULL) tile_up->down = WALL;

    Tile * tile_down= getTile(selectedXcoord,selectedYcoord-1);
    if (tile_down != NULL) tile_down->up = WALL;

    empty_map();
    draw_map();
    return 1;
}

void DungeonMap::drawTile(Tile * current) {

    updateFloor(current, current->floortype);
    if (current->left !=NO_WALL) updateWall(current, "left", current->left);
    if (current->right !=NO_WALL) updateWall(current, "right", current->right);
    if (current->up !=NO_WALL) updateWall(current, "up", current->up);
    if (current->down !=NO_WALL) updateWall(current, "down", current->down);

}

void DungeonMap::draw_map() {
    foreach (Tile *tile, currentMap->tiles) drawTile(tile);
    foreach (Text *textItem, currentMap->text) drawTextItem(textItem,false);


}

void DungeonMap::empty_map() {
    foreach (QGraphicsItem * g, this->items()) {
        if ( (g->data(2) == "floor") |
             (g->data(2) == "wall_left") |
             (g->data(2) == "wall_right") |
             (g->data(2) == "wall_up") |
             (g->data(2) == "wall_down") |
             (g->data(0) == "textitem"))
             { delete g;}
    }
}

void DungeonMap::placeSelector() {
    qreal xpos,ypos;
    ypos=-ycoord;
    xpos=xcoord*30;
    ypos=ypos*30;
    selectedXcoord = xcoord;
    selectedYcoord = ycoord;
    if (selector == NULL) {
        selector = this->addRect(xpos+3,ypos+3,24,24,QPen(Qt::blue));
    }
    selector->setRect(xpos+3,ypos+3,24,24);
}

void DungeonMap::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
  presspos = mouseEvent->scenePos();
  QGraphicsScene::mousePressEvent(mouseEvent);
  emit clicked();
}

void DungeonMap::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
  int prevxcoord=xcoord;
  int prevycoord=ycoord;
  movepos = mouseEvent->scenePos();
  QGraphicsScene::mouseMoveEvent(mouseEvent);
  xpos = movepos.x();
  ypos = movepos.y();

  xcoord=int(xpos/30);
  ycoord=int(ypos/30);
  if (int(xpos) < 0 ) xcoord--;
  if (int(ypos) < 0 ) ycoord--;
  ycoord=-ycoord;

  emit mouseMoved();
  if ((prevxcoord != xcoord) | (prevycoord != ycoord)) emit coordChanged();
}

void DungeonMap::save_map(QString map) {
    writeXML();
}

bool DungeonMap::load_map(QString mapBook) {
 /*   for (int i=0; i < this->mapBook.map.size(); ++i) {
        this->currentMap = this->mapBook.map.at(i);
        empty_map();
        this->mapBook.map.removeAt(i);

    }*/
    while (this->mapBook.map.size() > 0) {
        this->mapBook.map.removeAt(0);
    }
    QFile file(mapBook);

    Tile * t;
    Text * textItem;
    Map * tempmap;
    QByteArray tiledata;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;
    QByteArray ba;

    QStringList line;
    QXmlStreamReader xml(&file);
    QXmlStreamAttributes attributes;


    while(!xml.atEnd()) {
        qDebug() << xml.name();
        xml.readNext();
     //   if(token == QXmlStreamReader::StartElement) {

            if((xml.name() == "mapbook") && (xml.tokenString() == "StartElement")) {
                attributes = xml.attributes();
                this->mapBook.name = attributes.value("name").toString();

            }

            if((xml.name() == "map") && (xml.tokenString() == "StartElement")) {

                attributes = xml.attributes();
                tempmap = new Map;
                tempmap->id = attributes.value("id").toString().toShort();
                tempmap->name = attributes.value("name").toString();
                qDebug() << " map found" << tempmap->name;
                this->mapBook.map.append(tempmap);
                this->currentMap=tempmap;
                qDebug() << "mapbook size: " << this->mapBook.map.size() << this->currentMap->name;
            }

            if((xml.name() == "tiles") && (xml.tokenString() == "StartElement") ) {
                tiledata = QByteArray::fromBase64(xml.readElementText().toLatin1());
                QString data = tiledata.data();
                QStringList linedata = data.split("\n");
                for (int i=0; i < linedata.size()-1; ++i) {
                    line = linedata.at(i).split(";");

                    t = new Tile;
                    t->xcoord = line[0].toInt();
                    t->ycoord = line[1].toInt();
                    t->floortype = line[2].toInt();
                    t->left = line[3].toInt();
                    t->right = line[4].toInt();
                    t->up = line[5].toInt();
                    t->down = line[6].toInt();
                    debugTile(t);
                    this->currentMap->tiles.append(t);
                }
            }
            if((xml.name() == "textItem") && (xml.tokenString() == "StartElement") ) {
                attributes = xml.attributes();
                textItem = new Text;
                textItem->id = attributes.value("id").toString().toInt();
                textItem->xpos = attributes.value("xpos").toString().toFloat();
                textItem->ypos = attributes.value("ypos").toString().toFloat();



            }
    }

    this->currentMap = this->mapBook.map.at(0);
    draw_map();

   return 1;
}

void DungeonMap::debugTile() {
    Tile * t;
    for (int i = 0; i < tiles.size(); ++i) {
        t = tiles.at(i);
        qDebug() << t->xcoord << ";" << t->ycoord << ";" << t->floortype << ";" << t->left << ";" << t->right << ";" << t->up << ";" << t->down << endl;
    }
}

void DungeonMap::debugTile(Tile * t) {
    qDebug() << t->xcoord << ";" << t->ycoord << ";" << t->floortype << ";" << t->left << ";" << t->right << ";" << t->up << ";" << t->down << endl;
}

void DungeonMap::listItems() {
    QList<QGraphicsItem *>	itemlist;
    QGraphicsItem * g;
    for (int i = 0; i < this->items().size(); ++i) {
        g = this->items().at(i);
        qDebug() << g->data(0) << ";" << g->data(1) << ";" << g->data(2);
    }
}
QGraphicsItem * DungeonMap::getItem(int xcoord, int ycoord, QString itemtype) {
/* Returns a QGraphicsitem from the scene, matching the QGraphicsitem data elements xcoord,yxoord and itemtype */

    QGraphicsItem * g;
    for (int i = 0; i < this->items().size(); ++i) {
        g = this->items().at(i);
        if ((g->data(0) == xcoord) & (g->data(1) == ycoord) & (g->data(2) == itemtype) ) {
            return g;
        }
     }
    return NULL;
}

void DungeonMap::writeXML() {
    QFile file("//Users/erikruiter/mapbook.map");
    Tile * t;
    QString tiledata;
    if ( file.open(QIODevice::WriteOnly) )
    {
    QByteArray ba;
    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("mapbook");
    stream.writeAttribute("name", mapBook.name);

    for (int mi = 0; mi < mapBook.map.size(); ++mi) {
        stream.writeStartElement("map");
        stream.writeAttribute("id", QString::number(mapBook.map.at(mi)->id));
        stream.writeAttribute("name", mapBook.map.at(mi)->name);

        for (int i = 0; i < mapBook.map.at(mi)->tiles.size(); ++i) {
            t = mapBook.map.at(mi)->tiles.at(i);
            tiledata.append( QString::number(t->xcoord) + ";" + QString::number(t->ycoord) + ";" + QString::number(t->floortype) + ";");
            tiledata.append( QString::number(t->left) + ";" + QString::number(t->right) + ";");
            tiledata.append( QString::number(t->up) + ";" + QString::number(t->down) + "\n");
        }
        ba.append(tiledata);
        stream.writeTextElement("tiles", ba.toBase64());
        foreach (Text * t, mapBook.map.at(mi)->text) {
            stream.writeStartElement("textitem");
            stream.writeAttribute("id",QString::number(t->id));
            stream.writeAttribute("xpos",QString::number(t->xpos));
            stream.writeAttribute("xpos",QString::number(t->ypos));
            stream.writeTextElement("textString",t->textString);
            stream.writeEndElement(); // textitem
        }
        stream.writeEndElement(); // map

        tiledata.clear();
        ba.clear();
    }

    stream.writeEndElement(); // mapbook
    stream.writeEndDocument();
    }
}

Map * DungeonMap::addMap() {

    Map * tempmap = new Map;

    tempmap->id=maxMapId;
    maxMapId++;
    tempmap->name="untitled_"+QString::number(tempmap->id);
    currentMap = tempmap;
    mapBook.map.append(tempmap);
    qDebug() << "map " << currentMap->name << " added";
    return currentMap;
}
