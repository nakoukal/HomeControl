#include "homecontrol.h"
#include "ui_homecontrol.h"


/**
 * @brief HomeControl::HomeControl
 * @param parent
 * Home control constructor set up all variables and set all connections to slots
 */
HomeControl::HomeControl(QWidget *parent) : QMainWindow(parent), ui(new Ui::HomeControl)
{
    ui->setupUi(this);    
    //list of button status and names from db
    _buttonSetting = new QList<ButtonInfo>();

    //sett color of button in pressing
    _style_gray = QString("background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #FFF, stop: 1 #888);");
    _style_red = QString("background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #FF0000, stop: 1 #630000);");

    //set the writable save path for ini
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) ;
    _iniFileName = path + "/seting.ini";

    // create a timer object
    timer = new QTimer(this);

    // setup signal and slot for timer
    connect(timer, SIGNAL(timeout()),this, SLOT(MyTimerSlot()));

    //set signel and slot for button name
    connect(ui->Btn1NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton1TextSlot(QString)));
    connect(ui->Btn2NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton2TextSlot(QString)));
    connect(ui->Btn3NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton3TextSlot(QString)));
    connect(ui->Btn4NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton4TextSlot(QString)));
    connect(ui->Btn5NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton5TextSlot(QString)));
    connect(ui->Btn6NameLineEdit, SIGNAL(textChanged(QString)),this,SLOT(SetButton6TextSlot(QString)));

    //check if ini file exist and load settings
    if(QFile::exists(_iniFileName))
        loadSettings();

    //start load button info
    this->loadBitState();

    //start load temperature from db
    this->loadTemperatures();

    //start load graph with temperature
    this->loadGraph_A();

    //start load img with home temperature
    this->loadHomeTempImage();

    //show device info
    //this->showDeviceInfo();

    //show event info table
    this->loadEvents();

    // start timer with refreshing
    timer->start(10000);

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
    //start load button info
    this->loadBitState();

    //start load temperature from db
    this->loadTemperatures();

    //start load graph with temperature
    this->loadGraph_A();

    //start load img with home temperature
    this->loadHomeTempImage();

    //show event info table
    this->loadEvents();
}

/**
 * @brief HomeControl::loadBitState
 * Function load from url json gpio state and after finished call slot
 */
void HomeControl::loadBitState()
{
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=readall");
    down->doDownloadGpioState(_url);
    connect(down, SIGNAL(gpioStateChanged(QJsonObject)), this, SLOT(initButtonSlot(QJsonObject)));
}

/**
 * @brief HomeControl::initButtonSlot
 * @param jsonObject
 * Fuction of the gpio pins and add to list
 */
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
    ButtonInfo info;

    info = this->findButton("button1");
    this->SetButtonState(ui->pushButton1,info.parameter.value);

    info = this->findButton("button2");
    this->SetButtonState(ui->pushButton2,info.parameter.value);

    info = this->findButton("button3");
    this->SetButtonState(ui->pushButton3,info.parameter.value);

    info = this->findButton("button4");
    this->SetButtonState(ui->pushButton4,info.parameter.value);

    info = this->findButton("button5");
    this->SetButtonState(ui->pushButton5,info.parameter.value);

    info = this->findButton("button6");
    this->SetButtonState(ui->pushButton6,info.parameter.value);

}

void HomeControl::loadTemperatures()
{
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/actualTemp2.php");
    down->doDownloadTemperature(_url);
    connect(down, SIGNAL(tempChanged(QJsonObject)), this, SLOT(loadTempSlot(QJsonObject)));
}

/**
 * @brief MainWindow::InserItemsSlot
 * @param Lists
 * Slot to inserts lines to combobox
 */
void HomeControl::loadTempSlot(QJsonObject jsonObject)
{

    QJsonArray jsonArray = jsonObject["temp"].toArray();
    int rows = jsonArray.count();
    int row = 0;
    QStandardItemModel *model = new QStandardItemModel(rows,3,this); //2 Rows and 3 Columns
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("name")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("act")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("req")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("sens")));

    foreach (const QJsonValue & value, jsonArray)
    {

        QJsonObject obj = value.toObject();

        QStandardItem* itemName = new QStandardItem(obj["name"].toString());
        QStandardItem* itemAct = new QStandardItem(obj["act"].toString());
        QStandardItem* itemReq = new QStandardItem(obj["req"].toString());
        QStandardItem* itemSens = new QStandardItem(obj["sens"].toString());

        model->setItem(row, 0, itemName);
        model->setItem(row, 1, itemAct);
        model->setItem(row, 2, itemReq);
        model->setItem(row, 3, itemSens);

        row++;
    }



    ui->EventTableView->setModel(model);
}

void HomeControl::loadEvents()
{
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?act=readallevents");
    down->doDownloadTemperature(_url);
    connect(down, SIGNAL(tempChanged(QJsonObject)), this, SLOT(loadEventsSlot(QJsonObject)));
}

/**
 * @brief MainWindow::InserItemsSlot
 * @param Lists
 * Slot to inserts lines to combobox
 */
void HomeControl::loadEventsSlot(QJsonObject jsonObject)
{

    QJsonArray jsonArray = jsonObject["event"].toArray();
    int rows = jsonArray.count();
    int row = 0;
    QStandardItemModel *model = new QStandardItemModel(rows,5,this); //2 Rows and 3 Columns
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("time")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("ip")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("device")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("bit")));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString("val")));

    foreach (const QJsonValue & value, jsonArray)
    {

        QJsonObject obj = value.toObject();

        QStandardItem* itemTime = new QStandardItem(obj["time"].toString());
        QStandardItem* itemIp = new QStandardItem(obj["ip"].toString());
        QStandardItem* itemDev = new QStandardItem(obj["device"].toString());
        QStandardItem* itemBit = new QStandardItem(obj["bit"].toString());
        QStandardItem* itemVal = new QStandardItem(obj["value"].toString());

        model->setItem(row, 0, itemTime);
        model->setItem(row, 1, itemIp);
        model->setItem(row, 2, itemDev);
        model->setItem(row, 3, itemBit);
        model->setItem(row, 4, itemVal);

        row++;
    }



    ui->tempTableView->setModel(model);
    //ui->EventTableView->horizontalHeader()->setStyleSheet("QHeaderView { font-size: 10pt; }");
    ui->tempTableView->setStyleSheet("QHeaderView {font-size: 10pt;font-weight:bold }");
    ui->tempTableView->setStyleSheet("QTableView {font-size: 10pt; }");
    ui->tempTableView->resizeColumnsToContents();
}

/**
 * @brief MainWindow::ntwRequest
 * Function get data from url if data is downloade connect to slot
 */
void HomeControl::loadGraph_A()
{
    Downloader* down = new Downloader();
    //_url.setUrl("http://"+_hostName+":"+_port+"/smarthome/avg_temp_graph.php?type=day&nadpis=Teploty_za_24hod.");
    _url.setUrl("http://"+_hostName+":90/picture/1/current/");
    down->doDownloadGraph(_url);
    connect(down, SIGNAL(pixmapChanged(QPixmap)), this , SLOT(SetPixmapslot(QPixmap)));
}

void HomeControl::SetPixmapslot(QPixmap pixmap)
{
    ui->graphDayLabel->setPixmap(pixmap);
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
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=opengate&bit="+QString::number(bit));
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
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=writevalue&bit="+QString::number(bit));//17
    down->doWriteBit(_url);
    connect(down, SIGNAL(bitwrited(QString)), ui->pushButton3, SLOT(setStyleSheet(QString)));
}

void HomeControl::on_pushButton4_released()
{
    ui->pushButton4->setStyleSheet(_style_red);
    ButtonInfo info = this->findButton("button4");
    int bit = info.parameter.bit;
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=writevalue&bit="+QString::number(bit));//17
    down->doWriteBit(_url);
    connect(down, SIGNAL(bitwrited(QString)), ui->pushButton4, SLOT(setStyleSheet(QString)));
}

void HomeControl::on_pushButton5_released()
{
    ui->pushButton5->setStyleSheet(_style_red);
    ButtonInfo info = this->findButton("button5");
    int bit = info.parameter.bit;
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=writevalue&bit="+QString::number(bit));//17
    down->doWriteBit(_url);
    connect(down, SIGNAL(bitwrited(QString)), ui->pushButton5, SLOT(setStyleSheet(QString)));
}

void HomeControl::on_pushButton6_released()
{
    ui->pushButton6->setStyleSheet(_style_red);
    ButtonInfo info = this->findButton("button6");
    int bit = info.parameter.bit;
    Downloader* down = new Downloader();
    _url.setUrl("http://"+_hostName+":"+_port+"/smarthome/gpio_control.php?dev="+_deviceName+"&act=writevalue&bit="+QString::number(bit));//17
    down->doWriteBit(_url);
    connect(down, SIGNAL(bitwrited(QString)), ui->pushButton6, SLOT(setStyleSheet(QString)));
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

void HomeControl::showDeviceInfo()
{
    QString info = "Product: "+QSysInfo::prettyProductName();
    info += "\n";
    info += "CPU Arch.: "+QSysInfo::buildCpuArchitecture();
    info += "\n";
    info += "Kernel Type: "+QSysInfo::kernelType();
    info += "\n";
    info += "Kernel Ver: "+QSysInfo::kernelVersion();

    //ui->infoLabel->setText(info);
}
