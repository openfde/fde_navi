#include <QApplication>
#include <QPushButton>
#include <QScreen>
#include <QWidget>
#include <QMouseEvent>
#include <QIcon>

class FloatingButton : public QPushButton {
public:
    FloatingButton(const QString &text, const QString &iconPath, QWidget *parent = nullptr)
        : QPushButton(text, parent), otherButton(nullptr) {
        setFixedSize(100, 100); // 设置按钮大小
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); // 无边框，始终在最上层
        setAttribute(Qt::WA_TranslucentBackground); // 背景透明
        setIcon(QIcon(iconPath)); // 设置图标
        setIconSize(QSize(60, 60)); // 设置图标大小
    }

    void setOtherButton(FloatingButton *button) {
        otherButton = button;
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            dragging = true;
            dragStartPosition = event->globalPos() - pos();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (dragging && (event->buttons() & Qt::LeftButton)) {
            QPoint newPos = event->globalPos() - dragStartPosition;

            // 限制左右移动，只允许上下移动
            move(x(), newPos.y());

            // 同步另一个按钮的位置
            if (otherButton) {
                otherButton->move(otherButton->x(), newPos.y());
            }
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            dragging = false;
        }
    }

private:
    QPoint dragStartPosition;
    bool dragging = false;
    FloatingButton *otherButton;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 获取屏幕的几何信息
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();

    // 创建左侧悬浮按钮，添加向左箭头图标
    FloatingButton *leftButton = new FloatingButton("", ":/images/left.png");
    leftButton->move(10, (screenGeometry.height() - leftButton->height()) / 2); // 左侧位置
    leftButton->show(); // 显示按钮

    // 创建右侧悬浮按钮，添加向右箭头图标
    FloatingButton *rightButton = new FloatingButton("", ":/images/right.png");
    rightButton->move(screenGeometry.width() - rightButton->width() - 10,
                       (screenGeometry.height() - rightButton->height()) / 2); // 右侧位置
    rightButton->show(); // 显示按钮

    // 设置两个按钮互相知道对方
    leftButton->setOtherButton(rightButton);
    rightButton->setOtherButton(leftButton);

    return app.exec();
}

