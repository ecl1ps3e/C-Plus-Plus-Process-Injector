# Windows Process Injection Engine (C++)

## 🚩 Overview
This repository contains a raw C++ implementation of a **Process Injection** attack vector. It demonstrates how malware evades detection by injecting arbitrary shellcode into the memory space of legitimate running processes (e.g., `Notepad.exe`).

**⚠️ Educational Purpose Only:** This tool is designed to simulate advanced persistent threat (APT) techniques for Red Team engagements and EDR testing.

## ⚙️ Technical Mechanisms
The injector utilizes the Windows API to perform the following operations:
1.  **Process Enumeration:** Locates the target process ID (PID) using `CreateToolhelp32Snapshot`.
2.  **Handle Acquisition:** Opens the process with `PROCESS_ALL_ACCESS` rights.
3.  **Memory Allocation:** Uses `VirtualAllocEx` to reserve a readable/writable/executable (RWX) memory page in the remote process.
4.  **Payload Delivery:** Writes raw shellcode (x64 MessageBox payload) via `WriteProcessMemory`.
5.  **Execution:** Triggers the payload using `CreateRemoteThread`.

## 🛠️ Tech Stack
* **Language:** C++ (Standard 20)
* **Libraries:** Windows API (`windows.h`, `tlhelp32.h`)
* **Target Architecture:** x64 Windows

## 🚀 Usage
1.  Open `Notepad.exe` (Target).
2.  Run `ProcessInjector.exe` as **Administrator**.
3.  Observe the injected message box appearing from the Notepad process.

## 🛡️ Detection Indicators (Blue Team)
* **Sysmon Event ID 8:** CreateRemoteThread detected.
* **Memory anomalies:** Executable memory pages (RWX) not backed by a disk file.
