#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wiringPi.h"
#include <QTimer>
#include <QtMqtt/QtMqtt>
#include <QMqttClient>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

public slots:
    void readButton();
    void readUart();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTimer *uartTimer;
    QMqttClient *client = nullptr;
    QMqttSubscription *subscription = nullptr;
};

#endif // MAINWINDOW_H
