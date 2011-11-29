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


#ifndef BCBOARD_H
#define BCBOARD_H

#include <QDeclarativeItem>
#include <QDataStream>

class BCBoard;
class BCEnemyTank;
//class BCObstacle;
class BCItem;
class BCFalcon;
class BCPlayerTank;

class BCBoard : public QDeclarativeItem
{
    Q_OBJECT

    friend QDataStream &operator << (QDataStream &out, const BCBoard &board);
    friend QDataStream &operator >> (QDataStream &in, BCBoard &board);

    Q_PROPERTY(int boardSize READ boardSize WRITE setBoardSize NOTIFY boardSizeChanged)
    Q_PROPERTY(qreal cellSize READ cellSize WRITE setCellSize NOTIFY cellSizeChanged)
    Q_PROPERTY(qreal obsticaleSize READ obsticaleSize NOTIFY cellSizeChanged)
    Q_PROPERTY(bool gridVisible READ gridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
public:
    explicit BCBoard(QDeclarativeItem *parent = 0);

    void setBoardSize(int boardSize);
    int boardSize() const { return m_boardSize; }

    void setCellSize(qreal size);
    qreal cellSize() const { return m_cellSize; }
    qreal obsticaleSize() const;

    void setGridVisible(bool visible);
    bool gridVisible() const { return m_gridVisible; }

#ifdef BC_DEBUG_RECT
    void setDebugRect(const QRectF &rect) { m_debugRect = rect; update(); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
#endif

signals:
    void boardSizeChanged();
    void cellSizeChanged(qreal size);
    void gridVisibleChanged();

public slots:
//    int xToRow(qreal x) const;
//    int yToColumn(qreal y) const;
    BCItem *obstacle(int row, int column) const;
    void setObstacleType(int row, int column, int type);
    void setCursor(int type);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    void init();

private:
    int m_boardSize;
    qreal m_cellSize;

    mutable QList<QList<BCItem *> > m_cells;

    bool m_gridVisible;

    QList<BCEnemyTank *> m_enemyTanks;
    BCFalcon *m_falcon;
    BCPlayerTank *m_playerTank;

#ifdef BC_DEBUG_RECT
    QRectF m_debugRect;
#endif
};

QDataStream &operator << (QDataStream &out, const BCBoard &board);
QDataStream &operator >> (QDataStream &in, BCBoard &board);

#endif // BCBOARD_H
