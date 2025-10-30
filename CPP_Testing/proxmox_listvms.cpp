#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream> // Required for file I/O (local database)
#include <stdexcept> // Required for std::stoi exception handling

// Include external libraries (Ensure curl and json.hpp are available)
#include <curl/curl.h> // libcurl for HTTP requests
#include "json.hpp"    // nlohmann/json for JSON parsing

// Rename for convenience
using json = nlohmann::json;

// --- CONSTANTS ---
const int PROXMOX_PORT = 8006;
const bool VERIFY_SSL = false; // Set to true if you have a valid, trusted SSL cert
const std::string VM_FOLDERS_FILE = "vm_folders.json"; // Local file for persistence

// --- DATA STRUCTURES ---
struct Vm
{
    int vmid = 0;
    std::string type;
    std::string status;
    std::string node;
    std::string name;
    std::string folder = "Unassigned"; // New field for local grouping
};

// --- HELPER FUNCTION FOR CURL ---

/**
 * @brief Callback function for libcurl to write received data into a std::string.
 */
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// --- LOCAL PERSISTENCE FUNCTIONS ---

/**
 * @brief Loads the VM folder mapping from a local JSON file.
 * @return A map where key is VMID and value is the folder name.
 */
std::map<int, std::string> load_vm_folders() {
    std::map<int, std::string> folders;
    std::ifstream i(VM_FOLDERS_FILE);
    if (i.is_open()) {
        try {
            json j;
            i >> j;
            if (j.is_object()) {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    try {
                        // Keys are stored as strings in JSON but need to be ints here
                        int vmid = std::stoi(it.key());
                        folders[vmid] = it.value().get<std::string>();
                    } catch (const std::exception& e) {
                        std::cerr << "Warning: Skipping invalid entry in folder file: " << it.key() << std::endl;
                    }
                }
            }
            std::cout << "Loaded " << folders.size() << " VM folder assignments from " << VM_FOLDERS_FILE << "." << std::endl;
        } catch (const json::parse_error& e) {
            std::cerr << "Warning: Could not parse " << VM_FOLDERS_FILE << ". Starting with no folder assignments." << std::endl;
        }
        i.close();
    } else {
        std::cout << "Folder configuration file (" << VM_FOLDERS_FILE << ") not found. Starting fresh." << std::endl;
    }
    return folders;
}

/**
 * @brief Saves the current VM folder mapping to a local JSON file.
 */
void save_vm_folders(const std::map<int, std::string>& folders) {
    json j;
    // Store int keys as strings for JSON compatibility
    for (const auto& pair : folders) {
        j[std::to_string(pair.first)] = pair.second;
    }

    std::ofstream o(VM_FOLDERS_FILE);
    if (o.is_open()) {
        o << std::setw(4) << j << std::endl;
        std::cout << "Saved " << folders.size() << " VM folder assignments to " << VM_FOLDERS_FILE << "." << std::endl;
        o.close();
    } else {
        std::cerr << "Error: Could not open " << VM_FOLDERS_FILE << " for writing." << std::endl;
    }
}


// --- API FUNCTIONS (Core Networking) ---

/**
 * @brief Performs a generic authenticated GET request to the Proxmox API.
 */
std::string proxmox_get(const std::string& path, const std::string& auth_cookie, const std::string& csrf_token, const std::string& host)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    
    std::string url = "https://" + host + ":" + std::to_string(PROXMOX_PORT) + "/api2/json" + path;
    
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
        std::string cookie_header = "Cookie: " + auth_cookie;
        std::string csrf_header = "CSRFPreventionToken: " + csrf_token;
        
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
            std::cerr << "CURL Error (GET " << path << "): " << curl_easy_strerror(res) << " (HTTP: " << http_code << ")" << std::endl;
            return ""; 
        }
    }
    return readBuffer;
}

/**
 * @brief Performs a generic authenticated POST request to the Proxmox API.
 */
std::string proxmox_post(const std::string& path, const std::string& auth_cookie, const std::string& csrf_token, const std::string& host)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    
    std::string url = "https://" + host + ":" + std::to_string(PROXMOX_PORT) + "/api2/json" + path;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L); 
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ""); 

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (!VERIFY_SSL) {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        struct curl_slist *headers = NULL;
        std::string cookie_header = "Cookie: " + auth_cookie;
        std::string csrf_header = "CSRFPreventionToken: " + csrf_token;
        
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
            std::cerr << "CURL Error (POST " << path << "): " << curl_easy_strerror(res) << " (HTTP: " << http_code << ")" << std::endl;
            return ""; 
        }
    }
    return readBuffer;
}

/**
 * @brief Authenticates with Proxmox VE. (unchanged)
 */
std::map<std::string, std::string> proxmox_login(const std::string& password, const std::string& host, const std::string& username, const std::string& realm)
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
            std::cerr << "Login Failed. CURL Error: " << curl_easy_strerror(res) << " (HTTP: " << http_code << ")" << std::endl;
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
                std::cout << "Login successful for " << username_realm << "." << std::endl;
            } else {
                std::cerr << "Login Failed: Missing ticket or CSRF token in response." << std::endl;
            }

        } catch (const json::parse_error& e) {
            std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
        }
    }
    return tokens;
}

/**
 * @brief Fetches a list of VMs and containers and assigns local folders. (UPDATED)
 */
std::vector<Vm> get_vms_and_lxcs(const std::string& auth_cookie, const std::string& csrf_token, const std::string& host, const std::map<int, std::string>& vm_folders)
{
    std::cout << "Fetching accessible resources and applying local folders..." << std::endl;
    
    std::string path = "/cluster/resources?type=vm"; 
    std::string json_response = proxmox_get(path, auth_cookie, csrf_token, host);
    
    std::vector<Vm> vm_list;

    if (json_response.empty()) {
        std::cerr << "Failed to retrieve resources." << std::endl;
        return vm_list;
    }

    try {
        json response = json::parse(json_response);
        for (const auto& item : response["data"]) {
            std::string type = item.value("type", "");
            
            if (type == "qemu" || type == "lxc") {
                Vm vm;
                vm.vmid = item.value("vmid", 0);
                vm.type = type;
                vm.status = item.value("status", "N/A");
                vm.node = item.value("node", "N/A");
                vm.name = item.value("name", "N/A");
                
                // Assign folder based on local map
                auto folder_it = vm_folders.find(vm.vmid);
                if (folder_it != vm_folders.end()) {
                    vm.folder = folder_it->second;
                } else {
                    vm.folder = "Unassigned";
                }

                vm_list.push_back(vm);
            }
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
    }
    
    return vm_list;
}

/**
 * @brief Performs a VM/LXC power action (start, stop, shutdown, etc.). (unchanged)
 */
void perform_vm_action(const std::string& action, int vmid, const std::string& auth_cookie, const std::string& csrf_token, const std::string& host, const std::vector<Vm>& current_vms)
{
    auto it = std::find_if(current_vms.begin(), current_vms.end(), [vmid](const Vm& vm){
        return vm.vmid == vmid;
    });

    if (it == current_vms.end()) {
        std::cout << "Error: VMID " << vmid << " not found. Run 'list' to refresh resources." << std::endl;
        return;
    }

    const Vm& vm = *it;
    std::string vm_type_path = (vm.type == "qemu") ? "qemu" : "lxc";
    
    std::string api_path = "/nodes/" + vm.node + "/" + vm_type_path + "/" + std::to_string(vmid) + "/status/" + action;

    std::cout << "Attempting to send '" << action << "' command for VMID " << vmid << " (" << vm.name << ")..." << std::endl;
    
    std::string json_response = proxmox_post(api_path, auth_cookie, csrf_token, host);

    if (json_response.empty()) {
        std::cout << "Action failed or returned an error." << std::endl;
        return;
    }

    try {
        json response = json::parse(json_response);
        if (response.count("data")) {
            std::cout << "Success! Task ID: " << response["data"].get<std::string>() << ". Check the Proxmox UI for completion." << std::endl;
        } else {
            std::cout << "Action completed but response structure unexpected. Check server logs." << std::endl;
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parsing Error in action response: " << e.what() << std::endl;
    }
}

/**
 * @brief Displays the formatted VM list table, grouped by folder. (UPDATED)
 */
void display_vms(const std::vector<Vm>& vm_list)
{
    // 1. Group by folder name (map sorts keys alphabetically, which is a nice bonus)
    std::map<std::string, std::vector<Vm>> grouped_vms;
    for (const auto& vm : vm_list) {
        grouped_vms[vm.folder].push_back(vm);
    }

    std::cout << "\n\n";
    int total_resources = 0;

    // 2. Iterate through groups (folders)
    for (auto const& [folder_name, vms_in_folder] : grouped_vms)
    {
        // Print folder header
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "=== FOLDER: " << folder_name << " (Count: " << vms_in_folder.size() << ") ===" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        // Sort VMs within the current folder by VMID
        std::vector<Vm> sorted_vms = vms_in_folder;
        std::sort(sorted_vms.begin(), sorted_vms.end(), [](const Vm& a, const Vm& b) {
            return a.vmid < b.vmid;
        });

        // Print table header
        std::cout << "| " << std::left << std::setw(5) << "VMID" 
                  << " | " << std::left << std::setw(8) << "Type"
                  << " | " << std::left << std::setw(10) << "Status" 
                  << " | " << std::left << std::setw(10) << "Node" 
                  << " | " << std::left << std::setw(35) << "Name" << " |" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        
        // Print VMs in the folder
        for (const auto& vm : sorted_vms) {
            std::string display_name = vm.name.length() > 35 ? (vm.name.substr(0, 32) + "...") : vm.name;

            std::cout << "| " << std::left << std::setw(5) << vm.vmid
                      << " | " << std::left << std::setw(8) << vm.type
                      << " | " << std::left << std::setw(10) << vm.status
                      << " | " << std::left << std::setw(10) << vm.node
                      << " | " << std::left << std::setw(35) << display_name << " |" << std::endl;
            total_resources++;
        }
        std::cout << std::string(80, '-') << std::endl;
        std::cout << "\n"; // Extra newline between folders
    }
        
    std::cout << "Total accessible resources: " << total_resources << std::endl;
    std::cout << "\nAvailable Commands: list | start <vmid> | stop <vmid> | shutdown <vmid> | setfolder <vmid> <name> | exit" << std::endl;
}

// --- MAIN PROGRAM ---

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // --- Collect Configuration and Login ---
    std::string host, username, realm, password;
    
    std::cout << "\n--- Proxmox Server Configuration ---" << std::endl;
    std::cout << "Enter Proxmox Host (IP or Hostname): "; std::getline(std::cin, host);
    std::cout << "Enter Proxmox Username (e.g., user): "; std::getline(std::cin, username);
    std::cout << "Enter Login Realm (e.g., pam, pve, or your LDAP/AD realm): "; std::getline(std::cin, realm);
    std::cout << std::string(40, '-') << std::endl;
    
    std::cout << "Logging in as: " << username << "@" << realm << std::endl;
    std::cout << "Enter Proxmox Password: ";
    std::getline(std::cin, password);
    std::cout << std::string(40, '-') << std::endl;

    std::map<std::string, std::string> tokens = proxmox_login(password, host, username, realm);

    if (tokens.empty()) {
        std::cout << "\nScript aborted due to login failure." << std::endl;
        curl_global_cleanup();
        return 1;
    }

    std::string auth_cookie = tokens["PVEAuthCookie"];
    std::string csrf_token = tokens["CSRFPreventionToken"];
    
    // --- New: Load Local Folder Configuration ---
    std::map<int, std::string> vm_folders = load_vm_folders();
    std::vector<Vm> current_vms;

    // --- Initial VM List ---
    current_vms = get_vms_and_lxcs(auth_cookie, csrf_token, host, vm_folders);
    if (!current_vms.empty()) {
        display_vms(current_vms);
    } else {
        std::cout << "Warning: No resources found." << std::endl;
    }
    
    // --- Interactive Command Loop ---
    std::cout << "\n--- Interactive Client Started ---" << std::endl;
    std::string command_line;

    while (true) {
        std::cout << "\nproxmox> ";
        if (!std::getline(std::cin, command_line)) {
            break; // Exit on EOF
        }
        
        std::stringstream ss(command_line);
        std::string command;
        int vmid = -1;
        
        if (!(ss >> command)) { continue; }

        std::transform(command.begin(), command.end(), command.begin(), ::tolower);

        if (command == "exit") {
            break;
        } else if (command == "list") {
            // Re-fetch the list with the latest folder assignments
            current_vms = get_vms_and_lxcs(auth_cookie, csrf_token, host, vm_folders);
            if (!current_vms.empty()) {
                display_vms(current_vms);
            } else {
                std::cout << "Error: Failed to retrieve resources." << std::endl;
            }
        } else if (command == "start" || command == "stop" || command == "shutdown") {
            if (ss >> vmid && vmid > 0) {
                perform_vm_action(command, vmid, auth_cookie, csrf_token, host, current_vms);
                std::cout << "Action sent. Type 'list' to see the updated status." << std::endl;
            } else {
                std::cout << "Error: Missing or invalid VMID for '" << command << "'. Usage: " << command << " <vmid>" << std::endl;
            }
        } else if (command == "setfolder") {
            std::string folder_name;
            if (ss >> vmid && vmid > 0 && std::getline(ss, folder_name)) {
                // Trim leading whitespace from the rest of the line (the folder name)
                size_t first_char = folder_name.find_first_not_of(' ');
                if (std::string::npos != first_char) {
                    folder_name = folder_name.substr(first_char);
                } else {
                    folder_name = ""; // Handle empty string after trimming
                }

                if (folder_name.empty()) {
                    std::cout << "Error: Folder name cannot be empty. Usage: setfolder <vmid> <folder_name>" << std::endl;
                } else {
                    vm_folders[vmid] = folder_name;
                    save_vm_folders(vm_folders);
                    std::cout << "VMID " << vmid << " assigned to folder '" << folder_name << "'. Type 'list' to see the updated grouping." << std::endl;
                }
            } else {
                std::cout << "Error: Missing VMID or folder name. Usage: setfolder <vmid> <folder_name>" << std::endl;
            }
        }
        else {
            std::cout << "Unknown command. Available: list | start <vmid> | stop <vmid> | shutdown <vmid> | setfolder <vmid> <name> | exit" << std::endl;
        }
    }
    
    std::cout << "\nExiting Proxmox Client. Goodbye!" << std::endl;

    curl_global_cleanup();
    
    return 0;
}
