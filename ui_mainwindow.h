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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "glwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    GLWidget *glwidget;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label;
    QPushButton *draw_curvature;
    QPushButton *draw_mask;
    QPushButton *draw_convexity;
    QPushButton *draw_constraints;
    QPushButton *load_constraints;
    QPushButton *load_curvature;
    QPushButton *load_mask;
    QPushButton *load_convexity;
    QRadioButton *show_crossfields;
    QRadioButton *show_normals;
    QRadioButton *show_shading;
    QPushButton *export_shading;
    QSpacerItem *verticalSpacer;
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
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        glwidget = new GLWidget(centralwidget);
        glwidget->setObjectName(QStringLiteral("glwidget"));
        glwidget->setCursor(QCursor(Qt::CrossCursor));
        glwidget->setMouseTracking(true);

        horizontalLayout->addWidget(glwidget);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 3, 2, 1, 1);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 0, 2, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 2, 2, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 1, 2, 1, 1);

        draw_curvature = new QPushButton(centralwidget);
        draw_curvature->setObjectName(QStringLiteral("draw_curvature"));
        draw_curvature->setMaximumSize(QSize(31, 31));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/icons/brush.png"), QSize(), QIcon::Normal, QIcon::Off);
        draw_curvature->setIcon(icon);
        draw_curvature->setFlat(true);

        gridLayout_2->addWidget(draw_curvature, 1, 0, 1, 1);

        draw_mask = new QPushButton(centralwidget);
        draw_mask->setObjectName(QStringLiteral("draw_mask"));
        draw_mask->setMaximumSize(QSize(31, 31));
        draw_mask->setIcon(icon);
        draw_mask->setFlat(true);

        gridLayout_2->addWidget(draw_mask, 2, 0, 1, 1);

        draw_convexity = new QPushButton(centralwidget);
        draw_convexity->setObjectName(QStringLiteral("draw_convexity"));
        draw_convexity->setMaximumSize(QSize(31, 31));
        draw_convexity->setIcon(icon);
        draw_convexity->setFlat(true);

        gridLayout_2->addWidget(draw_convexity, 3, 0, 1, 1);

        draw_constraints = new QPushButton(centralwidget);
        draw_constraints->setObjectName(QStringLiteral("draw_constraints"));
        draw_constraints->setMaximumSize(QSize(31, 31));
        draw_constraints->setIcon(icon);
        draw_constraints->setFlat(true);

        gridLayout_2->addWidget(draw_constraints, 0, 0, 1, 1);

        load_constraints = new QPushButton(centralwidget);
        load_constraints->setObjectName(QStringLiteral("load_constraints"));
        load_constraints->setMaximumSize(QSize(31, 31));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/icons/image.png"), QSize(), QIcon::Normal, QIcon::Off);
        load_constraints->setIcon(icon1);
        load_constraints->setFlat(true);

        gridLayout_2->addWidget(load_constraints, 0, 1, 1, 1);

        load_curvature = new QPushButton(centralwidget);
        load_curvature->setObjectName(QStringLiteral("load_curvature"));
        load_curvature->setMaximumSize(QSize(31, 31));
        load_curvature->setIcon(icon1);
        load_curvature->setFlat(true);

        gridLayout_2->addWidget(load_curvature, 1, 1, 1, 1);

        load_mask = new QPushButton(centralwidget);
        load_mask->setObjectName(QStringLiteral("load_mask"));
        load_mask->setMaximumSize(QSize(31, 31));
        load_mask->setIcon(icon1);
        load_mask->setFlat(true);

        gridLayout_2->addWidget(load_mask, 2, 1, 1, 1);

        load_convexity = new QPushButton(centralwidget);
        load_convexity->setObjectName(QStringLiteral("load_convexity"));
        load_convexity->setMaximumSize(QSize(31, 31));
        load_convexity->setIcon(icon1);
        load_convexity->setFlat(true);

        gridLayout_2->addWidget(load_convexity, 3, 1, 1, 1);


        verticalLayout_2->addLayout(gridLayout_2);

        show_crossfields = new QRadioButton(centralwidget);
        show_crossfields->setObjectName(QStringLiteral("show_crossfields"));

        verticalLayout_2->addWidget(show_crossfields);

        show_normals = new QRadioButton(centralwidget);
        show_normals->setObjectName(QStringLiteral("show_normals"));

        verticalLayout_2->addWidget(show_normals);

        show_shading = new QRadioButton(centralwidget);
        show_shading->setObjectName(QStringLiteral("show_shading"));
        show_shading->setChecked(true);

        verticalLayout_2->addWidget(show_shading);

        export_shading = new QPushButton(centralwidget);
        export_shading->setObjectName(QStringLiteral("export_shading"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(export_shading->sizePolicy().hasHeightForWidth());
        export_shading->setSizePolicy(sizePolicy);

        verticalLayout_2->addWidget(export_shading);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout_2);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "BendFields", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Convexity guidance", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "Drawing", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Mask", nullptr));
        label->setText(QApplication::translate("MainWindow", "Curvature", nullptr));
#ifndef QT_NO_TOOLTIP
        draw_curvature->setToolTip(QApplication::translate("MainWindow", "Draw", nullptr));
#endif // QT_NO_TOOLTIP
        draw_curvature->setText(QString());
#ifndef QT_NO_TOOLTIP
        draw_mask->setToolTip(QApplication::translate("MainWindow", "Draw", nullptr));
#endif // QT_NO_TOOLTIP
        draw_mask->setText(QString());
#ifndef QT_NO_TOOLTIP
        draw_convexity->setToolTip(QApplication::translate("MainWindow", "Draw", nullptr));
#endif // QT_NO_TOOLTIP
        draw_convexity->setText(QString());
#ifndef QT_NO_TOOLTIP
        draw_constraints->setToolTip(QApplication::translate("MainWindow", "Draw", nullptr));
#endif // QT_NO_TOOLTIP
        draw_constraints->setText(QString());
#ifndef QT_NO_TOOLTIP
        load_constraints->setToolTip(QApplication::translate("MainWindow", "Load BendField", nullptr));
#endif // QT_NO_TOOLTIP
        load_constraints->setText(QString());
#ifndef QT_NO_TOOLTIP
        load_curvature->setToolTip(QApplication::translate("MainWindow", "Load BendField", nullptr));
#endif // QT_NO_TOOLTIP
        load_curvature->setText(QString());
#ifndef QT_NO_TOOLTIP
        load_mask->setToolTip(QApplication::translate("MainWindow", "Load BendField", nullptr));
#endif // QT_NO_TOOLTIP
        load_mask->setText(QString());
#ifndef QT_NO_TOOLTIP
        load_convexity->setToolTip(QApplication::translate("MainWindow", "Load BendField", nullptr));
#endif // QT_NO_TOOLTIP
        load_convexity->setText(QString());
        show_crossfields->setText(QApplication::translate("MainWindow", "Show crossfield", nullptr));
        show_normals->setText(QApplication::translate("MainWindow", "Show normals", nullptr));
        show_shading->setText(QApplication::translate("MainWindow", "Show shading", nullptr));
        export_shading->setText(QApplication::translate("MainWindow", "Restart Normal Generation", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
