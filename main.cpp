#include "mywidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QLabel>
#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDesktopWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create a main widget
    QWidget mainWidget;
    mainWidget.setWindowTitle("Floating Buttons");
    mainWidget.setGeometry(QApplication::desktop()->screenGeometry());

    // Create left and right buttons
    QPushButton leftButton(QIcon(":/images/left.jpg"), " ",&mainWidget);
    QPushButton rightButton(QIcon(":/images/right.jpg"), "",&mainWidget);

    // Set icons for the buttons using PNG images
    //leftButton.setIcon(QIcon(":/images/left.png"));
    //rightButton.setIcon(QIcon(":/images/right.png"));

    // Create a layout for the buttons
    QHBoxLayout layout(&mainWidget);
    layout.addWidget(&leftButton, 0, Qt::AlignLeft);
    layout.addWidget(&rightButton, 0, Qt::AlignRight);
    mainWidget.setLayout(&layout);

    // Set the main widget as the central widget
    app.setActiveWindow(&mainWidget);
    mainWidget.setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    mainWidget.setAttribute(Qt::WA_TranslucentBackground);
    mainWidget.show();

    return app.exec();
}
