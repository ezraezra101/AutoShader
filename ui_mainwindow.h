/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "glwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QPushButton *smooth;
    GLWidget *glwidget;
    QSlider *resolution;
    QPushButton *crossesMode;
    QPushButton *loadFromFile;
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_2;
    QFrame *line;
    QPushButton *loadImage;
    QPushButton *removeStroke;
    QPushButton *hatchingMode;
    QPushButton *smooth_covariant;
    QPushButton *save;
    QPushButton *normalMode;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(666, 497);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        smooth = new QPushButton(centralwidget);
        smooth->setObjectName(QStringLiteral("smooth"));
        smooth->setMaximumSize(QSize(68, 31));
        smooth->setFlat(true);

        gridLayout->addWidget(smooth, 1, 1, 1, 2);

        glwidget = new GLWidget(centralwidget);
        glwidget->setObjectName(QStringLiteral("glwidget"));
        glwidget->setCursor(QCursor(Qt::CrossCursor));
        glwidget->setMouseTracking(true);

        gridLayout->addWidget(glwidget, 0, 0, 9, 1);

        resolution = new QSlider(centralwidget);
        resolution->setObjectName(QStringLiteral("resolution"));
        resolution->setMinimumSize(QSize(0, 160));
        resolution->setMaximumSize(QSize(19, 160));
        resolution->setMinimum(2);
        resolution->setMaximum(40);
        resolution->setSliderPosition(20);
        resolution->setOrientation(Qt::Vertical);

        gridLayout->addWidget(resolution, 6, 2, 2, 1);

        crossesMode = new QPushButton(centralwidget);
        crossesMode->setObjectName(QStringLiteral("crossesMode"));
        crossesMode->setMaximumSize(QSize(31, 31));
        QPalette palette;
        QBrush brush(QColor(255, 254, 253, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        crossesMode->setPalette(palette);
        crossesMode->setFlat(true);

        gridLayout->addWidget(crossesMode, 8, 1, 1, 1);

        loadFromFile = new QPushButton(centralwidget);
        loadFromFile->setObjectName(QStringLiteral("loadFromFile"));
        loadFromFile->setMaximumSize(QSize(31, 31));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/icons/open_cross.png"), QSize(), QIcon::Normal, QIcon::Off);
        loadFromFile->setIcon(icon);
        loadFromFile->setFlat(true);

        gridLayout->addWidget(loadFromFile, 0, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 357, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 5, 1, 2, 1);

        verticalSpacer_2 = new QSpacerItem(20, 329, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 4, 2, 2, 1);

        line = new QFrame(centralwidget);
        line->setObjectName(QStringLiteral("line"));
        line->setMaximumSize(QSize(65, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 3, 1, 1, 2);

        loadImage = new QPushButton(centralwidget);
        loadImage->setObjectName(QStringLiteral("loadImage"));
        loadImage->setMaximumSize(QSize(31, 31));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/icons/open_sketch.png"), QSize(), QIcon::Normal, QIcon::Off);
        loadImage->setIcon(icon1);
        loadImage->setFlat(true);

        gridLayout->addWidget(loadImage, 0, 2, 1, 1);

        removeStroke = new QPushButton(centralwidget);
        removeStroke->setObjectName(QStringLiteral("removeStroke"));
        removeStroke->setMaximumSize(QSize(31, 31));
        removeStroke->setCheckable(true);
        removeStroke->setFlat(true);

        gridLayout->addWidget(removeStroke, 7, 1, 1, 1);

        hatchingMode = new QPushButton(centralwidget);
        hatchingMode->setObjectName(QStringLiteral("hatchingMode"));
        hatchingMode->setMaximumSize(QSize(31, 31));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush);
        hatchingMode->setPalette(palette1);
        hatchingMode->setFlat(true);

        gridLayout->addWidget(hatchingMode, 8, 2, 1, 1);

        smooth_covariant = new QPushButton(centralwidget);
        smooth_covariant->setObjectName(QStringLiteral("smooth_covariant"));
        smooth_covariant->setMaximumSize(QSize(68, 31));
        smooth_covariant->setFlat(true);

        gridLayout->addWidget(smooth_covariant, 2, 1, 1, 2);

        save = new QPushButton(centralwidget);
        save->setObjectName(QStringLiteral("save"));
        save->setMaximumSize(QSize(31, 31));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        save->setIcon(icon2);
        save->setFlat(true);

        gridLayout->addWidget(save, 4, 1, 1, 1);

        normalMode = new QPushButton(centralwidget);
        normalMode->setObjectName(QStringLiteral("normalMode"));
        normalMode->setEnabled(true);
        normalMode->setMaximumSize(QSize(31, 31));

        gridLayout->addWidget(normalMode, 9, 2, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(loadImage, loadFromFile);
        QWidget::setTabOrder(loadFromFile, smooth);
        QWidget::setTabOrder(smooth, smooth_covariant);
        QWidget::setTabOrder(smooth_covariant, save);
        QWidget::setTabOrder(save, resolution);
        QWidget::setTabOrder(resolution, crossesMode);
        QWidget::setTabOrder(crossesMode, removeStroke);
        QWidget::setTabOrder(removeStroke, hatchingMode);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "BendFields", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
#ifndef QT_NO_TOOLTIP
        smooth->setToolTip(QApplication::translate("MainWindow", "Compute harmonic cross field", nullptr));
#endif // QT_NO_TOOLTIP
        smooth->setText(QApplication::translate("MainWindow", "Harmonic", nullptr));
#ifndef QT_NO_TOOLTIP
        resolution->setToolTip(QApplication::translate("MainWindow", "Change scale", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        crossesMode->setToolTip(QApplication::translate("MainWindow", "Cross field view", nullptr));
#endif // QT_NO_TOOLTIP
        crossesMode->setText(QApplication::translate("MainWindow", "+", nullptr));
#ifndef QT_NO_TOOLTIP
        loadFromFile->setToolTip(QApplication::translate("MainWindow", "Load BendField", nullptr));
#endif // QT_NO_TOOLTIP
        loadFromFile->setText(QString());
#ifndef QT_NO_TOOLTIP
        loadImage->setToolTip(QApplication::translate("MainWindow", "Load Sketch", nullptr));
#endif // QT_NO_TOOLTIP
        loadImage->setText(QString());
        removeStroke->setText(QString());
#ifndef QT_NO_TOOLTIP
        hatchingMode->setToolTip(QApplication::translate("MainWindow", "Hatching view", nullptr));
#endif // QT_NO_TOOLTIP
        hatchingMode->setText(QApplication::translate("MainWindow", "#", nullptr));
#ifndef QT_NO_TOOLTIP
        smooth_covariant->setToolTip(QApplication::translate("MainWindow", "Compute BendField", nullptr));
#endif // QT_NO_TOOLTIP
        smooth_covariant->setText(QApplication::translate("MainWindow", "BendField", nullptr));
#ifndef QT_NO_TOOLTIP
        save->setToolTip(QApplication::translate("MainWindow", "Export BendField", nullptr));
#endif // QT_NO_TOOLTIP
        save->setText(QString());
        normalMode->setText(QApplication::translate("MainWindow", "N", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
