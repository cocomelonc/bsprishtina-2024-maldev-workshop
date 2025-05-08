import socket
import time
from scapy.all import *

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

iface = "wlan0"  # ← scanning interface
discovered_targets = set()

def mdns_discover(timeout=10):
    print(Colors.BLUE + f"scanning for AirPlay devices via mdns ({timeout}s)..." + Colors.ENDC)

    def handle_pkt(pkt):
        if pkt.haslayer(DNSRR) and "_airplay._tcp.local" in str(pkt[DNSRR].rrname.lower()):
            print (pkt[DNSRR].rrname)
            ip = pkt[IP].src
            if ip not in discovered_targets:
                print(Colors.GREEN + f"found AirPlay device: {ip}" + Colors.ENDC)
                discovered_targets.add(ip)
        if pkt.haslayer(DNSQR) and "_airplay.tcp.local" in str(pkt[DNSQR].qname.lower())):
            print (pkt[DNSQR].qname)
            ip = pkt[IP].src
            if ip not in discovered_targets:
                print(Colors.GREEN + f"found AirPlay device: {ip}" + Colors.ENDC)
                discovered_targets.add(ip)

    sniff(
        iface=iface,
        filter="udp port 5353",
        prn=handle_pkt,
        timeout=timeout
    )

if __name__ == "__main__":
    print(Colors.GREEN + "starting AirPlay scan...\n\n" + Colors.ENDC)
    try:
        while True:
            discovered_targets.clear()
            mdns_discover(timeout=10)

            if not discovered_targets:
                print(Colors.RED + "AirPlay devices not found at this round." + Colors.ENDC)

            print(Colors.YELLOW + "go to next scan cycle...\n" + Colors.ENDC)
            time.sleep(5)

    except KeyboardInterrupt:
        print(Colors.GREEN + "\ninterrupted by user. exiting." + Colors.ENDC)
