#include "mywidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QLabel>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create the main window
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Floating Windows Example");

    // Get the screen geometry
    QDesktopWidget desktop;
    QRect screenGeometry = desktop.availableGeometry();

    // Calculate the window width and height
    int windowWidth = screenGeometry.width() / 10;
    int windowHeight = screenGeometry.height() / 2;

    // Create the left floating window
    QMainWindow leftWindow ;
    leftWindow.setWindowTitle("Left Floating Window");
    leftWindow.setWindowIcon(QPixmap(":/images/left.png"));
    leftWindow.setGeometry(screenGeometry.left(), screenGeometry.top(), windowWidth, windowHeight);
    leftWindow.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    leftWindow.show();

    // Create the right floating window
    QMainWindow rightWindow;
    rightWindow.setWindowTitle("Right Floating Window");
    rightWindow.setWindowIcon(QIcon(":/images/right.png"));
    rightWindow.setGeometry(screenGeometry.right() - windowWidth, screenGeometry.top(), windowWidth, windowHeight);
    rightWindow.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    rightWindow.show();

    return app.exec();
    /*QApplication a(argc, argv);
    MyWidget w;
    w.setGeometry( 100, 100, 200, 120 );
    w.show();

    return a.exec();
    */
}
