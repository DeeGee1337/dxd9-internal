#include "globals.h"

void ManualMapper::load_dll(const char* dllname) {
	meDllpath = dllname;
	if (!GetFileAttributesA(meDllpath))
		meError("File doesn't exist");

	std::ifstream sFile(dllname, std::ios::binary | std::ios::ate);

	if (sFile.fail())
		meError("Couldn't open filestream");

	meDllsize = sFile.tellg();

	if (!meDllsize)
		meError("File too short");

	meDllCopy = malloc(meDllsize);
	if (!meDllCopy)
		meError("Error allocating memory in local process");
	meSuccess("Allocated memory for dll in local process at: 0x%p", meDllCopy);

	sFile.seekg(0, std::ios::beg);
	sFile.read((char*)meDllCopy, meDllsize);
	sFile.close();
	meSuccess("Read file! Dump {%x, %x}", *(uint8_t*)meDllCopy, *(uint8_t*)((uintptr_t)meDllCopy + 1));
}