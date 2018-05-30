#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Создаем строку для регулярного выражения */
   QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
   /* Создаем регулярное выражение с применением строки, как
    * повторяющегося элемента
    */
   QRegExp ipRegex ("^" + ipRange
                    + "\\." + ipRange
                    + "\\." + ipRange
                    + "\\." + ipRange + "$");
   /* Создаем Валидатор регулярного выражения с применением
    * созданного регулярного выражения
    */
   QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
   /* Устанавливаем Валидатор на QLineEdit */
   ui->ipLine->setValidator(ipValidator);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_onlineGameRadioButton_toggled(bool checked)
{
    if (checked)
    {
        ui->withComputerGameRadioButton->setChecked(false);
        ui->onlineGameGroupBox->setEnabled(true);
        ui->shareGameRadioButton->setEnabled(true);
        ui->shareGameRadioButton->setChecked(true);

        ui->onlineParametersGroupBox->setEnabled(true);
        ui->portGroupBox->setEnabled(true);
    }
}

void MainWindow::on_withComputerGameRadioButton_toggled(bool checked)
{
    if (checked)
    {
        ui->onlineGameRadioButton->setChecked(false);
        ui->onlineGameGroupBox->setEnabled(false);

        ui->onlineParametersGroupBox->setEnabled(false);
        ui->ipGroupBox->setEnabled(false);
        ui->portGroupBox->setEnabled(false);
    }
}

void MainWindow::on_shareGameRadioButton_toggled(bool checked)
{
    if (checked && ui->onlineGameRadioButton->isChecked())
    {
        ui->connectToGameRadioButton->setChecked(false);
        ui->onlineParametersGroupBox->setEnabled(true);
        ui->portGroupBox->setEnabled(true);

        ui->ipGroupBox->setEnabled(false);
    }
}

void MainWindow::on_connectToGameRadioButton_toggled(bool checked)
{
    if (checked && ui->onlineGameRadioButton->isChecked())
    {
        ui->shareGameRadioButton->setChecked(false);
        ui->onlineGameGroupBox->setEnabled(true);
        ui->portGroupBox->setEnabled(true);

        ui->ipGroupBox->setEnabled(true);
    }
}

void MainWindow::on_startGameButton_released()
{
    this->hide();
    game.show();
//    generateGame();
}
