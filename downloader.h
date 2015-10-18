#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QByteArray>
#include <QDebug>
#include <QTextCodec>
#include <QPixmap>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0);
    void doDownloadGpioState(QUrl url);
    void doDownloadTemperature(QUrl url);
    void doDownloadGraph(QUrl url);
    void doWriteBit(QUrl url);
    void emitGpioSignal(QJsonObject jsonObject);
    void emitTempSignal(QJsonObject jsonObject);
    void emitGraphSignal(QPixmap pixmap);
    void emitWriteBitSignal(QString style);

    signals:
        void gpioStateChanged(QJsonObject);
        void tempChanged(QJsonObject);
        void pixmapChanged(QPixmap);
        void bitwrited(QString);

    public slots:
        void slot_replyGpioState (QNetworkReply *reply);
        void slot_replyTemperature (QNetworkReply *reply);
        void slot_replyGraph (QNetworkReply *reply);
        void slot_replyBit (QNetworkReply *reply);

    private:
       QNetworkAccessManager *_manager;
       QString _style_gray,_style_red;

};

#endif // DOWNLOADER_H
