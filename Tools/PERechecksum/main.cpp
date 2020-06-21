#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>

/**
PEChecksum <PE file>
*/

int main(int argc, char * argv[])
{
	const char * peFile = nullptr;
	const char * outFile = nullptr;

	if (argc != 2)
	{
		printf("use: %s <PE file>", argv[0]);
	}
	else
	{
		peFile = argv[1];
		outFile = argv[2];

		DWORD currChecksum, realChecksum;
		if (MapFileAndCheckSumA(peFile, &currChecksum, &realChecksum) == CHECKSUM_SUCCESS)
		{
			if (currChecksum == realChecksum)
			{
				printf("Current checksum is valid\n");
			}
			else
			{
				printf("Current checksum: 0x%08p, real checksum: 0x%08p\n", currChecksum, realChecksum);

				HANDLE hFile = CreateFileA(peFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					IMAGE_DOS_HEADER dos;
					DWORD rb;
					ReadFile(hFile, &dos, sizeof(dos), &rb, NULL);

					IMAGE_NT_HEADERS ntHead;
					DWORD offseg = (char*)&ntHead.OptionalHeader.CheckSum - (char*)&ntHead;
					SetFilePointer(hFile, dos.e_lfanew + offseg, 0, FILE_BEGIN);

					WriteFile(hFile, &realChecksum, 4, &rb, NULL);

					CloseHandle(hFile);

					printf("update checksum complete\n");
				}
			}
		}
	}

	return 0;
}