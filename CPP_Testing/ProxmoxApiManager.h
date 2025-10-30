#ifndef PROXMOXAPIMANAGER_H
#define PROXMOXAPIMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVector>
#include <map>
#include <string>
#include "json.hpp" // Ensure nlohmann/json is accessible

using json = nlohmann::json;

// --- DATA STRUCTURES (Using Qt types where possible for GUI compatibility) ---
struct Vm
{
    int vmid = 0;
    QString type;
    QString status;
    QString node;
    QString name;
    QString folder = "Unassigned"; 
};

// Required for Q_DECLARE_METATYPE so Vm can be used in Signals/Slots
Q_DECLARE_METATYPE(Vm)

class ProxmoxApiManager : public QObject
{
    Q_OBJECT

public:
    explicit ProxmoxApiManager(QObject *parent = nullptr);

    // --- Public accessors for tokens ---
    QString getAuthCookie() const { return auth_cookie_qt; }
    QString getCsrfToken() const { return csrf_token_qt; }
    QString getHost() const { return host_qt; }

public slots:
    // Initiates login (will run on a background thread if implemented correctly)
    void doLogin(const QString& host, const QString& username, const QString& realm, const QString& password);
    
    // Initiates VM list fetch
    void fetchVmList();
    
    // Saves a folder assignment
    void setVmFolder(int vmid, const QString& folderName);

    // FIX: ADDED MISSING DECLARATION FOR THE VM ACTION METHOD (Declared as slot for signal connection)
    void performVmAction(const QString& action, int vmid, const Vm& vm_data);

signals:
    // Emitted on login success/failure
    void loginSuccess();
    void loginFailure(const QString& reason);
    
    // Emitted when the VM list is ready
    void vmListReady(const QVector<Vm>& vms);
    
    // Emitted when an action is successful
    void actionSuccess(const QString& message);
    
private:
    // --- Member variables for state ---
    QString host_qt;
    QString auth_cookie_qt;
    QString csrf_token_qt;
    
    // Local persistence map (int VMID -> QString Folder)
    std::map<int, std::string> vm_folders_std; 
    
    // --- Adapted versions of your existing functions (private implementation) ---
    std::map<std::string, std::string> proxmox_login_core(const std::string& password, const std::string& host, const std::string& username, const std::string& realm);
    std::string proxmox_get(const std::string& path) const;
    
    // Local persistence (your original code)
    std::map<int, std::string> load_vm_folders();
    void save_vm_folders(const std::map<int, std::string>& folders);
};

#endif // PROXMOXAPIMANAGER_H