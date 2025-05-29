﷽

# WINDOWS

### Task 1: Write simple reverse shell (10 points)

- Objective: Write a basic program that create reverse shell with Windows API functions.
- Skills Tested: Understanding WINAPI.
- Materials: Refer to the 03-winapi directory for basic examples.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 2: Basic DLL Injection (10 points)

- Objective: Write a basic program that injects a DLL into a running process using Windows API functions.
- Skills Tested: DLL injection, `CreateRemoteThread`, `VirtualAllocEx`.
- Materials: Refer to the 03-injection directory for basic examples.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 3: Basic Code Injection (10 points) 

- Objective: Implement payload injection.
- Skills Tested: Code injection methods.
- Materials: Refer to the 03-injection directory for basic examples.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 4: Dropper Creation (10 points)

- Objective: Write a dropper that drops a payload (e.g., a DLL or executable) on a victim's system and executes it.
- Skills Tested: File handling, process creation, and execution.
- Materials: Create an executable that downloads or copies a payload and runs it.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 5: Basic Stealer (Information Stealer, 10 points) 

- Objective: Develop a simple host information stealer that extracts information.
- Skills Tested: Basic C2 connection.
- Materials: Understand simple information stealing.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 6: Advanced Stealer (Information Stealer 2, 20 points)

- Objective: Develop a simple host information stealer that extracts information from file.
`C:\\Users\user1\Desktop\password.txt`
- Skills Tested: File reading, accessing data, simple infostealer.
- Materials: Understand how you can extract info from file and send it to C2.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 7: Simple File Encryption (20 points, XOR)

- Objective: Implement an encryption/decryption routine (e.g., using XOR or a basic algorithm) that encrypts a file and decrypts it back to its original form.
- Skills Tested: Cryptography basics, file handling.
- Materials: Create a basic file encryption tool with a simple key.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 8: Persistence: Creating a Service (20 points)

- Objective: Create a Windows service that runs your malware on startup.
- Skills Tested: Service creation, Windows API.
- Materials: Use `CreateService` to create a Windows service that starts on boot.

### Task 9: Ransomware (Basic Version) (20 points)

- Objective: Create a simple ransomware that encrypts files in a directory and appends a `.bahrain` extension.
- Skills Tested: File manipulation, encryption, and ransom note creation.
- Materials: Modify the `encryptFile` function to create a ransom scenario.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 10: Combine Dropper + Reverse shell (20 points) 

- Objective: Combine a dropper that installs your reverse shell.
- Skills Tested: Combining techniques - dropper with revshell.
- Materials: The dropper from Task 4 and reverse shell from Task 1.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Bonus Task 11: Full Malware Sample with Encryption + Persistence + Evasion (50 points)

- Objective: Create a full malware sample that:
    - Encrypts files,
    - Ensures persistence,
    - Evades basic anti-virus detection (e.g., by using simple obfuscation and encrypt).
- Skills Tested: Full malware creation with encryption, persistence, and evasion techniques.
- Materials: Combine encryption, persistence, and evasion techniques from previous tasks.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### results

First Blood:     
First Bonus:     
Passing average scores:     
Average time:     
Final points:     

# Android

### Task 1: Write simple reverse shell (10 points)

- Objective: Write a basic program that create reverse shell with C/C++ API functions.
- Skills Tested: Understanding C in Android.
- Materials: Refer to the 09-android Rev project directory for basic examples.
- Bonus points: Run in device (20 points)

### Task 2: Basic Kotlin/Java reverse shell (10 points)

- Objective: Write a basic program that create reverse shell with Java/Kotlin API functions.
- Skills Tested: Understanding C in Android.
- Materials: Refer to the 09-android Rev2 project directory for basic examples.
- Bonus points: Run in device (20 points)

### Task 3: Basic infostealer (10 points)

- Objective: Write a basic program that steal systeminfo with Java/Kotlin API functions via Telegram.
- Skills Tested: Understanding Stealers logic in Android.
- Materials: Refer to the 09-android Hack project directory for basic examples.
- Bonus points: Run in device (20 points)     

### Task 4: Basic contacts stealer (10 points)

- Objective: Write a basic program that steal contacts with Java/Kotlin API functions via Telegram.
- Skills Tested: Understanding Stealers logic in Android.
- Materials: Refer to the 09-android HackContacts project directory for basic examples.
- Bonus points: Run in device (20 points)     

### Task 5: Basic contacts stealer - file based (10 points)

- Objective: Write a basic program that steal contacts with Java/Kotlin API functions via Telegram.
- Skills Tested: Understanding Stealers logic in Android.
- Materials: Refer to the 09-android HackContacts2 project directory for basic examples.
- Bonus points: Run in device (20 points)     

### Task 6: Basic SMS logs stealer - file based (10 points)

- Objective: Write a basic program that steal SMS with Java/Kotlin API functions via Telegram.
- Skills Tested: Understanding Stealers logic in Android.
- Materials: Refer to the 09-android HackSms project directory for basic examples.
- Bonus points: Run in device (20 points)

### Task 7: Basic SMS listener (10 points)

- Objective: Write a basic program that receive SMS with Java/Kotlin API functions and send to Telegram.
- Skills Tested: Understanding Stealers logic in Android.
- Materials: Refer to the 09-android HackSms project directory for basic examples.
- Bonus points: Run in device (20 points)

### Task 8: Basic Gallery stealer - Android 12 (10 points)

- Objective: Write a basic program that steal gallery with Java/Kotlin API functions via Telegram.
- Skills Tested: Understanding Stealers logic in Android.
- Materials: Refer to the Bahrain Train project for basic examples.
- Bonus points: Run in device (20 points)

### Task 9: Basic Gallery stealer - Android 13+ (10 points)

- Objective: Write a basic program that steal gallery with Java/Kotlin API functions via Telegram.
- Skills Tested: Understanding Stealers logic in Android.
- Materials: Refer to the Bahrain Train project for basic examples.
- Bonus points: Run in device (20 points)

### Task 10: Hack all stealer - Android 12/13+ (50 points)

- Objective: Write a basic program that steal all (contacts, sms info, calls info, sms receiver, gallery) with Java/Kotlin API functions via Telegram.
- Skills Tested: Understanding Stealers logic in Android.
- Materials: Refer to the Bahrain Train project for basic examples.
- Bonus points: Run in device (50 points)

### results

First Blood:     
First Bonus:     
Passing average scores:     
Average time:     
Final points:     

# iOS

### Task 1: Basic infostealer (10 points)

- Objective: Write a basic program that steal systeminfo with Swift API functions via Telegram.
- Skills Tested: Understanding Stealers logic in iOS.
- Materials: Refer to the 10-macos Hack project directory for basic examples.
- Bonus points: Run in device (20 points)     

### Task 2: Location stealer (10 points)

- Objective: Write a basic program that steal location info with Swift API functions via Telegram.
- Skills Tested: Understanding Stealers logic in iOS.
- Materials: Refer to the 10-macos Hack project directory for basic examples.
- Bonus points: Run in device (20 points)     

### Task 3: IP address stealer (10 points)

- Objective: Write a basic program that steal ip address info with Swift API functions via Telegram.
- Skills Tested: Understanding Stealers logic in iOS.
- Materials: Refer to the 10-macos Hack project directory for basic examples.
- Bonus points: Run in device (20 points)     

### Task 4: Connection/Wifi stealer (10 points)

- Objective: Write a basic program that steal connection/wifi info with Swift API functions via Telegram.
- Skills Tested: Understanding Stealers logic in iOS.
- Materials: Refer to the 10-macos Hack project directory for basic examples.
- Bonus points: Run in device (20 points)     

### Task 5: Contacts stealer (10 points)

- Objective: Write a basic program that steal all contacts info with Swift API functions via Telegram.    
- Skills Tested: Understanding Stealers logic in iOS.    
- Materials: Refer to the 10-macos Hack project directory for basic examples.    
- Bonus points: Run in device (20 points)     

### Task 6: Images Gallery stealer (10 points)

- Objective: Write a basic program that steal images info with Swift API functions via Telegram.     
- Skills Tested: Understanding Stealers logic in iOS.     
- Materials: Refer to the 10-macos Hack project directory for basic examples.     
- Bonus points: Run in device (20 points)     

### Task 7: Deploy iOS Video stealer on localhost (10 points)

- Objective: Write a basic program that steal all info with Swift API/iOS functions via Telegram.     
- Skills Tested: Understanding Stealers logic in iOS devices.     
- Materials: Refer to the 10-macos Hack project directory for basic examples.     
- Bonus points: Run in iOS device (20 points)     

### Task 8: Deploy MacOS stealer on localhost (10 points)

- Objective: Write a basic program that steal info with MacOS API functions via Uploadcare.     
- Skills Tested: Understanding Stealers logic in MacOS X.     
- Materials: Refer to the MacStealer project directory for basic examples.     
- Bonus points: Run in mac device (20 points)     

### Task 9: Deploy iOS full featured stealer on localhost (50 points)

- Objective: Write a basic program that steal all info with Swift API/iOS functions via Telegram.     
- Skills Tested: Understanding Stealers logic in iOS devices.     
- Materials: Refer to the 10-macos Hack project directory for basic examples.     
- Bonus points: Run in iOS device (20 points)     

### Task 10: Run iOS AirPlay exploits (20 points)

- Objective: Write a script for sending fake AirPlay device.     
- Skills Tested: Understanding Airplay vulnerability logic in iOS devices.     
- Materials: Refer to the 10-macos Hack project directory for basic examples.     
- Bonus points: Run in mac device (20 points)     

### Bonus Task 11: Deploy Mac OSX reverse shell vulnerability via Package Manager (50 points)

- Objective: Write a basic program that exploitation Package installer vuln.     
- Skills Tested: Understanding 0-day logic in MacOSX devices.     
- Materials: Refer to the check/postinstall scripts for basic examples.     
- Bonus points: Run in MacOS device (20 points)     


### results

First Blood:     
First Bonus:     
Passing average scores:     
Average time:     
Final points:     

# IoT

### Task 1: Firmware OSINT (10 points)

- Objective: Find all harware/software info about victim's device.
- Skills Tested: Understanding OSINT logic for IoT hacking.
- Materials: Refer to the firmware dumping PDFs for basic examples.      
- Bonus points: Find the firmware file (20 points)     

### Task 2: Firmware dumping (10 points)

- Objective: Download firmware and dump and extract.     
- Skills Tested: Understanding firmware dumping logic for IoT hacking.
- Materials: Refer to the firmware dumping PDFs for basic examples.      
- Bonus points: Dump for different 2 devices (20 points)     

### Task 3: Advanced: GHidra finding classic BOF (20 points)

- Objective: From extracted firmware find a Buffer Overflow vulnerability.     
- Skills Tested: Understanding finding 0-day for IoT hacking.
- Materials: Refer to the firmware dumping PDFs and firmware file for basic examples.      
- Bonus points: Find BOF in `./bin/auth` file (20 points)     

### Task 4: Advanced: Run CVE exploitation (20 points)

- Objective: From extracted firmware find a Buffer Overflow vulnerability.     
- Skills Tested: Understanding running CVE for IoT hacking.
- Materials: Refer to the firmware dumping PDFs and firmware file for basic examples.      
- Bonus points: Find BOF in `./bin/auth` file (20 points)     

### Task 5: Basic WIFI hacking: online password bruteforce (10 points)

- Objective: Brute password via online requests.
- Skills Tested: Understanding WIFI hacking without dump handshake for IoT hacking.
- Materials: Refer to the `wifi/wpa_brute.sh` for basic examples.      
- Bonus points: Hack Batelco Wifi Access Point 2.4G (20 points)     

### Task 6: Basic WIFI hacking: online password bruteforce with threads(10 points)

- Objective: Brute password via online requests and threads.
- Skills Tested: Understanding WIFI hacking without dump handshake for IoT hacking.
- Materials: Refer to the `wifi/wpa_brute-width.sh` for basic examples.      
- Bonus points: Hack Batelco Wifi Access Point 2.4G (20 points)     

### Task 7: Advanced WIFI hacking: online password bruteforce with Python (10 points)

- Objective: Brute password via online M1/M2/M3 requests and Python Scapy.
- Skills Tested: Understanding WIFI hacking without dump handshake for IoT hacking.
- Materials: Refer to the `auth.py` for basic examples.      
- Bonus points: Hack Batelco Wifi Access Point 2.4G (20 points)     

### Task 8: Advanced WIFI hacking: run fake beacon attack (10 points)

- Objective: Run fake beacon attack with Scapy.
- Skills Tested: Understanding WIFI hacking without dump handshake for IoT hacking.
- Materials: Refer to the `auth.py` for basic examples.      
- Bonus points: Hack Batelco Wifi Access Point 2.4G (20 points)     

### Task 9: Advanced WIFI hacking: run WPA PSK attacks (10 points)

- Objective: Hack Wifi via WPA PSK attack.
- Skills Tested: Understanding WIFI hacking without dump handshake for IoT hacking.
- Materials: Refer to the `brute.py` for basic examples.      
- Bonus points: Hack Batelco Wifi Access Point 2.4G (20 points)     

### Task 10: Bonus WIFI hacking: run all together (50 points)

- Objective: Full automation of WIFI online hacking
- Skills Tested: Understanding WIFI hacking without dump handshake for IoT hacking.
- Materials: Refer to the `wifi.zip` for basic examples.      
- Bonus points: Hack Batelco Wifi Access Point 2.4G (20 points)     


### results

First Blood:     
First Bonus:     
Passing average scores:     
Average time:     
Final points:     

