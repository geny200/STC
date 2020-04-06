#pragma once
#include <QAbstractItemModel>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QList>
#include <vector>

enum GParseError {
    NullData = 0,
    NoError = 1,
    MissingFieldId = 2,
    MissingFieldName = 3,
    MissingFieldType = 4,
    BadHierarchy = 5
};

class GJsonModel : public QAbstractItemModel {
    Q_OBJECT

    enum GJsonType {
        Unrecognized = 0,
        Station = 1,
        Arm = 2,
        Device = 3
    };

    static const QString get_typename(const GJsonType);
    static GJsonType get_typeenum(const QString& name);

    class GJsonItem {
    public:
        GJsonItem(const QList<QVariant>& = QList<QVariant>(), GJsonItem* = nullptr);

        void append_child(GJsonItem* child);
        void append_child(const std::vector<GJsonItem*>& childs);
        GJsonItem* get_child(size_t);
        const std::vector<GJsonItem*>& get_child();


        bool set_data(size_t column, const QVariant& value);
        bool remove(size_t position);
        bool insert_content(size_t position, GJsonItem* oldParent);

        size_t child_count() const noexcept;
        size_t column_count() const noexcept;

        const QString get_name() const noexcept;
        const QString get_id() const noexcept;
        GJsonModel::GJsonType get_type() const noexcept;

        size_t row() const;
        GJsonItem*& parent() noexcept;
        GJsonItem* last() const noexcept;

        QJsonObject to_json() const;


        ~GJsonItem();

    private:
        std::vector<GJsonItem*> m_vChildItem;
        QList<QVariant> m_vData;
        GJsonItem *m_pParent;
    };

    GParseError setup_data(const QJsonValue& value, GJsonItem* parent);
    GJsonItem* get_item(const QModelIndex& index) const;

public:
    enum Roles  {
        NameRole = Qt::UserRole + 4,
        IdRole = Qt::UserRole + 5
    };

    Q_ENUM(Roles)

    GJsonModel(const QJsonDocument& dataJson, QObject* parent = nullptr);
    GJsonModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                               int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                          const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    bool setHeaderData(int section, Qt::Orientation orientation,
                                  const QVariant& value, int role = Qt::EditRole) override;
    Q_INVOKABLE bool set_data(const QModelIndex& index,
                              const QVariant& value, int column);
    Q_INVOKABLE bool removeRows(int position, int rows,
                                const QModelIndex& parent = QModelIndex()) override;
    Q_INVOKABLE void clear();

    Q_INVOKABLE void create();

    QByteArray get_row(const QModelIndex& item, QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;
    bool insert_row(const QModelIndex& item, size_t row, const QString& str = QString());
    bool insert_row(const QModelIndex& item);

    GParseError load(const QJsonDocument& fileName);
    QJsonArray to_json() const;

    QHash<int, QByteArray> roleNames() const override;
    ~GJsonModel();

private:
    GJsonItem *m_pRoot;
};

