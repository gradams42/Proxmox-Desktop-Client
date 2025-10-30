#include "VmModel.h"
#include <QDebug>
#include <QIcon>
#include <QMap> // Added for setVmList logic
#include <algorithm> // For std::sort
#include <functional> // For std::function in findVmItem

// NOTE: Implementation of TreeItem::row() is still required in the .cpp file, but is not needed for the current fixes.

// --- TreeItem Utility ---

// Helper: Finds the row of a TreeItem within its parent's children list.
int TreeItem::row() const
{
    if (parent)
        return parent->children.indexOf(const_cast<TreeItem*>(this));
    return 0; // Or -1 if the root item is not expected to call this
}

TreeItem* VmModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

// Helper: Finds a VM item anywhere in the tree by its VMID
TreeItem *VmModel::findVmItem(int vmid) const
{
    // Simple recursive search (can be optimized but is fine for moderate lists)
    std::function<TreeItem*(TreeItem*)> search = 
        [&](TreeItem* parentItem) -> TreeItem* {
        
        for (TreeItem* child : parentItem->children) {
            if (!child->isFolder) {
                if (child->vmData.vmid == vmid) {
                    return child;
                }
            } else {
                // Recurse into folders
                TreeItem* found = search(child);
                if (found) return found;
            }
        }
        return nullptr;
    };
    
    return search(rootItem);
}

// Helper: Finds a folder item directly under the root by name
TreeItem *VmModel::findFolderItem(const QString& folderName) const
{
    for (TreeItem* item : rootItem->children) {
        if (item->isFolder && item->name.toLower() == folderName.toLower()) {
            return item;
        }
    }
    return nullptr;
}


// --- VmModel Implementation ---

// Constructor: Initializes the hidden root item
VmModel::VmModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    // The root item is hidden, representing the entire collection
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
    // Check bounds before calling .value(row)
    if (row < 0 || row >= parentItem->children.count())
        return QModelIndex();
    
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
    
    // Safety check: parentItem->parent should always be valid here (it's the root or another folder)
    if (!parentItem->parent)
        return QModelIndex();

    // Correctly finding the parent's row number for the parent index
    return createIndex(parentItem->row(), 0, parentItem->parent);
}

int VmModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);
    
    // DEBUG: Check rowCount for the requested parent item
    qDebug() << "rowCount called for parent:" << parentItem->name << "Children count:" << parentItem->children.count();
    
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
            return QVariant(); // Only display name in column 0 for folders
        } else {
            // DEBUG: Check VM data being accessed for display
            if (index.column() == 0) {
                 qDebug() << "Data for VM:" << item->vmData.name 
                          << "VMID:" << item->vmData.vmid 
                          << "Status:" << item->vmData.status;
            }
            
            // Display data for VMs
            switch (index.column()) {
                case 0: return item->vmData.name;
                // FIX: Explicitly convert int to QString for safe QVariant storage
                case 1: return QString::number(item->vmData.vmid); 
                case 2: return item->vmData.status;
                case 3: return item->vmData.type;
            }
        }
    }
    
    if (role == Qt::DecorationRole && index.column() == 0) {
        if (item->isFolder) {
            return QIcon::fromTheme("folder");
        } else {
            // Logic for non-folder VM items
            QString vmType = item->vmData.type.toLower();
            
            if (vmType == "qemu") {
                return QIcon::fromTheme("computer"); 
            } else if (vmType == "lxc") {
                // FIX: Change "server" to "system-monitor" to avoid folder icon confusion
                return QIcon::fromTheme("system-monitor"); 
            }
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
    
    // 2. Map to hold actual, user-defined folders (QString -> TreeItem*)
    QMap<QString, TreeItem*> folders;
    
    // 3. Build the tree structure
    for (const Vm& vm : vms) {
        // NOTE: Assuming Vm struct has a 'folder' member (previously 'folderTag')
        QString folderName = vm.folder.trimmed();
        
        // Treat "Unassigned" (likely the parser's default) as a root-level item.
        // This makes the list flat by default.
        if (folderName.isEmpty() || folderName.toLower() == "unassigned") {
            // SCENARIO 1: No Folder Tag or 'Unassigned' default. Display VM directly under root.
            
            // Create the VM item and add it directly to the root
            // *** FIX CONSTRUCTOR CALL ***: Use TreeItem(const Vm& data, TreeItem *parentItem)
            TreeItem* vmItem = new TreeItem(vm, rootItem); 
            rootItem->children.append(vmItem);
        } else {
            // SCENARIO 2: A custom Folder Tag is present. Group VM under a folder item.

            // Find or create the custom folder item
            TreeItem* folderItem;
            if (!folders.contains(folderName)) {
                // Create new folder, child of rootItem
                // *** FIX CONSTRUCTOR CALL ***: Use TreeItem(const QString& itemName, bool folder, TreeItem *parentItem)
                folderItem = new TreeItem(folderName, true, rootItem); 
                folders.insert(folderName, folderItem);
                rootItem->children.append(folderItem);
            } else {
                folderItem = folders.value(folderName);
            }
            
            // Create the VM item and add it to the folder
            // *** FIX CONSTRUCTOR CALL ***: Use TreeItem(const Vm& data, TreeItem *parentItem)
            TreeItem* vmItem = new TreeItem(vm, folderItem);
            folderItem->children.append(vmItem);
        }
    }
    
    // 4. Sorting: 
    // Sort top-level items (Folders and standalone VMs) alphabetically by name/folder name.
    std::sort(rootItem->children.begin(), rootItem->children.end(), 
              [](const TreeItem* a, const TreeItem* b) { 
                return a->name < b->name; 
              });

    // Sort VMs within each folder by VMID
    for (TreeItem* folder : rootItem->children) {
        if (folder->isFolder) { // Only sort children of actual folder items
            std::sort(folder->children.begin(), folder->children.end(), 
                      [](const TreeItem* a, const TreeItem* b) { return a->vmData.vmid < b->vmData.vmid; });
        }
    }

    endResetModel();
}

// ----------------------------------------------------
// New Folder Management Implementation
// ----------------------------------------------------

bool VmModel::createFolder(const QString& name)
{
    QString trimmedName = name.trimmed();
    if (trimmedName.isEmpty()) return false;

    // Check for conflict with existing top-level folders or VMs
    for (const TreeItem* item : rootItem->children) {
        if (item->name.toLower() == trimmedName.toLower()) {
            qDebug() << "Folder or VM named" << trimmedName << "already exists at the root.";
            return false;
        }
    }
    
    // 1. Notify the view that rows are about to be inserted at the root level
    beginInsertRows(QModelIndex(), rootItem->children.count(), rootItem->children.count());
    
    // 2. Create the new folder item as a child of the root
    // *** FIX CONSTRUCTOR CALL ***: Use TreeItem(const QString& itemName, bool folder, TreeItem *parentItem)
    TreeItem* newFolder = new TreeItem(trimmedName, true, rootItem); 
    rootItem->children.append(newFolder);
    
    // 3. Notify the view that rows have been inserted
    endInsertRows();

    // OPTIONAL: Re-sort the root level after insertion to maintain order
    // Note: Re-sorting immediately causes issues with Qt's index management. 
    // It's better to rely on the next refresh or use QSortFilterProxyModel. 
    // For manual creation, we'll skip the immediate sort for stability.
    
    return true;
}

bool VmModel::assignVmToFolder(int vmid, const QString& folderName)
{
    TreeItem* vmItem = findVmItem(vmid);
    TreeItem* destinationFolder = findFolderItem(folderName);

    if (!vmItem || !destinationFolder || !destinationFolder->isFolder) {
        qDebug() << "Cannot assign VM:" << vmid << "to folder:" << folderName << ". Item(s) not found or folder is invalid.";
        return false;
    }
    
    TreeItem* currentParent = vmItem->parent;
    if (currentParent == destinationFolder) {
        qDebug() << "VM is already in the destination folder.";
        return true;
    }

    // 1. Remove the VM from its current parent
    int oldRow = currentParent->children.indexOf(vmItem);
    if (oldRow >= 0) {
        // Get the QModelIndex for the current parent before removal
        QModelIndex parentIndex = (currentParent == rootItem) ? QModelIndex() : createIndex(currentParent->row(), 0, currentParent->parent);
        
        beginRemoveRows(parentIndex, oldRow, oldRow);
        currentParent->children.removeAt(oldRow);
        endRemoveRows();
    } else {
        qDebug() << "Error: VM not found in its expected parent's children list.";
        return false;
    }

    // 2. Insert the VM into the new destination folder
    int newRow = destinationFolder->children.count();
    // Get the QModelIndex for the destination parent
    QModelIndex destIndex = (destinationFolder == rootItem) ? QModelIndex() : createIndex(destinationFolder->row(), 0, destinationFolder->parent);

    beginInsertRows(destIndex, newRow, newRow);
    destinationFolder->children.append(vmItem);
    vmItem->parent = destinationFolder; // Update parent pointer
    endInsertRows();

    // Optional: Re-sort the destination folder children
    std::sort(destinationFolder->children.begin(), destinationFolder->children.end(), 
              [](const TreeItem* a, const TreeItem* b) { return a->vmData.vmid < b->vmData.vmid; });
              
    // Since the index might have changed due to the sort, we notify the view
    // that the data in the folder might have changed (for the whole column 0)
    emit dataChanged(index(0, 0, destIndex), 
                     index(destinationFolder->children.count() - 1, 0, destIndex));

    return true;
}

QStringList VmModel::getFolderNames() const
{
    QStringList folderNames;
    for (const TreeItem* item : rootItem->children) {
        if (item->isFolder) {
            folderNames.append(item->name);
        }
    }
    return folderNames;
}

// --- NEW: Public helper implementation ---
bool VmModel::isRootParent(const TreeItem* item) const
{
    // Check if the item's parent pointer matches the private rootItem pointer
    return item && (item->parent == rootItem);
}

// NOTE: This implementation relies on VmModel.h having the two TreeItem constructors:
// 1. explicit TreeItem(const QString& itemName, bool folder = true, TreeItem *parentItem = nullptr)
// 2. explicit TreeItem(const Vm& data, TreeItem *parentItem = nullptr)