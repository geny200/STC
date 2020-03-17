#include "gjsonmodel.h"

GJsonModel::GJsonItem::GJsonItem(const QList<QVariant>& data, GJsonModel::GJsonItem* parent) :
    m_vData(data),
    m_pParent(parent) { }

void GJsonModel::GJsonItem::append_child(GJsonModel::GJsonItem* apChild) {
    apChild->m_pParent = this;
    m_vChildItem.push_back(apChild);
}

void GJsonModel::GJsonItem::append_child(const std::vector<GJsonModel::GJsonItem*>& childs) {
    for (auto& iChild : childs)
        iChild->m_pParent = this;

    m_vChildItem.insert(m_vChildItem.end(), childs.begin(), childs.end());
}

GJsonModel::GJsonItem* GJsonModel::GJsonItem::get_child(size_t index) {
    if (index >= m_vChildItem.size())
        return nullptr;
    return m_vChildItem[index];
}

const std::vector<GJsonModel::GJsonItem*>& GJsonModel::GJsonItem::get_child() {
    return m_vChildItem;
}

bool GJsonModel::GJsonItem::set_data(size_t column, const QVariant& value) {
    if (column >= m_vData.size())
        return false;

    m_vData.replace(column, value);
    return true;
}

bool GJsonModel::GJsonItem::remove(size_t position) {
    if (position >= m_vChildItem.size())
        return false;

    GJsonItem* local = m_vChildItem[position];
    m_vChildItem.erase(m_vChildItem.begin() + position);
    delete  local;
    return true;
}

bool GJsonModel::GJsonItem::insert_content(size_t position, GJsonItem* oldParent) {
    if (position >= m_vChildItem.size())
        return false;

    for (auto& iChild : oldParent->m_vChildItem)
        iChild->parent() = this;

    m_vChildItem.insert(m_vChildItem.begin() + position, oldParent->m_vChildItem.begin(), oldParent->m_vChildItem.end());
    oldParent->m_vChildItem.clear();
    return true;
}

size_t GJsonModel::GJsonItem::child_count() const noexcept {
    return m_vChildItem.size();
}

size_t GJsonModel::GJsonItem::column_count() const noexcept {
    return m_vData.size();
}

const QString GJsonModel::GJsonItem::get_name() const noexcept {
    return m_vData[0].value<QString>();
}

const QString GJsonModel::GJsonItem::get_id() const noexcept {
    return m_vData[1].value<QString>();
}

GJsonModel::GJsonType GJsonModel::GJsonItem::get_type() const noexcept {
    return (GJsonModel::GJsonType)m_vData[2].value<int>();
}

size_t GJsonModel::GJsonItem::row() const {
    if (m_pParent) {
        auto& vParentChild = m_pParent->m_vChildItem;
        if (auto result = std::find(vParentChild.begin(), vParentChild.end(), const_cast<GJsonItem*>(this));
                result != vParentChild.end())
            return result - vParentChild.begin();
    }
    return 0;
}

GJsonModel::GJsonItem*& GJsonModel::GJsonItem::parent() noexcept {
    return m_pParent;
}

GJsonModel::GJsonItem* GJsonModel::GJsonItem::last() const noexcept {
    if (!m_vChildItem.empty())
        return m_vChildItem.back();
    return nullptr;
}

QJsonObject GJsonModel::GJsonItem::to_json() const {
    QJsonObject result;
    result.insert("name", get_name());
    result.insert("id", get_id());
    result.insert("type", get_typename(get_type()));

    if (!m_vChildItem.empty()) {
        QJsonArray itemArray;

        for (auto& iChild : m_vChildItem)
            itemArray.push_back(iChild->to_json());

        result.insert("items", itemArray);
    }
    return result;
}

GJsonModel::GJsonItem::~GJsonItem() {
    qDeleteAll(m_vChildItem);
}

const QString GJsonModel::get_typename(const GJsonModel::GJsonType type) {
    switch (type) {
    case GJsonModel::GJsonType::Arm: return "arm";
    case GJsonModel::GJsonType::Station: return "station";
    case GJsonModel::GJsonType::Device: return "device";
    case GJsonModel::GJsonType::Unrecognized: return "";
    }
}

GJsonModel::GJsonType GJsonModel::get_typeenum(const QString& name) {
    switch (name.front().unicode()) {
    case 'a':
        return GJsonModel::GJsonType::Arm;
    case 's':
        return GJsonModel::GJsonType::Station;
    case 'd':
        return GJsonModel::GJsonType::Device;
    default:
        return GJsonModel::GJsonType::Unrecognized;
    }
}

GJsonModel::GJsonModel(const QJsonDocument& data, QObject* parent) :
    QAbstractItemModel(parent),
    m_pRoot(new GJsonItem()) {
    load(data);
}

GJsonModel::GJsonModel(QObject* parent) :
    QAbstractItemModel(parent),
    m_pRoot(new GJsonItem()) {}

QVariant GJsonModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    GJsonItem* localItem = static_cast<GJsonItem*>(index.internalPointer());

    if (Qt::EditRole == role || role == Qt::DisplayRole) //for QWidget and Editable
        switch (index.column()) {
        case 0:
            return localItem->get_name();
        case 1:
            return localItem->get_id();
        default:
            return QVariant();
        }

    if (role >= Roles::NameRole) // for Quick
        switch (role) {
        case NameRole:
            return localItem->get_name();
        case IdRole:
            return localItem->get_id();
        default:
            return QVariant();
        }

    return QVariant();
}

Qt::ItemFlags GJsonModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant GJsonModel::headerData(int, Qt::Orientation, int) const {
    return QVariant();
}

QModelIndex GJsonModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (GJsonItem* childItem = get_item(parent)->get_child(row);
            childItem != nullptr)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex GJsonModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    if (GJsonItem* localParent = static_cast<GJsonItem*>(index.internalPointer())->parent();
            localParent != m_pRoot)
        return createIndex(localParent->row(), 0, localParent);

    return QModelIndex();
}

int GJsonModel::rowCount(const QModelIndex& parent) const {
    if (parent.column() > 0)
        return 0;

    return get_item(parent)->child_count();
}

int GJsonModel::columnCount(const QModelIndex& parent) const {
    return get_item(parent)->column_count();
}

bool GJsonModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role) {
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    const bool result = m_pRoot->set_data(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

bool GJsonModel::set_data(const QModelIndex& index, const QVariant& value, int column) {
    GJsonItem *item = get_item(index);
    bool result = item->set_data(column, value);

    if (result)
        emit dataChanged(index, index);
    return result;
}

QByteArray GJsonModel::get_row(const QModelIndex& item, QJsonDocument::JsonFormat format) const {
    GJsonItem* localItem = get_item(item);

    if (localItem == m_pRoot) {
        QJsonArray result;

        for (auto& iItem : m_pRoot->get_child())
            result.push_back(iItem->to_json());

        const QJsonDocument doc(result);
        return doc.toJson(format);
    }
    const QJsonDocument doc(localItem->to_json());
    return doc.toJson(format);
}

bool GJsonModel::insert_row(const QModelIndex& item, size_t row, const QString& str) {
    if (GJsonItem* parentItem = get_item(item);
            parentItem != nullptr){

        GJsonItem newRows(QList<QVariant>({"name", "id", parentItem->get_type()}));
        setup_data(QJsonDocument::fromJson(str.toUtf8()).object(), &newRows);

        if (!newRows.child_count())
            return false;

        beginInsertRows(item, row, row + newRows.child_count() - 1);
        parentItem->insert_content(row, &newRows);
        endInsertRows();
        return true;
    }
    return false;
}

bool GJsonModel::insert_row(const QModelIndex& item) {
    if (GJsonItem* parentItem = get_item(item);
            parentItem != nullptr && parentItem->get_type() < GJsonModel::GJsonType::Device){

        beginInsertRows(item, parentItem->child_count(), parentItem->child_count());
        parentItem->append_child(
                    new GJsonItem(
                        QList<QVariant>(
                            {
                                "name - " + get_typename((GJsonModel::GJsonType)(parentItem->get_type() + 1)),
                                "id - "+ get_typename((GJsonModel::GJsonType)(parentItem->get_type() + 1)),
                                parentItem->get_type() + 1
                            }
                            ), parentItem));
        endInsertRows();
        return true;
    }
    return false;
}

bool GJsonModel::removeRows(int position, int rows, const QModelIndex& parent) {
    GJsonItem *parentItem = get_item(parent);

    if (!parentItem)
        return false;

    beginRemoveRows(parent, position, position + rows - 1);
    const bool success = parentItem->remove(position);
    endRemoveRows();

    return success;
}

void GJsonModel::clear() {
    beginResetModel();
    delete m_pRoot;
    m_pRoot = new GJsonItem(QList<QVariant>("name", "id"), nullptr);
    endResetModel();
}

bool GJsonModel::load(const QJsonDocument& dataJson) {
    if (dataJson.isNull())
        return false;

    beginResetModel();
    delete m_pRoot;
    m_pRoot = new GJsonItem(QList<QVariant>({"name", "id", GJsonModel::GJsonType::Unrecognized}), nullptr);
    setup_data(dataJson.array(), m_pRoot);
    endResetModel();

    return true;
}

QJsonArray GJsonModel::to_json() const {
    QJsonArray result;

    for (auto& iItem : m_pRoot->get_child())
        result.push_back(iItem->to_json());

    return result;
}

QHash<int, QByteArray> GJsonModel::roleNames() const {
    QHash<int, QByteArray> result = QAbstractItemModel::roleNames();

    result.insert(NameRole, QByteArrayLiteral("name"));
    result.insert(IdRole, QByteArrayLiteral("id"));

    return result;
}

GJsonModel::~GJsonModel() {
    delete m_pRoot;
}

void GJsonModel::setup_data(const QJsonValue& value, GJsonItem* parent) {
    if (value.isObject()) {
        if (!value["name"].isString() || !value["id"].isString() || !value["type"].isString())
            return;

        auto localType = get_typeenum(value["type"].toString());
        if (localType != parent->get_type() + 1)
            return;

        QList<QVariant> columnData;
        columnData << value["name"].toString() << value["id"].toString() << localType;
        parent->append_child(new GJsonItem(columnData, parent));

        if (auto& localValue = value["items"];
                localValue.isArray())
            setup_data(localValue, parent->last());
        return;
    }

    if (value.isArray()) {
        for (auto localValue : value.toArray())
            if (localValue.isObject())
                setup_data(localValue, parent);
    }
    return;
}

GJsonModel::GJsonItem* GJsonModel::get_item(const QModelIndex& ptr) const {
    return ptr.isValid() ? static_cast<GJsonItem*>(ptr.internalPointer()) : m_pRoot;
}
