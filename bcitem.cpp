/****************************************************************************
**
** Copyright (C) 2011 Kirill (spirit) Klochkov.
** Contact: klochkov.kirill@gmail.com
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPixmapCache>

#include "bcitem.h"
#include "bcglobal.h"
#include "bcboard.h"

BCItem::BCItem(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(ItemHasNoContents, false);
    setClip(true);
}

void BCItem::setPosition(int row, int column)
{
    m_position.setX(row);
    m_position.setY(column);
    reposItem();
}

void BCItem::reposItem()
{
    setPos(row() * implicitWidth(), column() * implicitHeight());
}

BCObstacle::BCObstacle(BCBoard *parent) :
    BCItem(parent),
    m_board(parent)
{
    setZValue(0);
}

void BCObstacle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

#ifdef BC_DEBUG_RECT
    painter->setOpacity(0.5);
#endif

    painter->drawPixmap(option->rect, BattleCity::obstacleTexture(BattleCity::ObstacleType(type())));
    if (m_board && m_board->gridVisible()) {
        painter->setPen(Qt::lightGray);
        painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
    }
}
