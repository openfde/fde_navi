#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QDebug>


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
        button->setMouseTracking(true);
        setMouseTracking(true);
    }

    void setOtherButton(DraggableButton *otherButton) {
        this->otherButton = otherButton;
    }

protected:

void mousePressEvent(QMouseEvent *event) override {
    qDebug()<<"press "<<pos();
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
    }
}

void mouseMoveEvent(QMouseEvent *event) override {
    if (dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPos() - dragStartPosition;

        // 限制左右移动，只允许上下移动
        move(pos().x(), newPos.y());

        // 同步另一个按钮的位置
        if (otherButton) {
            otherButton->move(otherButton->pos().x(), newPos.y());
        }
    }
}

// void mouseReleaseEvent(QMouseEvent *event) override {
//     if (event->button() == Qt::LeftButton) {
//         dragging = false;
//     }
// }



private:
    QPushButton *button;
    QPoint dragStartPosition;
    bool dragging = true;
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
