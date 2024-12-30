#include "server.h"

#include <iostream>
#include <QFile>
#include <QString>
#include <QDataStream>
#include <QIODevice>
#include "graphfunctions.h"

Server::Server(QObject *parent) : QTcpServer(parent) {
    initServer();
}

void vectorOutput(const std::vector<double>& x) {
    for (int i = 0; i < x.size(); i++) {
        std::cout << x[i] << ' ';
    }
    std::cout << std::endl;
}

void Server::initServer() {
    QFile file(":/configserver/config.txt");
    file.open(QIODevice::ReadOnly);

    QTextStream out(&file);

    QHostAddress adress(out.readLine());
    int host = out.readLine().toInt();

    if (!this->listen(adress, host)) {
        std::cerr << "Unable to start the server: " << this->errorString().toStdString() << std::endl;
        return;
    }

    std::cout << "The server is running on IP: " << adress.toString().toStdString() << "; Port: " << this->serverPort() << std::endl;

    file.close();
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    std::cout << "Client connected " << socketDescriptor << std::endl;
}

void Server::slotReadyRead() {
    concentrationVector.clear();
    levelVector.clear();

    clientSocket = (QTcpSocket*)sender();

    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_6_8);

    if (in.status() == QDataStream::Ok) {
        std::cout << "Read data from client..." << std::endl;

        if (clientSocket->bytesAvailable() < sizeof(int) * 2) {
            std::cerr << "Not enough data received!" << std::endl;
            return;
        }

        int concSize, levelSize, consumSize;

        in >> consumSize;
        for (int i = 0; i < consumSize; i++) {
            double value;
            in >> value;
            consumptionVector.push_back(value);
        }

        in >> concSize;
        for (int i = 0; i < concSize; i++) {
            double value;
            in >> value;
            concentrationVector.push_back(value);
        }

        in >> levelSize;
        for (int i = 0; i < levelSize; i++) {
            double value;
            in >> value;
            levelVector.push_back(value);
        }

        std::cout << "Received consumtion vector: ";
        vectorOutput(consumptionVector);
        std::cout << "Received concentration vector: ";
        vectorOutput(concentrationVector);
        std::cout << "Reveived level vector: ";
        vectorOutput(levelVector);

        std::cout << "Calculation..." << std::endl;

        std::vector<double> coeffsConc = linearApproximation(consumptionVector, concentrationVector);
        std::cout << "Concentration coefficients: ";
        vectorOutput(coeffsConc);

        std::vector<double> coeffsLevel = linearApproximation(consumptionVector, levelVector);
        std::cout << "Level coefficients: ";
        vectorOutput(coeffsLevel);

        std::cout << "Sending data..." << std::endl;
        sendDataToClient(coeffsConc, coeffsLevel);
        std::cout << "Done!" << std::endl;
    }
    else {
        std::cerr << "DataStream error" << std::endl;
    }
}

void Server::sendDataToClient(std::vector<double> coeffsConc, std::vector<double> coeffsLevel) {
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);

    out << (int)coeffsConc.size();
    for (int i = 0; i < coeffsConc.size(); i++) {
        out << coeffsConc[i];
    }
    out << (int)coeffsLevel.size();
    for (int i = 0; i < coeffsLevel.size(); i++) {
        out << coeffsLevel[i];
    }

    clientSocket->write(Data);
}
