#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QDateTime>
#include <residue/residue.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Residue::setInternalLoggingLevel(Residue::InternalLoggingLevel::crazy);
}

MainWindow::~MainWindow()
{
    Residue::disconnect();
    log("Clearing existing threads...");
    for (auto& t : m_threads) {
        t.join();
    }
    m_threads.clear();
    delete ui;
}

void MainWindow::log(const std::string& msg)
{
    ui->txtLog->append(QDateTime::currentDateTime().toString() + " " + QString::fromStdString(msg));
    ui->txtLog->verticalScrollBar()->setValue(ui->txtLog->verticalScrollBar()->maximum());
}

void MainWindow::on_btnLoadConfiguration_clicked()
{
    try {
        Residue::loadConfigurationFromJson(ui->txtConfiguration->toPlainText().toStdString());
        log("Configuration loaded");
    } catch (ResidueException& e) {
        log(e.what());
    }
}

void MainWindow::on_btnConnect_clicked()
{
    try {
        Residue::reconnect();
        ui->txtConnection->setPlainText(QString::fromStdString(Residue::connection()));
        on_pushButton_clicked();
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
        on_pushButton_clicked();
    } catch (ResidueException& e) {
        log(e.what());
    }
}


void MainWindow::on_btnLog_clicked()
{
    if (!Residue::connected()) {
        log("Not connected");
        return;
    }
    if (ui->txtMsg->text().isEmpty()) {
        return;
    }

    if (!m_threads.empty()) {
        log("Clearing existing threads...");
        for (auto& t : m_threads) {
            t.detach();
        }
        m_threads.clear();
    }
#define SEND(LEVEL) CLOG(LEVEL, ui->txtLogger->text().toStdString().c_str()) << ui->txtMsg->text().replace("%c", QString::number(j)).toStdString();

    log("Creating " + std::to_string(ui->spnThreads->value()) + " threads");
    log("Sending " + std::to_string(ui->spnMsgPerThread->value()) + " messages per thread");
    log("Sending " + std::to_string(ui->spnMsgPerThread->value() * ui->spnThreads->value()) + " messages altogether");

    auto createThread = [&](int i) -> std::thread {
        return std::thread([&]() {
            Residue::setThreadName("Thread " + std::to_string(i));

            for (int j = 1; j <= ui->spnMsgPerThread->value(); ++j) {
                if (ui->rdoLevelInfo->isChecked()) {
                    SEND(INFO);
                } else if (ui->rdoLevelError->isChecked()) {
                    SEND(ERROR);
                } else if (ui->rdoLevelDebug->isChecked()) {
                    SEND(DEBUG);
                } else if (ui->rdoLevelVerbose->isChecked()) {
                    CVLOG(1, ui->txtLogger->text().toStdString().c_str()) << ui->txtMsg->text().replace("%c", QString::number(j)).toStdString();
                }
            }
        });
    };

    for (int i = 1; i <= ui->spnThreads->value(); ++i) {
        // initialize threads
        m_threads.push_back(createThread(i));
    }

    log("All logs are being dispatched now in each thread");
    if (ui->chkWait->isChecked()) {
        log("Joining threads (waiting)");
        for (auto& t : m_threads) {
            t.join();
        }
        m_threads.clear();
    }
    log("Finished all the threads and dispatch");


#undef SEND
}

void MainWindow::on_pushButton_clicked()
{
    if (Residue::connected()) {
        log("Connection is estabilished");
        log(Residue::info());
    } else {
        log("Not connected");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->txtLog->clear();
}
