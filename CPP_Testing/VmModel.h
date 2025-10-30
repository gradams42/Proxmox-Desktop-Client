#ifndef VMMODEL_H
#define VMMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include "ProxmoxApiManager.h" // For Vm struct

// Forward declaration of the internal tree structure helper
struct TreeItem;

class VmModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit VmModel(QObject *parent = nullptr);
    ~VmModel() override;

    // --- Core QAbstractItemModel Overrides ---

    // The data() method must take 'int' for the role to match the base class.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    // Header Data
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    // Tree Structure Methods
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    // Row/Column Counting
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // --- Data Population Method ---
    void setVmList(const QVector<Vm>& vms);

private:
    TreeItem *rootItem;
    TreeItem *getItem(const QModelIndex &index) const;
};

// --- TreeItem Structure Definition ---
struct TreeItem
{
    QString name;
    bool isFolder = false;
    Vm vmData;
    QVector<TreeItem*> children;
    TreeItem *parent = nullptr;

    // C++11 initializer list constructor
    TreeItem(const QString& name, bool folder, const Vm& data = Vm(), QVector<TreeItem*> children = {}, TreeItem* parent = nullptr)
        : name(name), isFolder(folder), vmData(data), children(children), parent(parent) {}

    ~TreeItem() { qDeleteAll(children); }
};


#endif // VMMODEL_H