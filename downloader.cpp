#include "downloader.h"

Downloader::Downloader(QObject *parent):QObject(parent)
{
    _style_gray = QString("background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #FFF, stop: 1 #888);");
    _style_red = QString("background: qradialgradient(cx: 0.3, cy: -0.4,fx: 0.3, fy: -0.4,radius: 1.35, stop: 0 #FF0000, stop: 1 #630000);");
    _manager = new QNetworkAccessManager(this);
}

void Downloader::doDownloadGpioState(QUrl url)
{
    connect(_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(slot_replyGpioState(QNetworkReply*)));
    _manager->get(QNetworkRequest(url));
}

void Downloader::slot_replyGpioState(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {

        QMessageBox msgBox;
        msgBox.setText("Chyba při vykonávání dotazu loadTemperature \n"+reply->errorString());
        msgBox.exec();

        qDebug() << reply->errorString();
        delete reply;
        return;
    }

    QString strReply = (QString)reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();

    emitGpioSignal(jsonObject);

    reply->deleteLater();
}

void Downloader::emitGpioSignal(QJsonObject jsonObject)
{
    emit gpioStateChanged(jsonObject);
}

void Downloader::doDownloadTemperature(QUrl url)
{
    connect(_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(slot_replyTemperature(QNetworkReply*)));
    _manager->get(QNetworkRequest(url));
}

void Downloader::slot_replyTemperature(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {

        QMessageBox msgBox;
        msgBox.setText("Chyba při vykonávání dotazu loadTemperature \n"+reply->errorString());
        msgBox.exec();

        qDebug() << reply->errorString();
        delete reply;
        return;
    }

    QString strReply = (QString)reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();

    emitTempSignal(jsonObject);

    reply->deleteLater();
}

void Downloader::emitTempSignal(QJsonObject jsonObject)
{
    emit tempChanged(jsonObject);
}

void Downloader::doDownloadGraph(QUrl url)
{
    connect(_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(slot_replyGraph(QNetworkReply*)));
    _manager->get(QNetworkRequest(url));
}

void Downloader::slot_replyGraph(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
        return;
    }

    QByteArray pngData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(pngData);
    emitGraphSignal(pixmap);

    reply->deleteLater();
}

void Downloader::emitGraphSignal(QPixmap pixmap)
{
    emit pixmapChanged(pixmap);
}

void Downloader::doWriteBit(QUrl url)
{
    connect(_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(slot_replyBit(QNetworkReply*)));
    _manager->get(QNetworkRequest(url));
}

void Downloader::slot_replyBit (QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
        return;
    }

    emitWriteBitSignal(_style_gray);
    reply->deleteLater();
}

void Downloader::emitWriteBitSignal(QString style)
{
    emit bitwrited(style);
}
