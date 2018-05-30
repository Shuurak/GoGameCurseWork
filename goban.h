#ifndef GOBAN_H
#define GOBAN_H

#include <QObject>
#include <stone.h>
#include <limits.h>



class Goban : public QObject
{
    Q_OBJECT
public:
    explicit Goban(QObject *parent = 0);

    ~Goban();

private:

    quint16 movesCounter;

    byte getDame (QPair<byte, byte> position);

    Stone **StonesOnGoban;

    qint64 whitePoints=0;

    qint64 blackPoints=0;

    qint64 whiteTer=0;

    qint64 blackTer=0;

    qint64 markPosition (QPair<byte, byte> position, byte branchingLevel, qint64 al, qint64 bt);

    bool availableToAdd (QPair<byte, byte> position);

    inline void takeVirtualMove (QPair<byte, byte> position, stoneColors fraction)
    {
        StonesOnGoban[position.first][position.second].setStoneData(position, fraction);
    }

    inline void undoLastVirtualMove (QPair<byte, byte> position)
    {
        StonesOnGoban[position.first][position.second].disableStone();
    }

    qint64 calculateTerritory();

    qint64 defineCurrentGameSituation();

signals:

    void markPositionReturner (int mark);

    void addStoneReturner (QPair<byte, byte> position, GlobalConstants::stoneColors fraction);

    void availableToAddRetRuner (bool answer, QPair<quint8, quint8> position, stoneColors fraction);

    void removeStoneReturner(QPair<quint8, quint8> position);

    void stopGameReturner(QString);

public slots:

    void startMarkingAndStep();

    void availableToAdd (QPair<byte, byte> position, GlobalConstants::stoneColors fraction);

    void addStone (QPair<byte, byte> position, GlobalConstants::stoneColors fraction);

    void removeStone (QPair<byte, byte> position);

    void stopGame();

};

#endif // GOBAN_H
