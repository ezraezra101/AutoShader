#include "canvas.h"

Canvas::Canvas()
{
    init(100,100);
}

Canvas::Canvas(const QImage &i) {
    setImage(i);
}

Canvas::~Canvas() {

}

void Canvas::init(int width, int height) {
    image = QImage(width,height, QImage::Format_RGBA8888);
    image.fill(QColor(0,0,0,0));
}

void Canvas::setImage(const QImage &im) {
    image = im.convertToFormat(QImage::Format_RGBA8888);
}

void Canvas::strokeStart(int x, int y)
{

}
void Canvas::stroke(int x,int y)
{

}
void Canvas::strokeEnd(int x,int y)
{

}

#include <iostream>
void Canvas::draw(QPainter *painter, QPaintEvent *event, int w, int h) {
    QImage scaled;

    // Center the image in the WxH box.
    QRect pos;
    if(image.height()/(float)h > image.width()/(float)w) {
        // Center horizontally
       pos = QRect(0,0,(image.width() * h) / image.height(),h);
       pos.moveLeft((w - pos.width()) / 2);
    } else {
        // Center vertically
        pos = QRect(0,0,w,(image.height()*w) /image.width());
        pos.moveTop((h-pos.height())/2);
    }
    scaled = image.scaled(w,h,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    painter->drawImage(pos,scaled);
}
