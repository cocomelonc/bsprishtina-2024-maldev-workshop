# 05 exercises

### exercise 1

In `01-classic-registry-run-keys/pers.c`, the persistence installer calls `RegOpenKeyEx`. If the `Run` key doesn't exist yet (unusual, but possible on stripped systems), the call fails silently and the malware value is never written.

Replace `RegOpenKeyEx` with `RegCreateKeyEx` so the key is **created** if it doesn't exist, or opened if it does. Add basic error checking: if the call fails, print the error code with `GetLastError()` and return a non-zero exit code.

### exercise 2

Extend the installer from exercise 1 to support a **delete / cleanup mode**. Accept a command-line argument:

- `hack2.exe set` - installs the persistence value (same as exercise 1)
- `hack2.exe del` - removes the value using `RegDeleteValue`

If no argument or an unknown argument is passed, print a usage message and exit.

Key new API: `RegDeleteValue(hkey, valueName)` - removes a named value from an open key.

### exercise 3

In `03-screensaver-hijacking/pers.c`, the screensaver timeout is hardcoded to `"10"` seconds. Make it configurable:

1. Accept the timeout (in seconds) as `argv[1]` and the payload path as `argv[2]`. Default to `"60"` and `"Z:\\hack.exe"` if no arguments are given.
2. Also set the `ScreenSaverIsSecure` value to `"1"` - this makes Windows demand the unlock password when the screensaver exits, forcing the user to interact with the lock screen (and re-trigger the screensaver on next idle).

### exercise 4

In `07-accessibility-features/pers.c`, only `sethc.exe` (Sticky Keys) is hijacked. Defenders often monitor this single target. Register the IFEO `Debugger` key for **both** `sethc.exe` and `utilman.exe` (Utility Manager, also reachable from the login screen via Win+U) in a single run.

Refactor the code so the hijack logic lives in a helper function:

```c
static int set_ifeo_debugger(const char *target_exe, const char *debugger_path);
```

Call it once for each target. Print a status line for each registration attempt.

### exercise 5

`04-appinit-dlls/hack.cpp` is a DLL written in C++ (`extern "C"`, compiled with `g++`). Port it to **pure C**:

1. Remove the `extern "C"` wrapper - in C, functions are already exported with C linkage by default; `__declspec(dllexport)` is sufficient.
2. Replace the `#pragma comment(lib, ...)` with the equivalent linker flag in the compile command.
3. Add the same process-name guard as `hack2.cpp`: only show the MessageBox when the loading process contains `"notepad"` in its path. Use `GetModuleFileNameA` and `strstr`.

Compile as a DLL:
```
x86_64-w64-mingw32-gcc hack5.c -o hack5.dll -shared -luser32
```

### exercise 6

`10-file-extension/pers.cpp` has two issues that need fixing when porting to **pure C**:

1. The registry path passed to `RegOpenKeyEx` starts with `\\` - `"\\txtfile\\shell\\open\\command"` - which is wrong; HKCR paths must not start with a separator. Fix it to `"txtfile\\shell\\open\\command"`.
2. `RegOpenKeyEx` will fail if the key doesn't already exist. Replace it with `RegCreateKeyEx`.
3. Accept the **file extension** (e.g. `txt`, `py`, `docx`) and the **payload path** as command-line arguments so the tool can hijack any registered extension, not just `.txt`.

Usage:
```
hack6.exe txt C:\Windows\System32\calc.exe
hack6.exe py  C:\Windows\System32\calc.exe
```
