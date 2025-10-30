#include "VmModel.h"
#include <QDebug>
#include <QIcon>
#include <algorithm> // For std::sort

// The TreeItem structure definition should be available here (it is in VmModel.h now)

// --- TreeItem Utility ---

TreeItem* VmModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

// --- VmModel Implementation ---

// Constructor: Initializes the hidden root item
VmModel::VmModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    // The root item is hidden, representing the entire collection
    // Note: The warnings about missing initializers are handled by the TreeItem constructor.
    rootItem = new TreeItem("Root", true); 
}

// Destructor: Cleans up the tree structure
VmModel::~VmModel()
{
    delete rootItem;
}

// ----------------------------------------------------
// Core Model/View Overrides
// ----------------------------------------------------

int VmModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4; // Name/Folder, VMID, Status, Type
}

// CORRECT implementation of headerData
QVariant VmModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return "Name / Folder";
            case 1: return "VMID";
            case 2: return "Status";
            case 3: return "Type";
        }
    }
    return QVariant();
}

QModelIndex VmModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem = getItem(parent);
    TreeItem *childItem = parentItem->children.value(row);
    
    if (childItem)
        return createIndex(row, column, childItem);
    
    return QModelIndex();
}

QModelIndex VmModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(child);
    TreeItem *parentItem = childItem->parent;

    if (parentItem == rootItem || !parentItem)
        return QModelIndex();

    // Correctly finding the parent's row number for the parent index
    return createIndex(parentItem->parent->children.indexOf(parentItem), 0, parentItem->parent);
}

int VmModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);
    return parentItem->children.count();
}

// CORRECT implementation of data (using 'int' for role)
QVariant VmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = getItem(index);

    if (role == Qt::DisplayRole) {
        if (item->isFolder) {
            if (index.column() == 0) return item->name;
            return QVariant(); // Only display name for folders
        } else {
            // Display data for VMs
            switch (index.column()) {
                case 0: return item->vmData.name;
                case 1: return item->vmData.vmid;
                case 2: return item->vmData.status;
                case 3: return item->vmData.type;
            }
        }
    }
    
    if (role == Qt::DecorationRole && index.column() == 0) {
        if (item->isFolder) {
            return QIcon::fromTheme("folder");
        } else if (item->vmData.type.toLower() == "qemu") {
            return QIcon::fromTheme("computer");
        } else if (item->vmData.type.toLower() == "lxc") {
            return QIcon::fromTheme("server");
        }
    }
    
    return QVariant();
}

// ----------------------------------------------------
// Data Population Logic
// ----------------------------------------------------

void VmModel::setVmList(const QVector<Vm>& vms)
{
    beginResetModel();
    
    // 1. Clear existing data
    qDeleteAll(rootItem->children);
    rootItem->children.clear();
    
    // 2. Map to hold folders (QString -> TreeItem*)
    QMap<QString, TreeItem*> folders;
    
    // 3. Build the tree structure
    for (const Vm& vm : vms) {
        QString folderName = vm.folder;
        
        // Find or create the folder item
        TreeItem* folderItem;
        if (!folders.contains(folderName)) {
            // Use the C++11 constructor
            folderItem = new TreeItem(folderName, true, Vm{}, {}, rootItem);
            folders.insert(folderName, folderItem);
            rootItem->children.append(folderItem);
        } else {
            folderItem = folders.value(folderName);
        }
        
        // Create the VM item and add it to the folder
        // Use the C++11 constructor
        TreeItem* vmItem = new TreeItem(vm.name, false, vm, {}, folderItem);
        folderItem->children.append(vmItem);
    }
    
    // Sort folders alphabetically
    std::sort(rootItem->children.begin(), rootItem->children.end(), 
              [](const TreeItem* a, const TreeItem* b) { return a->name < b->name; });

    // Sort VMs within each folder by VMID
    for (TreeItem* folder : rootItem->children) {
        std::sort(folder->children.begin(), folder->children.end(), 
                  [](const TreeItem* a, const TreeItem* b) { return a->vmData.vmid < b->vmData.vmid; });
    }

    endResetModel();
    qDebug() << "Model reset complete. Total folders:" << folders.size();
}