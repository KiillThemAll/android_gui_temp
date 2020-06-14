#include <QGuiApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "ngcquerymodel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    QSqlDatabase db5 = QSqlDatabase::addDatabase("QPSQL","active_ngc");
    db5.setHostName("10.33.0.1");
    db5.setDatabaseName("postgres");
    db5.setUserName("postgres");
    db5.setPassword("34edd10347d2e54ad4c36b809471e287");
    NgcQueryModel ngcModel(nullptr, db5);
    engine.rootContext()->setContextProperty("ngc_table", &ngcModel);

    engine.load(url);

    return app.exec();
}
