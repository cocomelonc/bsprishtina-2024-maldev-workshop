import subprocess
import re


class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    PURPLE = '\033[95m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def scan_network(ip_range):
    print(Colors.GREEN + "scanning local network using nmap..." + Colors.ENDC)
    result = subprocess.run(['nmap', '-sn', ip_range], capture_output=True, text=True)
    lines = result.stdout.splitlines()
    ips = []

    for i in range(len(lines)):
        if 'nmap scan report for' in lines[i].lower():
            ip = lines[i].split()[-1].replace("(", "").replace(")", "")
            print (Colors.YELLOW + ip + Colors.ENDC)
            ips.append(ip)

    return ips

# nmap scan
def scan_iphone(target_ip):
    print(Colors.BLUE + f"[+] scanning {target_ip} for iPhone..." + Colors.ENDC)
    # scan nmap for search iPhone
    result = subprocess.check_output(
        ["nmap", "-sV", "-Pn", "-T4", "-A", "-p62078", target_ip],
        stderr=subprocess.DEVNULL
    ).decode('utf-8')


    print (result.lower())
    # search "iPhone"
    # if "iphone" in result.lower():
    match = re.search(r"62078/tcp (filtered|open)", result.lower())
    if match:
        print(Colors.GREEN + f"[+] found iphone with ios at {target_ip}" + Colors.ENDC)
        print (result.lower())
    else:
        print(Colors.YELLOW + f"[-] no iphone found at {target_ip}" + Colors.ENDC)

# discover devices in network
def discover_devices(ip_range="192.168.8.0/24"):
    ips = scan_network(ip_range)
    print (ips)
    print(Colors.BLUE + f"[+] scanning network {ip_range}..." + Colors.ENDC)
    for target_ip in ips:
        scan_iphone(target_ip)

if __name__ == "__main__":
    discover_devices()
