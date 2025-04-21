#include <windows.h>

void hackMe() {
  MessageBox(NULL, "Hello, Bahrain!", "=^..^=", MB_OK);
}

// Excel will call this on Add-in load
extern "C" __declspec(dllexport) int WINAPI xlAutoOpen(void) {
    hackMe();  // Trigger on Excel load
    return 1;            // Return success to Excel
}