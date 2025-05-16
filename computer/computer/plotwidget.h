#pragma once

#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QPoint>

class PlotWidget : public QWidget {
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = nullptr);

    void addPoint(int x, int y);
    void clear();
    void setLinePoints(const QVector<QPoint> &points);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QPoint> trajectory;
};
