#undef UNICODE 


#include <windows.h>
#include <stdio.h>


//sc.exe create MemoryStatus binpath=C:\\Users\\TUAN\\source\\repos\\
ConsoleApplication2212\\ConsoleApplication2212\\Debug\\memstatus.log
#define SLEEP_TIME 5000
#define LOGFILE "C:\\Users\\TUAN\\source\\repos\\ConsoleApplication2212\\ConsoleApplication2212\\Debug\\memstatus.log"
#pragma warning (disable:4996)
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;

void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);
int InitService();
int WriteToLog(char* str);
int WriteToPipes(HANDLE hPipe, char* WriteBuff);
int SendRecvPipes(HANDLE hPipe);

void main()
{
    SC_HANDLE schSCManager, schService;
    TCHAR PathExeServices[512];
    SERVICE_TABLE_ENTRY ServiceTable[2];
    ServiceTable[0].lpServiceName = (LPSTR)"AQueryMemory";
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;

    // Start the control dispatcher thread for our service
    StartServiceCtrlDispatcher(ServiceTable);
    GetModuleFileName(0, PathExeServices, MAX_PATH);
    schSCManager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    schService = CreateService(schSCManager,
        "AQueryMemory", "AQueryMemory",
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, // SERVICE_DEMAND_START: khi gọi hàm startservice thì chạy svc
        PathExeServices, 0, 0, 0, 0, 0);
}


void ServiceMain(int argc, char** argv)
{
    int error;
    BOOL bClientConnect = FALSE;
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;


    PSECURITY_DESCRIPTOR SecurityDes = NULL;
    BYTE  sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
    SecurityDes = (PSECURITY_DESCRIPTOR)sd;
    InitializeSecurityDescriptor(SecurityDes, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(SecurityDes, TRUE, (PACL)NULL, FALSE);
    SECURITY_ATTRIBUTES SecurityAttribute = { sizeof(SecurityAttribute), SecurityDes, FALSE };
    hStatus = RegisterServiceCtrlHandler(
        "AQueryMemory",
        (LPHANDLER_FUNCTION)ControlHandler);
    if (hStatus == (SERVICE_STATUS_HANDLE)0)
    {
        // Registering Control Handler failed
        return;
    }
    // Initialize Service 
    error = InitService();
    if (error)
    {
        // Initialization failed
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = -1;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;
    }
    // We report the running status to SCM. 
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);

    MEMORYSTATUS memory;
    // The worker loop of a service
    while (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
    {
        HANDLE hPipe = CreateNamedPipe(
            "\\\\.\\pipe\\AQueryMemory",             // pipe name 
            PIPE_ACCESS_DUPLEX,       // read/write access 
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,       // message type pipe  |   // message-read mode // blocking mode 
            PIPE_UNLIMITED_INSTANCES, // max. instances  
            512,                  // output buffer size 
            512,                  // input buffer size 
            0,                        // client time-out 
            &SecurityAttribute);


        char buffer[16];
        GlobalMemoryStatus(&memory);
        sprintf(buffer, "%d", memory.dwAvailPhys);
        int result = WriteToLog(buffer);
        bClientConnect = ConnectNamedPipe(hPipe, 0) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (bClientConnect)
        {
            SendRecvPipes(hPipe);
        }

       


        if (result)
        {
            ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            ServiceStatus.dwWin32ExitCode = -1;
            SetServiceStatus(hStatus, &ServiceStatus);
            //SendRecvPipes(hPipe);
            return;
        }

        Sleep(SLEEP_TIME);
    }
    return;
}

// Service initialization
int InitService()
{
    int result;
    result = WriteToLog((char*)"Monitoring started.");
    return(result);
}

// Control handler function
void ControlHandler(DWORD request)
{
    switch (request)
    {
    case SERVICE_CONTROL_STOP:
        WriteToLog((char*)"Monitoring stopped.");
        ServiceStatus.dwWin32ExitCode = 0;
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;

    case SERVICE_CONTROL_SHUTDOWN:
        WriteToLog((char*)"Monitoring stopped.");
        ServiceStatus.dwWin32ExitCode = 0;
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;

    default:
        break;
    }
    // Report current status
    SetServiceStatus(hStatus, &ServiceStatus);
    return;
}

int WriteToLog(char* str)
{
    FILE* fpLog;
    fpLog = fopen(LOGFILE, "a+");
    if (fpLog == NULL)
        return -1;
    if (fprintf(fpLog, "%s\n", str) < strlen(str))
    {
        fclose(fpLog);
        return -1;
    }
    fclose(fpLog);
    return 0;
}
int SendRecvPipes(HANDLE hPipe)
{
    char TempBuff[512] = { 0 };
    char Err[512] = { 0 };
    char WriteBuff[100] = { 0 };
    HANDLE hFile;
    BOOL fSuccess = FALSE;
    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0, cbToWrite = 0;
    
    HANDLE hHeap = GetProcessHeap();
    char* pchReceive = (char*)HeapAlloc(hHeap, 0, 512 * sizeof(char));
    WriteToLog((char*)"Client connected!");
    // Check message từ client ở đây
    while (1)
    {
        // Đọc pipes nó sẽ đợi cho có thông tin mới nó mới đọc
        fSuccess = ReadFile(
            hPipe,        // handle to pipe 
            pchReceive,    // buffer to receive data 
            512 * sizeof(char), // size of buffer 
            &cbBytesRead, // number of bytes read 
            NULL);
        // Check Nếu đọc ko thành công hoặc đọc được 0 Bytes
        if (!fSuccess || cbBytesRead == 0)
        {
            if (GetLastError() == ERROR_BROKEN_PIPE)
                WriteToLog((char*)"Client Disconnected");
            else
            {
                ZeroMemory(Err, sizeof(Err));
                sprintf(Err, "ReadFile failed Error: %d", GetLastError());
                WriteToLog(Err);
                //printf("ReadFile failed Error: %d", GetLastError());
            }
            break; // Quay lại trạng thái chờ client kết nối tới
        }
        ZeroMemory(TempBuff, sizeof("Delete:"));
        lstrcpyn(TempBuff, pchReceive, lstrlen("Delete:") + 1);
        if (lstrcmp(TempBuff, "AddFil:") == 0)
        {
            //printf(TempBuff);
            ZeroMemory(TempBuff, sizeof(TempBuff));
            lstrcpyn(TempBuff, &pchReceive[7], lstrlen(pchReceive) + 1);
            if ((hFile = CreateFile(TempBuff, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0)) != INVALID_HANDLE_VALUE)
                WriteToPipes(hPipe, (char*)"Create File Success !");
            else
            {
                sprintf(WriteBuff, "Create File Error: %d", GetLastError());
                WriteToPipes(hPipe, WriteBuff);
            }
            CloseHandle(hFile);

        }
        if (lstrcmp(TempBuff, "Delete:") == 0)
        {
            //printf(TempBuff);
            ZeroMemory(TempBuff, sizeof(TempBuff));
            lstrcpyn(TempBuff, &pchReceive[7], lstrlen(pchReceive) + 1);
            if ((hFile = CreateFile(TempBuff, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0)) != INVALID_HANDLE_VALUE)
            {
                if (DeleteFile(TempBuff))
                {
                    WriteToPipes(hPipe, (char*)"Delete File Success !");
                }
                else
                {
                    sprintf(WriteBuff, "Delete File Error: %d", GetLastError());
                    WriteToPipes(hPipe, WriteBuff);
                }
            }
            else
            {
                sprintf(WriteBuff, "Delete File Error: %d", GetLastError());
                WriteToPipes(hPipe, WriteBuff);
            }
            CloseHandle(hFile);
        }
    }
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    HeapFree(hHeap, 0, pchReceive);
    return 0;
}
int WriteToPipes(HANDLE hPipe, char* WriteBuff)
{
    HANDLE hFile;
    BOOL fSuccess = FALSE;
    DWORD cbToWrite = 0, cbWritten = 0;
    char Err[512] = { 0 };
    //ZeroMemory(WriteBuff, sizeof(WriteBuff));
    cbToWrite = lstrlen(WriteBuff);
    fSuccess = WriteFile(
        hPipe,                  // pipe handle 
        WriteBuff,             // message 
        cbToWrite,              // message length 
        &cbWritten,             // bytes written 
        NULL);                  // not overlapped 

    if (!fSuccess)
    {
        sprintf(Err, "WriteFile to pipe failed. GLE=%d\n", GetLastError());
        WriteToLog(Err);
        return -1;
    }
    return 0;
}