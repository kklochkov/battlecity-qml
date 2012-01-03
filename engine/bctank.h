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

#include <QPointer>

class BCBoard;
class QTimer;
class BCProjectile;

class BCAbstractTank : public BCMovableItem
{
    Q_OBJECT
public:
    explicit BCAbstractTank(BattleCity::MoveDirection direction, BCBoard *board);

    bool move(BattleCity::MoveDirection direction);

    virtual quint8 health() const { return 1; }

    qreal speed() const { return 5.0; }

    virtual void hit() { m_destroyed = true; }

    bool destroyed() const { return m_destroyed; }

    void fire();

private slots:
    void projectileExploded();

protected:
    quint8 currentAnimationStep() const { return m_currentAnimationStep; }

private:
    quint8 m_currentAnimationStep;
    bool m_destroyed;
    QPointer<BCProjectile> m_projectile;
};

class BCEnemyTank : public BCAbstractTank
{
    Q_OBJECT

    Q_ENUMS(Type)

    Q_PROPERTY(bool bonus READ bonus NOTIFY bonusChanged)
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

signals:
    void bonusChanged();

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
        BCAbstractTank(BattleCity::Forward, board) { }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

#endif // BCTANK_H
