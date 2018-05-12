#ifndef CANVAS_H
#define CANVAS_H

// External libraries / headers (Qt Objects)
#include <QPaintEvent>
#include <QPainter>
#include <QImage>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QWidget>

class Canvas
{
    QImage image;
    int strokeSize;
    int numInteractions;
public:
    Canvas();
    Canvas(const QImage &i);
    ~Canvas();

    void init(int width, int height);
    bool setImage(const QImage &im, bool matchSize = false);

    void mouseEvent(QMouseEvent *m, QWidget *parent);
    void mouseWheelEvent(QWheelEvent *w, QWidget *parent);
    void updateCursor(QWidget *parent);

    void draw(QPainter * painter, QPaintEvent *event, QWidget *parent);

    int getNumInteractions();

private:
    QRect getDrawRect(QWidget* parent);
};

#endif // CANVAS_H
