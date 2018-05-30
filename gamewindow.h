#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <globals.h>

#include <stone.h>

namespace Ui {
class gameWindow;
}

class gameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit gameWindow(QWidget *parent = 0);
    ~gameWindow();

private slots:

    void on_pushButton_released();

    void boardButton_clicked(int indx);

signals:

//    void

private:
    Ui::gameWindow *ui;

    int WHITE = 0;

    Stone **StonesOnGoban = 0;

    QAbstractButton **buttonsMatrix = 0;

    QButtonGroup *matrixGroup = 0;
};

#endif // GAMEWINDOW_H
