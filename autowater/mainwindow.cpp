#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <wiringPi.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <wiringSerial.h>
#include <QDebug>
#include <QtMqtt/QtMqtt>
#include <string.h>
#define PUMP_PIN 5
#define LAMP_PIN 6
#define PUMP_BUTTON 19
#define LAMP_BUTTON 13

// button bom: Xanh
// button den: Do

int pumpStatus = 0;
int lampStatus = 0;
int buttonState;           // the current reading from the input pin
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;
int limit = 30;
static int fd;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    fd = serialOpen ("/dev/ttyUSB0", 9600);
    ui->setupUi(this);
    serialFlush(fd);
    wiringPiSetupGpio();
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(LAMP_PIN, OUTPUT);
    pinMode(PUMP_BUTTON, INPUT);
    pinMode(LAMP_BUTTON, INPUT);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readButton()));
    timer->start(1000);

    uartTimer = new QTimer(this);
    connect(uartTimer, SIGNAL(timeout()), this, SLOT(readUart()));
    uartTimer->start(1000);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void control() {
  if (pumpStatus == 1) {
    digitalWrite(PUMP_PIN, HIGH);
  } else {
    digitalWrite(PUMP_PIN, LOW);
  }
  if (lampStatus == 1) {
    digitalWrite(LAMP_PIN, HIGH);
  } else {
    digitalWrite(LAMP_PIN, LOW);
  }
}


bool debounce(int pin) {
  bool state;
  bool previousState;
  const int debounceDelay = 30;

  previousState = digitalRead(pin);
  for (int counter = 0; counter < debounceDelay; counter++)
  {
    delay(1);
    state = digitalRead(pin);
    if (state != previousState)
    {
      counter = 0;
      previousState = state;
    }

  }

  return state;
}



void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    pumpStatus = !pumpStatus;
    control();
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    lampStatus = !lampStatus;
    control();
}

void MainWindow::readButton()
{
    bool buttonStatus = debounce(PUMP_BUTTON);
     if (buttonStatus == 1) {
      pumpStatus = !pumpStatus;
      control();
      if(pumpStatus) {
          ui -> checkBox_2 -> setChecked(true);
      } else {
          ui -> checkBox_2 -> setChecked(false);
      }
    }
    buttonStatus = debounce(LAMP_BUTTON);
    if (buttonStatus == 1) {
      lampStatus = !lampStatus;
      control();
      if(lampStatus) {
          ui -> checkBox -> setChecked(true);
      } else {
          ui -> checkBox -> setChecked(false);
      }
    }
}
static char receive_char;
static QString receive_string;
void MainWindow::readUart(){
    while(serialDataAvail(fd))   {
        receive_char = serialGetchar(fd);
        if(receive_char != '\r' && receive_char != '\n' && receive_char != ' ')
        {
//              while(serialDataAvail(fd)) serialGetchar(fd);
            receive_string+= receive_char;
        }
        fflush (stdout);
    }
    if(receive_string.length() > 0){
        qDebug() << receive_string;

        if (receive_string == "PumpON") {
           digitalWrite(PUMP_PIN, HIGH);
           ui -> checkBox_2 -> setChecked(true);
        } else if(receive_string == "PumpOff") {
            digitalWrite(PUMP_PIN, LOW);
            ui -> checkBox_2 -> setChecked(false);
        } else if (receive_string == "LampON") {
             digitalWrite(LAMP_PIN, HIGH);
             ui -> checkBox -> setChecked(true);
        } else if (receive_string == "LampOff") {
            digitalWrite(LAMP_PIN, LOW);
            ui -> checkBox -> setChecked(false);
        } else {
            qDebug() << receive_string;
        }
        receive_string = "";
     }

    receive_string = "";
}

