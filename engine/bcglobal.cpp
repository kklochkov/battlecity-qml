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

#include "bcglobal.h"
#include "bcitem.h"
#include "bctank.h"
#include "bcmapsmanager.h"
#include "bcboard.h"

const char *BATTLE_CITY_URI = "BattleCity";

static void initTankTextureFiles(QMap<QString, QString> &map, const QString &qrcPrefix, const QString &tankDir, const BattleCity::TankTexturesMap &texturesMap)
{
    QMap<BattleCity::MoveDirection, QString> directions;
    directions[BattleCity::Forward] = "forward";
    directions[BattleCity::Backward] = "backward";
    directions[BattleCity::Left] = "left";
    directions[BattleCity::Right] = "right";

    QMap<BattleCity::MoveDirection, QString>::const_iterator dIt = directions.constBegin();
    QMap<BattleCity::MoveDirection, QString>::const_iterator dEnd = directions.constEnd();
    for (; dIt != dEnd; ++dIt) {
        for (int i = 0; i < BattleCity::tankAnimationSteps; ++i)
            map[texturesMap[dIt.key()][i]] = QString("%1%2%3_%4.png").arg(qrcPrefix).arg(tankDir).arg(*dIt).arg(i + 1);
    }
}

void BattleCity::init()
{
    qmlRegisterUncreatableType<BattleCity>(BATTLE_CITY_URI, 1, 0, "BattleCity", "");
    qmlRegisterUncreatableType<BCEnemyTank>(BATTLE_CITY_URI, 1, 0, "BCEnemyTank", "");
    qmlRegisterType<BCBoard>(BATTLE_CITY_URI, 1, 0, "BCBoard");
    qmlRegisterType<BCMapsManager>(BATTLE_CITY_URI, 1, 0, "BCMapsManager");
    qmlRegisterType<Pixmap>(BATTLE_CITY_URI, 1, 0, "Pixmap");

    static const QString qrcPrefix = ":/battlecity/images/";

    QMap<QString, QString> map;

    QMap<ObstacleType, QString> obstalcesFileNames;
    obstalcesFileNames[Ground] = "ground.png";
    obstalcesFileNames[BricksWall] = "bricks.png";
    obstalcesFileNames[Ice] = "ice.png";
    obstalcesFileNames[Water] = "water.png";
    obstalcesFileNames[ConcreteWall] = "concrete.png";
    obstalcesFileNames[Camouflage] = "camouflage.png";
    obstalcesFileNames[Falcon] = "falcon_normal.png";
    obstalcesFileNames[FalconDestroyed] = "falcon_destroyed.png";

    static const QString obstaclesDir = "obstacles/";
    static const QString cursorsDir = "cursors/";

    QMap<ObstacleType, QString>::const_iterator oIt = obstalcesFileNames.constBegin();
    QMap<ObstacleType, QString>::const_iterator oEnd = obstalcesFileNames.constEnd();
    for (; oIt != oEnd; ++oIt) {
        map[obstacleNames[oIt.key()]] = QString("%1%2%3").arg(qrcPrefix).arg(obstaclesDir).arg(*oIt);
        if (oIt.key() == Falcon || oIt.key() == FalconDestroyed)
            continue;
        map[cursorNames[oIt.key()]] = QString("%1%2cursor_%3").arg(qrcPrefix).arg(cursorsDir).arg(*oIt);
    }

    static const QString basicTankDir = "tanks/basic/";
    static const QString basicTankBonusDir = "tanks/basic/bonus/";
    static const QString fastTankDir = "tanks/fast/";
    static const QString fastTankBonusDir = "tanks/fast/bonus/";
    static const QString powerTankDir = "tanks/power/";
    static const QString powerTankBonusDir = "tanks/power/bonus/";
    static const QString armorTankDir = "tanks/armor/";
    static const QString armorTankBonusDir = "tanks/armor/bonus/";
    static const QString armorTankGreenDir = "tanks/armor/green/";
    static const QString armorTankGoldDir = "tanks/armor/gold/";
    static const QString player1TankDir = "tanks/player1/";
    static const QString player1TankOneStarDir = "tanks/player1/one_star/";
    static const QString player1TankTwoStarsDir = "tanks/player1/two_stars/";
    static const QString player1TankThreeStarsDir = "tanks/player1/three_stars/";

    initTankTextureFiles(map, qrcPrefix, basicTankDir, basicTankNormalTexturesNames);
    initTankTextureFiles(map, qrcPrefix, basicTankBonusDir, basicTankBonusTexturesNames);
    initTankTextureFiles(map, qrcPrefix, fastTankDir, fastTankNormalTexturesNames);
    initTankTextureFiles(map, qrcPrefix, fastTankBonusDir, fastTankBonusTexturesNames);
    initTankTextureFiles(map, qrcPrefix, powerTankDir, powerTankNormalTexturesNames);
    initTankTextureFiles(map, qrcPrefix, powerTankBonusDir, powerTankBonusTexturesNames);
    initTankTextureFiles(map, qrcPrefix, armorTankDir, armorTankNormalTexturesNames);
    initTankTextureFiles(map, qrcPrefix, armorTankBonusDir, armorTankBonusTexturesNames);
    initTankTextureFiles(map, qrcPrefix, armorTankGreenDir, armorTankGreenTexturesNames);
    initTankTextureFiles(map, qrcPrefix, armorTankGoldDir, armorTankGoldTexturesNames);
    initTankTextureFiles(map, qrcPrefix, player1TankDir, player1TankTexturesNames);
    initTankTextureFiles(map, qrcPrefix, player1TankOneStarDir, player1TankOneStarTexturesNames);
    initTankTextureFiles(map, qrcPrefix, player1TankTwoStarsDir, player1TankTwoStarsTexturesNames);
    initTankTextureFiles(map, qrcPrefix, player1TankThreeStarsDir, player1TankThreeStarsTexturesNames);

    QPixmap pixmap;
    QMap<QString, QString>::const_iterator it = map.constBegin();
    for (; it != map.constEnd(); ++it) {
        pixmap.load(it.value());
        QPixmapCache::insert(it.key(), pixmap);
    }
}

static QPixmap pixmap(BattleCity::ObstacleType type, const BattleCity::ObstacleTexturesMap &map)
{
    QPixmap pixmap;
    QPixmapCache::find(map[type], &pixmap);
    return pixmap;
}

static BattleCity::TankTexturesMap initTexturesMap(const QString &prefix)
{
    BattleCity::TankTexturesMap map;

    QMap<BattleCity::MoveDirection, QString> directions;
    directions[BattleCity::Forward] = "Forward";
    directions[BattleCity::Backward] = "Backward";
    directions[BattleCity::Left] = "Left";
    directions[BattleCity::Right] = "Right";

    QMap<BattleCity::MoveDirection, QString>::const_iterator it = directions.constBegin();
    QMap<BattleCity::MoveDirection, QString>::const_iterator end = directions.constEnd();
    for (; it != end; ++it) {
        QStringList list;
        for (int i = 1; i <= BattleCity::tankAnimationSteps; ++i)
            list << QString("%1%2%3").arg(prefix).arg(*it).arg(i);
        map[it.key()] = list;
    }
    return map;
}

static QPixmap pixmap(BattleCity::MoveDirection direction, int step, const BattleCity::TankTexturesMap &map)
{
    QPixmap pixmap;
    QPixmapCache::find(map[direction][step], &pixmap);
    return pixmap;
}

static BattleCity::ObstacleTexturesMap initObstaclesMap()
{
    BattleCity::ObstacleTexturesMap map;
    map[BattleCity::Ground] = "ground";
    map[BattleCity::Ice] = "ice";
    map[BattleCity::BricksWall] = "bricks";
    map[BattleCity::ConcreteWall] = "concrete";
    map[BattleCity::Camouflage] = "camouflage";
    map[BattleCity::Water] = "water";
    map[BattleCity::Falcon] = "falconNormal";
    map[BattleCity::FalconDestroyed] = "falconDestroyed";
    return map;
}

const BattleCity::ObstacleTexturesMap BattleCity::obstacleNames = initObstaclesMap();

QPixmap BattleCity::obstacleTexture(ObstacleType type)
{
    return pixmap(type, obstacleNames);
}

static BattleCity::ObstacleTexturesMap initCursorsMap()
{
    BattleCity::ObstacleTexturesMap map;
    map[BattleCity::Ground] = "cursorGroud";
    map[BattleCity::Ice] = "cursorIce";
    map[BattleCity::BricksWall] = "cursorBricks";
    map[BattleCity::ConcreteWall] = "cursorConcrete";
    map[BattleCity::Camouflage] = "cursorCamouflage";
    map[BattleCity::Water] = "cursorWater";
    return map;
}

const BattleCity::ObstacleTexturesMap BattleCity::cursorNames = initCursorsMap();

QPixmap BattleCity::cursorPixmap(ObstacleType type)
{
    return pixmap(type, cursorNames);
}

const BattleCity::TankTexturesMap BattleCity::basicTankNormalTexturesNames = initTexturesMap("basicTank");
const BattleCity::TankTexturesMap BattleCity::basicTankBonusTexturesNames = initTexturesMap("basicTankBonus");
const BattleCity::TankTexturesMap BattleCity::fastTankNormalTexturesNames = initTexturesMap("fastTank");
const BattleCity::TankTexturesMap BattleCity::fastTankBonusTexturesNames = initTexturesMap("fastTankBonus");
const BattleCity::TankTexturesMap BattleCity::powerTankNormalTexturesNames = initTexturesMap("powerTank");
const BattleCity::TankTexturesMap BattleCity::powerTankBonusTexturesNames = initTexturesMap("powerTankBonus");
const BattleCity::TankTexturesMap BattleCity::armorTankNormalTexturesNames = initTexturesMap("armorTank");
const BattleCity::TankTexturesMap BattleCity::armorTankBonusTexturesNames = initTexturesMap("armorTankBonus");
const BattleCity::TankTexturesMap BattleCity::armorTankGreenTexturesNames = initTexturesMap("armorTankGreen");
const BattleCity::TankTexturesMap BattleCity::armorTankGoldTexturesNames = initTexturesMap("armorTankGold");

QMap<BattleCity::TankType, BattleCity::TankTexturesMap> BattleCity::initNormalTankTexturesMap()
{
    QMap<BattleCity::TankType, BattleCity::TankTexturesMap> map;
    map[BattleCity::Basic] = BattleCity::basicTankNormalTexturesNames;
    map[BattleCity::Fast] = BattleCity::fastTankNormalTexturesNames;
    map[BattleCity::Power] = BattleCity::powerTankNormalTexturesNames;
    map[BattleCity::Armor] = BattleCity::armorTankNormalTexturesNames;
    return map;
}

QMap<BattleCity::TankType, BattleCity::TankTexturesMap> BattleCity::initBonusTankTexturesMap()
{
    QMap<BattleCity::TankType, BattleCity::TankTexturesMap> map;
    map[BattleCity::Basic] = BattleCity::basicTankBonusTexturesNames;
    map[BattleCity::Fast] = BattleCity::fastTankBonusTexturesNames;
    map[BattleCity::Power] = BattleCity::powerTankBonusTexturesNames;
    map[BattleCity::Armor] = BattleCity::armorTankBonusTexturesNames;
    return map;
}

const QMap<BattleCity::TankType, BattleCity::TankTexturesMap> BattleCity::normalTankTextures = initNormalTankTexturesMap();
const QMap<BattleCity::TankType, BattleCity::TankTexturesMap> BattleCity::bonusTankTextures = initBonusTankTexturesMap();

QPixmap BattleCity::tankTexture(TankType type, MoveDirection direction, int step, bool bonus)
{
    return bonus ? pixmap(direction, step, bonusTankTextures[type]) : pixmap(direction, step, normalTankTextures[type]);
}

QPixmap BattleCity::armorTankGoldTexture(MoveDirection direction, int step)
{
    return pixmap(direction, step, armorTankGoldTexturesNames);
}

QPixmap BattleCity::armorTankGreenTexture(MoveDirection direction, int step)
{
    return pixmap(direction, step, armorTankGreenTexturesNames);
}

const BattleCity::TankTexturesMap BattleCity::player1TankTexturesNames = initTexturesMap("player1TankNoStars");
const BattleCity::TankTexturesMap BattleCity::player1TankOneStarTexturesNames = initTexturesMap("player1TankOneStar");
const BattleCity::TankTexturesMap BattleCity::player1TankTwoStarsTexturesNames = initTexturesMap("player1TankTwoStars");
const BattleCity::TankTexturesMap BattleCity::player1TankThreeStarsTexturesNames = initTexturesMap("player1TankThreeStars");

QPixmap BattleCity::player1TankTexture(MoveDirection direction, int step)
{
    return pixmap(direction, step, player1TankTexturesNames);
}

QPixmap BattleCity::player1TankOneStarTexture(MoveDirection direction, int step)
{
    return pixmap(direction, step, player1TankOneStarTexturesNames);
}

QPixmap BattleCity::player1TankTwoStarsTexture(MoveDirection direction, int step)
{
    return pixmap(direction, step, player1TankTwoStarsTexturesNames);
}

QPixmap BattleCity::player1TankThreeStarsTexture(MoveDirection direction, int step)
{
    return pixmap(direction, step, player1TankThreeStarsTexturesNames);
}

Pixmap::Pixmap(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

void Pixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    painter->drawPixmap(option->rect, m_pixmap);
}
