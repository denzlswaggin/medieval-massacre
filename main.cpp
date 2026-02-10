#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "qmlcontroller/GameController.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Basic");

    QQmlApplicationEngine engine;

    GameController gameController;

    engine.rootContext()->setContextProperty("gameController", &gameController);
    qmlRegisterType<MapModel>("Projekt_CPP", 1, 0, "MapModel");

    const QUrl url(QStringLiteral(u"qrc:/qt/qml/Projekt_CPP/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
