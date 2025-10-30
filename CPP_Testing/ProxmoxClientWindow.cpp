#include "ProxmoxClientWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout> // For structured layout
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QComboBox> // For realm dropdown
#include <QLineEdit>
#include <QPushButton>

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
    // ... (rest of the setupMainUI() function remains the same)
    
    // 1. Create the main splitter for left (VM list) and right (Console/View)
    splitter = new QSplitter(Qt::Horizontal, this);
    
    // 2. Left Side (VM Tree)
    QWidget *leftPanel = new QWidget();
    vmTreeView = new QTreeView(leftPanel);
    vmTreeView->setModel(vmModel);
    // Connect double-click to view console/connect (TODO: implement VNC/SPICE client here)
    connect(vmTreeView, &QTreeView::doubleClicked, this, &ProxmoxClientWindow::on_treeView_doubleClicked);

    // Add buttons (Start/Stop/Shutdown/List) to the left panel
    QPushButton *listButton = new QPushButton("Refresh List");
    QPushButton *startVmButton = new QPushButton("Start VM"); // Added for the slot below
    
    connect(listButton, &QPushButton::clicked, this, &ProxmoxClientWindow::on_listButton_clicked);
    connect(startVmButton, &QPushButton::clicked, this, &ProxmoxClientWindow::on_startVmButton_clicked);

    // Layout the left panel with vmTreeView and buttons
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->addWidget(vmTreeView);
    leftLayout->addWidget(listButton);
    leftLayout->addWidget(startVmButton); // Add the button to the layout

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

// FIX: ADDED MISSING IMPLEMENTATION
void ProxmoxClientWindow::handleLoginFailure(const QString& reason)
{
    if (loginButton) loginButton->setEnabled(true);
    QMessageBox::critical(this, "Login Failed", reason);
    if (consoleLog) consoleLog->append(QString("Login failed: %1").arg(reason));
}

// FIX: ADDED MISSING IMPLEMENTATION
void ProxmoxClientWindow::handleActionSuccess(const QString& message)
{
    QMessageBox::information(this, "Success", message);
    if (consoleLog) consoleLog->append(QString("Action successful: %1").arg(message));
    apiManager->fetchVmList(); // Refresh list after action
}

void ProxmoxClientWindow::handleVmListReady(const QVector<Vm>& vms)
{
    // Pass the raw data to the model for structuring
    vmModel->setVmList(vms);
    vmTreeView->expandAll(); // Show the folders
    if (consoleLog) consoleLog->append("VM list successfully loaded/refreshed.");
}

void ProxmoxClientWindow::on_listButton_clicked()
{
    apiManager->fetchVmList();
}

// FIX: ADDED MISSING IMPLEMENTATION
void ProxmoxClientWindow::on_startVmButton_clicked()
{
    // Check if the main UI is set up
    if (!vmTreeView) return;

    QModelIndex index = vmTreeView->currentIndex();
    if (!index.isValid()) return;

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