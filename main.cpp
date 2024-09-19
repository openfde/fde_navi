#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include<QDateTime>
#include <QProcess>
#include "draggable_button.h"



//添加main方法
int main(int argc , char * argv[]){
    QApplication app(argc, argv);
    //新增一个托盘图标功能
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(":/images/tray_icon.png"), &app);
    QMenu *trayMenu = new QMenu();
    QAction *quitAction = new QAction("退出", &app);
    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();


    QRect screenRect = QGuiApplication::primaryScreen()->geometry();

    DraggableButton *rbtn = new DraggableButton(RIGHT,":/images/right.png",screenRect);
    DraggableButton *lbtn = new DraggableButton(LEFT,":/images/left.png",screenRect);
    rbtn->setWindowOpacity(0.8);
    rbtn->setStyleSheet("background-color: rgba(192,192,192,0.8);");
    lbtn->setWindowOpacity(0.8);
    lbtn->setStyleSheet("background-color: rgba(192,192,192, 0.8);");

//	rbtn->setStyleSheet("QWidget{border-radius: 50px;  }");
    //设置draggableButton的大小为100 * 100
    rbtn->setFixedSize(100, 100);
    lbtn->setFixedSize(100, 100);
    //rbtn 移动到屏幕的右侧中间
    rbtn->move(screenRect.width() - rbtn->width(), screenRect.height() / 2 - rbtn->height() / 2);
    //lbtn 移动到屏幕的左侧中间
    lbtn->move(0, screenRect.height() / 2 - lbtn->height() / 2);
    //设置draggableButton的另一个按钮为lbtn
    rbtn->setOtherButton(lbtn);
    lbtn->setOtherButton(rbtn);
    lbtn->show();
    rbtn->show();
    return app.exec();
}
