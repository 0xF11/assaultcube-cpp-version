#include <Windows.h>
#include<TlHelp32.h>
#include <iostream>
#include <tchar.h> // _tcscmp
#include <vector>
#include <cmath>
DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID)
{
	DWORD dwModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(THAI_CHARSET, pID);// make snapshot of all modules within process
	MODULEENTRY32 mEntry32 = { 0 }; // equals initialization?
	mEntry32.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(hSnapshot, &mEntry32))//store first Module in ModuleEntry32
		{
			do {
				if (_tcscmp(mEntry32.szModule, lpszModuleName) == 0) // if module we're looking for is found -> done
				{
					dwModuleBaseAddress = (DWORD)mEntry32.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnapshot, &mEntry32)); // goes through snapshot, stores them in moduleentry32
		}
		CloseHandle(hSnapshot);
		return dwModuleBaseAddress;
}




int main()
{

	int loopcheat;
	loopcheat:
	HWND hGameWindow = FindWindow(NULL, "AssaultCube");
	if (hGameWindow == NULL)
	{
		std::cout << "Open the game!";
		return 0;
	}

	DWORD pID = NULL; // ApID of the game (processID)
	GetWindowThreadProcessId(hGameWindow, &pID);

	HANDLE processHandle = NULL; // handle to the process
	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID); // give administrator perms to the processes with this process
	if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL) // checks if process fails to open (error handler)
	{
		std::cout << "Process failed to open, Sorry!" << std::endl;
	}

	char gamename[] = "ac_client.exe";
	DWORD gameBaseAddress = GetModuleBaseAddress(_T(gamename),pID); // gamename module base address, and appid
	DWORD BaseGAddress = 0x10F4F4; // Entity offset list
	std::vector<DWORD> CWAOffset{ 0x374,0x14,0x0 }; 
	std::vector<DWORD> HPOffset{ 0xF8 };
	DWORD basead = NULL;
	ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress+ BaseGAddress), &basead, sizeof(basead), NULL);
	std::cout << "debugginfo: baseaddress = " << std::hex << basead << std::endl;
	DWORD CWAAddress = basead;
	for (int loopi = 0; loopi < CWAOffset.size() - 1; loopi++) // loops through vector containing our offsets for current weapon (cwaoffset)
	{
		ReadProcessMemory(processHandle, (LPVOID)(CWAAddress + CWAOffset.at(loopi)), &CWAAddress, sizeof(CWAAddress), NULL);
		std::cout << "debugging: value at offset = " << std::hex << CWAAddress << std::endl;
	}
	CWAAddress += CWAOffset.at(CWAOffset.size() - 1); // add last offset then we're done
	
	DWORD HPAddress = basead;
	for (int healthloop{ 0 }; healthloop < HPOffset.size() - 1; healthloop++)
	{
		ReadProcessMemory(processHandle, (LPVOID)(HPAddress + HPOffset.at(healthloop)), &HPAddress, sizeof(HPAddress), NULL);
	}
	HPAddress += HPOffset.at(HPOffset.size() - 1); // adds last offset brah


	int NAmmo = 999999; // new ammo
	WriteProcessMemory(processHandle, (LPVOID)(CWAAddress), &NAmmo, 4, 0); // writes new value to current value 
	
	

	int lotsofhealth = 999999; // max of unsigned int
	WriteProcessMemory(processHandle, (LPVOID)(HPAddress), &lotsofhealth, 4, 0);

	while (true) // every 100 miliseconds repeats the code - basically infinite health & ammo
	{
		Sleep(100);
		goto loopcheat;
	}



}
