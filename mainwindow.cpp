#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>

int WINDOW_WIDTH;
int WINDOW_HEIGHT;
int MENU_WIDTH;
int MENU_HEIGHT;
int TIMER_WIDTH;
int TIMER_HEIGHT;
int UNIT_LENGTH;
int TIMER_UNIT_LENGTH;
int zx, zy;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    paintZone = new QWidget;
    this->setCentralWidget(paintZone);

    menuBar = new QMenuBar(this);
    gameMenu = new QMenu(QString::fromLocal8Bit("game"));
    helpMenu = new QMenu(QString::fromLocal8Bit("help"));

    primaryAct = new QAction(QString::fromLocal8Bit("Primary"), this);
    mediumAct = new QAction(QString::fromLocal8Bit("Medium"), this);
    advancedAct = new QAction(QString::fromLocal8Bit("Advanced"), this);
    userDesignAct = new QAction(QString::fromLocal8Bit("User Design"), this);
    howToPlayAct = new QAction(QString::fromLocal8Bit("how to play"), this);



    gameMenu->addAction(primaryAct);
    gameMenu->addAction(mediumAct);
    gameMenu->addAction(advancedAct);
    gameMenu->addAction(userDesignAct);
    helpMenu->addAction(howToPlayAct);

    menuBar->addMenu(gameMenu);
    menuBar->addMenu(helpMenu);

    menuBar->addSeparator();
    this->setMenuBar(menuBar);

    this->statusBar()->setSizeGripEnabled(false); //cancel the changing-size symbol

    //loading pictures
    bmpmap = QPixmap(":/new/prefix1/pictures/complete.bmp");
    bmpnumber = QPixmap(":/new/prefix1/pictures/complete3.bmp");
    bmpframe = QPixmap(":/new/prefix1/pictures/complete2.bmp");
    bmpface = QPixmap(":/new/prefix1/pictures/complete4.bmp");

    UNIT_LENGTH = bmpmap.height();
    MENU_WIDTH = menuBar->width();
    TIMER_UNIT_LENGTH = bmpnumber.width()/10;
    MENU_HEIGHT = menuBar->height();
    TIMER_WIDTH = bmpframe.width();
    TIMER_HEIGHT = bmpframe.height();

    //this->showMaximized();
    nx = 16;
    ny = 16;
    nMine = 5; //number of mine

    runTimer = new QTimer(this);
    connect(runTimer, SIGNAL(timeout()), this, SLOT(countTime()));
    initiate();
    connect(primaryAct, SIGNAL(triggered()), this, SLOT(setPrimary()));
    connect(mediumAct, SIGNAL(triggered()), this, SLOT(setMedium()));
    connect(advancedAct, SIGNAL(triggered()), this, SLOT(setAdvanced()));
    connect(userDesignAct, SIGNAL(triggered()), this, SLOT(showUserDesign()));

    QVBoxLayout *setting = new QVBoxLayout;
    settingWindow = new QDialog(this);
    QLabel *labelx = new QLabel(QString::fromLocal8Bit("Columns: "));
    QLabel *labely = new QLabel(QString::fromLocal8Bit("Rows: "));
    QLabel *labelm = new QLabel(QString::fromLocal8Bit("Mines: "));
    xEdit = new QLineEdit;
    yEdit = new QLineEdit;
    mEdit = new QLineEdit;
    QPushButton *okButton = new QPushButton(QString::fromLocal8Bit("Ok"));
    QPushButton *cancelButton = new QPushButton(QString::fromLocal8Bit("Cancel"));
    //connect(findButton, &QPushButton::clicked, this, &MainWindow::showFindResult);
    setting->addWidget(labelx);
    setting->addWidget(xEdit);
    setting->addWidget(labely);
    setting->addWidget(yEdit);
    setting->addWidget(labelm);
    setting->addWidget(mEdit);
    xEdit->setText(QString::number(nx));
    yEdit->setText(QString::number(ny));
    mEdit->setText(QString::number(nMine));
    xEdit->setValidator(new QIntValidator(1,100,xEdit));
    yEdit->setValidator(new QIntValidator(1,100,yEdit));
    mEdit->setValidator(new QIntValidator(1,nx*ny-1,mEdit));
    QHBoxLayout *settingButtons = new QHBoxLayout;
    settingButtons->addWidget(okButton);
    settingButtons->addWidget(cancelButton);
    setting->addWidget(okButton);
//    setting->addWidget(cancelButton);
    //setting->addWidget(settingButtons);
    settingWindow->setLayout(setting);
    //settingWindow->setL
    //findWindow->setWindowFlags(Qt::WindowStaysOnTopHint);
    settingWindow->hide();
    settingWindow->setWindowTitle("Setting");
    connect(okButton, SIGNAL(clicked()), this, SLOT(setUserDesign()));
}

MainWindow::~MainWindow()
{
    for(int i = 0; i < ny; i++) {
        delete[]Map[i];
    }
    delete[] Map;
    delete ui;

}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    //draw the frame
    QRect source_leftFrame(0, 0, 70, 40);
    QRect target_leftFrame(0, MENU_HEIGHT, 3*TIMER_UNIT_LENGTH, TIMER_HEIGHT);
    painter.drawPixmap(target_leftFrame, bmpframe, source_leftFrame);

    QRect source_rightFrame(130, 0, 70, 40);
    QRect target_rightFrame(WINDOW_WIDTH-3*TIMER_UNIT_LENGTH, MENU_HEIGHT,
                            3*TIMER_UNIT_LENGTH, 40);
    painter.drawPixmap(target_rightFrame, bmpframe, source_rightFrame);

    QRect source_leftFrame2(70, 0, 10, 40);
    QRect target_leftFrame2(3*TIMER_UNIT_LENGTH, MENU_HEIGHT,
                            WINDOW_WIDTH*0.5-3*TIMER_UNIT_LENGTH-20, TIMER_HEIGHT);
    painter.drawPixmap(target_leftFrame2, bmpframe, source_leftFrame2);

    QRect source_rightFrame2(120, 0, 10, 40);
    QRect target_rightFrame2(WINDOW_WIDTH*0.5+20, MENU_HEIGHT,
                             WINDOW_WIDTH*0.5-3*TIMER_UNIT_LENGTH-20, TIMER_HEIGHT);
    painter.drawPixmap(target_rightFrame2, bmpframe, source_rightFrame2);

    QRect source_facePlacer(80, 0, 40, 40);
    QRect target_facePlacer(WINDOW_WIDTH*0.5-20, MENU_HEIGHT, 40, 40);
    painter.drawPixmap(target_facePlacer, bmpframe, source_facePlacer);

    //draw the face
    QRect source_onProcess((bmpface.width()/4), 0, bmpface.width()/4, bmpface.width()/4);
    QRect source_win((bmpface.width()*2/4), 0, bmpface.width()/4, bmpface.width()/4);
    QRect source_lose(0, 0, bmpface.width()/4, bmpface.width()/4);
    QRect source_mouseUnreleased((bmpface.width()*3/4), 0, bmpface.width()/4, bmpface.width()/4);
    QRect target_face(WINDOW_WIDTH*0.5-12, MENU_HEIGHT+0.5*TIMER_HEIGHT-12, 24, 24);

    if (winFlag == 0) painter.drawPixmap(target_face, bmpface, source_onProcess); //on process
    else if (winFlag == 1) painter.drawPixmap(target_face, bmpface, source_win); //win
    else if (winFlag == 2) painter.drawPixmap(target_face, bmpface, source_lose); //lose

    //draw the clock numbers
    int num1 = clockTime/100;
    int num2 = (clockTime%100)/10;
    int num3 = clockTime%10;
    painter.drawPixmap(WINDOW_WIDTH-56, 36, 17, 26, bmpnumber, (bmpnumber.width()*num1/10), 0, (bmpnumber.width()/10), bmpnumber.height());
    painter.drawPixmap(WINDOW_WIDTH-39, 36, 17, 26, bmpnumber, (bmpnumber.width()*num2/10), 0, (bmpnumber.width()/10), bmpnumber.height());
    painter.drawPixmap(WINDOW_WIDTH-22, 36, 17, 26, bmpnumber, (bmpnumber.width()*num3/10), 0, (bmpnumber.width()/10), bmpnumber.height());

    //draw the number of unused little red flags
    // num of unused flags = nMine - nUsedFlags
    unUsedFlags = nMine - nUsedFlags;
    int num4 = unUsedFlags/100;
    int num5 = (unUsedFlags%100)/10;
    int num6 = unUsedFlags%10;
    qDebug() << "check unUsedFlags :" << num4 << " " << num5 <<" "<< num6;
    painter.drawPixmap(5, 36, 17, 26, bmpnumber, (bmpnumber.width()*num4/10), 0, (bmpnumber.width()/10), bmpnumber.height());
    painter.drawPixmap(22, 36, 17, 26, bmpnumber, (bmpnumber.width()*num5/10), 0, (bmpnumber.width()/10), bmpnumber.height());
    painter.drawPixmap(39, 36, 17, 26, bmpnumber, (bmpnumber.width()*num6/10), 0, (bmpnumber.width()/10), bmpnumber.height());

    //draw mine zone
    QRectF source_pressed((bmpmap.width()/13)*0, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_1((bmpmap.width()/13)*1, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_2((bmpmap.width()/13)*2, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_3((bmpmap.width()/13)*3, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_4((bmpmap.width()/13)*4, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_5((bmpmap.width()/13)*5, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_6((bmpmap.width()/13)*6, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_7((bmpmap.width()/13)*7, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_8((bmpmap.width()/13)*8, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_bomb((bmpmap.width()/13)*9, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_unpressed((bmpmap.width()/13)*10, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_marked((bmpmap.width()/13)*11, 0.0, (bmpmap.width()/13), bmpmap.height());
    QRectF source_bombTriggered((bmpmap.width()/13)*12, 0.0, (bmpmap.width()/13), bmpmap.height());

   /*painter.drawPixmap(QPointF(zx+UNIT_LENGTH*2, zy + 2*UNIT_LENGTH), bmpmap, source_bomb);*/ //for trial

    for (int i = 1; i <= ny; i ++) {
        for (int j = 1; j <= nx; j ++) {

//            if (Map[i][j] == 0) {
//                painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy+(i-1)*UNIT_LENGTH), bmpmap, source_bomb);
//            }

            if ((Map[i][j] >= 0 && Map[i][j] <= 8) || Map[i][j] == 99) {
                painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy+(i-1)*UNIT_LENGTH), bmpmap, source_unpressed);
            }
            if ((Map[i][j] <= 58 && Map[i][j] >= 50) || Map[i][j] == 149) {
                painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_marked);
            }
            if (Map[i][j] == 200) {
                painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_bombTriggered);
            }
            if (Map[i][j] == 199) {
                painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_bomb);
            }

            if (Map[i][j] <= 108 && Map[i][j] >= 100) {
                switch(Map[i][j]) {
                    case 100:
                        painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_pressed);
                        break;
                    case 101:
                        painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_1);
                        break;
                    case 102:
                        painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_2);
                        break;
                    case 103:
                        painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_3);
                        break;
                    case 104:
                        painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_4);
                        break;
                    case 105:
                        painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_5);
                        break;
                    case 106:
                        painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_6);
                        break;
                    case 107:
                        painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_7);
                        break;
                    case 108:
                        painter.drawPixmap(QPoint(zx+(j-1)*UNIT_LENGTH, zy + (i-1)*UNIT_LENGTH), bmpmap, source_8);
                        break;
                }
            }
        }
    }
}

void MainWindow::updateNumber(int nrow, int ncol) {
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j<= 1; j++) {
            if (Map[nrow+i][ncol+j] == 99) continue;
            Map[nrow+i][ncol+j] += 1;
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    int px = event->x();
    int py = event->y();
    qDebug() << "click : " << px << " " << py;

    if (px < 0 || py-zy < 0) {
        if ( (px >= WINDOW_WIDTH*0.5-12  && px <= WINDOW_WIDTH*0.5+12)
             && (py <= 61 && py >= 37 )) {
            qDebug() << "click face ~";
            qDebug() << "click face ~";
            initiate();
            update();
            //renew the game;
        }
        return;
    }

    //if click mine zone
    //if pressing the first block in a new game
    if (startTimer == 0){
        qDebug() << "start timing!";
        runTimer->start(1000);
    }
    startTimer++;
    runTimer->start();

    if (winFlag != 0) return;
    int ncol = (px-zx)/UNIT_LENGTH + 1; //nrow starts from 1 to nx
    int nrow =(py-zy)/UNIT_LENGTH + 1;
//    qDebug() << "click2 : " << nrow << " " << ncol << " " <<Map[nrow][ncol];
    if (event->buttons() == Qt::LeftButton) {
        left_button_clicked(nrow, ncol);
        if(Map[nrow][ncol] <= 8 && Map[nrow][ncol] >= 0) {
            //Map[nrow][ncol] += 100;
            mineSearch(nrow, ncol);
            int k = 0;
            for (int i = 1; i <= ny; i ++) {
                for (int j = 1; j <= nx; j ++) {
                    if (Map[i][j] == 99 || (Map[i][j] <= 8 && Map[i][j] >= 0) ||
                            (Map[i][j] <= 58 && Map[i][j] >= 50) || Map[i][j] == 149) k++;
                }
            }
            if (k == nMine) winFlag = 1;
            qDebug() << "k " << k;
        } else if(Map[nrow][ncol] == 99) {
            runTimer->stop();
            qDebug()<<"Mine";
            //endGame;
            winFlag = 2;
            for (int i = 1; i <= ny; i ++) {
                for (int j = 1; j <= nx; j ++) {
                    if (Map[i][j] == 99 || Map[i][j] == 149) Map[i][j] = 199;
                }
            }
            Map[nrow][ncol] = 200;
        }
        if (winFlag == 1) {
            runTimer->stop();
            for (int i = 1; i <= ny; i ++) {
                for (int j = 1; j <= nx; j ++) {
                    if (Map[i][j] == 99 || Map[i][j] == 149) Map[i][j] = 199;
                }
            }
        }
    }
    // Win?
    else if (event->buttons() == Qt::RightButton) {
        //click right button
        //mark the block
        if(Map[nrow][ncol] <= 8 && Map[nrow][ncol] >= 0) {
            if (nUsedFlags < nMine) {
                Map[nrow][ncol] += 50;
                nUsedFlags++;
            }
        } else if(Map[nrow][ncol] == 99) {
            if (nUsedFlags < nMine) {
                Map[nrow][ncol] = 149;
                nUsedFlags++;
            }
        } else if(Map[nrow][ncol] <= 58 && Map[nrow][ncol] >= 50) {
            if (nUsedFlags >= 1) {
                Map[nrow][ncol] -= 50;
                nUsedFlags--;
            }
        } else if(Map[nrow][ncol] == 149) {
            if (nUsedFlags >= 1) {
                Map[nrow][ncol] = 99;
                nUsedFlags--;
            }
        }
    }
    update();
}

void MainWindow::left_button_clicked(int nrow, int ncol) {
    if (Map[ncol][nrow] == 99) {
        // lose the game : turn winFlag to 2;
    }

}

void MainWindow::mineSearch(int nrow, int ncol) {
    while(!bfx.empty()) bfx.pop(), bfy.pop();
    if(visited[nrow][ncol] == 1) return;
//    qDebug()<<"In "<< nrow << " " << ncol << " " <<Map[nrow][ncol] <<" "<<visited[nrow][ncol];
    //visited[nrow][ncol] = 1;
    if (Map[nrow][ncol] > 0 && Map[nrow][ncol] <= 8) {
        visited[nrow][ncol] = 1;
        Map[nrow][ncol] += 100;
        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j<= 1; j++) {
                if (i==0 || j == 0) {
                    if (visited[nrow+i][ncol+j] == 1) continue;
                    if (Map[nrow+i][ncol+j] == 0) {
                        bfx.push(nrow+i);
                        bfy.push(ncol+j);
                        Map[nrow+i][ncol+j] += 100;
                    }
                }
            }
        }
    } else if(Map[nrow][ncol] == 0) bfx.push(nrow), bfy.push(ncol);
    while(!bfx.empty()) {
        int xrow = bfx.top();
        int ycol = bfy.top();
        bfx.pop();
        bfy.pop();
//        qDebug()<<"In "<< xrow << " " << ycol << " " <<Map[xrow][ycol];
        if(xrow <= 0 || xrow > ny || ycol <= 0 || ycol > nx) continue;

        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j<= 1; j++) {
                if (visited[xrow+i][ycol+j] == 1) continue;
                if (Map[xrow+i][ycol+j] >= 0 && Map[xrow+i][ycol+j] <= 8) {
                    //visited[ncol][nrow] = 1;
                    if(Map[xrow+i][ycol+j] == 0) {
                        bfx.push(xrow+i);
                        bfy.push(ycol+j);
                    }
//                    qDebug()<<"In1 "<< xrow+i << " " << ycol+j << " " <<Map[xrow+i][ycol+j];
                    Map[xrow+i][ycol+j] += 100;
                }
            }
        }
        visited[xrow][ycol] = 1;
    }
}

void MainWindow::initiate() {
    qDebug() << "runTime activated? " << runTimer->isActive();
    runTimer->stop();
    qDebug() << "runTime activated? " << runTimer->isActive();
    clockTime = 0;
    startTimer = 0;
    winFlag = 0;
    nUsedFlags = 0;
    WINDOW_WIDTH = UNIT_LENGTH*nx;
    WINDOW_HEIGHT = UNIT_LENGTH*ny + MENU_HEIGHT + TIMER_HEIGHT;

    zx = 0; zy = MENU_HEIGHT+TIMER_HEIGHT;
    setMaximumSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMinimumSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    this->resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    qDebug() << "size " << WINDOW_WIDTH<< " " <<WINDOW_HEIGHT;
    //renew game with current nx, ny, nmine
    Map = new int *[ny+2];
    visited = new int *[ny+2];
    for (int i = 0; i < ny+2; i++) {
        Map[i] = new int[nx+2];
        visited[i] = new int[nx+2];
    }

    //correct the number of mine if necessary
    int nrow, ncol;
    if (nMine > nx*ny) {
        nMine = nx*ny;
    }

    //set initial value 0 to all blocks
    for(int i=1; i <= ny; i++) {
        for (int j=1; j<= nx; j++) {
            Map[i][j] = 0;
            visited[i][j] = 0;
        }
    }

//    //spread the mine in random selected blocks
    int k = 0;

    while(k < nMine) {
        nrow = rand()%ny+1;
        ncol = rand()%nx+1;
        if (Map[nrow][ncol] == 99) continue;
        //update the digits on nearest 8 blocks
        Map[nrow][ncol] = 99;
        updateNumber(nrow, ncol);
        k++;
    }
}

void MainWindow::countTime() {
    clockTime++;
    qDebug() << "clock: " << clockTime;
    update();
}

void MainWindow::setPrimary() {
    nx = 16;
    ny = 16;
    nMine = 5; //number of mine
    runTimer = new QTimer(this);
    connect(runTimer, SIGNAL(timeout()), this, SLOT(countTime()));
    initiate();
    update();
}

void MainWindow::setMedium() {
    nx = 25;
    ny = 25;
    nMine = 5; //number of mine
    runTimer = new QTimer(this);
    connect(runTimer, SIGNAL(timeout()), this, SLOT(countTime()));
    initiate();
    update();
}

void MainWindow::setAdvanced() {
    nx = 50;
    ny = 30;
    nMine = 5; //number of mine
    runTimer = new QTimer(this);
    connect(runTimer, SIGNAL(timeout()), this, SLOT(countTime()));
    initiate();
    update();
}

void MainWindow::showUserDesign() {
    settingWindow->show();
}

void MainWindow::setUserDesign() {
    nx = xEdit->text().toInt();
    ny = yEdit->text().toInt();
    nMine = mEdit->text().toInt();
    if(nx < 12 || ny < 12 || nMine == 0 || nMine >= nx*ny) {
        QMessageBox message;
        message.setWindowTitle(QString::fromLocal8Bit("Warning"));
        message.setIcon(QMessageBox::Warning);
        message.setText(QString::fromLocal8Bit("Invalid setting."));
//        QIcon icon;
//        icon.addFile(QString::fromLocal8Bit(":/resource/icon/file-1.png"), QSize(), QIcon::Normal, QIcon::Off);
//        message.setWindowIcon(icon);
        message.setStandardButtons(QMessageBox::Ok);
        message.exec();
        return;
    }
    settingWindow->hide();
    initiate();
    update();
}



