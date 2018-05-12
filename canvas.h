#ifndef CANVAS_H
#define CANVAS_H

// External libraries / headers (Solvers, IO, Debugging)
#include <QPaintEvent>
#include <QPainter>
#include <QImage>

class Canvas
{
    QImage image;
public:
    Canvas();
    Canvas(const QImage &i);
    ~Canvas();

    void init(int width, int height);
    void setImage(const QImage &im);

    void strokeStart(int x, int y);
    void stroke(int x,int y);
    void strokeEnd(int x,int y);

    void draw(QPainter * painter, QPaintEvent *event, int w, int h);
};

#endif // CANVAS_H
