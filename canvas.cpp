#include "canvas.h"

Canvas::Canvas()
{
    strokeSize = 5;
    init(100,100);
}

Canvas::Canvas(const QImage &i) {
    strokeSize = 5;
    setImage(i);
}

Canvas::~Canvas() {

}

void Canvas::init(int width, int height) {
    image = QImage(width,height, QImage::Format_RGBA8888);
    image.fill(Qt::transparent);
    numInteractions = 0;
}

bool Canvas::setImage(const QImage &im, bool matchSize) {
    if(matchSize) {
        if(im.height() != image.height() || im.width() != image.width()) {
            return false;
        }
    }
    image = im.convertToFormat(QImage::Format_RGBA8888);
    numInteractions = numInteractions == 1 ? 2 : 1;
    return true;
}

QImage Canvas::getImage() {
    return image;
}


void Canvas::mouseEvent(QMouseEvent *event, QWidget *parent) {
    // event->type() == QEvent::MouseMove or the like
    QPainter::CompositionMode q; // Composition mode
    if(event->buttons() & Qt::LeftButton) {
        q = QPainter::CompositionMode_SourceOver;
    } else if(event->buttons() & Qt::RightButton) {
        q = QPainter::CompositionMode_DestinationOut;
    }

    if(event->buttons() & (Qt::LeftButton | Qt::RightButton)) {
        // Determine mouse location on image:
        QRect pos = getDrawRect(parent);
        int x = (image.width()*(event->x() - pos.left()))/pos.width();
        int y = (image.height()*(event->y() - pos.top()))/pos.height();

        // Draw
        QPainter p(&image);
        p.setCompositionMode(q);
        p.setRenderHints( QPainter::HighQualityAntialiasing);
        p.setBrush(Qt::black);
        p.drawEllipse(QPoint(x,y), strokeSize, strokeSize);
        p.end();

        numInteractions++;

        parent->repaint();
    }
}
void Canvas::mouseWheelEvent(QWheelEvent *event, QWidget *parent) {
    // get delta
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    // modify size of stroke
    int newSize =  this->strokeSize  + numSteps;

    this->strokeSize = newSize < 1 ? 1 : newSize;

    updateCursor(parent);
}

void Canvas::updateCursor(QWidget *parent) {
    // Determine cursor size:
    QRect pos = getDrawRect(parent);
    int apparentstrokeSize = 1+(strokeSize * pos.height()) / image.height();


    // create cursor
    QPixmap cursor_img(64,64);
    cursor_img.fill(Qt::transparent);
    QPainter painter(&cursor_img);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QBrush brush(QColor(255,0,0,200));
    QPen pen(QColor(0,0,0,0),0);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(32,32),apparentstrokeSize,apparentstrokeSize);

    painter.end();

    // Set it
    QCursor cursor(cursor_img,32,32);
    parent->setCursor(cursor);
}

void Canvas::draw(QPainter *painter, QPaintEvent *event, QWidget *parent) {
    QRect pos = getDrawRect(parent);

    //QImage scaled = image.scaled(pos.width(),pos.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    painter->drawImage(pos,image);
}

QRect Canvas::getDrawRect(QWidget* parent) {
    int h = parent->height();
    int w = parent->width();

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
    return pos;
}
