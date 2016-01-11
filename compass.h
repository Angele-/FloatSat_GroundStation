
#ifndef COMPASS_H
#define COMPASS_H

#include <QWidget>

class Compass : public QWidget
{
    Q_OBJECT

public:
    explicit Compass(QWidget *parent = 0);
    float heading;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

signals:

public slots:
};

#endif // COMPASS_H
