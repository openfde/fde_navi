#include <QApplication>
#include <QPushButton>
#include <QScreen>
#include <QWidget>

class FloatingButton : public QPushButton {
public:
    FloatingButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent) {
        setFixedSize(100, 100); // 设置按钮大小
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); // 无边框，始终在最上层
        setAttribute(Qt::WA_TranslucentBackground); // 背景透明
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 获取屏幕的几何信息
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();

    // 创建左侧悬浮按钮
    FloatingButton *leftButton = new FloatingButton("Left");
    leftButton->move(10, (screenGeometry.height() - leftButton->height()) / 2); // 左侧位置
    leftButton->show(); // 显示按钮

    // 创建右侧悬浮按钮
    FloatingButton *rightButton = new FloatingButton("Right");
    rightButton->move(screenGeometry.width() - rightButton->width() - 10,
                       (screenGeometry.height() - rightButton->height()) / 2); // 右侧位置
    rightButton->show(); // 显示按钮

    return app.exec();
}
