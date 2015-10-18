#ifndef HOMECONTROL_H
#define HOMECONTROL_H

#include <QMainWindow>
#include <QDebug>
#include <QJsonArray>
#include <QMessageBox>
#include <QTimer>
#include <QSysInfo>
#include <QSettings>
#include <QFile>
#include <QStandardPaths>
#include <QList>
#include <QListIterator>
#include <QStandardItemModel>
#include "downloader.h"

namespace Ui {
class HomeControl;
}
struct ButtonInfo
{
  QString objectName;
  struct ParameterStruct
  {
      QString text;
      int bit;
      int value;
  } parameter;

};

class HomeControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeControl(QWidget *parent = 0);
    ~HomeControl();
    QTimer *timer;
private:
    Ui::HomeControl *ui;
    QUrl _url;
    QString _button1Name,_button2Name,_button3Name,_iniFileName,_deviceName,_hostName,_port;
    QString _style_gray,_style_red,_button1Bit,_button2Bit,_button3Bit;
    int _refreshtime;
    QList<ButtonInfo> *_buttonSetting;
    ButtonInfo findButton(QString name);
    QStandardItemModel *model;
    void loadTemperatures();
    void loadEvents();
    void loadBitState();
    void loadGraph_A();
    void loadHomeTempImage();
    void loadSettings();
    void saveSettings();
    void showDeviceInfo();
    void SetButtonState(QPushButton *btn, int val);

    bool setButtonValue(int bit,int value);

public slots:
    void loadTempSlot(QJsonObject jsonObject);
    void loadEventsSlot(QJsonObject jsonObject);
    void initButtonSlot(QJsonObject jsonObject);
    void SetPixmapslot(QPixmap pixmap);
    void MyTimerSlot();

private slots:    
    void SetButton1TextSlot(QString text);
    void SetButton2TextSlot(QString text);
    void SetButton3TextSlot(QString text);
    void SetButton4TextSlot(QString text);
    void SetButton5TextSlot(QString text);
    void SetButton6TextSlot(QString text);
    void on_pushButtonSave_released();
    void on_pushButton1_released();
    void on_pushButton2_released();
    void on_pushButton3_released();
};

#endif // HOMECONTROL_H
