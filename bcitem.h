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
public:
    explicit BCItem(QDeclarativeItem *parent = 0);

    int row() const { return m_position.x(); }
    int column() const { return m_position.y(); }

    qreal size() const { return implicitWidth(); }

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
    QPoint m_position;
};

class BCObstacle : public BCItem
{
    Q_OBJECT

    friend class BCBoard;
public:
    explicit BCObstacle(BCBoard *parent = 0);

    virtual BattleCity::ObstacleProperty property() const = 0;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    BCBoard *m_board;
};

class BCTraversableObstacle : public BCObstacle
{
    Q_OBJECT
public:
    explicit BCTraversableObstacle(BCBoard *parent = 0) :
        BCObstacle(parent) { }

    virtual BattleCity::ObstacleProperty property() const { return BattleCity::Traversable; }
};

class BCNontraversableObstacle : public BCObstacle
{
    Q_OBJECT
public:
    explicit BCNontraversableObstacle(BCBoard *parent = 0) :
        BCObstacle(parent) { }

    virtual BattleCity::ObstacleProperty property() const { return BattleCity::Nontraversable; }
};

class BCDestroyableObstacle : public BCNontraversableObstacle
{
    Q_OBJECT
public:
    explicit BCDestroyableObstacle(BCBoard *parent = 0) :
        BCNontraversableObstacle(parent) { }

    virtual BattleCity::ObstacleProperty property() const { return BattleCity::Destroyable; }
};

class BCGroud : public BCTraversableObstacle
{
    Q_OBJECT
public:
    explicit BCGroud(BCBoard *parent = 0) :
        BCTraversableObstacle(parent) { }

    int type() const { return BattleCity::Ground; }
};

class BCIce : public BCTraversableObstacle
{
    Q_OBJECT
public:
    explicit BCIce(BCBoard *parent = 0) :
        BCTraversableObstacle(parent) { }

    int type() const { return BattleCity::Ice; }
};

class BCCamouflage : public BCTraversableObstacle
{
    Q_OBJECT
public:
    explicit BCCamouflage(BCBoard *parent = 0) :
        BCTraversableObstacle(parent) { setZValue(1); }

    int type() const { return BattleCity::Camouflage; }
};

class BCWater : public BCNontraversableObstacle
{
    Q_OBJECT
public:
    explicit BCWater(BCBoard *parent = 0) :
        BCNontraversableObstacle(parent) { }

    int type() const { return BattleCity::Water; }
};

class BCBricks : public BCDestroyableObstacle
{
    Q_OBJECT
public:
    explicit BCBricks(BCBoard *parent = 0) :
        BCDestroyableObstacle(parent) { }

    int type() const { return BattleCity::BricksWall; }
};

class BCConcrete : public BCDestroyableObstacle
{
    Q_OBJECT
public:
    explicit BCConcrete(BCBoard *parent = 0) :
        BCDestroyableObstacle(parent) { }

    int type() const { return BattleCity::ConcreteWall; }
};

class BCFalcon : public BCDestroyableObstacle
{
    Q_OBJECT
public:
    explicit BCFalcon(BCBoard *parent = 0) :
        BCDestroyableObstacle(parent) { }

    int type() const { return BattleCity::Falcon; }
};

#endif // BCITEM_H
