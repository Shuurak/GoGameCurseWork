#include "clientnetgame.h"

clientNetGame::clientNetGame(QString _host, quint16 _port, QString _nickname, QString _gameVersion, QString _platformType, QObject *parent) : QObject(parent),
    host(_host), port(_port), nickname(_nickname), gameVersion(_gameVersion), platformType(_platformType)
{
    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

    socket.connectToHost(host, port, QTcpSocket::ReadWrite);
    if (!socket.waitForConnected(10000))
    {
        emit errorReturner(socket.errorString());
        debugIO << socket.errorString();
        this->deleteLater();
    }

    socket.setReadBufferSize(8192);

    connect(&socket, SIGNAL(readyRead()), this, SLOT(newDataAvailable()));

    start();
}

clientNetGame::~clientNetGame()
{

    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

}

void clientNetGame::setNewStoneOnEnemyBoard(QPair<quint8, quint8> position, GlobalConstants::stoneColors fraction)
{
    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

    QJsonObject msg;
    msg.insert("ask", "gameStep");

    QJsonObject details;
        details.insert("stepToCell", position.first*100+position.second);
        details.insert("fraction", fraction==GlobalConstants::stoneColors::BLACK ? "black" : "white");
        details.insert("onBoard", onBoard);

    msg.insert("details", details);


    QJsonDocument textMsg;
    textMsg.setObject(msg);

    lastMsgDirective = "ask";
    socket.write(textMsg.toJson(QJsonDocument::Compact));
    debugIO << textMsg.toJson();

    connectionTimeoutTimer.start(20000);

    onBoard++;
}

void clientNetGame::isPositionAvailableAnswer(QPair<quint8, quint8> position, GlobalConstants::stoneColors fraction, bool answer)
{
    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

    if (answer)
    {
        emit setNewStoneOnAllyBoardSingle(position, fraction);

        QJsonObject msg;
        msg.insert("answer", "gameStep");

        QJsonObject details;
            details.insert("solution", true);
            details.insert("stepToCell", position.first*100+position.second);
            details.insert("fraction", fraction==GlobalConstants::stoneColors::BLACK ? "black" : "white");
            details.insert("onBoard", onBoard);

        msg.insert("details", details);


        QJsonDocument textMsg;
        textMsg.setObject(msg);

        lastMsgDirective = "answer";
        socket.write(textMsg.toJson(QJsonDocument::Compact));
        debugIO << textMsg.toJson();

        connectionTimeoutTimer.start(20000);

        onBoard++;
    }
    else
    {
        helpNeeded(position.first*100+position.second);
    }
}

void clientNetGame::newDataAvailable()
{
    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

    connectionTimeoutTimer.stop();

    QJsonObject msg;
    QJsonDocument _new;
    _new.fromJson(socket.readAll());
    socket.flush();

    msg = _new.object();

    checkProtocol(msg);                 //вызвать диалог, с ошибкой и вопросом о дальшем общении с сервером!!

    debugIO << _new;

    if (!isConnected && msg["answer"]=="tryToConnect")  //connection state
    {
        lastMsgDirective = "tryToConnect";
        if (msg["details"].toObject()["solution"] == true)
        {
            isConnected = true;
            enemyNick = msg["details"].toObject()["game"].toObject()["enemyNickname"].toString();
            enemyPlatformType = msg["details"].toObject()["game"].toObject()["platformType"].toString();
            if (msg["details"].toObject()["game"].toObject()["yourFraction"].toString() == "black")
            {
                yourFraction = GlobalConstants::stoneColors::BLACK;
                return;
            }
            else
            {
                yourFraction = GlobalConstants::stoneColors::WHITE;
                return;
            }
        }
        else
        {
            emit errorReturner(msg["details"].toObject()["txtReason"].toString());
            debugIO << msg["details"].toObject()["txtReason"] << msg["details"].toObject()["reason"];
            //ERRORS LOG!!!

            this->deleteLater();
            return;
        }
    }
    else
        if (isConnected && !msg.contains("order"))      //game step (ask or answer)
        {
            if (msg.contains("ask"))
            {
                lastMsgDirective = "ask";
                if (msg["details"].toObject()["onBoard"].toInt() == onBoard)
                {
                    lastAskPos = bytePair (msg["details"].toObject()["stepToCell"].toInt()/100, msg["details"].toObject()["stepToCell"].toInt()%100);

                    emit isPositionAvailable(lastAskPos,msg["details"].toObject()["fraction"].toString() == "black" ? GlobalConstants::stoneColors::BLACK
                                                                                                                    : GlobalConstants::stoneColors::WHITE);
                    return;
                }
                else
                {
                    helpNeeded(msg["details"].toObject()["stepToCell"].toInt());
                    return;
                }
            }
            if (isConnected && msg.contains("answer"))
            {
                lastMsgDirective = "answer";
                if (msg["details"].toObject()["onBoard"].toInt() == onBoard)
                {
                    emit setNewStoneOnAllyBoardSingle(bytePair(msg["details"].toObject()["stepToCell"].toInt()/100,
                                                            msg["details"].toObject()["stepToCell"].toInt()%100),
                                                (GlobalConstants::stoneColors) msg["details"].toObject()["fraction"].toInt());
                    onBoard++;
                    return;
                }
                else
                {
                    helpNeeded(msg["details"].toObject()["stepToCell"].toInt());
                    return;
                }
            }
        }
    else
        if (isConnected && msg.contains("order"))       //forced sync
        {
            lastMsgDirective = "order";

            forcedBoardSyncIncoming(msg["details"].toObject()["stones"].toArray());
            onBoard = msg["details"].toObject()["onBoard"].toInt();
            dsyncCounter++;

            lastMsgDirective = "echo";
            socket.write(_new.toJson(QJsonDocument::Compact));
        }
}

void clientNetGame::timeoutConnection()
{
    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

    emit errorReturner("Connection time out. Try again later!");

    socket.disconnectFromHost();
    this->deleteLater();
}

void clientNetGame::start()
{
    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

    QJsonObject msg;
    msg.insert("ask", "tryToConnect");

    QJsonObject details;
    details.insert("protocolVersion", netProtocolVersion);

        QJsonArray modifiers{};

        details.insert("protocolModifiers", false);
        details.insert("modefiers", modifiers);

            QJsonObject game;
            game.insert("majorVersion", gameMainVer);
            game.insert("platformType", platformType);
            game.insert("nickname", nickname);

        details.insert("game", game);

    msg.insert("details", details);


    QJsonDocument textMsg;
    textMsg.setObject(msg);

    lastMsgDirective = "ask";
    socket.write(textMsg.toJson(QJsonDocument::Compact));
    debugIO << textMsg.toJson();

    connectionTimeoutTimer.setSingleShot(true);
    connectionTimeoutTimer.start(20000);

    connect(&connectionTimeoutTimer, SIGNAL(timeout()), this, SLOT (timeoutConnection()));
}

bool clientNetGame::checkProtocol(QJsonObject in)
{
    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

    if (in.contains("details"))
    {
        if (lastMsgDirective=="ask" && in.contains("answer"))
        {
            if (in["answer"].toString() == "tryToConnect")
            {
                if (in["details"].toObject().contains("protocol")
                        && in["details"].toObject().contains("protocolModifiers")
                        && in["details"].toObject().contains("modifiers")
                        && in["details"].toObject().contains("game"))
                {
                    if (in["details"].toObject()["game"].toObject().contains("version")
                            && in["details"].toObject()["game"].toObject().contains("platformType")
                            && in["details"].toObject()["game"].toObject().contains("enemyNickname")
                            && in["details"].toObject()["game"].toObject().contains("yourFraction"))
                    {
                        return true;
                    }
                    else
                    {
                        debugIO << "Incorrect msg section at key \"game\"" << "msg: \n" << in;
                        return false;
                    }
                }
                else
                {
                    debugIO << "Incorrect msg section at key \"details\"" << "msg: \n" << in;
                    return false;
                }
            }
            if (in["answer"].toString() == "gameStep")
            {
                if (in["details"].toObject().contains("solution")
                        && in["details"].toObject().contains("stepToCell")
                        && in["details"].toObject().contains("fraction")
                        && in["details"].toObject().contains("onBoard"))
                {
                    return true;
                }
                else
                {
                    debugIO << "Incorrect msg section at key \"details\"" << "msg: \n" << in;
                    return false;
                }
            }
            else
            {
                debugIO << "Unknown val at key \"answer\": " << in["answer"].toString() << '\n' << in;
                return false;
            }
        }
        if (lastMsgDirective=="answer" && in.contains("ask"))
        {
            if (in["ask"].toString() == "gameStep")
            {
                if (in["details"].toObject().contains("stepToCell")
                        && in["details"].toObject().contains("fraction")
                        && in["details"].toObject().contains("onBoard"))
                {
                    return true;
                }
                else
                {
                    debugIO << "Incorrect msg section at key \"details\"" << "msg: \n" << in;
                    return false;
                }
            }
        }
        if (in.contains("order") && lastSendedMsg.contains("ask"))
        {
            if (in["details"].toObject().contains("stones")
                    && in["details"].toObject().contains("onBoard"))
            {
                QString orderValue = in["order"].toString();

                if (orderValue != "sync"+QString(dsyncCounter)
                                            +QString(lastSendedMsg["details"].toObject()["stepToCell"].toInt()))
                {
                    debugIO << "WARNING! Incorrect order value string: " << in["order"].toString() << '\n' << in;
                }
                return true;
            }
            else
            {
                debugIO << "Incorrect msg section at key \"details\"" << "msg: \n" << in;
                return false;
            }
        }
        if (in.contains("order") && lastMsgDirective=="help")
        {
            if (in["details"].toObject().contains("stones")
                    && in["details"].toObject().contains("onBoard"))
            {
                QString orderValue = in["order"].toString();

                if (orderValue != "sync"+QString(dsyncCounter)
                                            +QString(lastSendedMsg["details"].toObject()["stepToCell"].toInt()))
                {
                    debugIO << "WARNING! Incorrect order value string: " << in["order"].toString() << '\n' << in;
                }
                return true;
            }
            else
            {
                debugIO << "Incorrect msg section at key \"details\"" << "msg: \n" << in;
                return false;
            }
        }
    }

    debugIO << "Incomming msg haven't \"details\" key" << "lastOutgoinDirective: "<< lastMsgDirective << '\n' << in;
    return false;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void clientNetGame::helpNeeded(int xxYY)
{
    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

    QJsonObject msg;
    msg.insert("help", 123);        //int in enum of help reasons!!

    QJsonObject details;
    details.insert("stepToCell", xxYY);

    msg.insert("details", details);


    QJsonDocument textMsg;
    textMsg.setObject(msg);

    lastMsgDirective = "help";
    socket.write(textMsg.toJson(QJsonDocument::Compact));
    debugIO << textMsg.toJson();

    connectionTimeoutTimer.start(20000);
}

void clientNetGame::forcedBoardSyncIncoming(QJsonArray in)
{
    debugIO << "--------------------------------------------"
            << Q_FUNC_INFO << '\n'
            << "--------------------------------------------";

    debugIO << "client onBoard:" << onBoard << "server onBoard:" << in.count();

    onBoard = in.count();
    syncedStones = new QPair<byte, byte>[onBoard];
    syncedFractionsForStones = new GlobalConstants::stoneColors [onBoard];

    for (int i=0; i<onBoard; i++)
    {
        syncedStones[i] = bytePairType (in[i].toObject()["cell"].toInt()/100, in[i].toObject()["cell"].toInt()%100);
        syncedFractionsForStones[i] = in[i].toObject()["fraction"].toString() == "black" ? GlobalConstants::stoneColors::BLACK
                                                                                         : GlobalConstants::stoneColors::WHITE;
    }

    emit setNewStoneOnAllyBoardSync(syncedStones, syncedFractionsForStones, onBoard);
}
