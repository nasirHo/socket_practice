#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QHash>
#include <QTcpSocket>

class server : public QObject
{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);

signals:
    void newMessage(const QString &type, const QString &name, const QString &message);

public slots:
    void sendMessage(const QString &message);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();
    void onNewMessage(const QString &type, const QString &name, const QString &message);

private:
    QString getClientKey(const QTcpSocket *client) const;
private:
    QTcpServer* server_socket;
    QHash<QString, QTcpSocket*> clients;
    QHash<QString, QString> clients_names;
    QHash<QString, QString> clients_names_reverse;
};

#endif // SERVER_H
