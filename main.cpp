#include <QtGui/QApplication>
#include <QtDeclarative>

#include "qmlapplicationviewer.h"
#include "bcglobal.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    BattleCity bc;
    bc.init();

    QScopedPointer<QmlApplicationViewer> viewer(QmlApplicationViewer::create());

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer->rootContext()->setContextProperty("BattleCityInstance", &bc);
    viewer->setMainQmlFile(QLatin1String("qml/battlecity/main.qml"));
    viewer->showExpanded();

    return app->exec();
}
