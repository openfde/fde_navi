#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include<QDateTime>


class DraggableButton : public QWidget {
public:
    DraggableButton(const QString &iconPath, QWidget *parent = nullptr) : QWidget(parent) {
        button = new QPushButton("", this);
        //设置为无框透明，且总在最上面
        // button->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        button->setIcon(QIcon(iconPath));
        button->setIconSize(QSize(80, 80));
        button->setFixedSize(100, 100);
        button->move(0, 0);
        // button->setMouseTracking(true);
        //connect button的按下事件和一个槽函数
        connect(button, &QPushButton::pressed, this, &DraggableButton::onButtonPressed);
        //connect button的释放事件和一个槽函数
        connect(button, &QPushButton::released, this, &DraggableButton::onButtonReleased);

        button->setAutoRepeat(false);
        setMouseTracking(true);
    }
    void setOtherButton(DraggableButton *otherButton) {
        this->otherButton = otherButton;
    }
//新增槽函数
public slots:
    void onButtonPressed(){
        lastPressedTime = QDateTime::currentDateTime();
        QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, button->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	    dragging = true;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
	delete event;
    }

    void onButtonReleased(){
        //如果按下的时刻和释放的时刻相差小于200ms，则认为是点击
        if (lastPressedTime.msecsTo(QDateTime::currentDateTime()) < 200 ) {
            //判断鼠标左键现在是否依然是按下的
            if (QApplication::mouseButtons() & Qt::LeftButton) { //鼠标依然按下，说明只是焦点离开了按钮，用户依然在长按拖动
		return;
            }else if ( !longPressed){ 
		        qDebug()<<"button clicked";
		        return;
            }
        }
		longPressed = true;
	    if ((longPressed) && !(QApplication::mouseButtons() & Qt::LeftButton)) { //鼠标左键已然弹
		    longPressed = false;
		    dragging = false;
		    qDebug()<<"floating release by btn";
	    }
        return ;
    }



protected:

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



private:
    //添加一个时间戳
    QDateTime lastPressedTime;
    QPushButton *button;
    QPoint dragStartPosition;
    bool longPressed = false;
    bool dragging = false;
    DraggableButton *otherButton = nullptr;
};

//添加main方法
int main(int argc , char * argv[]){
    QApplication app(argc, argv);
    DraggableButton *lbtn = new DraggableButton(":/images/left.png");
    DraggableButton *draggableButton = new DraggableButton(":/images/right.png");
    //设置draggableButton的大小为100 * 100
    draggableButton->setFixedSize(100, 100);
    lbtn->setFixedSize(100, 100);
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();
    //draggableButton 移动到屏幕的右侧中间
    draggableButton->move(screenRect.width() - draggableButton->width(), screenRect.height() / 2 - draggableButton->height() / 2);
    //lbtn 移动到屏幕的左侧中间
    lbtn->move(0, screenRect.height() / 2 - lbtn->height() / 2);
    //设置draggableButton的另一个按钮为lbtn
    draggableButton->setOtherButton(lbtn);
    lbtn->show();
    draggableButton->show();
    lbtn->setOtherButton(draggableButton);
    return app.exec();
}
