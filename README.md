# taskmgr_hooking

Dump LSASS process in Task Manager without triggering Defender.

Hooks API calls
```
RtlInitUnicodeString
RtlInitUnicodeStringEx
RtlDosPathNameToRelativeNtPathName_U
RtlDosPathNameToRelativeNtPathName_U_WithStatus
SetDlgItemTextW
```
And changes the file path from `C:\Users\<username>\AppData\Local\Temp\lsass.DMP` to what ever you specify in the `newStr` variable. 
<br>
<br>
Update `newStr` with the new path you want, and update the username in the `matchStr` variable.
<br>
<br>
Compiles to a DLL that you can [inject](https://github.com/djackreuter/dllinject) into Taskmgr.exe

![image](https://user-images.githubusercontent.com/27731554/229834820-103269a6-027e-4cc6-9113-c2c24622368e.png)
