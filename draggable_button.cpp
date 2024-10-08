#include "draggable_button.h"

DraggableButton::DraggableButton(ArrayDirection direction, const QString &iconPath, QRect rect, QWidget *parent ) : QWidget(parent) {
    //设置为无框tool 模式，且总在最上面
    setWindowFlags( Qt::FramelessWindowHint |Qt::ToolTip|  Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    button = new QPushButton("", this);
    button->setStyleSheet("QPushButton{border-radius: 15px; }");
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(80, 80));
    button->setFixedSize(100, 100);
    reservedIconPath = iconPath;
    reservedRect = rect;
    //connect button的按下事件和一个槽函数
    connect(button, &QPushButton::pressed, this, &DraggableButton::onButtonPressed);
    //connect button的释放事件和一个槽函数
    connect(button, &QPushButton::released, this, &DraggableButton::onButtonReleased);

    button->setAutoRepeat(false);
    setMouseTracking(true);
    //标记自己的方向
    arrayDirection = direction;
    QTimer::singleShot(3000, this, [this](){
        moveByHoverLeaved();
    });
 }

void DraggableButton::onButtonPressed(){
        lastPressedTime = QDateTime::currentDateTime();
        QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, button->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	dragging = true;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
	delete event;
}

void DraggableButton::onButtonReleased(){
    //如果按下的时刻和释放的时刻相差小于200ms，则认为是点击
    if (lastPressedTime.msecsTo(QDateTime::currentDateTime()) < 200 ) {
        //判断鼠标左键现在是否依然是按下的
        if (QApplication::mouseButtons() & Qt::LeftButton) { //鼠标依然按下，说明只是焦点离开了按钮，用户依然在长按拖动
            return;
        }else if ( !longPressed){ 
            return moveByClick();
        }
    }
    longPressed = true;
    if ((longPressed) && !(QApplication::mouseButtons() & Qt::LeftButton)) { //鼠标左键已然弹
        longPressed = false;
        dragging = false;
    }
    return ;
}

void DraggableButton::setOtherButton(DraggableButton *otherButton) {
    this->otherButton = otherButton;
}


void DraggableButton::moveByHoverEntered() {
    if ( (arrayDirection == RIGHT && movedRight) || (arrayDirection == LEFT) ) {
        button->setFixedSize(100,100);
        setFixedSize(100,100);
        button->setIcon(QIcon(reservedIconPath));
        button->setIconSize(QSize(80,80));
        button->setStyleSheet("QPushButton{border-radius: 15px; }");
    }
    if ((arrayDirection == RIGHT && movedRight) ){
        move(reservedRect.width()-100, pos().y());
        movedRight = false;
    }
}


void DraggableButton::moveByHoverLeaved() {
    if ( (arrayDirection == RIGHT && !movedRight) || (arrayDirection == LEFT) ) {
        button->setFixedSize(10,100);
        button->setIcon(QIcon(":/images/line.png"));
        button->setStyleSheet("QPushButton{border-radius: 5px; }");
        button->setIconSize(QSize(50,50));
        setFixedSize(10,100);
    }
    if ( arrayDirection == RIGHT && !movedRight) {
        move(reservedRect.width()-10, pos().y());
        button->setStyleSheet("QPushButton { padding-right: 5px; }");
        movedRight = true;
    }
}


void DraggableButton::moveByClick() {
    //获取当前桌面的index
    QProcess process;
    process.start("wmctrl", QStringList() << "-d");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QStringList desktops_info = output.trimmed().split("\n");
    QList<int> tmpDesktopList ;
    for (const QString& line : desktops_info) {
        tmpDesktopList.append(line.split(" ").first().toInt());
        /*
        0  * DG: 1920x1080  VP: 0,0  WA: 0,0 1920x1080  桌面 1   //当前桌面
        1  - DG: 1920x1080  VP: 0,0  WA: 0,0 1920x1080  桌面 2
        2  - DG: 1920x1080  VP: 0,0  WA: 0,0 1920x1034  桌面 3
        */
        if (line.simplified().split(" ").at(1).contains("*")) {    //按空格分割，多个空格也视作1个空格
            currentDesktopIndex = line.split(" ").first().toInt();
        }
    }
    if (tmpDesktopList.size() == 1) {//如果只有一个桌面，直接返回
        return ;
    }
    int targetDesktop = 0;
    if (arrayDirection == LEFT) {
        if (currentDesktopIndex == tmpDesktopList.first()) {
            targetDesktop = tmpDesktopList.last();
        } else {
            targetDesktop = currentDesktopIndex - 1;
        }
    } else if (arrayDirection == RIGHT) {
        if (currentDesktopIndex == tmpDesktopList.size() - 1) {
            targetDesktop = tmpDesktopList.first();
        } else {
            targetDesktop = currentDesktopIndex + 1;
        }
    }
    moveToDesktop(targetDesktop);
}


void DraggableButton::moveToDesktop(int desktopIndex) {
    QProcess process;
    process.start("wmctrl", QStringList() << "-s" << QString::number(desktopIndex));
    process.waitForFinished();
    currentDesktopIndex = desktopIndex;
}

