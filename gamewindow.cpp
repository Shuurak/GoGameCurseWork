#include "gamewindow.h"
#include "ui_gamewindow.h"

gameWindow::gameWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gameWindow)
{
    ui->setupUi(this);

    matrixGroup = new QButtonGroup;

    buttonsMatrix = new QAbstractButton* [gameConstant::BOARD_SIZE];

    for (int i = 0; i< gameConstant::BOARD_SIZE; i++)
    {
        buttonsMatrix[i] = new QPushButton[gameConstant::BOARD_SIZE];
    }

    for (int i = 0; i< gameConstant::BOARD_SIZE; i++)
    {
        for (int j = 0; j< gameConstant::BOARD_SIZE; j++)
        {
            buttonsMatrix[i][j].setParent(this);

            buttonsMatrix[i][j].setGeometry(ui->openGLWidget->geometry().x() + (ui->openGLWidget->width()/20)*0.7 + (ui->openGLWidget->width()/20)*i,
                                            ui->openGLWidget->geometry().y() + (ui->openGLWidget->height()/20)*0.7 + (ui->openGLWidget->height()/20)*j,
                                            (ui->openGLWidget->width()/20)*1.4,
                                            (ui->openGLWidget->height()/20)*1.4);


            buttonsMatrix[i][j].setVisible(true);

            matrixGroup->addButton(&buttonsMatrix[i][j], 100*i+j);
        }
    }

    QObject::connect(matrixGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(boardButton_clicked(int)));

    StonesOnGoban = new Stone* [gameConstant::BOARD_SIZE];

    for (int i=0; i<gameConstant::BOARD_SIZE; i++)
    {
        StonesOnGoban[i] = new Stone [gameConstant::BOARD_SIZE];
    }

    for (int i = 0; i< gameConstant::BOARD_SIZE; i++)
    {
        for (int j = 0; j< gameConstant::BOARD_SIZE; j++)
        {
            StonesOnGoban[i][j].setStoneData(bytePair(i, j), (i%2)&&(j%2!=1)? stoneColors::BLACK:stoneColors::WHITE);
        }
    }
}

gameWindow::~gameWindow()
{
    delete ui;

    for (int i = 0; i< gameConstant::BOARD_SIZE; i++)
    {
        delete [] buttonsMatrix[i];
    }

    delete [] buttonsMatrix;

    delete matrixGroup;
}

void gameWindow::on_pushButton_released()
{
    ui->openGLWidget->drawUnits(StonesOnGoban);
}

void gameWindow::boardButton_clicked(int indx)
{
    debugIO << "id:" << indx;

    if (StonesOnGoban[indx/100][indx%100].isValidStone())
    {
        StonesOnGoban[indx/100][indx%100].disableStone();
        matrixGroup->button(indx)->setEnabled(false);
    }/*
    else
    {
        StonesOnGoban[indx%10][indx/10].
    }*/

    ui->openGLWidget->drawUnits(StonesOnGoban);
}
