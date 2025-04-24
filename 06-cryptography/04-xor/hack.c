/*
 * hack.c
 * encrypt/decrypt file with XOR
 * author: @cocomelonc
*/
#include <windows.h>
#include <stdio.h>

#define KEY_SIZE 16
#define MAX_PATH_LENGTH 260

// XOR encryption / decryption function
void deXOR(char *buffer, size_t bufferLength, char *key, size_t keyLength) {
  int keyIndex = 0;
  for (int i = 0; i < bufferLength; i++) {
    if (keyIndex == keyLength - 1) keyIndex = 0;
    buffer[i] = buffer[i] ^ key[keyIndex];
    keyIndex++;
  }
}

// Add padding to file if the size isn't a multiple of the block size
void addPadding(HANDLE fh) {
  LARGE_INTEGER fs;
  GetFileSizeEx(fh, &fs);

  size_t paddingS = 16 - (fs.QuadPart % 16);
  if (paddingS != 16) {
    SetFilePointer(fh, 0, NULL, FILE_END);
    for (size_t i = 0; i < paddingS; ++i) {
      char paddingB = static_cast<char>(paddingS);
      WriteFile(fh, &paddingB, 1, NULL, NULL);
    }
  }
}

// Remove padding from file
void removePadding(HANDLE fileHandle) {
  LARGE_INTEGER fileSize;
  GetFileSizeEx(fileHandle, &fileSize);

  DWORD paddingSize;
  SetFilePointer(fileHandle, -1, NULL, FILE_END);
  ReadFile(fileHandle, &paddingSize, 1, NULL, NULL);

  if (paddingSize <= 16 && paddingSize > 0) {
    size_t originalSize = fileSize.LowPart - paddingSize;
    SetEndOfFile(fileHandle);
  } else {
    printf("Invalid padding size: %d\n", paddingSize);
  }
}

// Encrypt file using XOR
void encryptFile(const char* inputFile, const char* outputFile, const char* xorKey) {
  HANDLE ifh = CreateFileA(inputFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  HANDLE ofh = CreateFileA(outputFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (ifh == INVALID_HANDLE_VALUE && ofh == INVALID_HANDLE_VALUE) {
    printf("Error opening file.\n");
    return;
  }

  LARGE_INTEGER fileSize;
  GetFileSizeEx(ifh, &fileSize);

  unsigned char* fileData = (unsigned char*)malloc(fileSize.LowPart);
  DWORD bytesRead;
  ReadFile(ifh, fileData, fileSize.LowPart, &bytesRead, NULL);

  unsigned char key[KEY_SIZE];
  memcpy(key, xorKey, KEY_SIZE);

  // Encrypt the file data
  deXOR((char*)fileData, fileSize.LowPart, (char*)key, KEY_SIZE);

  // Write the encrypted data to the output file
  DWORD bw;
  WriteFile(ofh, fileData, fileSize.LowPart, &bw, NULL);

  printf("XOR encryption successful\n");

  CloseHandle(ifh);
  CloseHandle(ofh);
  free(fileData);
}

// Decrypt file using XOR
void decryptFile(const char* inputFile, const char* outputFile, const char* xorKey) {
  HANDLE ifh = CreateFileA(inputFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  HANDLE ofh = CreateFileA(outputFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (ifh == INVALID_HANDLE_VALUE  && ofh == INVALID_HANDLE_VALUE) {
    printf("Error opening file.\n");
    return;
  }

  LARGE_INTEGER fileSize;
  GetFileSizeEx(ifh, &fileSize);

  unsigned char* fileData = (unsigned char*)malloc(fileSize.LowPart);
  DWORD bytesRead;
  ReadFile(ifh, fileData, fileSize.LowPart, &bytesRead, NULL);

  unsigned char key[KEY_SIZE];
  memcpy(key, xorKey, KEY_SIZE);

  // Decrypt the file data using XOR (XOR is symmetric, same function for encryption and decryption)
  deXOR((char*)fileData, fileSize.LowPart, (char*)key, KEY_SIZE);

  // Write the decrypted data to the output file
  DWORD bw;
  WriteFile(ofh, fileData, fileSize.LowPart, &bw, NULL);

  printf("XOR decryption successful\n");

  CloseHandle(ifh);
  CloseHandle(ofh);
  free(fileData);
}

int main() {
  const char* inputFile = "test.txt";
  const char* outputFile = "test.txt.xor";
  const char* decryptedFile = "test.txt.xor.decrypted";
  const char* xorKey = "hackersfrombahrain";  // XOR key can be any string

  // Encrypt the file using XOR
  encryptFile(inputFile, outputFile, xorKey);

  // Decrypt the file using XOR
  decryptFile(outputFile, decryptedFile, xorKey);

  return 0;
}