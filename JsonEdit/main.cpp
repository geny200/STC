#include <QApplication>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QUrl>

#include "gjsoneditor.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

    QApplication app(argc, argv);

    app.setOrganizationName("ITMO");
    app.setOrganizationDomain("ru");
    app.setApplicationName("Json Editor");

    QQmlApplicationEngine engine;
    GJsonEditor editor(&app);
    editor.add_model(engine, "jsonModel");

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [&](QObject* obj, const QUrl& objUrl) -> void {
                        if (!obj && url == objUrl)
                            QCoreApplication::exit(-1);
                    },
                    Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
