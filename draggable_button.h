
#ifndef DRAGGABLE_BUTTON_H
#define DRAGGABLE_BUTTON_H


#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QApplication>
#include <QTimer>
#include <QMouseEvent>
#include <QDateTime>
#include <QProcess>
#include <QRect>


//增加enum 用于记录方向
enum ArrayDirection {
    LEFT = 0,
    RIGHT = 1
};


class DraggableButton : public QWidget {
public:
   DraggableButton(ArrayDirection direction, const QString &iconPath, QRect rect, QWidget *parent = nullptr);
   void setOtherButton(DraggableButton *otherButton) ;
   void moveByHoverEntered();
   void moveByHoverLeaved();
  

//新增槽函数
public slots:
    void onButtonPressed();
    void onButtonReleased();

protected:
    void enterEvent(QEvent *) override {
        moveByHoverEntered();
        if (otherButton) {
            otherButton->moveByHoverEntered();
        }
    }

    void leaveEvent(QEvent *) override {
    QTimer::singleShot(3000, this, [this](){
	     //判断当前鼠标是否在按钮上，如果在按钮上，不做任何事情
        if (button->rect().contains(button->mapFromGlobal(QCursor::pos()))) {
            return;
        }
        moveByHoverLeaved();
        if (otherButton){
            otherButton->moveByHoverLeaved();
	}
    });
    }

void mousePressEvent(QMouseEvent *event) override {
    qDebug()<<"floating pressed"<<pos();
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
    }
}

void mouseMoveEvent(QMouseEvent *event) override {
    if (dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPos() - dragStartPosition;
	    longPressed = true;
        // 限制左右移动，只允许上下移动
        move(pos().x(), newPos.y());

        // 同步另一个按钮的位置
        if (otherButton) {
            otherButton->move(otherButton->pos().x(), newPos.y());
        }
        event->ignore();
    }
}

void mouseReleaseEvent(QMouseEvent *event) override {
	qDebug()<<"floating released";
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
}

    //切换本窗口到当前桌面去
    void moveToCurrentDesktop();
    void moveByClick();
    void moveToDesktop(int desktopIndex);
   

private:
    //添加一个时间戳
    QDateTime lastPressedTime;
    QPushButton *button;
    QPoint dragStartPosition;
    bool longPressed = false;
    QString reservedIconPath;
    QRect reservedRect;
    bool movedRight = false;
    bool dragging = false;
    DraggableButton *otherButton = nullptr;
    //增加字段，记录当前的桌面index
    int currentDesktopIndex;
    //增加字段，记录自己的方向
    ArrayDirection arrayDirection ;
};

#endif // DRAGGABLE_BUTTON_H
