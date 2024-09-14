#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include<QDateTime>
#include <QProcess>

//增加enum 用于记录方向
enum ArrayDirection {
    LEFT = 0,
    RIGHT = 1
};

class DraggableButton : public QWidget {
public:
    DraggableButton(ArrayDirection direction, const QString &iconPath, QWidget *parent = nullptr) : QWidget(parent) {
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
        //新增qt代码 执行linux shell 命令wmctrl -d 获取所有屏幕数量
        arrayDirection = direction;

        QProcess process;
        process.start("wmctrl", QStringList() << "-d");
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        QStringList desktops_info = output.trimmed().split("\n");
	for (const QString& line : desktops_info) {
            desktopList.append(line.split(" ").first().toInt());
            if (line.contains("*")) {
                currentDesktopIndex = line.split(" ").first().toInt();
                break;
            }
        }
    }
    void setOtherButton(DraggableButton *otherButton) {
        this->otherButton = otherButton;
    }
    //调用
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
                QProcess process;
                QList <int> tmpDesktopList;
                process.start("wmctrl", QStringList() << "-d");
                process.waitForFinished();
                QString output = process.readAllStandardOutput();
                QStringList desktops_info = output.trimmed().split("\n");
                for (const QString& line : desktops_info) {
		qDebug()<<line.split(" ").first().toInt();
                    tmpDesktopList.append(line.split(" ").first().toInt());
                }
                QProcess sprocess;
                if (arrayDirection == LEFT) {
                    if (currentDesktopIndex == tmpDesktopList.first() ) {
                        //获取最后的dekstop index,并切换到最后一个
                        sprocess.start("wmctrl", QStringList() << "-s" << QString::number(tmpDesktopList.last()));
                        sprocess.waitForFinished();
                        //更新currentDesktopIndex
                        qDebug()<<"currentDesktopIndex:"<<currentDesktopIndex <<"the last target desktop index:"<<tmpDesktopList.last();
                        currentDesktopIndex = tmpDesktopList.last();
                    }else {
                        //增加调试语句
                        sprocess.start("wmctrl", QStringList() << "-s" << QString::number(currentDesktopIndex -1));
                        sprocess.waitForFinished();
                        //更新currentDesktopIndex
                        qDebug()<<"currentDesktopIndex:"<<currentDesktopIndex <<"the left target desktop index:"<< QString::number(currentDesktopIndex -1 );
                        currentDesktopIndex = currentDesktopIndex -1;
                    }
                }else if (arrayDirection == RIGHT) {
                      //参考上述代码，调整为向右方向，也执行上述逻辑
                    if (currentDesktopIndex == tmpDesktopList.size() - 1 ) {
                        // 获取第一个desktop index，并切换到第一个
                        sprocess.start("wmctrl", QStringList() << "-s" << QString::number(tmpDesktopList.first()));
                        sprocess.waitForFinished();
                        //更新currentDesktopIndex  
                        qDebug()<<"currentDesktopIndex:"<<currentDesktopIndex <<"the first desktop index:" + tmpDesktopList.first();
                        currentDesktopIndex = tmpDesktopList.first();
                        qDebug()<<"currentDesktopIndex:"<<currentDesktopIndex; 
                    } else {
                        //增加调试语句
                        sprocess.start("wmctrl", QStringList() << "-s" << QString::number(currentDesktopIndex + 1));
                        sprocess.waitForFinished();
                        //更新currentDesktopIndex
                        qDebug()<<"currentDesktopIndex:"<<currentDesktopIndex <<"the right target desktop index:"<< QString::number(currentDesktopIndex + 1);
                        currentDesktopIndex = currentDesktopIndex +1;
                    }
                }
                //通过wmctrl -l 获取当前窗口的id
                process.start("wmctrl", QStringList() << "-l");
                process.waitForFinished();
                output = process.readAllStandardOutput();
                QStringList windows = output.trimmed().split("\n");
                for (const QString& window : windows) {
                    if (window.contains("navi")) {
                        QString windowId = window.split(" ").first();
                        //将当前窗口移动到当前桌面
                        QProcess sprocess;
                        sprocess.start("wmctrl", QStringList() << "-ir"<< windowId<<"-t"<< QString::number(currentDesktopIndex));
                        sprocess.waitForFinished();
                    }
                }
                return  ;
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
    //增加一个列表成员，用于存放一组动态变化的数值
    QList<int> desktopList;
    //增加字段，记录当前的桌面index
    int currentDesktopIndex;
    //增加字段，记录自己的方向
    ArrayDirection arrayDirection ;
};

//添加main方法
int main(int argc , char * argv[]){
    QApplication app(argc, argv);
    DraggableButton *lbtn = new DraggableButton(LEFT,":/images/left.png");
    DraggableButton *draggableButton = new DraggableButton(RIGHT,":/images/right.png");
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
