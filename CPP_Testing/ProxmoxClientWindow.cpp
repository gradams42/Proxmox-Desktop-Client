#include "ProxmoxClientWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout> // For structured layout
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QComboBox> 
#include <QLineEdit>
#include <QPushButton>
#include <QTimer> 
#include <QMenu>       // For context menu
#include <QInputDialog> // For folder creation prompt


ProxmoxClientWindow::ProxmoxClientWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // FIX 1: Set a decent default size
    resize(500, 350); 

    // Initialize core components
    apiManager = new ProxmoxApiManager(this);
    vmModel = new VmModel(this);
    
    // Connect Signals from the API Manager
    connect(apiManager, &ProxmoxApiManager::loginSuccess, this, &ProxmoxClientWindow::handleLoginSuccess);
    connect(apiManager, &ProxmoxApiManager::loginFailure, this, &ProxmoxClientWindow::handleLoginFailure);
    connect(apiManager, &ProxmoxApiManager::vmListReady, this, &ProxmoxClientWindow::handleVmListReady);
    connect(apiManager, &ProxmoxApiManager::actionSuccess, this, &ProxmoxClientWindow::handleActionSuccess);
    
    // Initial UI setup (show login form first)
    setupLoginUI();
    setWindowTitle("Proxmox Workstation Client - Login");
}

ProxmoxClientWindow::~ProxmoxClientWindow()
{
    // Components are deleted by the parent QObject (this) or qDeleteAll in VmModel
}

void ProxmoxClientWindow::setupLoginUI()
{
    QWidget *loginWidget = new QWidget;
    QGridLayout *layout = new QGridLayout(loginWidget);
    
    // Create member variables (declared in header) to avoid Seg Fault
    hostEdit = new QLineEdit("https://your.proxmox.host:8006");
    userEdit = new QLineEdit("root");
    passEdit = new QLineEdit;
    passEdit->setEchoMode(QLineEdit::Password);
    
    // FIX 2: Use QComboBox for realm selection
    realmCombo = new QComboBox;
    realmCombo->addItem("pam");
    realmCombo->addItem("pve");
    realmCombo->addItem("ldap"); // Example realms
    
    loginButton = new QPushButton("LOGIN");
    connect(loginButton, &QPushButton::clicked, this, &ProxmoxClientWindow::on_loginButton_clicked);
    
    // FIX 4: Build the structured login layout (based on the example image)
    
    // Add Labels and Input fields to the grid
    int row = 0;
    
    // Row 0: Server/Host
    layout->addWidget(new QLabel("Server/Host:"), row, 0);
    layout->addWidget(hostEdit, row, 1);
    row++;
    
    // Row 1: Username
    layout->addWidget(new QLabel("Username:"), row, 0);
    layout->addWidget(userEdit, row, 1);
    row++;
    
    // Row 2: Password
    layout->addWidget(new QLabel("Password:"), row, 0);
    layout->addWidget(passEdit, row, 1);
    row++;
    
    // Row 3: Realm (Dropdown)
    layout->addWidget(new QLabel("Realm:"), row, 0);
    layout->addWidget(realmCombo, row, 1);
    row++;

    // Row 4: Empty spacer for alignment
    row++;
    
    // Row 5: Login button (right-aligned using QHBoxLayout in a wrapper or span column 1)
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(loginButton);
    
    layout->addLayout(buttonLayout, row, 0, 1, 2); // Span two columns

    // Set the layout and central widget
    setCentralWidget(loginWidget);
}

void ProxmoxClientWindow::setupMainUI()
{
    // 1. Create the main splitter for left (VM list) and right (Console/View)
    splitter = new QSplitter(Qt::Horizontal, this);
    
    // 2. Left Side (VM Tree and Buttons)
    QWidget *leftPanel = new QWidget();
    vmTreeView = new QTreeView(leftPanel);
    vmTreeView->setModel(vmModel);
    
    // --- NEW: Context Menu Setup ---
    vmTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(vmTreeView, &QTreeView::customContextMenuRequested, 
             this, &ProxmoxClientWindow::on_vmTreeView_customContextMenuRequested);
    // -------------------------------
    
    // Connect double-click to view console/connect 
    connect(vmTreeView, &QTreeView::doubleClicked, this, &ProxmoxClientWindow::on_treeView_doubleClicked);

    // Initialize buttons
    refreshListButton = new QPushButton("Refresh List");
    startVmButton = new QPushButton("Start VM"); 
    createFolderButton = new QPushButton("New Folder"); // NEW BUTTON

    // Connect buttons to their slots
    connect(refreshListButton, &QPushButton::clicked, this, &ProxmoxClientWindow::on_listButton_clicked);
    connect(startVmButton, &QPushButton::clicked, this, &ProxmoxClientWindow::on_startVmButton_clicked);
    connect(createFolderButton, &QPushButton::clicked, this, &ProxmoxClientWindow::on_createFolderButton_clicked); // NEW CONNECTION

    // Layout the left panel with vmTreeView and buttons
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->addWidget(vmTreeView);
    
    // Create a horizontal layout for the buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(refreshListButton);
    buttonLayout->addWidget(startVmButton);
    buttonLayout->addWidget(createFolderButton); // ADD NEW BUTTON
    
    leftLayout->addLayout(buttonLayout);


    // 3. Right Side (VM Console/Log)
    consoleLog = new QTextEdit();
    consoleLog->setReadOnly(true);
    consoleLog->setText("Welcome to the Proxmox Client. Please refresh the VM list.");
    
    // 4. Add panels to splitter
    splitter->addWidget(leftPanel);
    splitter->addWidget(consoleLog);
    
    // Set the splitter as the central widget
    setCentralWidget(splitter);
    setWindowTitle("Proxmox Workstation Client");
    resize(1200, 800);
}

// --- Slot Implementations ---

void ProxmoxClientWindow::on_loginButton_clicked()
{
    // 1. Disable button/UI
    loginButton->setEnabled(false);
    
    // 2. Call the API Manager (using realmCombo->currentText() now)
    apiManager->doLogin(hostEdit->text(), userEdit->text(), realmCombo->currentText(), passEdit->text());
}

void ProxmoxClientWindow::handleLoginSuccess()
{
    // 1. Transition UI from login to main view
    setupMainUI(); 
    
    // 2. Automatically fetch the initial list
    apiManager->fetchVmList();
}

void ProxmoxClientWindow::handleLoginFailure(const QString& reason)
{
    if (loginButton) loginButton->setEnabled(true);
    QMessageBox::critical(this, "Login Failed", reason);
    if (consoleLog) consoleLog->append(QString("Login failed: %1").arg(reason));
}

void ProxmoxClientWindow::handleActionSuccess(const QString& message)
{
    QMessageBox::information(this, "Success", message);
    if (consoleLog) consoleLog->append(QString("Action successful: %1").arg(message));
    apiManager->fetchVmList(); // Refresh list after action
}

void ProxmoxClientWindow::handleVmListReady(const QVector<Vm>& vms)
{
    // 1. Pass the raw data to the model. This triggers beginResetModel/endResetModel internally.
    vmModel->setVmList(vms);
    
    if (vmTreeView) {
        // Use QTimer::singleShot to defer view updates, ensuring they happen AFTER 
        // the QTreeView has fully processed the endResetModel() signal.
        QTimer::singleShot(0, [this]() {
            // A. Expand all items. (Essential step)
            vmTreeView->expandAll(); 
            
            // C. Ensure columns are wide enough to display the data (prevents "invisible" data)
            vmTreeView->resizeColumnToContents(0); // Name / Folder
            vmTreeView->resizeColumnToContents(1); // VMID
            vmTreeView->resizeColumnToContents(2); // Status
            vmTreeView->resizeColumnToContents(3); // Type
        });
    }
    
    if (consoleLog) {
        consoleLog->append("VM list successfully loaded/refreshed.");
    }
}

void ProxmoxClientWindow::on_listButton_clicked()
{
    apiManager->fetchVmList();
}

void ProxmoxClientWindow::on_startVmButton_clicked()
{
    // Check if the main UI is set up
    if (!vmTreeView) return;

    QModelIndex index = vmTreeView->currentIndex();
    if (!index.isValid()) return;

    // Use getItem() from the model (or internalPointer())
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    
    if (item && !item->isFolder) {
        if (item->vmData.status.toLower() != "running") {
            // Call the API Manager slot to perform the action
            apiManager->performVmAction("start", item->vmData.vmid, item->vmData);
            if (consoleLog) consoleLog->append(QString("Attempting to START VMID: %1").arg(item->vmData.vmid));
        } else {
            if (consoleLog) consoleLog->append(QString("VMID %1 is already running.").arg(item->vmData.vmid));
        }
    }
}

void ProxmoxClientWindow::on_treeView_doubleClicked(const QModelIndex& index)
{
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    if (item && !item->isFolder) {
        // --- THIS IS WHERE YOU START THE REMOTE DISPLAY CONNECTION ---
        if (consoleLog) consoleLog->append(QString("Attempting to connect to console for VMID: %1").arg(item->vmData.vmid));
    }
}

// ----------------------------------------------------
// NEW FOLDER MANAGEMENT IMPLEMENTATION
// ----------------------------------------------------

void ProxmoxClientWindow::on_createFolderButton_clicked()
{
    // 1. Get folder name from the user via a modal dialog
    bool ok;
    QString folderName = QInputDialog::getText(this, 
                                               tr("Create New Folder"),
                                               tr("Folder Name:"), 
                                               QLineEdit::Normal,
                                               QString(), 
                                               &ok);

    if (ok && !folderName.trimmed().isEmpty()) {
        // 2. Pass the name to the model
        if (vmModel->createFolder(folderName)) {
            if (consoleLog) consoleLog->append(QString("Folder '%1' created successfully.").arg(folderName));
        } else {
            QMessageBox::warning(this, tr("Error"), 
                                 tr("A folder or VM named '%1' already exists at the root level.").arg(folderName));
        }
    }
}

void ProxmoxClientWindow::on_vmTreeView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = vmTreeView->indexAt(pos);
    if (!index.isValid()) return;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    
    // Only show the context menu on actual VM items
    if (item && !item->isFolder) {
        // Map the local position to global screen coordinates
        showVmContextMenu(index, vmTreeView->viewport()->mapToGlobal(pos));
    }
}

void ProxmoxClientWindow::showVmContextMenu(const QModelIndex& index, const QPoint& globalPos)
{
    TreeItem *vmItem = static_cast<TreeItem*>(index.internalPointer());
    if (!vmItem || vmItem->isFolder) return;

    QMenu menu(this);
    QMenu *moveToFolderMenu = menu.addMenu("Move to Folder");
    
    QStringList folders = vmModel->getFolderNames();
    
    // Add an action for each existing folder
    if (folders.isEmpty()) {
        QAction *noFolders = moveToFolderMenu->addAction("(No Folders Available)");
        noFolders->setEnabled(false);
    } else {
        for (const QString& folderName : folders) {
            
            // FIX: Use the public helper method isRootParent() to check if the item is unassigned
            QString currentParentName;
            
            if (vmModel->isRootParent(vmItem)) {
                currentParentName = "Unassigned"; 
            } else if (vmItem->parent) {
                currentParentName = vmItem->parent->name;
            } else {
                continue; // Should not happen for a valid VM item
            }
            
            // Only allow move if the destination is different from the current parent
            if (currentParentName.toLower() == folderName.toLower()) {
                QAction *currentFolder = moveToFolderMenu->addAction(folderName + " (Current)");
                currentFolder->setEnabled(false);
                continue;
            }
            
            QAction *folderAction = moveToFolderMenu->addAction(folderName);
            // Use a lambda to capture the VMID and folder name
            connect(folderAction, &QAction::triggered, this, [this, vmItem, folderName]() {
                if (vmModel->assignVmToFolder(vmItem->vmData.vmid, folderName)) {
                    if (consoleLog) consoleLog->append(QString("VM '%1' assigned to folder '%2'.").arg(vmItem->name).arg(folderName));
                } else {
                    QMessageBox::warning(this, tr("Move Error"), 
                                         tr("Failed to move VM %1 to folder %2. Check console log.").arg(vmItem->name).arg(folderName));
                }
            });
        }
    }
    
    // You can add other VM-specific actions here (e.g., Start/Stop)
    // menu.addAction(startVmButton->text());
    
    menu.exec(globalPos);
}