#pragma once
#include "gjsonmodel.h"
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QClipboard>
#include <QtQml>

class GJsonEditor : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool fileSaved READ is_saved WRITE set_saved)
public:
    explicit GJsonEditor(QObject* parent = nullptr);
    void add_model(QQmlEngine& engine, const QString& modelName);

    bool is_saved() const noexcept;
    void set_saved(const bool saved);

public slots:
    Q_INVOKABLE void sl_saveFile(const QString& fileName);
    Q_INVOKABLE void sl_loadFile(const QString& fileName);
    Q_INVOKABLE void sl_copy(const QModelIndex& item);
    Q_INVOKABLE void sl_paste(const QModelIndex& item);
    Q_INVOKABLE void sl_insert(const QModelIndex& item);

private:
    GJsonModel m_JsonModel;
    QString m_sFileName;
};
