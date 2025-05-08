# airplay_response_socket.py — raw socket AirPlay response without Scapy

import socket
import struct
import time

MCAST_GRP = '224.0.0.251'
MCAST_PORT = 5353
MY_IP = '172.16.16.251'
MY_HOST = 'faketv.local'
SERVICE = '_airplay._tcp.local'
INSTANCE = 'LivingRoom._airplay._tcp.local'

TXT = (
    b'\x0fdeviceid=12:34:56:78:90:ab'
    b'\x13features=0x5A7FFFF7'
    b'\x10model=AppleTV5,3'
    b'\x10srcvers=220.68'
    b'\x10protovers=1.1'
    b'\x05vv=2'
    b'\x0cflags=0x4'
)

def build_dns_response():
    response = b''

    # DNS Header
    response += b'\x00\x00'     # Transaction ID
    response += b'\x84\x00'     # Flags: standard response, authoritative
    response += b'\x00\x00'     # Questions
    response += b'\x00\x04'     # Answers: 4
    response += b'\x00\x00'     # Authority RRs
    response += b'\x00\x00'     # Additional RRs

    def encode_name(name):
        parts = name.split('.')
        encoded = b''
        for part in parts:
            encoded += bytes([len(part)]) + part.encode()
        return encoded + b'\x00'

    # PTR
    response += encode_name(SERVICE)
    response += b'\x00\x0c'  # Type PTR
    response += b'\x00\x01'  # Class IN
    response += b'\x00\x00\x11\x94'  # TTL
    ptr_data = encode_name(INSTANCE)
    response += struct.pack('!H', len(ptr_data)) + ptr_data

    # SRV
    response += encode_name(INSTANCE)
    response += b'\x00\x21'  # Type SRV
    response += b'\x00\x01'
    response += b'\x00\x00\x00\x78'  # TTL
    srv_rdata = struct.pack('!HHH', 0, 0, 7000) + encode_name(MY_HOST)
    response += struct.pack('!H', len(srv_rdata)) + srv_rdata

    # TXT
    response += encode_name(INSTANCE)
    response += b'\x00\x10'  # Type TXT
    response += b'\x00\x01'
    response += b'\x00\x00\x00\x78'
    response += struct.pack('!H', len(TXT)) + TXT

    # A
    response += encode_name(MY_HOST)
    response += b'\x00\x01'  # Type A
    response += b'\x00\x01'
    response += b'\x00\x00\x00\x78'
    response += struct.pack('!H', 4)
    response += socket.inet_aton(MY_IP)

    return response

# setup multicast UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 255)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, 1)
sock.bind((MY_IP, 5353))

print("[*] sending raw multicast AirPlay response without Scapy...")

dns_response = build_dns_response()
while True:
    sock.sendto(dns_response, (MCAST_GRP, MCAST_PORT))
    time.sleep(2)
