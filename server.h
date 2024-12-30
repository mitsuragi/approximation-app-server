#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork>
#include <QVector>
#include <vector>

class Server : public QTcpServer {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
private:
    QByteArray Data;
    QTcpSocket *clientSocket;
    std::vector<double> concentrationVector;
    std::vector<double> levelVector;
    std::vector<double> consumptionVector;

    void initServer();
    void getDataFromClient();
    void sendDataToClient(std::vector<double> coeffsConc, std::vector<double> coeffsLevel);

    std::vector<double> fitSinusoidal(const std::vector<double> &parameter);

private slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
