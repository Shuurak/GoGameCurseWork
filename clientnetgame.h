#ifndef CLIENTNETGAME_H
#define CLIENTNETGAME_H


#include <globals.h>
#include <netprotocolconstant.h>
#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

class clientNetGame : public QObject
{
    Q_OBJECT
public:
    explicit clientNetGame(QString _host, quint16 _port, QString _nickname, QString _gameVersion, QString _platformType,  QObject *parent = 0);
    ~clientNetGame();

signals:

    void setNewStoneOnAllyBoardSingle(QPair<byte, byte> position, GlobalConstants::stoneColors fraction);

    void setNewStoneOnAllyBoardSync(QPair<byte, byte> *position, GlobalConstants::stoneColors *fraction, int newStonesCount);

    void errorReturner(QString);

    void isPositionAvailable(QPair<byte, byte> position, GlobalConstants::stoneColors fraction);

public slots:

    void setNewStoneOnEnemyBoard (QPair<byte, byte> position, GlobalConstants::stoneColors fraction);

    void isPositionAvailableAnswer(QPair<byte, byte> position, GlobalConstants::stoneColors fraction, bool answer);

private slots:

    void newDataAvailable();

    void timeoutConnection();

private:

    QTimer connectionTimeoutTimer;

    QTcpSocket socket;
    QString host;
    quint16 port;
    QString nickname;
    QString enemyNick;
    QString enemyPlatformType;
    GlobalConstants::stoneColors yourFraction;
    QString gameVersion;
    QString platformType;

    bool isConnected = false;
    QString lastMsgDirective;

    int onBoard = 0;
    bytePairType lastAskPos;

    quint16 dsyncCounter = 0;

    void start();

    bool checkProtocol(QJsonObject in);

    void helpNeeded(int xxYY);

    void forcedBoardSyncIncoming(QJsonArray in);


    QPair<byte, byte> *syncedStones = 0;
    GlobalConstants::stoneColors *syncedFractionsForStones = 0;

    QJsonObject lastSendedMsg;
};

#endif // CLIENTNETGAME_H
