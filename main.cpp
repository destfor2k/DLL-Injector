// First we need to connect the libraries we need:
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <io.h>

// Create the main function and write the following there:
int main()
{
    char process_name[32];
    char dll_name[32];
    char path[256];
    printf("enter process name: ");
    scanf_s("%s", process_name);
    DWORD pID = GetProcessByName(process_name);
    printf("Waiting %s for start...\n", process_name);
    for (;; Sleep(50))
    {
        if (pID == 0)
            pID = GetProcessByName(process_name);
        if (pID != 0) break;
    }
    printf("%s found (pid = %X)!\n", process_name, pID);
    while (FileExist(path) == false)
    {
        printf("Enter DLL name: ");
        scanf_s("%s", dll_name);
        GetFullPathName(dll_name, sizeof(path), path, NULL);
        if (FileExist(path))
        {
            printf("DLL found!\n");
            break;
        }
        else
            printf("DLL not found!\n");
    }
    printf("Preparing DLL for injection...\n");
    if (Inject(pID, path))
    {
        printf("DLL successfully injected!\n");
        system("pause");
    }
    else
    {
        printf("CRITICAL ERROR! \nDestroying window...\n");
        Sleep(500);
    }
}

// Creating a function that will receive the process id. To do this, we will use DWORD:
DWORD GetProcessByName(char* process_name)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    DWORD proc_id = 0;
    if (Process32First(snapshot, &process))
    {
        while (Process32Next(snapshot, &process))
        {
            if (_stricmp(process.szExeFile, process_name) == 0)
            {
                proc_id = process.th32ProcessID;
                break;
            }
        }
    }
    CloseHandle(snapshot);
    return proc_id;
}

// Let's describe the function of checking the existence of a file:
bool FileExist(char* name)
{
    return _access(name, 0) != -1;
}

// Creating the bool method. It will take the id of the process that we will inject and the path to the dll itself:
bool Inject(DWORD pID, char*path)
{
    HANDLE proc_handle;
    LPVOID RemoteString;
    LPCVOID LoadLibAddy;
    if (pID == 0)
        return false;
    proc_handle = OpenProcess(PROCESS_ALL_ACCESS, false, pID);
    if (proc_handle == 0)
        return false;
    LoadLibAddy = GetProcAddress(GetModuleHandle("kernel32.dll")), "LoadLibraryA";
    RemoteString = VirtualAllocEx(proc_handle, NULL, strlen(path), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(proc_handle, RemoteString, path, strlen(path), NULL);
    CreateRemoteThread(proc_handle, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, RemoteString, NULL, NULL);
    CloseHandle(proc_handle);
    return true;
}

// end
