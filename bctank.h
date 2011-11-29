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


#ifndef BCTANK_H
#define BCTANK_H

#include "bcitem.h"

class BCBoard;
class QTimer;

class BCMovableItem : public BCItem
{
    Q_OBJECT

    friend class BCBoard;
public:
    explicit BCMovableItem(QDeclarativeItem *parent = 0);

    virtual bool move(BattleCity::MoveDirection direction) = 0;
    virtual qreal speed() const = 0;

protected:
    BattleCity::MoveDirection direction() const { return m_direction; }

private:
    BattleCity::MoveDirection m_direction;
};

class BCAbstractTank : public BCMovableItem
{
    Q_OBJECT
public:
    explicit BCAbstractTank(BCBoard *board);

    bool move(BattleCity::MoveDirection direction);

    virtual quint8 health() const { return 1; }

    qreal speed() const { return 5.0; }

    virtual void hit() { m_destroyed = true; }

    bool destroyed() const { return m_destroyed; }

protected:
    BCBoard *board() const { return m_board; }
    quint8 currentAnimationStep() const { return m_currentAnimationStep; }

private:
    BCBoard *m_board;
    quint8 m_currentAnimationStep;
    bool m_destroyed;
};

class BCEnemyTank : public BCAbstractTank
{
    Q_OBJECT

    Q_ENUMS(Type)
public:
    explicit BCEnemyTank(BCBoard *board);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void setBonus(bool bonus);
    bool bonus() const { return m_bonus; }

    void hit()
    {
        BCAbstractTank::hit();
        setBonus(false);
    }

protected slots:
    virtual void timerFired();

private:
    bool m_bonus;
    bool m_bonusTexture;

protected:
    QTimer *m_timer;
};

class BCBasicTank : public BCEnemyTank
{
    Q_OBJECT
public:
    explicit BCBasicTank(BCBoard *board) :
        BCEnemyTank(board) { }

    int type() const { return BattleCity::Basic; }
};

class BCFastTank : public BCEnemyTank
{
    Q_OBJECT
public:
    explicit BCFastTank(BCBoard *board) :
        BCEnemyTank(board) { }

    int type() const { return BattleCity::Fast; }

    qreal speed() const { return 2 * BCEnemyTank::speed(); }
};

class BCPowerTank : public BCEnemyTank
{
    Q_OBJECT
public:
    explicit BCPowerTank(BCBoard *board) :
        BCEnemyTank(board) { }

    int type() const { return BattleCity::Power; }
};

class BCArmorTank : public BCEnemyTank
{
    Q_OBJECT
public:
    explicit BCArmorTank(BCBoard *board);

    int type() const { return BattleCity::Armor; }

    qreal speed() const { return BCEnemyTank::speed() / 2.0; }
    quint8 health() const { return 4; }

    void hit();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected slots:
    void timerFired();

private:
    quint8 m_currentHealth;
    bool m_greenToGoldTexture;
};

class BCPlayerTank : public BCAbstractTank
{
    Q_OBJECT
public:
    explicit BCPlayerTank(BCBoard *board) :
        BCAbstractTank(board) { }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

#endif // BCTANK_H
