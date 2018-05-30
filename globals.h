#ifndef GLOBALS_H
#define GLOBALS_H

#define byte quint8

#include <QPair>
#include <QDebug>

#define bytePair(x,y) QPair<byte, byte>((x),(y))

#define bytePairType QPair<byte, byte>

#define QINT64_MINIMUM LONG_LONG_MIN

#define QINT64_MAXIMUM LONG_LONG_MAX

#define debugIO qDebug()

namespace GlobalConstants
{
    enum gameConstant
    {
        /* коми - компенсация белым, за то
         * что чёрные ходят первыми
         *
         * BOARD_SIZE_INDEXSED - максимальный индекс на доске
         * BOARD_SIZE - размер для циклов
         */
        KOMI = 7,
        BOARD_SIZE_INDEXSED = 18,
        BOARD_SIZE = 19
    };

    enum stoneColors
    {
        /* фракции, существующие на доске
         * белые и чёрные
         *
         * компьютер ходит белыми
         * игрок ходит чёрными
         */
        BLACK,
        WHITE,
        SHARE_TERRITORY
    };

    enum gameStages
    {
        /* игра в го формально разбивается на три этапа
         * фусэки, тюбан и ёсэ
         *
         * эти этапы можно определить по заполненности доски
         * в процентах
         */
        FUSEKI = 9,
        TYUBAN = 10,
        YOSE = 85
    };

    enum sectorSignificance
    {
        /* значимость линий в го определяется
         * путём развития камню, поставленного на эту линию
         * камни на линии 1 - убегающие камни, их очень просто поймать
         *
         * камни на линиях 2 и 3 - основоположники/защитники группы
         * однако без них в некоторых случаях можно обойтись
         *
         * камни на границах го (4-6 линии) - владельцы умения связывать
         * небо и земли - они помогают группам или отдельным камням
         * добраться с земли на небеса или наоборот
         *
         * камни  камни точно на границе хоши - называют оракулами
         * эти камни позволяют построить мощьное влияние
         * вокруг своего угла/стороны
         *
         * в соответствии с важностью линий им проставленны коэффициенты
         * важности
         */
        LINE_1 = -5,
        LINES_2AND3 = -4,
        LINES_4TO6 = 1,
        HOSHI_LINES = 2
    };
}

const QString gameMainVer = "0.1b15";

#ifdef Q_OS_WIN
    #define CLEAR_SCREEN_OS "cls"
    #define PLATFORM_TYPE_DETECTED "WIN"
#else
    #define CLEAR_SCREEN_OS "clear"
    #define PLATFORM_TYPE_DETECTED "UNIX"
#endif


const QString platformType = PLATFORM_TYPE_DETECTED;

#endif // GLOBALS_H
