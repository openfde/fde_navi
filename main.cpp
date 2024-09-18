#include <QWidget>
#include <QPushButton>
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
    DraggableButton(ArrayDirection direction, const QString &iconPath, QWidget *parent = nullptr) : QWidget(parent) {
        button = new QPushButton("", this);
        //设置为无框透明，且总在最上面
        // button->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	    QPoint globalPos = button->mapToGlobal(QPoint(0, 0));
        qDebug() << "Global position of the top-left corner:" << globalPos;
        setWindowFlags(Qt::FramelessWindowHint |Qt::Tool|  Qt::WindowStaysOnTopHint);
        button->setIcon(QIcon(iconPath));
        button->setIconSize(QSize(80, 80));
        button->setFixedSize(100, 100);
        button->move(0, 0);
        reservedIconPath = iconPath;
        // button->setMouseTracking(true);
        //connect button的按下事件和一个槽函数
        connect(button, &QPushButton::pressed, this, &DraggableButton::onButtonPressed);
        //connect button的释放事件和一个槽函数
        connect(button, &QPushButton::released, this, &DraggableButton::onButtonReleased);

        button->setAutoRepeat(false);
        setMouseTracking(true);
        //标记自己的方向
        arrayDirection = direction;
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

        if (arrayDirection == LEFT) {
        }else {
             if ( movedRight) {
		     qDebug()<<"restore 90";
                movedRight = false;
                move(this->pos() - QPoint(90, 0));
            }
            //往左移动窗口90个像素
            //button->move(button->pos().x() - 90, button->pos().y());
        }
        button->setFixedSize(100, 100);
	button->setIcon(QIcon(reservedIconPath));
	button->setIconSize(QSize(80, 80));
        setFixedSize(100, 100);
    }

    void leaveEvent(QEvent *event) override {
        if (arrayDirection == LEFT) {
             button->setFixedSize(10, 100);
        }else{
            //往右移动窗口90个像素
            if ( !movedRight) {
		     qDebug()<<"rgith 90";
                move(this->pos() + QPoint(90, 0));
                movedRight = true;
            }
             //button->move(button->pos().x() + 90, button->pos().y());
        }
    	button->setFixedSize(10, 100);
	button->setIcon(QIcon(":/images/icon-line.png"));
	button->setIconSize(QSize(10, 10));
        setFixedSize(10, 100);
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

    DraggableButton *rbtn = new DraggableButton(RIGHT,":/images/right.png");
    DraggableButton *lbtn = new DraggableButton(LEFT,":/images/left.png");
    //设置draggableButton的大小为100 * 100
    rbtn->setFixedSize(100, 100);
    lbtn->setFixedSize(100, 100);
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();
    //draggableButton 移动到屏幕的右侧中间
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
