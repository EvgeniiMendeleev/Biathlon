#include "mainwindow.h"
#include "ui_mainwindow.h"

#define WidthOfFrame 580
#define HeightOfFrame 415

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(QSize(WidthOfFrame, HeightOfFrame));
    ui->Frame->setFixedSize(QSize(WidthOfFrame, HeightOfFrame));
    ui->Frame->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->Frame->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene = new QGraphicsScene;
    MyTimer = new QTimer;
    MyTimer->setInterval(50);
}

void MainWindow::Main_Menu_off()
{
    ui->MainPicture->hide();
    ui->InputLine->hide();
    ui->Connection->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Connection_clicked()
{
    QString IPaddress = ui->InputLine->text();

    if(IPaddress == NULL)
    {
        QMessageBox::information(this, "Ошибка", "Адрес сервера не был введён");
    }
    else
    {
        QByteArray tmp = IPaddress.toLatin1();
        const char* IP = tmp.data();

        ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        struct sockaddr_in ServAddr;

        ServAddr.sin_family = AF_INET;
        ServAddr.sin_port = htons(25678);
        inet_pton(AF_INET, IP, &(ServAddr.sin_addr));

        if(::connect(ClientSocket, (struct sockaddr*) &ServAddr, sizeof(ServAddr)) != -1)
        {
            connect(MyTimer, &QTimer::timeout, this, &MainWindow::ReadFromServer);
            fcntl(ClientSocket, F_SETFL, O_NONBLOCK);
            MyTimer->start();
        }
        else
        {
            QMessageBox::information(this, "Ошибка", "Введён неправильный адрес или сервер не запущен");
        }
    }
}

void MainWindow::BATTLE()
{

}

void MainWindow::ReadFromServer()
{
    int16_t buffer[2];

    if(recv(ClientSocket, &buffer, sizeof(int16_t), MSG_NOSIGNAL) > 0)
    {
       Msg_type type = static_cast<Msg_type>(buffer[0]);

       if(type == state_for_client)
       {
           recv(ClientSocket, &buffer[1], sizeof(int16_t), MSG_NOSIGNAL);

           States state = static_cast<States>(buffer[1]);

           if(state == WaitingOfConnection)
           {
               Main_Menu_off();

               QPixmap frame;

               frame.load(":/img/WaitingOfConnection.jpg");
               frame = frame.scaled(WidthOfFrame, HeightOfFrame);
               scene->addPixmap(frame);

               ui->Frame->setScene(scene);
               ui->Frame->show();
           }
           else if(state == Combat)
           {
               qDebug() << "It's a BATTLE, baby!\n";
           }
           else if(state == Win)
           {

           }
           else if(state == Lose)
           {

           }
       }
       else if(type == result_of_shot)
       {

       }
    }
}
