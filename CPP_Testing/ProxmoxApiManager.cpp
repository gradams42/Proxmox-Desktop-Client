#include "ProxmoxApiManager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <QDebug> // For internal logging/debugging
#include <QStringList>
#include <curl/curl.h>

// --- CONSTANTS ---
const int PROXMOX_PORT = 8006;
const bool VERIFY_SSL = false; 
const std::string VM_FOLDERS_FILE = "vm_folders.json"; 

// --- HELPER FUNCTION FOR CURL (std::string based, same as original) ---

/**
 * @brief Callback function for libcurl to write received data into a std::string.
 */
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// --- LOCAL PERSISTENCE FUNCTIONS (Adapted from original) ---

/**
 * @brief Loads the VM folder mapping from a local JSON file.
 * @return A map where key is VMID and value is the folder name.
 */
std::map<int, std::string> ProxmoxApiManager::load_vm_folders() {
    std::map<int, std::string> folders;
    std::ifstream i(VM_FOLDERS_FILE);
    if (i.is_open()) {
        try {
            json j;
            i >> j;
            if (j.is_object()) {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    try {
                        int vmid = std::stoi(it.key());
                        folders[vmid] = it.value().get<std::string>();
                    } catch (const std::exception& e) {
                        qWarning() << "Warning: Skipping invalid entry in folder file:" << QString::fromStdString(it.key());
                    }
                }
            }
            qInfo() << "Loaded" << folders.size() << "VM folder assignments from" << QString::fromStdString(VM_FOLDERS_FILE);
        } catch (const json::parse_error& e) {
            qWarning() << "Warning: Could not parse" << QString::fromStdString(VM_FOLDERS_FILE) << ". Starting with no folder assignments.";
        }
        i.close();
    } else {
        qInfo() << "Folder configuration file (" << QString::fromStdString(VM_FOLDERS_FILE) << ") not found. Starting fresh.";
    }
    return folders;
}

/**
 * @brief Saves the current VM folder mapping to a local JSON file.
 */
void ProxmoxApiManager::save_vm_folders(const std::map<int, std::string>& folders) {
    json j;
    for (const auto& pair : folders) {
        j[std::to_string(pair.first)] = pair.second;
    }

    std::ofstream o(VM_FOLDERS_FILE);
    if (o.is_open()) {
        o << std::setw(4) << j << std::endl;
        qInfo() << "Saved" << folders.size() << "VM folder assignments to" << QString::fromStdString(VM_FOLDERS_FILE);
        o.close();
    } else {
        qCritical() << "Error: Could not open" << QString::fromStdString(VM_FOLDERS_FILE) << "for writing.";
    }
}


// --- CONSTRUCTOR & SLOTS (Qt Integration) ---

ProxmoxApiManager::ProxmoxApiManager(QObject *parent)
    : QObject(parent)
{
    // Load local data on initialization
    vm_folders_std = load_vm_folders();
}

/**
 * @brief Performs a generic authenticated GET request to the Proxmox API.
 */
std::string ProxmoxApiManager::proxmox_get(const std::string& path) const
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    
    std::string host_std = host_qt.toStdString();
    std::string auth_cookie_std = auth_cookie_qt.toStdString();
    std::string csrf_token_std = csrf_token_qt.toStdString();
    
    std::string url = "https://" + host_std + ":" + std::to_string(PROXMOX_PORT) + "/api2/json" + path;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (!VERIFY_SSL) {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        struct curl_slist *headers = NULL;
        std::string cookie_header = "Cookie: " + auth_cookie_std;
        std::string csrf_header = "CSRFPreventionToken: " + csrf_token_std;
        
        headers = curl_slist_append(headers, cookie_header.c_str());
        headers = curl_slist_append(headers, csrf_header.c_str());
        headers = curl_slist_append(headers, "Accept: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK || http_code != 200) {
            qWarning() << "CURL Error (GET" << QString::fromStdString(path) << "):" << curl_easy_strerror(res) << "(HTTP:" << http_code << ")";
            return ""; 
        }
    }
    return readBuffer;
}

/**
 * @brief Core login function. Returns tokens or an empty map on failure.
 */
std::map<std::string, std::string> ProxmoxApiManager::proxmox_login_core(
    const std::string& password, const std::string& host, 
    const std::string& username, const std::string& realm)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::map<std::string, std::string> tokens;
    
    std::string ticket_url = "https://" + host + ":" + std::to_string(PROXMOX_PORT) + "/api2/json/access/ticket";
    
    std::string username_realm = username + "@" + realm;
    std::string post_data = "username=" + username_realm + "&password=" + password;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, ticket_url.c_str()); 
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (!VERIFY_SSL) {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        res = curl_easy_perform(curl);
        
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        
        curl_easy_cleanup(curl);

        if (res != CURLE_OK || http_code != 200) {
            qCritical() << "Login Failed. CURL Error:" << curl_easy_strerror(res) << "(HTTP:" << http_code << ")";
            return tokens;
        }

        try {
            json response = json::parse(readBuffer);
            json data = response["data"];
            
            std::string ticket = data.value("ticket", "");
            std::string csrf_token = data.value("CSRFPreventionToken", "");

            if (!ticket.empty() && !csrf_token.empty()) {
                tokens["PVEAuthCookie"] = "PVEAuthCookie=" + ticket;
                tokens["CSRFPreventionToken"] = csrf_token;
            } else {
                qCritical() << "Login Failed: Missing ticket or CSRF token in response.";
            }

        } catch (const json::parse_error& e) {
            qCritical() << "JSON Parsing Error:" << e.what();
        }
    }
    return tokens;
}

// --- PUBLIC SLOTS (Main entry points for the GUI) ---

/**
 * @brief Handles the login process and emits signals.
 */
void ProxmoxApiManager::doLogin(const QString& host, const QString& username, const QString& realm, const QString& password)
{
    // Clear previous state
    auth_cookie_qt.clear();
    csrf_token_qt.clear();
    host_qt.clear();

    // Core login (uses std::string)
    std::map<std::string, std::string> tokens = proxmox_login_core(
        password.toStdString(), 
        host.toStdString(), 
        username.toStdString(), 
        realm.toStdString());

    if (!tokens.empty()) {
        host_qt = host;
        auth_cookie_qt = QString::fromStdString(tokens["PVEAuthCookie"]);
        csrf_token_qt = QString::fromStdString(tokens["CSRFPreventionToken"]);
        
        qInfo() << "Login successful for" << username + "@" + realm + ".";
        emit loginSuccess();
    } else {
        emit loginFailure("Login failed. Check host, username, password, and realm.");
    }
}

/**
 * @brief Fetches a list of VMs/LXC and assigns local folders, then emits vmListReady.
 */
void ProxmoxApiManager::fetchVmList()
{
    if (auth_cookie_qt.isEmpty() || csrf_token_qt.isEmpty()) {
        qCritical() << "Authentication tokens are missing. Please log in first.";
        return;
    }
    
    QString path = "/cluster/resources?type=vm"; 
    std::string json_response = proxmox_get(path.toStdString());
    
    QVector<Vm> vm_list;

    if (json_response.empty()) {
        qCritical() << "Failed to retrieve resources. Check Proxmox status and permissions.";
        return;
    }

    try {
        json response = json::parse(json_response);
        for (const auto& item : response["data"]) {
            std::string type_std = item.value("type", "");
            
            if (type_std == "qemu" || type_std == "lxc") {
                Vm vm;
                vm.vmid = item.value("vmid", 0);
                vm.type = QString::fromStdString(type_std);
                vm.status = QString::fromStdString(item.value("status", "N/A"));
                vm.node = QString::fromStdString(item.value("node", "N/A"));
                vm.name = QString::fromStdString(item.value("name", "N/A"));
                
                // Assign folder based on local map
                auto folder_it = vm_folders_std.find(vm.vmid);
                if (folder_it != vm_folders_std.end()) {
                    vm.folder = QString::fromStdString(folder_it->second);
                } else {
                    vm.folder = "Unassigned";
                }

                vm_list.push_back(vm);
            }
        }
    } catch (const json::parse_error& e) {
        qCritical() << "JSON Parsing Error:" << e.what();
    }
    
    emit vmListReady(vm_list);
}


/**
 * @brief Saves a folder assignment and updates the local persistence file.
 */
void ProxmoxApiManager::setVmFolder(int vmid, const QString& folderName)
{
    if (vmid <= 0 || folderName.trimmed().isEmpty()) {
        qWarning() << "Invalid VMID or empty folder name.";
        return;
    }
    
    std::string folderName_std = folderName.trimmed().toStdString();
    
    // Update the local map
    vm_folders_std[vmid] = folderName_std;
    
    // Save to disk
    save_vm_folders(vm_folders_std);
    
    emit actionSuccess(QString("VMID %1 assigned to folder '%2'. Refresh list to see grouping.").arg(vmid).arg(folderName));
}

// --- POST REQUEST / VM ACTION (Similar adaptation to proxmox_get) ---

/**
 * @brief Performs a generic authenticated POST request to the Proxmox API.
 * NOTE: This is implemented here for simplicity, but ideally should be separate 
 * from proxmox_get or use a common helper function.
 */
std::string proxmox_post_core(const std::string& path, const QString& auth_cookie_qt, const QString& csrf_token_qt, const QString& host_qt)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    
    std::string host_std = host_qt.toStdString();
    std::string auth_cookie_std = auth_cookie_qt.toStdString();
    std::string csrf_token_std = csrf_token_qt.toStdString();
    
    std::string url = "https://" + host_std + ":" + std::to_string(PROXMOX_PORT) + "/api2/json" + path;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L); 
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ""); // Empty POST for status actions

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (!VERIFY_SSL) {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        struct curl_slist *headers = NULL;
        std::string cookie_header = "Cookie: " + auth_cookie_std;
        std::string csrf_header = "CSRFPreventionToken: " + csrf_token_std;
        
        headers = curl_slist_append(headers, cookie_header.c_str());
        headers = curl_slist_append(headers, csrf_header.c_str());
        headers = curl_slist_append(headers, "Accept: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK || http_code != 200) {
            qWarning() << "CURL Error (POST" << QString::fromStdString(path) << "):" << curl_easy_strerror(res) << "(HTTP:" << http_code << ")";
            return ""; 
        }
    }
    return readBuffer;
}

/**
 * @brief Performs a VM/LXC power action (start, stop, shutdown).
 * NOTE: This requires the current VM list to be in memory in the UI layer 
 * (which is generally bad practice in MVC). A better design would pass the 
 * VM's node and type along with the VMID, but we'll adapt your original logic for now.
 * We'll use a placeholder since the full VM list state is better managed by the GUI.
 */
void ProxmoxApiManager::performVmAction(const QString& action, int vmid, const Vm& vm_data)
{
    if (vm_data.vmid == 0 || vm_data.node.isEmpty()) {
        emit actionSuccess(QString("Error: VMID %1 not found or data is incomplete.").arg(vmid));
        return;
    }
    
    QString vm_type_path = (vm_data.type.toLower() == "qemu") ? "qemu" : "lxc";
    
    QString api_path = QString("/nodes/%1/%2/%3/status/%4")
                            .arg(vm_data.node)
                            .arg(vm_type_path)
                            .arg(vmid)
                            .arg(action);

    qInfo() << "Attempting to send '" << action << "' command for VMID" << vmid << "(" << vm_data.name << ")...";
    
    std::string json_response = proxmox_post_core(api_path.toStdString(), auth_cookie_qt, csrf_token_qt, host_qt);

    if (json_response.empty()) {
        emit actionSuccess("Action failed or returned an error.");
        return;
    }

    try {
        json response = json::parse(json_response);
        if (response.count("data")) {
            QString taskId = QString::fromStdString(response["data"].get<std::string>());
            emit actionSuccess(QString("Success! Task ID: %1. Check the Proxmox UI.").arg(taskId));
        } else {
            emit actionSuccess("Action completed but response structure unexpected. Check server logs.");
        }
    } catch (const json::parse_error& e) {
        qCritical() << "JSON Parsing Error in action response:" << e.what();
    }
}