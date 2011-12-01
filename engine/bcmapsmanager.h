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


#ifndef BCMAPSMANAGER_H
#define BCMAPSMANAGER_H

#include <QObject>
#include <QStringList>

class BCBoard;

class BCMapsManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList maps READ maps NOTIFY mapsListChanged)
public:
    explicit BCMapsManager(QObject *parent = 0);

    QStringList maps() const { return m_mapsList; }

    Q_INVOKABLE bool saveMap(BCBoard *board);
    Q_INVOKABLE bool loadMap(const QString &mapName, BCBoard *board);

signals:
    void mapsListChanged();
    void mapSaved();
    void mapLoaded();

private slots:
    void reloadMapsList();

private:
    QString m_mapsDir;
    QStringList m_mapsList;
    quint64 m_lastStage;
};

#endif // BCMAPSMANAGER_H
