#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QPixmap>
#include <QGraphicsScene>

#include <unistd.h>

//-------------Работа с сетью---------
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
//------------------------------------

namespace Ui {
class MainWindow;
}

struct StateForClient
{
    int16_t type;
    int16_t state;
};

enum States {WaitingOfConnection, Win, Lose, Combat};
enum Msg_type {result_of_shot, state_for_client};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void Main_Menu_off();
    void BATTLE();

private slots:
    void on_Connection_clicked();
    void ReadFromServer();

private:
    Ui::MainWindow *ui;

    QGraphicsScene* scene;
    QTimer* MyTimer;

    int ClientSocket;
};

#endif // MAINWINDOW_H
