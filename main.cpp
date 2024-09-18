#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QEvent>
#include <QPalette>
#include <QColor>
#include <QPropertyAnimation>


class HoverButton : public QPushButton {

public:
    HoverButton(const QString &text, QWidget *parent = nullptr) : QPushButton(text, parent) {
        setMouseTracking(true);
    }

protected:
    void enterEvent(QEvent *event) override {
        QPalette pal = palette();
        pal.setColor(QPalette::Button, QColor(Qt::blue));
        setAutoFillBackground(true);
        setPalette(pal);
        update();
        //将按钮尺寸放大为300*300
        //用动画效果完成尺寸变化
        QPropertyAnimation *animation = new QPropertyAnimation(this, "size");
        animation->setDuration(1000);
        animation->setStartValue(QSize(100, 40));
        animation->setEndValue(QSize(100, 60));
        animation->start();

        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override {
        QPalette pal = palette();
        pal.setColor(QPalette::Button, QColor(Qt::gray));
        setAutoFillBackground(true);
        setPalette(pal);
        update();
        //将按钮尺寸还原为100*40
        setFixedSize(100, 40);
        QPushButton::leaveEvent(event);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setFixedSize(200, 100);

    HoverButton *button = new HoverButton("Hover Me", &window);
    button->setGeometry(50, 30, 100, 40);

    window.show();
    return app.exec();
}
