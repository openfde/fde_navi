#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QMouseEvent>

class FloatingButtonsWidget : public QWidget {
public:
    FloatingButtonsWidget(QRect screenGeometry, QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Floating Buttons");
        setGeometry(screenGeometry);

        // Create left and right buttons
        leftButton.setIcon(QIcon(":/images/left.jpg"));
        rightButton.setIcon(QIcon(":/images/right.jpg"));

        // Create a layout for the buttons
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addWidget(&leftButton, 0, Qt::AlignLeft);
        layout->addWidget(&rightButton, 0, Qt::AlignRight);
        setLayout(layout);

        // Set the window flags and attributes
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
    }

protected:
void mousePressEvent(QMouseEvent *event) override {
	if (event->button() == Qt::LeftButton) {
    	// Store the initial position of the mouse when the left button is pressed
    	dragStartPosition = event->globalPos() - frameGeometry().topLeft();
	}
}

void mouseMoveEvent(QMouseEvent *event) override {
	if (event->buttons() & Qt::LeftButton) {
    	// Calculate the distance the mouse has moved
    	QPoint distance = event->globalPos() - dragStartPosition;
    	// Move the window by the distance
    	move(distance);
	}
}

private:
    QPushButton leftButton;
    QPushButton rightButton;
    QPoint dragStartPosition;
};
