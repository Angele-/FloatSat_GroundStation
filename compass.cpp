
#include <QtWidgets>

#include "compass.h"

Compass::Compass(QWidget *parent)
    : QWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start();

}

void Compass::paintEvent(QPaintEvent *)
{
    static const QPoint minuteHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -70)
    };

    QColor minuteColor(Qt::black);

    int side = qMin(width(), height());

    //Initializing painter
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    //Moving painter to the middle of the square
    painter.translate(width() / 2, height() / 2);
    //Scaling the canvas so that there are 100 "points" in each direction?
    painter.scale(side / 200.0, side / 200.0);

    //Initializing painter
    painter.setPen(Qt::NoPen);
    painter.setBrush(minuteColor);

    painter.save();

    //Paint hand
    painter.rotate(heading);
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();

    //Paint minute markings
    painter.setPen(minuteColor);
    for (int j = 0; j < 60; ++j) {
        painter.drawLine(92, 0, 96, 0);
        painter.rotate(6.0);
    }

    static const QPoint north = QPoint(-4.5,-85);
    static const QPoint east = QPoint(85,5);
    static const QPoint south = QPoint(-4.5,95);
    static const QPoint west = QPoint(-96,5);

    QFont font=painter.font();
    font.setPointSize(10);
    font.setWeight(QFont::DemiBold);
    painter.setFont(font);
    painter.drawText(north, "N");
    painter.drawText(east, "E");
    painter.drawText(south, "S");
    painter.drawText(west, "W");
}

