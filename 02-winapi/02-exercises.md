# 02 exercises

### exercise 1

In the code of reverse shell, we use `cmd.exe` to provide the shell. Modern Windows environments often use PowerShell. Locate the `CreateProcess` function and change `cmd.exe` to `powershell.exe`. Compile and run.     


### exercise 2

In the source code of reverse shell, currently, when the shell starts, it might show a window. Research the `dwFlags` in the `STARTUPINFO` structure. Try adding the `STARTF_USESHOWWINDOW` flag and setting wShowWindow to `SW_HIDE`.     
