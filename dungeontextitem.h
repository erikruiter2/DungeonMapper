#ifndef DUNGEONTEXTITEM_H
#define DUNGEONTEXTITEM_H

#include <QtWidgets/QGraphicsTextItem>
#include <QFocusEvent>

class DungeonTextItem: public QGraphicsTextItem
{
Q_OBJECT

void focusOutEvent(QFocusEvent *e);

signals:
void lostFocus(DungeonTextItem * t);

};


#endif // DUNGEONTEXTITEM_H
