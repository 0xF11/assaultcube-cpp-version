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
	DWORD gameBaseAddress = GetModuleBaseAddress(_T(gamename), pID); // gamename module base address, and appid
	DWORD BaseGAddress = 0x10F4F4; // Entity offset list
	std::vector<DWORD> CWAOffset{ 0x374,0x14,0x0 };
	std::vector<DWORD> HPOffset{ 0xF8 };
	std::vector<DWORD> KillsOffset{ 0x1FC };
	std::vector<DWORD> DeathsOffset{ 0x204 };
	std::vector<DWORD> GrenadeOffset{ 0x158 };
	DWORD basead = NULL;
	ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + BaseGAddress), &basead, sizeof(basead), NULL);
	DWORD CWAAddress = basead;
	for (int loopi = 0; loopi < CWAOffset.size() - 1; loopi++) // loops through vector containing our offsets for current weapon (cwaoffset)
	{
		ReadProcessMemory(processHandle, (LPVOID)(CWAAddress + CWAOffset.at(loopi)), &CWAAddress, sizeof(CWAAddress), NULL);
	}

	DWORD HPAddress = basead;
	for (int healthloop{ 0 }; healthloop < HPOffset.size() - 1; healthloop++)
	{
		ReadProcessMemory(processHandle, (LPVOID)(HPAddress + HPOffset.at(healthloop)), &HPAddress, sizeof(HPAddress), NULL);
	}
	DWORD Killsaddress = basead;
	for (int maxkillsloop{ 0 }; maxkillsloop < KillsOffset.size() - 1; maxkillsloop++)
	{
		ReadProcessMemory(processHandle, (LPVOID)(Killsaddress + KillsOffset.at(maxkillsloop)), &Killsaddress, sizeof(Killsaddress), NULL);
	}
	DWORD deathsaddress = basead;
	for (int deathloop{ 0 }; deathloop < DeathsOffset.size() - 1; deathloop++)
	{
		ReadProcessMemory(processHandle, (LPVOID)(deathsaddress + DeathsOffset.at(deathloop)), &deathsaddress, sizeof(deathsaddress), NULL);
	}
	DWORD grenaded = basead; // grenade offset blah blah blah
	for (int grenadeloop{ 0 }; grenadeloop < GrenadeOffset.size() - 1; grenadeloop++)
	{
		ReadProcessMemory(processHandle, (LPVOID)(grenaded + GrenadeOffset.at(grenadeloop)), &grenaded, sizeof(grenaded), NULL);
	}

	HPAddress += HPOffset.at(HPOffset.size() - 1); // adds last offset brah
	CWAAddress += CWAOffset.at(CWAOffset.size() - 1); // add last offset 
	Killsaddress += KillsOffset.at(KillsOffset.size() - 1);
	deathsaddress += DeathsOffset.at(DeathsOffset.size() - 1);
	grenaded += GrenadeOffset.at(GrenadeOffset.size() - 1);

	std::cout << "debugginfo: baseaddress = " << std::hex << basead << std::endl;
	std::cout << "debugging: death value at offset = " << std::hex << deathsaddress << std::endl;
	std::cout << "debugging: kills at offset = " << std::hex << Killsaddress << std::endl;
	std::cout << "debugging: grenades at offset = " << std::hex << grenaded << std::endl;
	std::cout << "debugging: current ammo at offset = " << std::hex << CWAAddress << std::endl;
	int NAmmo = 9'999; // new ammo
	int lotsofhealth = 999'999'999; // new health
	int killingmachine = 999'999'999; // new kills on scoreboard
	int NeverDied = -999'999'999; // negative deaths on scoreboard
	int grenades = 999'999'999; // grenade since player cant hold grenade until they grab one
	WriteProcessMemory(processHandle, (LPVOID)(CWAAddress), &NAmmo, 4, 0); // writes new value to current value 
	WriteProcessMemory(processHandle, (LPVOID)(HPAddress), &lotsofhealth, 4, 0); // writes to health
	WriteProcessMemory(processHandle, (LPVOID)(Killsaddress), &killingmachine, 4, 0);
	WriteProcessMemory(processHandle, (LPVOID)(deathsaddress), &NeverDied, 4, 0);
	WriteProcessMemory(processHandle, (LPVOID)(grenaded), &grenades, 4, 0);
	while (true) // every 100 miliseconds repeats the code - basically infinite health & ammo
	{
		Sleep(100);
		goto loopcheat;


	}

}
