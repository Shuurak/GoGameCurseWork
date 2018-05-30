#include "goban.h"

Goban::Goban(QObject *parent) : QObject(parent)
{
    movesCounter=0;
    srand(time(0));

    StonesOnGoban = new Stone* [gameConstant::BOARD_SIZE];

    for (int i=0; i<gameConstant::BOARD_SIZE; i++)
    {
        StonesOnGoban[i] = new Stone [gameConstant::BOARD_SIZE];
    }
}

Goban::~Goban()
{
    for (int i=0; i<gameConstant::BOARD_SIZE; i++)
    {
        delete []  StonesOnGoban[i];
    }

    delete [] StonesOnGoban;
}

quint8 Goban::getDame(QPair<quint8, quint8> position)
{
    /* проверяет, является ли поставленный камень частью группы его фракции
     * если да, то вернёт количество дамэ всей группы после постановки камня
     * в противном случае - выяснит, окружена ли вражеская группа постановкой камня
     * и вернёт количество пленных (со знаком минус) после постановки камня
     *
     * если камень не является частью группы, то вернёт
     * количество дамэ при его постановке (проверяя соседние пункты)
     */

    return 1;
}

qint64 Goban::markPosition(QPair<byte, byte> position, byte branchingLevel, qint64 al, qint64 bt)
{
    /* для проставления оценок в соответсвующих пунтак использует метод
     * повременно-пространственной оценки - значение оценки для групп пунктов
     * со временем протекания игры изменяются
     * --------------------------------------------------------------------
     * в период фусэки определяется влияние на углы и стороны, поэтому важно занять хоши
     * в этот период игры создаётся влияние на стороны и углы доски, а так же определяется
     * основание для развития в сторону небес
     *
     * в период тюбан приоритетом считаются стороны, влияния на которые оказывают
     * камни, поставленные в фусэки
     * ближе к концу первой трети фазы тюбан, внимание активно уделяется небесам
     * и границам отделающим его от земли (границы го, го-линии, 4-ые линии)
     *
     * в завершающей фазе ёсэ важно ставить камни таким образом, чтобы выиграть как можно
     * больше территории и взять в плен большее количество камней противника - а значит и
     * захваченной им торритории
     * --------------------------------------------------------------------
     * для проставления оценки в определённое время используется характеристика камня
     * или в случае соединения - группы камней, как дамэ - количество дыханий,
     * которыми он (или группа) обладает
     * важно принять во внимание возможность захвата группы противника или защиты своей территории
     * для последующего подсчёта захваченных пунктов
     * --------------------------------------------------------------------
     * в начале партии оценки проставляются по принципу начальной влиятельности камней - важно
     * занять хоши, чтобы построить мощный потенциал развития, поэтому первоочерёдной задачей
     * выступает захват точек хоши - пересечения границ го
     *
     * как только наступает период тюбан - качественная величина оценки смещается к сторонам рядом
     * с дружественными хоши, а так же влиянию на территории небес
     *
     * фаза ёсэ оценивается опираяюсь исключительно на даме камня (или его группы)
     * поскольку в этой фазе возможны как захваты группы противника, так и собственной группы
     * следует действовать предельно аккуратно
     */

    if (branchingLevel == 4)
    {
        return defineCurrentGameSituation();
    }

    qint64 additionalMark = 0;

    takeVirtualMove(position, branchingLevel%2 == 0? stoneColors::WHITE : stoneColors::BLACK);

    if (movesCounter <= (gameConstant::BOARD_SIZE_INDEXSED/100 * gameStages::FUSEKI))
    {
        if (    (position == bytePair(0, 0))
                || (position == bytePair(0, gameConstant::BOARD_SIZE_INDEXSED))
                || (position == bytePair(gameConstant::BOARD_SIZE_INDEXSED, 0))
                || (position == bytePair(gameConstant::BOARD_SIZE_INDEXSED, gameConstant::BOARD_SIZE_INDEXSED))
            )
        {
            additionalMark = (2 * sectorSignificance::LINE_1 * gameStages::FUSEKI);
        }

        if (    (position <= bytePair(2, 2))
                || (position <= bytePair(2, gameConstant::BOARD_SIZE_INDEXSED - 2))
                || (position <= bytePair(gameConstant::BOARD_SIZE_INDEXSED - 2, 2))
                || (position >= bytePair(gameConstant::BOARD_SIZE_INDEXSED - 2, gameConstant::BOARD_SIZE_INDEXSED - 2))
            )
        {
            additionalMark = (3 * sectorSignificance::LINES_2AND3 * gameStages::FUSEKI);
        }

        if (    (position == bytePair(3, 3))
                || (position == bytePair(3, gameConstant::BOARD_SIZE_INDEXSED - 3))
                || (position == bytePair(gameConstant::BOARD_SIZE_INDEXSED, 3))
                || (position == bytePair(gameConstant::BOARD_SIZE_INDEXSED - 3, gameConstant::BOARD_SIZE_INDEXSED - 3))
           )
        {
            additionalMark = (4 * sectorSignificance::LINES_4TO6 * gameStages::FUSEKI);
        }

        if ((position > bytePair(3, 3)) && (position < bytePair(gameConstant::BOARD_SIZE_INDEXSED - 3, gameConstant::BOARD_SIZE_INDEXSED - 3)))
        {
            additionalMark = (4 * sectorSignificance::HOSHI_LINES * gameStages::FUSEKI - 100);
        }

    }

    if ((movesCounter >= (gameConstant::BOARD_SIZE_INDEXSED/100 * gameStages::TYUBAN))
            && (movesCounter < (gameConstant::BOARD_SIZE_INDEXSED/100 * gameStages::YOSE)))
    {
        if (    (position == bytePair(0, 0))
                || (position == bytePair(0, gameConstant::BOARD_SIZE_INDEXSED))
                || (position == bytePair(gameConstant::BOARD_SIZE_INDEXSED, 0))
                || (position == bytePair(gameConstant::BOARD_SIZE_INDEXSED, gameConstant::BOARD_SIZE_INDEXSED))
            )
        {
            additionalMark = (2 * sectorSignificance::LINE_1 * gameStages::TYUBAN - 100);
        }

        if (    (position < bytePair(2, 2))
                || (position < bytePair(2, gameConstant::BOARD_SIZE_INDEXSED - 2))
                || (position < bytePair(gameConstant::BOARD_SIZE_INDEXSED - 2, 2))
                || (position > bytePair(gameConstant::BOARD_SIZE_INDEXSED - 2, gameConstant::BOARD_SIZE_INDEXSED))
            )
        {
            additionalMark = (3 * sectorSignificance::LINES_2AND3 * gameStages::TYUBAN + 250);
        }

        if ((position > bytePair(3, 3)) && (position < bytePair(gameConstant::BOARD_SIZE_INDEXSED - 3, gameConstant::BOARD_SIZE_INDEXSED - 3)))
        {
            additionalMark = (4 * sectorSignificance::HOSHI_LINES * gameStages::TYUBAN + 250);
        }
    }

    if (movesCounter >= (gameConstant::BOARD_SIZE_INDEXSED/100 * gameStages::YOSE))
    {
        int temp = getDame(position);

        if (temp == -1)
        {
            additionalMark = (10 * gameStages::YOSE);
        }
        else
        {
            additionalMark = (temp);
        }
    }

    qint64 MinMax;
    qint64 value;

    if (branchingLevel % 2)
    {
        value = QINT64_MAXIMUM;
    }
    else
    {
        value = QINT64_MINIMUM;
    }

    for (int i=0; i<gameConstant::BOARD_SIZE; i++)
    {
        for (int j=0; j<gameConstant::BOARD_SIZE; j++)
        {
            if (availableToAdd(bytePair(i, j)))
            {
                MinMax = markPosition(position, branchingLevel+1, al, bt);

                MinMax+=additionalMark;

                if ((branchingLevel % 2) && MinMax<value) value = MinMax; //ход игрока - минимизация

                else if ((!(branchingLevel % 2)) && MinMax>value) value = MinMax; // ход компьютера максимизация

                if (branchingLevel % 2 == 0)

                {
                    if (al<MinMax) al = MinMax;
                }

                else if (branchingLevel % 2 == 1)

                {
                    if (bt>MinMax) bt = MinMax;
                 }

                if (bt < al)
                {
                    undoLastVirtualMove(position);
                    return value;
                }
            }
        }
    }

    return value;
}

bool Goban::availableToAdd(QPair<quint8, quint8> position)
{
    /* возвращает true если можно поставить камень
     */

    if (!StonesOnGoban[position.first][position.second].isValidStone())
    {
        return false;
    }
    else
    {
        return true;
    }
}

qint64 Goban::calculateTerritory()
{
    /* определяет кому принадледит свободная территория
     * если больше влияние чёрных - чёрным
     * если белых - белым
     * в случае равенства влияния территория считается общей
     */

    bool allTerFind = 0;

    findTerStart:

    for (int i=0; i<gameConstant::BOARD_SIZE; i++)
    {
        for (int j=0; j<gameConstant::BOARD_SIZE; j++)
        {
            if (!StonesOnGoban[i][j].isValidStone())
            {
                if (i==0 && j==0)
                {
                    if (StonesOnGoban[i][j+1].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i][j+1].isValidTerritory()
                            && StonesOnGoban[i+1][j].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i+1][j].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::BLACK);
                        continue;
                    }
                    else
                    if (StonesOnGoban[i][j+1].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i][j+1].isValidTerritory()
                            && StonesOnGoban[i+1][j].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i+1][j].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::WHITE);
                        continue;
                    }
                    else
                    if (!StonesOnGoban[i][j+1].isValidTerritory() || !StonesOnGoban[i+1][j].isValidTerritory())
                    {
                        allTerFind = 1;
                        continue;
                    }
                }
                if (i==0 && j==gameConstant::BOARD_SIZE)
                {
                    if (StonesOnGoban[i+1][j].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i+1][j].isValidTerritory()
                            && StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i][j-1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::BLACK);
                        continue;
                    }
                    else
                    if (StonesOnGoban[i+1][j].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i+1][j].isValidTerritory()
                            && StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i][j-1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::WHITE);
                        continue;
                    }
                    else
                    if (!StonesOnGoban[i+1][j].isValidTerritory() || !StonesOnGoban[i][j-1].isValidTerritory())
                    {
                        allTerFind = 1;
                        continue;
                    }
                }
                if (i==gameConstant::BOARD_SIZE && j==0)
                {
                    if (StonesOnGoban[i-1][j].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            && StonesOnGoban[i][j+1].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i][j+1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::BLACK);
                        continue;
                    }
                    else
                    if (StonesOnGoban[i-1][j].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            && StonesOnGoban[i][j+1].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i][j+1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::WHITE);
                        continue;
                    }
                    else
                    if (!StonesOnGoban[i-1][j].isValidTerritory() || !StonesOnGoban[i][j+1].isValidTerritory())
                    {
                        allTerFind = 1;
                        continue;
                    }
                }
                if (i==gameConstant::BOARD_SIZE && j== gameConstant::BOARD_SIZE)
                {
                    if (StonesOnGoban[i-1][j].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            && StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i][j-1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::BLACK);
                        continue;
                    }
                    else
                    if (StonesOnGoban[i-1][j].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            && StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i][j-1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::WHITE);
                        continue;
                    }
                    else
                    if (!StonesOnGoban[i+1][j].isValidTerritory() || !StonesOnGoban[i][j-1].isValidTerritory())
                    {
                        allTerFind = 1;
                        continue;
                    }
                }
                if (i==0 && (j>0 && j<gameConstant::BOARD_SIZE))
                {
                    if (StonesOnGoban[i][j+1].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            &&( ( (StonesOnGoban[i+1][j].getTerritoryColor() == stoneColors::BLACK
                                  && StonesOnGoban[i+1][j].isValidTerritory())
                                 || (StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::BLACK
                                     && StonesOnGoban[i][j-1].isValidTerritory())
                                )
                                ||
                                (StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                                 && StonesOnGoban[i][j-1].isValidTerritory())
                               )
                          )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::BLACK);
                        continue;
                    }
                    else
                    if (StonesOnGoban[i-1][j].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            && StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i][j-1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::WHITE);
                        continue;
                    }
                    else
                    if (!StonesOnGoban[i+1][j].isValidTerritory() || !StonesOnGoban[i][j-1].isValidTerritory())
                    {
                        allTerFind = 1;
                        continue;
                    }
                }
                if (i==gameConstant::BOARD_SIZE && (j>0 && j<gameConstant::BOARD_SIZE))
                {
                    if (StonesOnGoban[i][j+1].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            &&( ( (StonesOnGoban[i+1][j].getTerritoryColor() == stoneColors::BLACK
                                  && StonesOnGoban[i+1][j].isValidTerritory())
                                 || (StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::BLACK
                                     && StonesOnGoban[i][j-1].isValidTerritory())
                                )
                                ||
                                (StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                                 && StonesOnGoban[i][j-1].isValidTerritory())
                               )
                          )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::BLACK);
                        continue;
                    }
                    else
                    if (StonesOnGoban[i-1][j].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            && StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i][j-1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::WHITE);
                        continue;
                    }
                    else
                    if (!StonesOnGoban[i+1][j].isValidTerritory() || !StonesOnGoban[i][j-1].isValidTerritory())
                    {
                        allTerFind = 1;
                        continue;
                    }
                }
                if (j==0 && (i>0 && i<gameConstant::BOARD_SIZE))
                {
                    if (StonesOnGoban[i][j+1].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            &&( ( (StonesOnGoban[i+1][j].getTerritoryColor() == stoneColors::BLACK
                                  && StonesOnGoban[i+1][j].isValidTerritory())
                                 || (StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::BLACK
                                     && StonesOnGoban[i][j-1].isValidTerritory())
                                )
                                ||
                                (StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                                 && StonesOnGoban[i][j-1].isValidTerritory())
                               )
                          )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::BLACK);
                        continue;
                    }
                    else
                    if (StonesOnGoban[i-1][j].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            && StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i][j-1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::WHITE);
                        continue;
                    }
                    else
                    if (!StonesOnGoban[i+1][j].isValidTerritory() || !StonesOnGoban[i][j-1].isValidTerritory())
                    {
                        allTerFind = 1;
                        continue;
                    }
                }
                if (j==gameConstant::BOARD_SIZE && (i>0 && i<gameConstant::BOARD_SIZE))
                {
                    if (StonesOnGoban[i][j+1].getTerritoryColor() == stoneColors::BLACK
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            &&( ( (StonesOnGoban[i+1][j].getTerritoryColor() == stoneColors::BLACK
                                  && StonesOnGoban[i+1][j].isValidTerritory())
                                 || (StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::BLACK
                                     && StonesOnGoban[i][j-1].isValidTerritory())
                                )
                                ||
                                (StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                                 && StonesOnGoban[i][j-1].isValidTerritory())
                               )
                          )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::BLACK);
                        continue;
                    }
                    else
                    if (StonesOnGoban[i-1][j].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i-1][j].isValidTerritory()
                            && StonesOnGoban[i][j-1].getTerritoryColor() == stoneColors::WHITE
                            && StonesOnGoban[i][j-1].isValidTerritory() )
                    {
                        StonesOnGoban[i][j].setTerritoryColor(stoneColors::WHITE);
                        continue;
                    }
                    else
                    if (!StonesOnGoban[i+1][j].isValidTerritory() || !StonesOnGoban[i][j-1].isValidTerritory())
                    {
                        allTerFind = 1;
                        continue;
                    }
                }
            }
        }
    }

    if (allTerFind)
    {
        allTerFind = 0;
        goto findTerStart;
    }
    else
    {
        qint64 white=0;
        qint64 black=0;
        for (int i=0; i<gameConstant::BOARD_SIZE; i++)
        {
            for (int j=0; j<gameConstant::BOARD_SIZE; j++)
            {
                if (!StonesOnGoban[i][j].isValidStone())
                {
                    if (StonesOnGoban[i][j].getTerritoryColor()==stoneColors::BLACK)
                    {
                        black++;
                    }
                    if (StonesOnGoban[i][j].getTerritoryColor()==stoneColors::WHITE)
                    {
                        white++;
                    }
                    if (StonesOnGoban[i][j].getTerritoryColor()==stoneColors::SHARE_TERRITORY)
                    {
                        continue;
                    }
                }
            }
        }

        return white-black;
    }
}

qint64 Goban::defineCurrentGameSituation()
{
    /* функция вычисляет разницу между очками белых и чёрных
     */

    return (whitePoints-blackPoints)+calculateTerritory()+gameConstant::KOMI;
}

void Goban::startMarkingAndStep()
{
    /* вводим переменные для определения максимальной оценки
     * и её положении на доске, а так же вспомогательную переменную
     *
     * для каждой свободной клетки проверяем возможность постановки камня
     * и запускаем функцию оценки для этих клеток
     */

    bytePairType maxPointPosition;
    qint64 maxPoint=QINT64_MINIMUM;

    qint64 temp;

    for (int i=0; i<gameConstant::BOARD_SIZE; i++)
    {
        for (int j=0; j<gameConstant::BOARD_SIZE; j++)
        {
            if (availableToAdd(bytePair(i, j)))
            {
                if ( (temp = markPosition(bytePair(i, j), 0, QINT64_MINIMUM, QINT64_MAXIMUM) ) > maxPoint)
                {
                    maxPoint = temp;
                    maxPointPosition = bytePair(i, j);
                }
            }
        }
    }

    addStone(maxPointPosition, stoneColors::WHITE);
}

void Goban::availableToAdd(QPair<quint8, quint8> position, stoneColors fraction)
{
    /* проверяет клетки на возможность постановки камня фракции
     * после чего сигналит в интерфейс
     */

    if (StonesOnGoban[position.first][position.second].isValidStone())
    {
        emit availableToAddRetRuner(false, position, fraction);
    }
    else
    {
        emit availableToAddRetRuner(true, position, fraction);
    }
}

void Goban::addStone(QPair<quint8, quint8> position, stoneColors fraction)
{
    /* проверяем, поставлен ли камень в данной позиции
     * если нет, то ставим туда нужный камень и увеличиваем кол-во ходов
     * а так же передаём сигнал интерфейсу на отрисовку
     *
     * если нет, то пишем в дебаг информацию
     * о попытке добавить камень на место уже существующего
     */

    if (!StonesOnGoban[position.first][position.second].isValidStone())
    {
        StonesOnGoban[position.first][position.second].setStoneData(position, fraction);

        movesCounter++;

        emit addStoneReturner(position, fraction);
    }
    else
    {
        debugIO << Q_FUNC_INFO;
        debugIO << "Attempt to re-create an existing object at position :" << position;
    }
}

void Goban::removeStone(QPair<quint8, quint8> position)
{
    /* проверяем, поставлен ли камень в данной позиции
     * если да, если это камень чёрных - прибавляем очко белым
     * если камень белых - прибавляем очко чёрны
     *
     * если камня нет, то пишем в дебаг информацию
     * о попытке удалить несуществующий камень
     */

    if (StonesOnGoban[position.first][position.second].isValidStone())
    {
        if (StonesOnGoban[position.first][position.second].getFraction() == stoneColors::BLACK)
        {
            whitePoints++;

            emit removeStoneReturner(position);
        }
        else
        {
            blackPoints++;

            emit removeStoneReturner(position);
        }
    }
    else
    {
        debugIO << Q_FUNC_INFO;
        debugIO << "Attempt to delete a non-existent object at positon: " << position;
    }
}

void Goban::stopGame()
{
    /* при остановке игры происходит подсчёт кол-ва очков
     * если функция подсчёте возвращает положительное чисто
     * то победили белые, в противном случае победили чёрные
     *
     * возможен так же исход ничьей
     * возвращаем соответствующие сигналы для интерфейса
     */

    qint64 temp;
    if ((temp=defineCurrentGameSituation()) > 0)
    {
        emit stopGameReturner("Win for WHITE!");
    }
    if (temp<0)
    {
        emit stopGameReturner("Win for BLACK!");
    }
    if (temp==0)
    {
        emit stopGameReturner("Draw game!");
    }
}
