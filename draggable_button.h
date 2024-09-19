
#ifndef DRAGGABLE_BUTTON_H
#define DRAGGABLE_BUTTON_H


#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>
#include<QDateTime>
#include <QProcess>


//增加enum 用于记录方向
enum ArrayDirection {
    LEFT = 0,
    RIGHT = 1
};

const QString WindowName = "fde_navi";
const int WindowCount = 2 ; //have two navi



class DraggableButton : public QWidget {
public:
   void setOtherButton(DraggabelButton *otherButton) ;
   void moveByHoverEntered();
   void moveByHoverLeaved();
  

//新增槽函数
public slots:
    void onButtonPressed();
    void onButtonReleased();

protected:
    void enterEvent(QEvent *event) override {
        moveByHoverEntered();
        if (otherButton) {
            otherButton->moveByHoverEntered();
        }
    }

    void leaveEvent(QEvent *event) override {
        moveByHoverLeaved();
        if (otherButton){
            otherButton->moveByHoverLeaved();
        }
    }

    //切换本窗口到当前桌面去
    void moveToCurrentDesktop();
    void moveByClick();
    void moveToDesktop(int desktopIndex);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
   

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