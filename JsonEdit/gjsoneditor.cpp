#include "gjsoneditor.h"

GJsonEditor::GJsonEditor(QObject* parent) :
    QObject(parent),
    m_JsonModel() { }

void GJsonEditor::add_model(QQmlEngine& engine, const QString& modelName) {
    engine.rootContext()->setContextProperty(modelName, &m_JsonModel);
    engine.rootContext()->setContextProperty("jsonEditor", this);
}

bool GJsonEditor::is_saved() const noexcept {
    return m_sFileName.size();
}

void GJsonEditor::set_saved(const bool saved) {
    if (saved)
        return;
    m_sFileName = QString();
}

void GJsonEditor::sl_saveFile(const QString& fileName) {
    if (fileName.size())
        m_sFileName = fileName;

    QFile outPut(m_sFileName);
    outPut.open(QIODevice::WriteOnly);

    if (outPut.isOpen()) {
        QJsonArray json = m_JsonModel.to_json();
        QJsonDocument doc(json);
        outPut.write(doc.toJson(QJsonDocument::Indented));
    }
    else
        m_sFileName = QString();
    outPut.close();
}

void GJsonEditor::sl_loadFile(const QString& fileName) {
    QFile inPut(fileName);
    if (inPut.permissions() & QFileDevice::ReadUser) {
        if (inPut.permissions() & QFileDevice::WriteUser)
            m_sFileName = fileName;

        inPut.open(QIODevice::ReadOnly);
        QByteArray const data = inPut.readAll();
        inPut.close();

        QJsonParseError parseError; //ignore
        GParseError err = m_JsonModel.load(QJsonDocument::fromJson(data, &parseError));

        if (parseError.error != QJsonParseError::NoError) {
            QMessageBox msgBox;
            msgBox.setText(parseError.errorString());
            msgBox.exec();
        }
        else {
            if (err != GParseError::NoError && err != GParseError::NullData) {
                QMessageBox msgBox;
                QString sErrorStr = "Ошибка в структуре json: ";
                switch (err) {
                case GParseError::BadHierarchy:
                    sErrorStr += "Неверная иерархия вложенности Station -> Arm -> Device";
                    break;
                case GParseError::MissingFieldId:
                    sErrorStr += "Отсутствует поле \"id\"";
                    break;
                case GParseError::MissingFieldName:
                    sErrorStr += "Отсутствует поле \"name\"";
                    break;
                case GParseError::MissingFieldType:
                    sErrorStr += "Отсутствует поле \"type\"";
                    break;
                }
                msgBox.setText(sErrorStr);
                msgBox.exec();
            }
        }
    }
}

void GJsonEditor::sl_copy(const QModelIndex& item) {
    QApplication::clipboard()->setText(m_JsonModel.get_row(item, QJsonDocument::Indented), QClipboard::Clipboard);
}

void GJsonEditor::sl_paste(const QModelIndex& item) {
    if (item.isValid() && item.parent() != item)
        m_JsonModel.insert_row(item.parent(), item.row(), QApplication::clipboard()->text(QClipboard::Clipboard));
}

void GJsonEditor::sl_insert(const QModelIndex& item) {
    if (item.isValid())
        if (!m_JsonModel.insert_row(item, 0, QApplication::clipboard()->text(QClipboard::Clipboard)))
            m_JsonModel.insert_row(item);
}

void GJsonEditor::sl_new(const QModelIndex& item) {
    if (item.isValid() && item.parent() != item)
        m_JsonModel.insert_row(item.parent());
}
