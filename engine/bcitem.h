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


#ifndef BCITEM_H
#define BCITEM_H

#include <QDeclarativeItem>
#include <QPoint>

#include "bcglobal.h"

class BCBoard;

class BCItem : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(int row READ row CONSTANT)
    Q_PROPERTY(int column READ column CONSTANT)

    friend class BCBoard;

    Q_PROPERTY(int type READ type CONSTANT)
public:
    enum ItemProperty { Traversable, Nontraversable, Destroyable, Movable };

    explicit BCItem(BCBoard *parent = 0);

    int row() const { return m_position.y(); }
    int column() const { return m_position.x(); }

    qreal size() const { return implicitWidth(); }

    virtual ItemProperty itemProperty() const = 0;

    BCBoard *board() const { return m_board; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    void setSize(qreal size)
    {
        setImplicitWidth(size);
        setImplicitHeight(size);
        reposItem();
        emit sizeChanged(size);
    }

signals:
    void sizeChanged(qreal size);

protected:
    void setPosition(int row, int column);

private:
    void reposItem();

private:
    BCBoard *m_board;
    QPoint m_position;
};

class BCTraversableItem : public BCItem
{
    Q_OBJECT
public:
    explicit BCTraversableItem(BCBoard *parent = 0) :
        BCItem(parent) { }

    ItemProperty itemProperty() const { return Traversable; }
};

class BCNontraversableItem : public BCItem
{
    Q_OBJECT
public:
    explicit BCNontraversableItem(BCBoard *parent = 0) :
        BCItem(parent) { }

    ItemProperty itemProperty() const { return Nontraversable; }
};

class BCDestroyableItem : public BCNontraversableItem
{
    Q_OBJECT
public:
    explicit BCDestroyableItem(BCBoard *parent = 0) :
        BCNontraversableItem(parent) { }

    ItemProperty itemProperty() const { return Destroyable; }
};

class BCMovableItem : public BCItem
{
    Q_OBJECT
public:
    explicit BCMovableItem(BCBoard *parent = 0) :
        BCItem(parent), m_direction(BattleCity::Forward) { setZValue(1); }

    ItemProperty itemProperty() const { return Movable; }

    virtual bool move(BattleCity::MoveDirection direction) = 0;
    virtual qreal speed() const = 0;

protected:
    BattleCity::MoveDirection direction() const { return m_direction; }

private:
    BattleCity::MoveDirection m_direction;
};

class BCGroud : public BCTraversableItem
{
    Q_OBJECT
public:
    explicit BCGroud(BCBoard *parent = 0) :
        BCTraversableItem(parent) { }

    int type() const { return BattleCity::Ground; }
};

class BCIce : public BCTraversableItem
{
    Q_OBJECT
public:
    explicit BCIce(BCBoard *parent = 0) :
        BCTraversableItem(parent) { }

    int type() const { return BattleCity::Ice; }
};

class BCCamouflage : public BCTraversableItem
{
    Q_OBJECT
public:
    explicit BCCamouflage(BCBoard *parent = 0) :
        BCTraversableItem(parent) { setZValue(1); }

    int type() const { return BattleCity::Camouflage; }
};

class BCWater : public BCNontraversableItem
{
    Q_OBJECT
public:
    explicit BCWater(BCBoard *parent = 0) :
        BCNontraversableItem(parent) { }

    int type() const { return BattleCity::Water; }
};

class BCBricks : public BCDestroyableItem
{
    Q_OBJECT
public:
    explicit BCBricks(BCBoard *parent = 0) :
        BCDestroyableItem(parent) { }

    int type() const { return BattleCity::BricksWall; }
};

class BCConcrete : public BCDestroyableItem
{
    Q_OBJECT
public:
    explicit BCConcrete(BCBoard *parent = 0) :
        BCDestroyableItem(parent) { }

    int type() const { return BattleCity::ConcreteWall; }
};

class BCFalcon : public BCDestroyableItem
{
    Q_OBJECT
public:
    explicit BCFalcon(BCBoard *parent = 0) :
        BCDestroyableItem(parent) { }

    int type() const { return BattleCity::Falcon; }
};

#endif // BCITEM_H
