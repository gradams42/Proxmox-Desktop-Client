import requests
import json
import urllib3
from getpass import getpass

# Suppress warnings for insecure requests (if using skip_ssl=True)
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

# --- CONFIGURATION ---
PROXMOX_PORT = 8006
VERIFY_SSL = False                          # Set to True if you have a valid, trusted SSL cert

# --- FUNCTIONS ---
def proxmox_login(api_base_url, username_full, password, verify_ssl):
    """Authenticates with Proxmox VE and returns the PVEAuthCookie and CSRF token."""
    
    print("Attempting to log in...")
    
    ticket_endpoint = f"{api_base_url}/access/ticket"
    
    login_data = {
        'username': username_full,
        'password': password
    }

    try:
        # Step 1: Request an authentication ticket
        response = requests.post(ticket_endpoint, data=login_data, verify=verify_ssl)
        response.raise_for_status() # Raises an HTTPError for bad responses (4xx or 5xx)
        
        login_data = response.json().get('data')

        if not login_data:
            print("Login failed: Could not retrieve data from response.")
            return None, None
            
        ticket = login_data.get('ticket')
        csrf_token = login_data.get('CSRFPreventionToken')
        
        if not ticket or not csrf_token:
            print("Login failed: Ticket or CSRF token missing in response.")
            return None, None

        # The cookie required for subsequent requests
        auth_cookie = f"PVEAuthCookie={ticket}"
        
        print(f"Login successful for {username_full}.")
        return auth_cookie, csrf_token
        
    except requests.exceptions.RequestException as e:
        print(f"Login failed: A request error occurred ({e})")
        if 'response' in locals():
             print(f"Server response status: {response.status_code}")
             try:
                 error_details = response.json()
                 print(f"Server error details: {json.dumps(error_details, indent=4)}")
             except json.JSONDecodeError:
                 pass
        return None, None


def get_vms_and_lxcs(api_base_url, auth_cookie, csrf_token, verify_ssl):
    """Fetches a list of VMs and containers using the authentication tokens."""
    
    print("Fetching accessible resources...")
    
    resources_endpoint = f"{api_base_url}/cluster/resources"
    
    # Headers required for all authenticated API requests
    headers = {
        'Cookie': auth_cookie,
        'CSRFPreventionToken': csrf_token,
        'Accept': 'application/json'
    }
    
    # Query parameters to filter for only VMs (qemu) and containers (lxc)
    params = {'type': 'vm'} 

    try:
        response = requests.get(resources_endpoint, headers=headers, params=params, verify=verify_ssl)
        response.raise_for_status()
        
        resources = response.json().get('data', [])
        
        # Filter the raw list to only include qemu (VMs) and lxc (Containers)
        vm_list = [r for r in resources if r.get('type') in ['qemu', 'lxc']]
        
        return vm_list
        
    except requests.exceptions.RequestException as e:
        print(f"Failed to fetch resources: A request error occurred ({e})")
        return None


def main():
    """Main function to run the script."""
    
    # --- Collect Configuration ---
    proxmox_host = input("Enter Proxmox IP/Hostname: ")
    username = input("Enter Proxmox Username (e.g., user): ")
    realm = input("Enter Proxmox Realm (e.g., pam, pve, or company.com): ")
    password = getpass("Enter Proxmox Password: ")
    
    username_full = f"{username}@{realm}"
    api_base_url = f"https://{proxmox_host}:{PROXMOX_PORT}/api2/json"
    
    print("-" * 40)
    print(f"Attempting connection to: {proxmox_host}:{PROXMOX_PORT}")

    # --- Login ---
    auth_cookie, csrf_token = proxmox_login(api_base_url, username_full, password, VERIFY_SSL)

    if not auth_cookie:
        print("\nScript aborted due to login failure.")
        return

    # --- Get VM List ---
    vm_list = get_vms_and_lxcs(api_base_url, auth_cookie, csrf_token, VERIFY_SSL)

    if not vm_list:
        print("No VMs or LXCs found or failed to retrieve list.")
        return

    # --- Display Results ---
    print("\n" * 2)
    print(f"| {'VMID':<5} | {'Type':<8} | {'Status':<10} | {'Node':<10} | {'Name':<35} |")
    print("-" * 80)
    
    for vm in sorted(vm_list, key=lambda x: x.get('vmid', 0)):
        vmid = vm.get('vmid', 'N/A')
        vtype = vm.get('type', 'N/A')
        status = vm.get('status', 'N/A')
        node = vm.get('node', 'N/A')
        name = vm.get('name', 'N/A')
        
        # Truncate long names for clean formatting
        display_name = (name[:32] + '...') if len(name) > 35 else name
        
        print(f"| {vmid:<5} | {vtype:<8} | {status:<10} | {node:<10} | {display_name:<35} |")
        
    print("-" * 80)
    print(f"Total accessible resources: {len(vm_list)}")
    print("\nVerification complete.")

if __name__ == "__main__":
    main()
