#ifndef PROXMOXCLIENTWINDOW_H
#define PROXMOXCLIENTWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTreeView>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox> // NEW: Include QComboBox for the Realm dropdown
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

private:
    // --- GUI Elements ---
    QTreeView *vmTreeView = nullptr; // Initialize pointers to nullptr to prevent Seg Fault on access
    QTextEdit *consoleLog = nullptr;
    QSplitter *splitter = nullptr;
    
    // Login form elements (Declared as members to prevent Seg Fault)
    QLineEdit *hostEdit = nullptr;
    QLineEdit *userEdit = nullptr;
    QComboBox *realmCombo = nullptr; // CHANGED: Replaced QLineEdit with QComboBox
    QLineEdit *passEdit = nullptr;
    QPushButton *loginButton = nullptr;

    // --- Core Logic ---
    ProxmoxApiManager *apiManager = nullptr;
    VmModel *vmModel = nullptr;
    
    // --- Helper function for UI setup ---
    void setupLoginUI();
    void setupMainUI();
};

#endif // PROXMOXCLIENTWINDOW_H