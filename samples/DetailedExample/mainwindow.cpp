#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <residue/residue.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::log(const std::string& msg)
{
    ui->txtLog->append(QString::fromStdString(msg) + "\n");
}

void MainWindow::on_btnLoadConfiguration_clicked()
{
    try {
        Residue::loadConfigurationFromJson(ui->txtConfiguration->toPlainText().toStdString());
    } catch (ResidueException& e) {
        log(e.what());
    }
}

void MainWindow::on_btnConnect_clicked()
{
    try {
        Residue::reconnect();
        ui->txtConnection->setText(QString::fromStdString(Residue::connection()));
    } catch (ResidueException& e) {
        log(e.what());
    }
}

void MainWindow::on_btnDisconnect_clicked()
{
    Residue::disconnect();
}

void MainWindow::on_btnLoadConnection_clicked()
{
    try {
        Residue::loadConnectionFromJson(ui->txtConnection->toPlainText().toStdString());
    } catch (ResidueException& e) {
        log(e.what());
    }
}

