#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <thread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void log(const std::string&);

private slots:
    void on_btnLoadConfiguration_clicked();

    void on_btnConnect_clicked();

    void on_btnDisconnect_clicked();

    void on_btnLoadConnection_clicked();

    void on_btnLog_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    std::vector<std::thread> m_threads;
};

#endif // MAINWINDOW_H
