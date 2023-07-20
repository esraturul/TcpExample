#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QPushButton>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onpushButtonclicked);


    QTcpServer* server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);

    if (!server->listen(QHostAddress::Any, 8080)) {
        ui->plainTextEdit->appendPlainText("Sunucu başlatılamadı!");
    } else {
        ui->plainTextEdit->appendPlainText("Sunucu başlatıldı. Bağlantı bekleniyor...");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewConnection()
{
    QTcpServer* server = qobject_cast<QTcpServer*>(sender());
    if (server) {
        QTcpSocket* client = server->nextPendingConnection();
        if (client) {
            connect(client, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
            connect(client, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconnected);

            client->write("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello, World!");
            ui->plainTextEdit->appendPlainText("Cevap gönderildi.");

                clients.append(client);
        }
    }
}

void MainWindow::onSocketReadyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        QByteArray requestData = client->readAll();
        ui->plainTextEdit->appendPlainText("Gelen veri: " + requestData);
    }
}

void MainWindow::onSocketDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        clients.removeOne(client);
        client->deleteLater();
    }
}
void MainWindow::onpushButtonclicked()
{
    QTcpSocket client;
    client.connectToHost(QHostAddress("127.0.0.1"), 8080);  // Hedef IP adresini ve bağlantı noktasını burada belirtin

    if (client.waitForConnected()) {
        qDebug() << "Bağlantı başarılı.";

        QByteArray requestData = "Merhaba, Sunucu!";
        client.write(requestData);
        client.waitForBytesWritten();

        client.close();
    } else {
        qDebug() << "Bağlantı hatası: " << client.errorString();
    }
}

