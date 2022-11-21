#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <stack>
#include <QPainter>
#include <QDialog>
#include <QLineEdit>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateNumber(int nrow, int ncol); //this function adds 1 to
                                           // the nearest 8 blocks if they are not bomb


    //int finished = 0;
    int **Map; //for saving message about the mine zone
    int **visited;
    int nMine;
    int nx; //number of rows
    int ny; //number of lines

    int winf;
    int rMine;
    bool first;
    int timer;

    std::stack<int> bfx;
    std::stack<int> bfy;

private slots:
    void mousePressEvent(QMouseEvent *event);
    void countTime();

    void setPrimary();
    void setMedium();
    void setAdvanced();
    void setUserDesign();
    void showUserDesign();

private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *event);
    void left_button_clicked(int nrow, int ncol);

    QMenuBar *menuBar;
    QMenu *gameMenu;
    QMenu *helpMenu;

    QToolBar *gameToolBar;
    QToolBar *helpToolBar;

    QAction *howToPlayAct;
    QAction *primaryAct;
    QAction *mediumAct;
    QAction *advancedAct;
    QAction *userDesignAct;

    QWidget *centralwidget;
    QWidget *paintZone;
    QVBoxLayout *mainEditorLayout;
    //QHBoxLayout *setting;
    QDialog *settingWindow;
    QLineEdit *xEdit;
    QLineEdit *yEdit;
    QLineEdit *mEdit;

    void initiate();

    int winFlag = 0;
    // 0: on process; 1: win; 2: lose;

    void mineSearch(int nrow, int ncol);

    int startTimer; //if 0, do not start time.
    int clockTime;
    QTimer *runTimer;

    QPainter painter;
    QPixmap bmpmap;
    QPixmap bmpnumber;
    QPixmap bmpframe;
    QPixmap bmpface;

//    void setPrimary();
//    void setMedium();
//    void setAdvanced();

    int nUsedFlags;
    int unUsedFlags;

};

#endif // MAINWINDOW_H
