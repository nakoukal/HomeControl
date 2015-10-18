#include "homecontrol.h"
#include "ui_homecontrol.h"



HomeControl::HomeControl(QWidget *parent) : QMainWindow(parent), ui(new Ui::HomeControl)
{
    ui->setupUi(this);
    _buttonSetting = new QList<ButtonInfo>();
    _style_gray = QString("background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #FFF, stop: 1 #888);");
    _style_red = QString("background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #FF0000, stop: 1 #630000);");

    //set the writable path for ini
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) ;
    _iniFileName = path + "/seting.ini";

    // create a timer
    timer = new QTimer(this);

    // setup signal and slot
    connect(timer, SIGNAL(timeout()),this, SLOT(MyTimerSlot()));

    connect(ui->Btn1NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton1TextSlot(QString)));
    connect(ui->Btn2NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton2TextSlot(QString)));
    connect(ui->Btn3NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton3TextSlot(QString)));
    connect(ui->Btn4NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton4TextSlot(QString)));
    connect(ui->Btn5NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton5TextSlot(QString)));
    connect(ui->Btn6NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton6TextSlot(QString)));

    if(QFile::exists(_iniFileName))
        loadSettings();


    this->loadBitState();

    // start timer with refreshing
    timer->start(60000);

    QString info = "Product: "+QSysInfo::prettyProductName();
    info += "\n";
    info += "CPU Arch.: "+QSysInfo::buildCpuArchitecture();
    info += "\n";
    info += "Kernel Type: "+QSysInfo::kernelType();
    info += "\n";
    info += "Kernel Ver: "+QSysInfo::kernelVersion();

    ui->infoLabel->setText(info);


}

HomeControl::~HomeControl()
{
    delete ui;
}

/**
 * @brief MainWindow::MyTimerSlot
 * Timer slot to call funtions
 */
void HomeControl::MyTimerSlot()
{
    //this->loadTemperatures();
    this->loadBitState();
}

void HomeControl::loadBitState()
{
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=readall");
    down->doDownloadGpioState(_url);
    connect(down, SIGNAL(gpioStateChanged(QJsonObject)), this, SLOT(initButtonSlot(QJsonObject)));
}

void HomeControl::initButtonSlot(QJsonObject jsonObject)
{
    QJsonArray jsonArray = jsonObject["state"].toArray();
    foreach (const QJsonValue & value, jsonArray)
    {
        QJsonObject obj = value.toObject();
        int val = obj["val"].toInt();
        int bit = obj["bit"].toInt();
        this->setButtonValue(bit,val);
    }
    ButtonInfo info = this->findButton("button1");
    this->SetButtonState(ui->pushButton1,info.parameter.value);
    this->loadTemperatures();
}

void HomeControl::loadTemperatures()
{
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/actualTemp.php");
    down->doDownloadTemperature(_url);
    connect(down, SIGNAL(tempChanged(QJsonObject)), this, SLOT(InsertItemsSlot(QJsonObject)));
}

/**
 * @brief MainWindow::InserItemsSlot
 * @param Lists
 * Slot to inserts lines to combobox
 */
void HomeControl::InsertItemsSlot(QJsonObject jsonObject)
{
    ui->groupBoxTemp->size();
    QLayoutItem* item;
        while ( ( item = ui->groupBoxTemp->layout()->takeAt( 0 ) ) != NULL )
        {
            delete item->widget();
            delete item;
        }
    QJsonArray jsonArray = jsonObject["temp"].toArray();
    foreach (const QJsonValue & value, jsonArray)
    {
        QJsonObject obj = value.toObject();
        QString name = obj["name"].toString();
        QString act_value = obj["act"].toString();
        QString req_value = obj["req"].toString();
        QLabel *lab_name  = new QLabel(name);
        QLabel *lab_act  = new QLabel(act_value);
        QLabel *lab_req  = new QLabel(req_value);

        ui->groupBoxTemp->layout()->addWidget(lab_name);
        ui->groupBoxTemp->layout()->addWidget(lab_act);
        ui->groupBoxTemp->layout()->addWidget(lab_req);
    }

    this->loadGraph_A();
}

/**
 * @brief MainWindow::ntwRequest
 * Function get data from url if data is downloade connect to slot
 */
void HomeControl::loadGraph_A()
{
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/avg_temp_graph.php?type=day&nadpis=Teploty_za_24hod.");
    down->doDownloadGraph(_url);
    connect(down, SIGNAL(pixmapChanged(QPixmap)), this , SLOT(SetPixmapslot(QPixmap)));
}

void HomeControl::SetPixmapslot(QPixmap pixmap)
{
    ui->graphDayLabel->setPixmap(pixmap);
    this->loadHomeTempImage();
}


void HomeControl::loadHomeTempImage()
{    
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/home.php");
    down->doDownloadGraph(_url);
    connect(down, SIGNAL(pixmapChanged(QPixmap)), ui->graphHomeLabel, SLOT(setPixmap(QPixmap)));
}


void HomeControl::SetButton1TextSlot(QString text)
{
    ui->pushButton1->setText(text);
}

void HomeControl::SetButton2TextSlot(QString text)
{
    ui->pushButton2->setText(text);
}

void HomeControl::SetButton3TextSlot(QString text)
{
    ui->pushButton3->setText(text);
}

void HomeControl::SetButton4TextSlot(QString text)
{
    ui->pushButton4->setText(text);
}

void HomeControl::SetButton5TextSlot(QString text)
{
    ui->pushButton5->setText(text);
}

void HomeControl::SetButton6TextSlot(QString text)
{
    ui->pushButton6->setText(text);
}

void HomeControl::loadSettings()
{
    ButtonInfo info;//new struct
    QString btnName;
    QString btnText;
    int btnBit;

    QSettings _settings(_iniFileName,QSettings::IniFormat);
    _settings.beginGroup("GEN");

    ////////////////////////////////////////////////////////
    btnName = "button1";
    btnText = _settings.value("button1").toString();
    btnBit = _settings.value("btn1Gpio").toInt();

    info.objectName = btnName;
    info.parameter.bit = btnBit;
    info.parameter.text = btnText;
    _buttonSetting->append(info); // add new list item

    ui->Btn1NameLineEdit->setText(btnText);
    ui->Btn1GpioLineEdit->setText(QString::number(btnBit));


    ////////////////////////////////////////////////////////
    btnName = "button2";
    btnText = _settings.value("button2").toString();
    btnBit = _settings.value("btn2Gpio").toInt();

    info.objectName = btnName;
    info.parameter.bit = btnBit;
    info.parameter.text = btnText;
    _buttonSetting->append(info); // add new list item

    ui->Btn2NameLineEdit->setText(btnText);
    ui->Btn2GpioLineEdit->setText(QString::number(btnBit));


    ////////////////////////////////////////////////////////
    btnName = "button3";
    btnText = _settings.value("button3").toString();
    btnBit = _settings.value("btn3Gpio").toInt();

    info.objectName = btnName;
    info.parameter.bit = btnBit;
    info.parameter.text = btnText;
    _buttonSetting->append(info); // add new list item

    ui->Btn3NameLineEdit->setText(btnText);
    ui->Btn3GpioLineEdit->setText(QString::number(btnBit));


    ////////////////////////////////////////////////////////
    btnName = "button4";
    btnText = _settings.value("button4").toString();
    btnBit = _settings.value("btn4Gpio").toInt();

    info.objectName = btnName;
    info.parameter.bit = btnBit;
    info.parameter.text = btnText;
    _buttonSetting->append(info); // add new list item

    ui->Btn4NameLineEdit->setText(btnText);
    ui->Btn4GpioLineEdit->setText(QString::number(btnBit));


    ////////////////////////////////////////////////////////
    btnName = "button5";
    btnText = _settings.value("button5").toString();
    btnBit = _settings.value("btn5Gpio").toInt();

    info.objectName = btnName;
    info.parameter.bit = btnBit;
    info.parameter.text = btnText;
    _buttonSetting->append(info); // add new list item

    ui->Btn5NameLineEdit->setText(btnText);
    ui->Btn5GpioLineEdit->setText(QString::number(btnBit));


    ////////////////////////////////////////////////////////
    btnName = "button6";
    btnText = _settings.value("button6").toString();
    btnBit = _settings.value("btn6Gpio").toInt();

    info.objectName = btnName;
    info.parameter.bit = btnBit;
    info.parameter.text = btnText;
    _buttonSetting->append(info); // add new list item

    ui->Btn6NameLineEdit->setText(btnText);
    ui->Btn6GpioLineEdit->setText(QString::number(btnBit));




    _settings.endGroup();

    _settings.beginGroup("DEV");
    _deviceName = _settings.value("deviceName").toString();

    ui->DevNameLineEdit->setText(_deviceName);
    _settings.endGroup();

    _settings.beginGroup("SRV");
    _hostName = _settings.value("hostName").toString();
    _port = _settings.value("port").toString();
    QString user = _settings.value("userName").toString();
    QString password = _settings.value("password").toString();
    _url.setUserName(user);
    _url.setPassword(password);

    ui->HostLineEdit->setText(_hostName);
    ui->PortLineEdit->setText(_port);
    ui->UserLineEdit->setText(user);
    ui->PasswordLineEdit->setText(password);

    _settings.endGroup();
}

void HomeControl::saveSettings()
{
    QSettings _settings(_iniFileName,QSettings::IniFormat);
    _settings.beginGroup("GEN");
    _settings.setValue("button1", ui->Btn1NameLineEdit->text());
    _settings.setValue("button2", ui->Btn2NameLineEdit->text());
    _settings.setValue("button3", ui->Btn3NameLineEdit->text());    
    _settings.setValue("button4", ui->Btn4NameLineEdit->text());
    _settings.setValue("button5", ui->Btn5NameLineEdit->text());
    _settings.setValue("button6", ui->Btn6NameLineEdit->text());
    _settings.setValue("btn1Gpio", ui->Btn1GpioLineEdit->text());
    _settings.setValue("btn2Gpio", ui->Btn2GpioLineEdit->text());
    _settings.setValue("btn3Gpio", ui->Btn3GpioLineEdit->text());
    _settings.setValue("btn4Gpio", ui->Btn4GpioLineEdit->text());
    _settings.setValue("btn5Gpio", ui->Btn5GpioLineEdit->text());
    _settings.setValue("btn6Gpio", ui->Btn6GpioLineEdit->text());

    _settings.endGroup();

    _settings.beginGroup("DEV");
    _settings.setValue("deviceName", ui->DevNameLineEdit->text());
    _settings.endGroup();

    _settings.beginGroup("SRV");
    _settings.setValue("hostName", ui->HostLineEdit->text());
    _settings.setValue("port", ui->PortLineEdit->text());
    _settings.setValue("userName", ui->UserLineEdit->text());
    _settings.setValue("password", ui->PasswordLineEdit->text());
    _settings.endGroup();

    if(_settings.status() == QSettings::NoError)
    {
        QMessageBox::information(this,"Setting", "Setting was saved !");
    }
    else
    {
        QMessageBox::information(this,"Setting", "Error in saving setting!");
    }


}

void HomeControl::on_pushButtonSave_released()
{
    saveSettings();
}

void HomeControl::on_pushButton1_released()
{    
    ui->pushButton1->setStyleSheet(_style_red);
    ButtonInfo info = this->findButton("button1");
    int bit = info.parameter.bit;
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=writevalue&bit="+QString::number(bit));
    down->doWriteBit(_url);
    connect(down, SIGNAL(bitwrited(QString)), ui->pushButton1, SLOT(setStyleSheet(QString)));
}

void HomeControl::on_pushButton2_released()
{
     ui->pushButton2->setStyleSheet(_style_red);
     ButtonInfo info = this->findButton("button2");
     int bit = info.parameter.bit;
     Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=opengate&bit="+QString::number(bit));//18
    down->doWriteBit(_url);
    connect(down, SIGNAL(bitwrited(QString)), ui->pushButton2, SLOT(setStyleSheet(QString)));
}

void HomeControl::on_pushButton3_released()
{    
    ui->pushButton3->setStyleSheet(_style_red);
    ButtonInfo info = this->findButton("button3");
    int bit = info.parameter.bit;
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=opengate&bit="+QString::number(bit));//17
    down->doWriteBit(_url);
    connect(down, SIGNAL(bitwrited(QString)), ui->pushButton3, SLOT(setStyleSheet(QString)));
}
/**
 * Function find relevant item in list and sets valus
 * @brief HomeControl::setButtonValue
 * @param bit
 * @param value
 * @return bool
 */
bool HomeControl::setButtonValue(int bit, int value)
{
    QList<ButtonInfo>::iterator i;
    for (i = _buttonSetting->begin(); i != _buttonSetting->end(); ++i)
    {
        if(i->parameter.bit == bit)
        {
            i->parameter.value = value;
            return true;
        }

    }
    return false;
}


ButtonInfo HomeControl::findButton(QString name)
{
    ButtonInfo info;
    QList<ButtonInfo>::iterator i;
    for (i = _buttonSetting->begin(); i != _buttonSetting->end(); ++i)
    {
        if(i->objectName == name)
        {
            return *i;
        }

    }
    return info;
}

void HomeControl::SetButtonState(QPushButton *btn, int val)
{
    if(val)
    {
         btn->setStyleSheet(_style_red);
    }
    else
    {
         btn->setStyleSheet(_style_gray);
    }

}
