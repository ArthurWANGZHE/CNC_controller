#include "plotwidget.h"

PlotWidget::PlotWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(400, 400);
    setAutoFillBackground(true);
}

void PlotWidget::addPoint(int x, int y) {
    trajectory.append(QPoint(x, y));
    update();
}

void PlotWidget::setLinePoints(const QVector<QPoint> &points) {
    trajectory = points;
    update();
}

void PlotWidget::clear() {
    trajectory.clear();
    update();
}

void PlotWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景坐标系
    QPen axisPen(Qt::gray, 1, Qt::DashLine);
    painter.setPen(axisPen);
    int width = this->width();
    int height = this->height();

    // 坐标轴中心为视图中央
    QPoint origin(width / 2, height / 2);
    painter.drawLine(0, origin.y(), width, origin.y()); // X轴
    painter.drawLine(origin.x(), 0, origin.x(), height); // Y轴

    // 坐标刻度
    for (int i = -width/2; i <= width/2; i += 50) {
        painter.drawLine(origin.x() + i, origin.y() - 5, origin.x() + i, origin.y() + 5);
        painter.drawText(origin.x() + i + 2, origin.y() - 7, QString::number(i));
    }
    for (int j = -height/2; j <= height/2; j += 50) {
        painter.drawLine(origin.x() - 5, origin.y() - j, origin.x() + 5, origin.y() - j);
        if (j != 0) painter.drawText(origin.x() + 6, origin.y() - j - 2, QString::number(j));
    }

    // 绘制轨迹
    QPen pen(Qt::blue, 2);
    painter.setPen(pen);

    if (trajectory.size() >= 2) {
        for (int i = 0; i < trajectory.size() - 1; ++i) {
            QPoint p1 = origin + QPoint(trajectory[i].x(), -trajectory[i].y());
            QPoint p2 = origin + QPoint(trajectory[i + 1].x(), -trajectory[i + 1].y());
            painter.drawLine(p1, p2);
        }
    } else if (trajectory.size() == 1) {
        QPoint p = origin + QPoint(trajectory[0].x(), -trajectory[0].y());
        painter.drawPoint(p);
    }
}
