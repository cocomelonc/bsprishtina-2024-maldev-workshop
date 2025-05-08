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

# nmap scan
def scan_iphone(target_ip):
    print(Colors.BLUE + f"[+] scanning {target_ip} for iPhone..." + Colors.ENDC)
    # scan nmap for search iPhone
    result = subprocess.check_output(
        ["nmap", "-sV", "-Pn", "-T4", "-A", "--script", "http-headers", "-p7000", target_ip],
        stderr=subprocess.DEVNULL
    ).decode('utf-8')

    # search "iPhone"
    if "iphone" in result.lower():
        print(Colors.GREEN + f"[+] found iphone with ios at {target_ip}" + Colors.ENDC)
        print (result.lower())
    else:
        print(Colors.YELLOW + f"[-] no iphone found at {target_ip}" + Colors.ENDC)

# discover devices in network
def discover_devices(ip_range="192.168.8.0/24"):
    print(Colors.BLUE + f"[+] scanning network {ip_range}..." + Colors.ENDC)
    for i in range(1, 255):
        target_ip = f"192.168.8.{i}"
        scan_iphone(target_ip)

if __name__ == "__main__":
    discover_devices()
