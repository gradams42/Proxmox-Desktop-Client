#ifndef PROXMOXCLIENTWINDOW_H
#define PROXMOXCLIENTWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTreeView>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox> // NEW: Include QComboBox for the Realm dropdown
#include <QMenu>     // NEW: Include QMenu for context menu
#include "ProxmoxApiManager.h"
#include "VmModel.h"

class ProxmoxClientWindow : public QMainWindow
{
        Q_OBJECT

public:
        explicit ProxmoxClientWindow(QWidget *parent = nullptr);
        ~ProxmoxClientWindow();

private slots:
        void handleLoginSuccess();
        void handleLoginFailure(const QString& reason);
        void handleVmListReady(const QVector<Vm>& vms);
        void handleActionSuccess(const QString& message);
        
        // User interactions
        void on_loginButton_clicked();
        void on_listButton_clicked();
        void on_treeView_doubleClicked(const QModelIndex& index);
        void on_startVmButton_clicked();

    // --- NEW FOLDER MANAGEMENT SLOTS ---
    void on_createFolderButton_clicked();
    void on_vmTreeView_customContextMenuRequested(const QPoint &pos);
    // ------------------------------------

private:
        // --- GUI Elements ---
        QTreeView *vmTreeView = nullptr; // Initialize pointers to nullptr to prevent Seg Fault on access
        QTextEdit *consoleLog = nullptr;
        QSplitter *splitter = nullptr;
        
        // Login form elements (Declared as members to prevent Seg Fault)
        QLineEdit *hostEdit = nullptr;
        QLineEdit *userEdit = nullptr;
        QComboBox *realmCombo = nullptr; 
        QLineEdit *passEdit = nullptr;
        QPushButton *loginButton = nullptr;

    // --- MAIN UI BUTTONS ---
    QPushButton *startVmButton = nullptr; // Ensure this is a member for accessibility
    QPushButton *refreshListButton = nullptr; // Ensure this is a member for consistency
    QPushButton *createFolderButton = nullptr; // NEW: Button to create a new folder
    // -----------------------

        // --- Core Logic ---
        ProxmoxApiManager *apiManager = nullptr;
        VmModel *vmModel = nullptr;
        
        // --- Helper functions for UI setup ---
        void setupLoginUI();
        void setupMainUI();
    void showVmContextMenu(const QModelIndex& index, const QPoint& globalPos); // NEW: Context menu helper
};

#endif // PROXMOXCLIENTWINDOW_H