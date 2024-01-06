

#include "stdio.h"
#include "stdlib.h"
#include "windows.h"


void ShowErrorProcess(const char* notify, DWORD errInp)
{
    DWORD err = errInp;
    LPVOID errBuf = NULL;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        err,
        0,
        (LPSTR)&errBuf,
        0,
        NULL);

    fprintf(stderr, "%s\n", notify);
    fprintf(stderr, "=====ERROR=====\n%u: %s", err, (LPSTR)errBuf);
    LocalFree(errBuf);

    exit(1);
}

void ShowError(const char* notify)
{
    fprintf(stderr, "%s\n", notify);
    exit(1);
}



int main(int argc, char** argv) {
    if (argc != 2)
    {
        ShowError("Wrong count of arguments (Must be 1)\n");
        exit(1);
    }

    int arg = atoi(argv[1]);

    if (arg <= 0)
    {
        ShowError("Argument is not an integer number (Must be an integer)\n");
        exit(2);
    }

    if (arg > 13 || arg < 1)
    {
        ShowError("Argument is out of scope (Must be between 1 and 13)\n");
        exit(3);
    }

    if (arg == 1 || arg == 2)
    {
        return 1;
    }

    STARTUPINFO start_info[2];
    PROCESS_INFORMATION process_info[2];

    for (int i = 1; i <= 2; i++)
    {
        char newArgVector[256];
        int newArgValue = (arg - i);
        sprintf(newArgVector, "%s %d", argv[0], newArgValue);

        ZeroMemory(&start_info[i - 1], sizeof(STARTUPINFO));
        start_info[i - 1].cb = sizeof(STARTUPINFO);

        if (!CreateProcessA(
            NULL,
            newArgVector,
            NULL,
            NULL,
            0,
            0,
            NULL,
            NULL,
            (LPSTARTUPINFOA)&start_info[i - 1],
            &process_info[i - 1]))
        {
            ShowErrorProcess("Programm don't create new process!!!", GetLastError());
        }
    }


    WaitForSingleObject(process_info[0].hProcess, INFINITE);
    WaitForSingleObject(process_info[1].hProcess, INFINITE);

    DWORD firstExitInfo, secondExitInfo;
    GetExitCodeProcess(process_info[0].hProcess, &firstExitInfo);
    GetExitCodeProcess(process_info[1].hProcess, &secondExitInfo);

    printf("My:\t%lu\tFirst:\t%lu\tResult:\t%d\tReturn Code:\t%lu\n", GetCurrentProcessId(), process_info[0].dwProcessId, arg - 1, firstExitInfo);
    printf("My:\t%lu\tSecond:\t%lu\tResult:\t%d\tReturn Code:\t%lu\n", GetCurrentProcessId(), process_info[1].dwProcessId, arg - 2, secondExitInfo);
    printf("My:\t%lu\t\t\t\t\tReturn Code:\t%lu\n\n", GetCurrentProcessId(), firstExitInfo + secondExitInfo);


    CloseHandle(process_info[0].hProcess);
    CloseHandle(process_info[0].hThread);

    CloseHandle(process_info[1].hProcess);
    CloseHandle(process_info[1].hThread);

    return firstExitInfo + secondExitInfo;
}
