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


#ifndef BATTLECITYGLOBAL_H
#define BATTLECITYGLOBAL_H

#include <QString>
#include <QDeclarativeItem>

class BattleCity : public QObject
{
    Q_OBJECT

    Q_ENUMS(ObstacleType)
    Q_ENUMS(TankType)
    Q_ENUMS(MoveDirection)
public:
    enum MoveDirection { Forward, Backward, Left, Right };
    enum ObstacleType { Ground = QDeclarativeItem::UserType + 1, BricksWall, ConcreteWall, Ice, Camouflage, Falcon, FalconDestroyed, Water };
    enum ItemProperty { Traversable, Nontraversable, Destroyable, Movable };
    enum TankType { Basic = Water + 1, Fast, Power, Armor };
    enum Edge { NoneEdge, TopEdge, RightEdge, BottomEdge, LeftEdge };

    typedef QMap<ObstacleType, QString> ObstacleTexturesMap;
    typedef QMap<MoveDirection, QStringList> TankTexturesMap;
    typedef QMap<MoveDirection, QString> ProjectileTexturesMap;

    BattleCity(QObject *parent = 0) : QObject(parent) { }

    static void init();

    static const quint8 tankAnimationSteps = 2;

    Q_INVOKABLE static QPixmap obstacleTexture(ObstacleType type);
    static QPixmap cursorPixmap(ObstacleType type);

    Q_INVOKABLE static QPixmap projectileTexture(MoveDirection direction);

    Q_INVOKABLE static QPixmap tankTexture(TankType type, MoveDirection direction, int step, bool bonus);

    static QPixmap armorTankGoldTexture(MoveDirection direction, int step);
    static QPixmap armorTankGreenTexture(MoveDirection direction, int step);

    static QPixmap player1TankTexture(MoveDirection direction, int step);
    static QPixmap player1TankOneStarTexture(MoveDirection direction, int step);
    static QPixmap player1TankTwoStarsTexture(MoveDirection direction, int step);
    static QPixmap player1TankThreeStarsTexture(MoveDirection direction, int step);

private:
    static QMap<TankType, TankTexturesMap> initNormalTankTexturesMap();
    static QMap<TankType, TankTexturesMap> initBonusTankTexturesMap();

private:
    static const ObstacleTexturesMap obstacleNames;
    static const ObstacleTexturesMap cursorNames;
    static const ProjectileTexturesMap projectileTextureNames;

    static const TankTexturesMap basicTankNormalTexturesNames;
    static const TankTexturesMap basicTankBonusTexturesNames;
    static const TankTexturesMap fastTankNormalTexturesNames;
    static const TankTexturesMap fastTankBonusTexturesNames;
    static const TankTexturesMap powerTankNormalTexturesNames;
    static const TankTexturesMap powerTankBonusTexturesNames;
    static const TankTexturesMap armorTankNormalTexturesNames;
    static const TankTexturesMap armorTankBonusTexturesNames;
    static const TankTexturesMap armorTankGreenTexturesNames;
    static const TankTexturesMap armorTankGoldTexturesNames;

    static const TankTexturesMap player1TankTexturesNames;
    static const TankTexturesMap player1TankOneStarTexturesNames;
    static const TankTexturesMap player1TankTwoStarsTexturesNames;
    static const TankTexturesMap player1TankThreeStarsTexturesNames;

    static const QMap<TankType, TankTexturesMap> normalTankTextures;
    static const QMap<TankType, TankTexturesMap> bonusTankTextures;
};

class Pixmap : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap NOTIFY pixmapChcanged)
public:
    Pixmap(QDeclarativeItem *parent = 0);

    void setPixmap(const QPixmap &pixmap) { m_pixmap = pixmap; emit pixmapChcanged(); update(); }
    QPixmap pixmap() const { return m_pixmap; }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
    void pixmapChcanged();

private:
    QPixmap m_pixmap;
};

#endif // BATTLECITYGLOBAL_H
