#include "server.h"
#include "QJsonObject"
#include "QJsonDocument"

server::server(QObject *parent)
    : QObject{parent}
{
    server_socket = new QTcpServer(this);
    connect(server_socket, &QTcpServer::newConnection, this, &server::onNewConnection);
    connect(this, &server::newMessage, this, &server::onNewMessage);
    if(server_socket->listen(QHostAddress::Any, 8080)) {
        qInfo() << "Listening ...";
    }
}

void server::sendMessage(const QString &message)
{
    emit newMessage("msg", "Server", message);
}

void server::onNewConnection()
{
    const auto client = server_socket->nextPendingConnection();
    if(client == nullptr) {
        return;
    }

    qInfo() << "New client connected.";

    clients.insert(this->getClientKey(client), client);

    connect(client, &QTcpSocket::readyRead, this, &server::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &server::onClientDisconnected);
    //emit newMessage("join", "Server", this->getClientKey(client) + " Joined");
}

void server::onReadyRead()
{
    const auto client = qobject_cast<QTcpSocket*>(sender());

    if(client == nullptr) {
        return;
    }

    const auto message = client->readAll();

    QJsonParseError jError;
    QJsonDocument recvJD = QJsonDocument::fromJson(message, &jError);
    if(jError.error != QJsonParseError::NoError){
        qWarning() << "Json Error: " << jError.errorString();
    }

    QJsonObject recvJO = recvJD.object();

    if(recvJO.value("type").toString() == "join"){
        if(clients_names_reverse.contains(recvJO.value("name").toString())){
            QJsonObject toSendO;
            toSendO.insert("type", "changeName");
            toSendO.insert("name", "Server");
            toSendO.insert("body", recvJO.value("name").toString());


            QJsonDocument toSendD;
            toSendD.setObject(toSendO);
            QByteArray toSendB = toSendD.toJson();
            client->write(toSendB);
            client->flush();
        }else{
            clients_names.insert(this->getClientKey(client), recvJO.value("name").toString());
            clients_names_reverse.insert(recvJO.value("name").toString(), this->getClientKey(client));
            emit newMessage(recvJO.value("type").toString().toUtf8(), "Server", recvJO.value("body").toString().toUtf8());
        }
    }else{
        emit newMessage(recvJO.value("type").toString().toUtf8(), recvJO.value("name").toString().toUtf8(), recvJO.value("body").toString().toUtf8());
    }

}

void server::onClientDisconnected()
{
    const auto client = qobject_cast<QTcpSocket*>(sender());

    if(client == nullptr) {
        return;
    }

    emit newMessage("left", "Server", clients_names[this->getClientKey(client)] +" ("+ this->getClientKey(client) + ") Left");
    clients.remove(this->getClientKey(client));
    clients_names_reverse.remove(clients_names[this->getClientKey(client)]);
    clients_names.remove(this->getClientKey(client));

}

void server::onNewMessage(const QString &type, const QString &name, const QString &message)
{
    QJsonObject toSendO;
    toSendO.insert("type", type);
    toSendO.insert("name", name);
    toSendO.insert("body", message);


    QJsonDocument toSendD;
    toSendD.setObject(toSendO);
    QByteArray toSendB = toSendD.toJson();

    for(const auto &client : qAsConst(clients)) {
        client->write(toSendB);
        client->flush();
    }
}

QString server::getClientKey(const QTcpSocket *client) const
{
    return client->peerAddress().toString().append(":").append(QString::number(client->peerPort()));
}
