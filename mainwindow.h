#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <gamewindow.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_onlineGameRadioButton_toggled(bool checked);

    void on_withComputerGameRadioButton_toggled(bool checked);

    void on_shareGameRadioButton_toggled(bool checked);

    void on_connectToGameRadioButton_toggled(bool checked);

    void on_startGameButton_released();

private:
    Ui::MainWindow *ui;

    gameWindow game;
};

#endif // MAINWINDOW_H
