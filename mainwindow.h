#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    bool initHIKSDK();
    long loginDevice();
private slots:
    void on_LoginBtn_clicked();

    void on_RealPlayBtn_clicked();

private:
    Ui::MainWindow *ui;
    bool isLogin;
    long g_lUserID;
};

#endif // MAINWINDOW_H
