#include "stone.h"

Stone::Stone()
{

}

Stone::Stone(const Stone &_reference) : Position(_reference.Position), Fraction(_reference.Fraction), Dame(_reference.Dame)
{

}

Stone::Stone(QPair<byte, byte> boardPosition, stoneColors stoneFraction)
    : Position(boardPosition), Fraction(stoneFraction)
{
    /* для определения дамэ поставленного камня, необходимо определить его положение
     * относительно граничных линий
     *
     * в углах доски у камня существует изначально только два дамэ
     * на первых линиях (кроме углов) у камня три дамэ
     * на остальных позиция (при условии отсутвия других камней рядом) четыре дамэ
     */

    if (       ((Position.first == 0) && (Position.second == 0))
            || ((Position.first == 0) && (Position.second == gameConstant::BOARD_SIZE))
            || ((Position.first == gameConstant::BOARD_SIZE) && (Position.second == 0))
            || ((Position.first == gameConstant::BOARD_SIZE) && (Position.second == gameConstant::BOARD_SIZE))
       )
    {
        Dame = 2;
        return;
    }

    if (Position.first > 0 && Position.first < BOARD_SIZE && (Position.second == 0 || Position.second == BOARD_SIZE))
    {
        Dame = 3;
        return;
    }
    if (Position.second > 0 && Position.second < BOARD_SIZE && (Position.first == 0 || Position.first == BOARD_SIZE))
    {
        Dame = 3;
        return;
    }

    Dame = 4;
}

Stone::~Stone()
{

}

quint8 Stone::calculateDame()
{

}
