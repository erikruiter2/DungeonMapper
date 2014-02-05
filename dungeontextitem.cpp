#include <QtWidgets/QGraphicsTextItem>
#include "dungeontextitem.h"

void DungeonTextItem::focusOutEvent(QFocusEvent *e)
{
if (e->lostFocus() )
emit(lostFocus(this) );

QGraphicsTextItem::focusOutEvent(e);

}
