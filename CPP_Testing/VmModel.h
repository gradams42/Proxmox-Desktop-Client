#ifndef VMMODEL_H
#define VMMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QStringList> 
#include "ProxmoxApiManager.h" // For Vm struct

// --- TreeItem Structure Definition ---
// MUST BE DEFINED BEFORE VmModel uses it, or use a forward declaration + full definition later.
// Defining it fully here simplifies the header.
struct TreeItem
{
    // C++ Model/View members
    TreeItem *parent;                 // Required for model traversal (Error 315, 316)
    QVector<TreeItem*> children;      // List of child items (for folders or root)

    // Data members
    bool isFolder;                    // Flag to distinguish between a Folder and a VM (Error 233, 247, 287, 296)
    QString name;                     // Display name (for Folders or VMs) (Error 332, 335)
    
    // Proxmox VM Data (only valid if isFolder is false)
    Vm vmData;                        // Holds VM details (vmid, status, etc.) (Error 234, 236, 331, etc.)

    // Constructor for Folder (or Root)
    explicit TreeItem(const QString& itemName, bool folder = true, TreeItem *parentItem = nullptr)
        : parent(parentItem), isFolder(folder), name(itemName) {}

    // Constructor for VM
    explicit TreeItem(const Vm& data, TreeItem *parentItem = nullptr)
        : parent(parentItem), isFolder(false), name(data.name), vmData(data) {}
    
    // Destructor (recursively deletes children)
    ~TreeItem() {
        qDeleteAll(children);
        children.clear();
    }

    void appendChild(TreeItem *child) {
        children.append(child);
    }

    TreeItem *child(int row) const {
        return children.value(row);
    }

    int childCount() const {
        return children.count();
    }

    int row() const; // Implementation will be in .cpp file
};


class VmModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit VmModel(QObject *parent = nullptr);
    ~VmModel() override;

    // --- Core QAbstractItemModel Overrides ---
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // --- Data Population Method ---
    void setVmList(const QVector<Vm>& vms);

    // --- Folder Management Methods ---
    bool createFolder(const QString& name); 
    bool assignVmToFolder(int vmid, const QString& folderName);
    QStringList getFolderNames() const;
    
    // --- NEW: Public helper to check if an item is directly under the root ---
    bool isRootParent(const TreeItem* item) const; 
    // ------------------------------------------------------------------------

private:
    TreeItem *rootItem; // <-- STILL PRIVATE
    TreeItem *getItem(const QModelIndex &index) const;
    TreeItem *findVmItem(int vmid) const; 
    TreeItem *findFolderItem(const QString& folderName) const; 
};


#endif // VMMODEL_H