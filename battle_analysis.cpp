#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <time.h>

// Make sure this Window Name is exactly as your MapleStory window name.
const char* kWindowName = "SpiritMS";

const ULONG kEntryPoint = 0x00400000;

const ULONG kMaplePointBase = 0x1EC41B8;
const ULONG kMaplePointOffset = 0x67FC;

ULONG ReadPointers(HANDLE &handle, ULONG base_address, std::vector<ULONG> offsets){
  ULONG pointer;
  ReadProcessMemory(handle, (PBYTE*)(base_address), &pointer, sizeof(pointer), 0);
  for(auto offset: offsets) {
    ReadProcessMemory(handle, (PBYTE*)(pointer + offset), &pointer, sizeof(pointer), 0);
  }
  return pointer;
}

int main() {

  std::cout << "Starting Battle Analysis...\n";

  HWND hwnd = FindWindowA(NULL, kWindowName);
  if (hwnd == NULL) {
    std::cout << "Could not find Window:" << kWindowName << std::endl;
    Sleep(3000);
    exit(-1);
  }

  DWORD process_id = 0;
  GetWindowThreadProcessId(hwnd, &process_id);
  HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
  // Open with ALL ACCESS to be able to read and write memory (basically run as admin)

  if(handle == 0){
    std::cout << "Make sure this program is ran as administrator.\n";
  } else if(process_id == 0) {
    std::cout << "Could not obtain the process id.\n";
    Sleep(3000);
    exit(-1);
  }
  clock_t start_time = clock();

  std::vector<ULONG> offsets{kMaplePointOffset};
  auto maple_points = ReadPointers(handle, kEntryPoint + kMaplePointBase, offsets);
  std::cout << "\nStarting Stats:\n";
  std::cout << "Maple Points: " << maple_points << std::endl;

  std::string confirm;
  std::cout << "\nType in anything and press enter to end Battle Analysis.";
  std::cin >> confirm;

  clock_t end_time = clock();

  auto end_maple_points = ReadPointers(handle, kEntryPoint + kMaplePointBase, offsets);
  auto seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  auto minutes = seconds / 60.0;

  std::cout << "You've gained a total of " << end_maple_points - maple_points << " maple points in " << minutes << " minutes." << std::endl;
  return 0;
}
