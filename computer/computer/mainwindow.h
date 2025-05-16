#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "plotwidget.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>


QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_baudRateComboBox_currentTextChanged(const QString &arg1);
    void on_serialbox_currentTextChanged(const QString &arg1);
    void on_connectButton_clicked();
    void on_refreshButton_clicked();
    void onSerialReceived(); 

    void on_algorithmComboBox_currentTextChanged(const QString &arg1);

    void on_subdivComboBox_currentTextChanged(const QString &arg1);

    void on_interpTypeComboBox_currentTextChanged(const QString &arg1);

    void on_quadrantComboBox_currentTextChanged(const QString &arg1);

    void on_x_s_textChanged(const QString &arg1);

    void on_y_s_textChanged(const QString &arg1);

    void on_x_f_textChanged(const QString &arg1);

    void on_y_f_textChanged(const QString &arg1);

    void on_run_clicked();



    void on_draw_clicked();

    void on_clear_clicked();

private:
    void refreshSerialPorts();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    bool isConnected = false;
    int x_start = 0;
    int y_start = 0;
    int x_end = 0;
    int y_end = 0;
    PlotWidget *plot;

};

#endif // MAINWINDOW_H
