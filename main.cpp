#include <QWidget>
#include <QPainter>
#include <QRegion>
#include <QPushButton>
#include <QTimer>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QScreen>
#include <QDebug>
#include<QDateTime>
#include <QProcess>

//增加enum 用于记录方向
enum ArrayDirection {
    LEFT = 0,
    RIGHT = 1
};

const QString WindowName = "navi";
const int WindowCount = 2 ; //have two navi

class DraggableButton : public QWidget {
public:
    DraggableButton(ArrayDirection direction, const QString &iconPath, QRect rect, QWidget *parent = nullptr) : QWidget(parent) {
        //设置为无框tool 模式，且总在最上面
        setWindowFlags(Qt::FramelessWindowHint |Qt::Tool|  Qt::WindowStaysOnTopHint);
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
    void setOtherButton(DraggableButton *otherButton) {
        this->otherButton = otherButton;
    }


    /*void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setBrush(QColor(192,192,192)); // 灰色填充色
        painter.drawRoundedRect(rect(), 15,15); // 绘制带圆角的矩形
    }

    void resizeEvent(QResizeEvent *event) override {
        QRegion region = QRegion(0, 0, width(), height(), QRegion::Rectangle);
        setMask(region);
    }
    */


    void moveByHoverEntered() {
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

    void moveByHoverLeaved() {
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
	        qDebug()<< arrayDirection << "button clicked";
                return moveByClick();
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
    void moveToCurrentDesktop() {
        //通过wmctrl -l 获取当前窗口的id   
        QProcess process;
        process.start("wmctrl", QStringList() << "-l");
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        QStringList windows = output.trimmed().split("\n");
	int count =  0 ;
        for (const QString& window : windows) {
	if (count == WindowCount) {
                    break;
                }
            if (window.simplified().split(" ").at(3).contains(WindowName)) {
		count++;
                QString windowId = window.split(" ").first();
                //将当前窗口移动到当前桌面
                QProcess sprocess;
                sprocess.start("wmctrl", QStringList() << "-ir"<< windowId<<"-t"<< QString::number(currentDesktopIndex));
                sprocess.waitForFinished();
            }
        }
    }



void moveByClick() {
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
    moveToCurrentDesktop();
    }


void moveToDesktop(int desktopIndex) {
    QProcess process;
    process.start("wmctrl", QStringList() << "-s" << QString::number(desktopIndex));
    process.waitForFinished();
    currentDesktopIndex = desktopIndex;
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


//添加main方法
int main(int argc , char * argv[]){
    QApplication app(argc, argv);

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
