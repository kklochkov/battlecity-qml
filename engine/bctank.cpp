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

BCAbstractTank::BCAbstractTank(BattleCity::MoveDirection direction, BCBoard *board) :
    BCMovableItem(direction, board),
    m_currentAnimationStep(0),
    m_destroyed(false),
    m_projectile(0)
{

}

bool BCAbstractTank::move(BattleCity::MoveDirection direction)
{
    ++m_currentAnimationStep;
    if (m_currentAnimationStep == BattleCity::tankAnimationSteps)
        m_currentAnimationStep = 0;

    return BCMovableItem::move(direction);
}

void BCAbstractTank::adjustIntersectionPointWithBoardBoundingRect(BattleCity::Edge edge, qreal &x, qreal &y) const
{
    switch (edge) {
    case BattleCity::LeftEdge:
        x = 0;
        break;
    case BattleCity::RightEdge:
        x = board()->implicitWidth() - implicitWidth();// - extraPixel;
        break;
    case BattleCity::TopEdge:
        y = 0;
        break;
    case BattleCity::BottomEdge:
        y = board()->implicitHeight() - implicitHeight();// - extraPixel;
        break;
    case BattleCity::NoneEdge:
        break;
    }
}

void BCAbstractTank::adjustIntersectionPointWithObstacle(const BCItem *obstacle, BattleCity::Edge edge, qreal &x, qreal &y) const
{
    if (!obstacle)
        return;
    switch (edge) {
    case BattleCity::LeftEdge:
        x = obstacle->x() - implicitWidth();
        break;
    case BattleCity::RightEdge:
        x = obstacle->x() + obstacle->implicitWidth();
        break;
    case BattleCity::TopEdge:
        y = obstacle->y() - implicitHeight();
        break;
    case BattleCity::BottomEdge:
        y = obstacle->y() + obstacle->implicitHeight();
        break;
    case BattleCity::NoneEdge:
        break;
    }
}

void BCAbstractTank::fire()
{
    if (m_projectile)
        return;
    //TODO: review me
    m_projectile = new BCProjectile(5.0, direction(), board());
    m_projectile->setSize(implicitWidth() / 6.0);
    if (direction() == BattleCity::Forward) {
        m_projectile->setPos(x() + (implicitWidth() - m_projectile->implicitWidth()) / 2.0, y() - 5.0);
    } else if (direction() == BattleCity::Backward) {
        m_projectile->setPos(x() + (implicitWidth() - m_projectile->implicitWidth()) / 2.0, y() + implicitWidth());
    } else if (direction() == BattleCity::Left) {
        m_projectile->setPos(x() - 5.0, y() + (implicitWidth() - m_projectile->implicitWidth()) / 2.0);
    } else if (direction() == BattleCity::Right) {
        m_projectile->setPos(x() + implicitWidth(), y() + (implicitWidth() - m_projectile->implicitWidth()) / 2.0);
    }
    connect(m_projectile, SIGNAL(exploded()), SLOT(projectileExploded()));
    m_projectile->launch();
}

void BCAbstractTank::projectileExploded()
{
    delete m_projectile;
}

BCEnemyTank::BCEnemyTank(BCBoard *board) :
    BCAbstractTank(BattleCity::Backward, board),
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
