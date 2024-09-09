#include <QApplication>
#include "navi.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
	QRect geo = QApplication::desktop()->screenGeometry();
    FloatingButtonsWidget mainWidget(geo);
    mainWidget.show();

    return app.exec();
}
