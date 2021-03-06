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
    QColor overlayColor;
public:
    Canvas();
    Canvas(const QImage &i);
    ~Canvas();

    void init(int width, int height, QColor fill=Qt::transparent);
    bool setImage(const QImage &im, bool matchSize = false);
    QImage getImage();
    void setColor(QColor);

    void mouseEvent(QMouseEvent *m, QWidget *parent);
    void mouseWheelEvent(QWheelEvent *w, QWidget *parent);
    void updateCursor(QWidget *parent);

    void draw(QPainter * painter, QPaintEvent *event, QWidget *parent);

    int getNumInteractions() const;

private:
    QRect getDrawRect(QWidget* parent);
};

#endif // CANVAS_H
