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
#include <QTimer>
#include <QGraphicsScene>

#include "bcitem.h"
#include "bcglobal.h"
#include "bcboard.h"

BCItem::BCItem(BCBoard *parent) :
    QDeclarativeItem(parent),
    m_board(parent)
{
    setFlag(ItemHasNoContents, false);
    setClip(true);
    setZValue(0);
}

void BCItem::setPosition(int row, int column)
{
    m_position.setX(column);
    m_position.setY(row);
    reposItem();
}

void BCItem::reposItem()
{
    setPos(column() * implicitWidth(), row() * implicitHeight());
}

#ifdef BC_DEBUG_RECT
static QColor rectColor(BattleCity::ObstacleType type)
{
    QColor color;
    switch (type) {
    case BattleCity::Ice: color = Qt::white;
        break;
    case BattleCity::ConcreteWall: color = Qt::lightGray;
        break;
    case BattleCity::BricksWall: color = Qt::darkRed;
        break;
    case BattleCity::Water: color = Qt::blue;
        break;
    case BattleCity::Camouflage: color = Qt::green;
        break;
    default: color = Qt::black;
        break;
    }
//    color = QColor(qrand() % 255, qrand() % 255, qrand() % 255);
    return color;
}
#endif

void BCItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    const BattleCity::ObstacleType type = BattleCity::ObstacleType(this->type());
    if (type < BattleCity::Ground || type > BattleCity::Water)
        return;

#ifdef BC_DEBUG_RECT
    painter->setOpacity(0.5);
    painter->setPen(rectColor(type));
    painter->drawRect(option->rect);
#else
    painter->drawPixmap(option->rect, BattleCity::obstacleTexture(type));
    if (board() && board()->gridVisible()) {
        painter->setPen(Qt::lightGray);
        painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
    }
#endif
}

bool BCMovableItem::move(BattleCity::MoveDirection direction)
{
    if (!board())
        return false;

    if (m_direction != direction)
        m_direction = direction;

    qreal speed = this->speed();
    qreal x = this->x();
    qreal y = this->y();
    QRectF viewRect;
    static const qreal extraPixel = 1.0;

    if (direction == BattleCity::Left) {
        x -= speed;
        viewRect.setRect(this->x() - speed, this->y(), speed, implicitHeight() + extraPixel);
    }
    if (direction == BattleCity::Right) {
        x += speed;
        viewRect.setRect(this->x() + implicitWidth() + speed + extraPixel, this->y(), speed, implicitHeight() + extraPixel);
    }
    if (direction == BattleCity::Forward) {
        y -= speed;
        viewRect.setRect(this->x(), this->y() - speed, implicitWidth() + extraPixel, speed);
    }
    if (direction == BattleCity::Backward) {
        y += speed;
        viewRect.setRect(this->x(), this->y() + implicitHeight() + speed + extraPixel, implicitWidth() + extraPixel, speed);
    }

#ifdef BC_DEBUG_RECT
    board()->setDebugRect(viewRect);
#endif

    const BattleCity::Edge edge = intersectsBoardBoundingRect(x, y, direction);
    bool res = edge == BattleCity::NoneEdge ? true : false;
    if (edge == BattleCity::NoneEdge) {
        BattleCity::Edge obstacleEdge = BattleCity::NoneEdge;
        const BCItem *obstacle = collidesWithObstacle(viewRect, direction, &obstacleEdge);
        res = obstacleEdge == BattleCity::NoneEdge ? true : false;
        adjustIntersectionPointWithObstacle(obstacle, obstacleEdge, x, y);
    } else {
        adjustIntersectionPointWithBoardBoundingRect(edge, x, y);
    }

    setPos(x, y);
    update();
    return res;
}

BattleCity::Edge BCMovableItem::intersectsBoardBoundingRect(qreal x, qreal y, BattleCity::MoveDirection direction) const
{
    BattleCity::Edge edge = BattleCity::NoneEdge;
    switch (direction) {
    case BattleCity::Left:
        if (x < 0)
            edge = BattleCity::LeftEdge;
        break;
    case BattleCity::Right:
        if ((x + implicitWidth()) >= board()->implicitWidth())
            edge = BattleCity::RightEdge;
        break;
    case BattleCity::Forward:
        if (y < 0)
            edge = BattleCity::TopEdge;
        break;
    case  BattleCity::Backward:
        if ((y + implicitHeight()) >= board()->implicitHeight())
            edge = BattleCity::BottomEdge;
        break;
    }
    return edge;
}

BCItem *BCMovableItem::collidesWithObstacle(const QRectF &viewRect, BattleCity::MoveDirection direction, BattleCity::Edge *edge) const
{
    const QList<QGraphicsItem *> items = scene()->items(viewRect);
    foreach (QGraphicsItem *item, items) {
        BCItem *obstacle = qobject_cast<BCItem *>(item);
        if (item == this || !obstacle || obstacle->itemProperty() == BattleCity::Traversable)
            continue;
        const QRectF obstacleRect(obstacle->x(), obstacle->y(), obstacle->implicitWidth(), obstacle->implicitHeight());
        if (viewRect.intersects(obstacleRect)) {
            if (edge) {
                switch (direction) {
                case BattleCity::Left:
                    (*edge) = BattleCity::RightEdge;
                    break;
                case BattleCity::Right:
                    (*edge) = BattleCity::LeftEdge;
                    break;
                case BattleCity::Forward:
                    (*edge) = BattleCity::BottomEdge;
                    break;
                case BattleCity::Backward:
                    (*edge) = BattleCity::TopEdge;
                    break;
                }
            }
            return obstacle;
        }
    }
    if (edge)
        (*edge) = BattleCity::NoneEdge;
    return 0;
}

void BCMovableItem::adjustIntersectionPointWithBoardBoundingRect(BattleCity::Edge edge, qreal &x, qreal &y) const
{
    Q_UNUSED(edge);
    Q_UNUSED(x);
    Q_UNUSED(y);
}

void BCMovableItem::adjustIntersectionPointWithObstacle(const BCItem *obstacle, BattleCity::Edge edge, qreal &x, qreal &y) const
{
    Q_UNUSED(obstacle);
    Q_UNUSED(edge);
    Q_UNUSED(x);
    Q_UNUSED(y);
}

BCProjectile::BCProjectile(qreal speed, BattleCity::MoveDirection direction, BCBoard *parent) :
    BCMovableItem(direction, parent),
    m_speed(speed)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), SLOT(timerFired()));
}

void BCProjectile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    painter->drawPixmap(option->rect, BattleCity::projectileTexture(direction()));
}

void BCProjectile::launch()
{
    m_timer->start();
}

void BCProjectile::timerFired()
{
    if (move(direction())) {
        m_timer->start();
    } else {
        emit exploded();
    }
}
