#ifndef STONE_H
#define STONE_H

#include <globals.h>
#include <QObject>

using namespace GlobalConstants;


class Stone : public QObject
{
    Q_OBJECT
public:
    Stone();

    Stone(Stone const &_reference);

    Stone(QPair<byte, byte>, stoneColors stoneFraction);    //конструктор класса

    ~Stone();

    inline stoneColors getFraction()
    {
        return Fraction;
    }

    inline byte getDame()
    {
        return Dame;
    }

    inline bool isValidStone()
    {
        return validStone;
    }

    inline bool isValidTerritory()
    {
        return validTerritory;
    }

    inline void setTerritoryColor(stoneColors terr)
    {
        territoryColor = terr;

        validTerritory = true;
    }

    inline stoneColors getTerritoryColor()
    {
        return territoryColor;
    }

    inline Stone& operator = (Stone right)
    {
        if (&right == this)
        {
            return *this;
        }
        else
        {
            this->Position = right.Position;
            this->Fraction = right.Fraction;
            this->Dame = right.Dame;

            return *this;
        }
    }

    inline void setStoneData(QPair<byte, byte> _position, stoneColors _fraction)
    {
        Position = _position;
        Fraction = _fraction;

        calculateDame();
        validStone = true;
    }

    inline bool disableStone()
    {
        debugIO << Q_FUNC_INFO << Position;

        validStone = false;
    }

private:
    bool validStone = false;

    bool validTerritory = false;

    QPair<byte, byte> Position;

    stoneColors Fraction;

    stoneColors territoryColor;

    byte Dame;

    byte calculateDame();

//signals:

//public slots:
};

#endif // STONE_H
