#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[94m'
NC='\033[0m' # No Color

# IP-address iPhone interface в monitor mode
IPHONE_IP="172.16.16.111"
IFACE="wlan1"

echo -e "${GREEN}[*] starting scan for $IPHONE_IP using $IFACE...${NC}"

for CH in {1..13}; do
    echo -e "${YELLOW}[*] switching to channel $CH...${NC}"
    sudo iwconfig $IFACE channel $CH
    echo -e "${BLUE}[*] listening for packets on channel $CH...${NC}"
    
    # слушаем 5 секунд на каждом канале, ждем хотя бы 1 пакет
    COUNT=$(sudo timeout 5 tcpdump -i $IFACE -n "host $IPHONE_IP" 2>/dev/null | wc -l)

    if [ "$COUNT" -gt 0 ]; then
        echo -e "${GREEN}[+] 🔥 found activity from $IPHONE_IP on channel $CH!${NC}"
        exit 0
    else
        echo -e "${RED}[-] no activity on channel $CH.${NC}"
    fi
done

echo "${RED}[!] no active channel found for $IPHONE_IP. Is it sleeping or AirPlay off?${NC}"