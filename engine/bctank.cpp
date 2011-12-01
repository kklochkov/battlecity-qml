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

#include <QPixmapCache>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTimer>
#include <QGraphicsScene>

#include "bctank.h"
#include "bcboard.h"

BCAbstractTank::BCAbstractTank(BCBoard *board) :
    BCMovableItem(board),
    m_currentAnimationStep(0),
    m_destroyed(false)
{

}

bool BCAbstractTank::move(BattleCity::MoveDirection direction)
{
    if (!BCMovableItem::move(direction))
        return false;

    if (!board())
        return false;

    ++m_currentAnimationStep;
    if (m_currentAnimationStep == BattleCity::tankAnimationSteps)
        m_currentAnimationStep = 0;

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

    const QList<QGraphicsItem *> items = scene()->items(viewRect);
    foreach (QGraphicsItem *item, items) {
        BCItem *obstacle = qobject_cast<BCItem *>(item);
        if (item == this || !obstacle || obstacle->itemProperty() == BattleCity::Traversable)
            continue;
        const QRectF obstacleRect(obstacle->x(), obstacle->y(), obstacle->implicitWidth(), obstacle->implicitHeight());
        if (viewRect.intersects(obstacleRect)) {
            if (direction == BattleCity::Left)
                x = obstacle->x() + obstacle->implicitWidth();
            if (direction == BattleCity::Right)
                x = obstacle->x() - implicitWidth();
            if (direction == BattleCity::Forward)
                y = obstacle->y() + obstacle->implicitHeight();
            if (direction == BattleCity::Backward)
                y = obstacle->y() - implicitHeight();
            break;
        }
    }

    if (x < 0 && direction == BattleCity::Left)
        x = 0;
    if (((x + implicitWidth()) >= board()->implicitWidth()) && direction == BattleCity::Right)
        x = board()->implicitWidth() - implicitWidth() - extraPixel;
    if (y < 0 && direction == BattleCity::Forward)
        y = 0;
    if (((y + implicitHeight()) >= board()->implicitHeight()) && direction == BattleCity::Backward)
        y = board()->implicitHeight() - implicitHeight() - extraPixel;

    setPos(x, y);
    update();
    return true;
}

BCEnemyTank::BCEnemyTank(BCBoard *board) :
    BCAbstractTank(board),
    m_bonus(false),
    m_bonusTexture(false),
    m_timer(new QTimer(this))
{
    m_timer->setInterval(250);
    connect(m_timer, SIGNAL(timeout()), SLOT(timerFired()));
}

void BCEnemyTank::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
#ifdef BC_DEBUG_RECT
    painter->setPen(Qt::white);
    painter->drawRect(option->rect);
#else
    painter->drawPixmap(option->rect, BattleCity::tankTexture(BattleCity::TankType(type()), direction(), currentAnimationStep(), m_bonusTexture));
#endif
}

void BCEnemyTank::setBonus(bool bonus)
{
    if (m_bonus == bonus)
        return;
    m_bonus = bonus;
    m_bonusTexture = m_bonus;
    update();

    if (m_bonus) {
        m_timer->start();
    } else {
        m_timer->stop();
    }
    emit bonusChanged();
}

void BCEnemyTank::timerFired()
{
    m_bonusTexture = !m_bonusTexture;
    update();
    m_timer->start();
}

BCArmorTank::BCArmorTank(BCBoard *board) :
    BCEnemyTank(board),
    m_currentHealth(BCArmorTank::health()),
    m_greenToGoldTexture(false)
{

}

void BCArmorTank::hit()
{
    if (destroyed())
        return;

    if (m_currentHealth == health())
        setBonus(false);

    --m_currentHealth;
    if (m_currentHealth > 0) {
        if (m_currentHealth == health() - 2) {
            m_timer->start();
        } else {
            m_timer->stop();
        }
        return;
    }

    BCArmorTank::hit();
}

void BCArmorTank::timerFired()
{
    if (m_currentHealth == health() - 2) {
        m_greenToGoldTexture = !m_greenToGoldTexture;
        update();
        return;
    }
    BCEnemyTank::timerFired();
}

void BCArmorTank::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
#ifdef BC_DEBUG_RECT
    painter->setPen(Qt::white);
    painter->drawRect(option->rect);
#else
    if (bonus() || m_currentHealth == health() - 3) {
        BCEnemyTank::paint(painter, option, widget);
        return;
    }

    if (m_currentHealth == health()) {
        painter->drawPixmap(option->rect, BattleCity::armorTankGreenTexture(direction(), currentAnimationStep()));
    } else if (m_currentHealth == health() - 1) {
        painter->drawPixmap(option->rect, BattleCity::armorTankGoldTexture(direction(), currentAnimationStep()));
    } else if (m_currentHealth == health() - 2) {
        if (m_greenToGoldTexture) {
            painter->drawPixmap(option->rect, BattleCity::armorTankGoldTexture(direction(), currentAnimationStep()));
        } else {
            painter->drawPixmap(option->rect, BattleCity::armorTankGreenTexture(direction(), currentAnimationStep()));
        }
    }
#endif
}

void BCPlayerTank::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
#ifdef BC_DEBUG_RECT
    painter->setPen(Qt::white);
    painter->drawRect(option->rect);
#else
    painter->drawPixmap(option->rect, BattleCity::player1TankTexture(direction(), currentAnimationStep()));
#endif
}
