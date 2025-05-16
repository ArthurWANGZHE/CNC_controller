/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QComboBox *baudRateComboBox;
    QLabel *label;
    QComboBox *serialbox;
    QPushButton *connectButton;
    QPushButton *refreshButton;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QComboBox *algorithmComboBox;
    QComboBox *interpTypeComboBox;
    QComboBox *quadrantComboBox;
    QLabel *label_6;
    QComboBox *subdivComboBox;
    QLabel *label_7;
    QLabel *label_8;
    QSpinBox *x_s;
    QSpinBox *y_s;
    QSpinBox *x_f;
    QSpinBox *y_f;
    QPushButton *run;
    QPlainTextEdit *commandoutputBox;
    QSpinBox *speedbox;
    QLabel *label_9;
    QPushButton *draw;
    QPushButton *clear;
    QLabel *label_11;
    QSpinBox *x_c;
    QSpinBox *y_c;
    QPlainTextEdit *commandoutput;
    QChartView *chartView;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        baudRateComboBox = new QComboBox(centralwidget);
        baudRateComboBox->setObjectName("baudRateComboBox");
        baudRateComboBox->setGeometry(QRect(160, 70, 72, 24));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(40, 80, 54, 16));
        serialbox = new QComboBox(centralwidget);
        serialbox->setObjectName("serialbox");
        serialbox->setGeometry(QRect(160, 40, 72, 24));
        connectButton = new QPushButton(centralwidget);
        connectButton->setObjectName("connectButton");
        connectButton->setGeometry(QRect(110, 110, 80, 24));
        refreshButton = new QPushButton(centralwidget);
        refreshButton->setObjectName("refreshButton");
        refreshButton->setGeometry(QRect(220, 110, 80, 24));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(40, 40, 54, 16));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(40, 200, 54, 16));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(40, 250, 54, 16));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(40, 300, 54, 16));
        algorithmComboBox = new QComboBox(centralwidget);
        algorithmComboBox->setObjectName("algorithmComboBox");
        algorithmComboBox->setGeometry(QRect(160, 200, 72, 24));
        interpTypeComboBox = new QComboBox(centralwidget);
        interpTypeComboBox->setObjectName("interpTypeComboBox");
        interpTypeComboBox->setGeometry(QRect(160, 250, 72, 24));
        quadrantComboBox = new QComboBox(centralwidget);
        quadrantComboBox->setObjectName("quadrantComboBox");
        quadrantComboBox->setGeometry(QRect(160, 300, 72, 24));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(40, 150, 54, 16));
        subdivComboBox = new QComboBox(centralwidget);
        subdivComboBox->setObjectName("subdivComboBox");
        subdivComboBox->setGeometry(QRect(160, 150, 72, 24));
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(40, 400, 54, 16));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(40, 450, 54, 16));
        x_s = new QSpinBox(centralwidget);
        x_s->setObjectName("x_s");
        x_s->setGeometry(QRect(150, 400, 61, 24));
        y_s = new QSpinBox(centralwidget);
        y_s->setObjectName("y_s");
        y_s->setGeometry(QRect(240, 400, 61, 24));
        x_f = new QSpinBox(centralwidget);
        x_f->setObjectName("x_f");
        x_f->setGeometry(QRect(150, 450, 61, 24));
        y_f = new QSpinBox(centralwidget);
        y_f->setObjectName("y_f");
        y_f->setGeometry(QRect(240, 450, 61, 24));
        run = new QPushButton(centralwidget);
        run->setObjectName("run");
        run->setGeometry(QRect(360, 440, 80, 24));
        commandoutputBox = new QPlainTextEdit(centralwidget);
        commandoutputBox->setObjectName("commandoutputBox");
        commandoutputBox->setGeometry(QRect(320, 20, 141, 171));
        speedbox = new QSpinBox(centralwidget);
        speedbox->setObjectName("speedbox");
        speedbox->setGeometry(QRect(160, 350, 71, 24));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(40, 350, 54, 16));
        draw = new QPushButton(centralwidget);
        draw->setObjectName("draw");
        draw->setGeometry(QRect(500, 430, 80, 24));
        clear = new QPushButton(centralwidget);
        clear->setObjectName("clear");
        clear->setGeometry(QRect(660, 430, 80, 24));
        label_11 = new QLabel(centralwidget);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(40, 500, 54, 16));
        x_c = new QSpinBox(centralwidget);
        x_c->setObjectName("x_c");
        x_c->setGeometry(QRect(150, 500, 61, 24));
        y_c = new QSpinBox(centralwidget);
        y_c->setObjectName("y_c");
        y_c->setGeometry(QRect(240, 500, 61, 24));
        commandoutput = new QPlainTextEdit(centralwidget);
        commandoutput->setObjectName("commandoutput");
        commandoutput->setGeometry(QRect(320, 210, 141, 171));
        chartView = new QChartView(centralwidget);
        chartView->setObjectName("chartView");
        chartView->setGeometry(QRect(500, 110, 256, 192));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245", nullptr));
        refreshButton->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\351\200\211\346\213\251", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\347\256\227\346\263\225\347\261\273\345\236\213", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\346\217\222\350\241\245\346\250\241\345\274\217", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\350\261\241\351\231\220", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\347\273\206\345\210\206", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\350\265\267\347\202\271\345\235\220\346\240\207", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\347\273\210\347\202\271\345\235\220\346\240\207", nullptr));
        run->setText(QCoreApplication::translate("MainWindow", "\346\211\247\350\241\214", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246", nullptr));
        draw->setText(QCoreApplication::translate("MainWindow", "\347\273\230\345\210\266", nullptr));
        clear->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\345\234\206\345\277\203\345\235\220\346\240\207", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
