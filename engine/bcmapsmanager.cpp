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

#include <QDir>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QCoreApplication>

#include "bcmapsmanager.h"
#include "bcboard.h"

static QString BC_MAP_EXT = "bc";
static QString BC_STAGE = "stage";

BCMapsManager::BCMapsManager(QObject *parent) :
    QObject(parent),
    m_mapsDir(qApp->applicationDirPath() + "/maps"),
    m_lastStage(0)
{
    QDir mapsDir(m_mapsDir);
    if (!mapsDir.exists(m_mapsDir))
        mapsDir.mkdir(m_mapsDir);
    reloadMapsList();
}

static bool lessThen(QString s1, QString s2)    //TODO: optimaze this later
{
    const quint64 n1 = s1.remove(BC_STAGE).remove('.' + BC_MAP_EXT).toULongLong();
    const quint64 n2 = s2.remove(BC_STAGE).remove('.' + BC_MAP_EXT).toULongLong();
    return n1 < n2;
}

void BCMapsManager::reloadMapsList()    //TODO: optimaze this later
{
    QDir dir(m_mapsDir);
    m_mapsList = dir.entryList(QStringList() << QString("*.%1").arg(BC_MAP_EXT));
    qSort(m_mapsList.begin(), m_mapsList.end(), lessThen);
    m_lastStage = QString(m_mapsList.last()).remove(BC_STAGE).remove('.' + BC_MAP_EXT).toULongLong();
    emit mapsListChanged();
}

bool BCMapsManager::saveMap(BCBoard *board)
{
    QFile file(QString("%1/%2%3.%4").arg(m_mapsDir).arg(BC_STAGE).arg(m_lastStage + 1).arg(BC_MAP_EXT));
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file);
    out << (*board);

    QPixmap pixmap(board->implicitWidth(), board->implicitHeight());
    QPainter painter(&pixmap);
    const bool gridVisible = board->gridVisible();
    board->setGridVisible(false);
    board->scene()->render(&painter, QRect(0, 0, board->implicitWidth(), board->implicitHeight()),
                           QRect(5, 5, board->implicitWidth(), board->implicitHeight()));
    board->setGridVisible(gridVisible);

    QMetaObject::invokeMethod(const_cast<BCMapsManager *>(this), "reloadMapsList");
    emit mapSaved();
    return true;
}

bool BCMapsManager::loadMap(const QString &mapName, BCBoard *board)
{
    QFile file(QString("%1/%2").arg(m_mapsDir).arg(mapName));
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QDataStream in(&file);
    in >> (*board);
    emit mapLoaded();
    return true;
}
