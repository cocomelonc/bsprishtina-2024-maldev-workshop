﷽

### Task 1: Write simple reverse shell (10 points) ++++++ 39 min

- Objective: Write a basic program that create reverse shell with Windows API functions.
- Skills Tested: Understanding WINAPI.
- Materials: Refer to the 03-winapi directory for basic examples.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 2: Basic DLL Injection (10 points) ++++++ 2h 34min

- Objective: Write a basic program that injects a DLL into a running process using Windows API functions.
- Skills Tested: DLL injection, `CreateRemoteThread`, `VirtualAllocEx`.
- Materials: Refer to the 03-injection directory for basic examples.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 3: Basic Code Injection (10 points) ++++++++ 1h 39min

- Objective: Implement payload injection.
- Skills Tested: Code injection methods.
- Materials: Refer to the 03-injection directory for basic examples.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 4: Dropper Creation (10 points) ++++++ 1h 09min

- Objective: Write a dropper that drops a payload (e.g., a DLL or executable) on a victim's system and executes it.
- Skills Tested: File handling, process creation, and execution.
- Materials: Create an executable that downloads or copies a payload and runs it.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 5: Basic Stealer (Information Stealer, 10 points) ++++++++ second blood 30 min

- Objective: Develop a simple host information stealer that extracts information.
- Skills Tested: Basic C2 connection.
- Materials: Understand simple information stealing.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 6: Advanced Stealer (Information Stealer 2, 20 points) ++++++

- Objective: Develop a simple host information stealer that extracts information from file.
`C:\\Users\user1\Desktop\password.txt`
- Skills Tested: File reading, accessing data, simple infostealer.
- Materials: Understand how you can extract info from file and send it to C2.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 7: Simple File Encryption (20 points, XOR) +++++ 2h

- Objective: Implement an encryption/decryption routine (e.g., using XOR or a basic algorithm) that encrypts a file and decrypts it back to its original form.
- Skills Tested: Cryptography basics, file handling.
- Materials: Create a basic file encryption tool with a simple key.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 8: Persistence: Creating a Service (20 points)

- Objective: Create a Windows service that runs your malware on startup.
- Skills Tested: Service creation, Windows API.
- Materials: Use `CreateService` to create a Windows service that starts on boot.

### Task 9: Ransomware (Basic Version) (20 points) +++++ 41 min

- Objective: Create a simple ransomware that encrypts files in a directory and appends a `.bahrain` extension.
- Skills Tested: File manipulation, encryption, and ransom note creation.
- Materials: Modify the `encryptFile` function to create a ransom scenario.
- Bonus points: Add persistence (10 points, service: 50 points), macro (20 points)

### Task 10: Combine Dropper + Reverse shell (20 points) +++++ first blood 28 min
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

First Blood: 28 min
First Bonus: 1h 22min
Passing average scores: 60
Average time: 20 min
Pass exam: .......... (200 points, 1 macro, 1 persistence for pass)
Final points: 170 pts, 3h 15m 370 350-400