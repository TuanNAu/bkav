
// MFCApplication2412Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication2412.h"
#include "MFCApplication2412Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#undef UNICODE 
// CAboutDlg dialog used for App About
#pragma warning(disable : 4996)

#define LOGFILE "C:\\Users\\TUAN\\source\\repos\\ConsoleApplication2212\\ConsoleApplication2212\\Debug\\memstatus.log"
#define NAMEPIPES "\\\\.\\pipe\\AQueryMemory"
#define BUFSIZE 512

BOOL bCreatedPipes = FALSE;
int ChangeFile(char* PathFile);

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNADD, &CMFCApplication2412Dlg::OnBnClickedBtnadd)
	ON_BN_CLICKED(IDC_BTNDELETE, &CMFCApplication2412Dlg::OnBnClickedBtndelete)
END_MESSAGE_MAP()


// CMFCApplication2412Dlg dialog



CMFCApplication2412Dlg::CMFCApplication2412Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2412_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2412Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication2412Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNADD, &CMFCApplication2412Dlg::OnBnClickedBtnadd)
	ON_BN_CLICKED(IDC_BTNDELETE, &CMFCApplication2412Dlg::OnBnClickedBtndelete)
END_MESSAGE_MAP()


// CMFCApplication2412Dlg message handlers

BOOL CMFCApplication2412Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCApplication2412Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCApplication2412Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication2412Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication2412Dlg::OnBnClickedBtnadd()
{
	// TODO: Add your control notification handler code here
	//Create File o day
	char buff[512 + 7] = { 0 };
	char Temp[512] = { 0 };
	lstrcatA(buff, "AddFile:");
	GetWindowTextA((GetDlgItem(IDC_MFCEDITBROWSE1)->m_hWnd), Temp, 512);
	lstrcatA(buff, Temp);
	//const char* PathAddFile = "AddFil:C:\\Users\\Vanh\\Desktop\\TestService\\taofile.txt";
	ChangeFile(buff);
}


void CMFCApplication2412Dlg::OnBnClickedBtndelete()
{
	// TODO: Add your control notification handler code here
	 // Delete File o day
	char buff[512 + 7] = { 0 };
	char Temp[512] = { 0 };
	lstrcatA(buff, "Delete:");
	GetWindowTextA((GetDlgItem(IDC_MFCEDITBROWSE1)->m_hWnd), Temp, 512);
	lstrcatA(buff, Temp);
	ChangeFile((char*)buff);
}
int ChangeFile(char* PathFile)
{
	HANDLE hPipe;
	BOOL   fSuccess = FALSE;
	char Err[512] = { 0 };
	DWORD  cbRead, cbToWrite, cbWritten, dwMode;
	// Khởi tạo kết nối tới Pipes
	while (1)
	{

		hPipe = CreateFileA(
			NAMEPIPES,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 
		// Nếu tạo đc thì break
		if (hPipe != INVALID_HANDLE_VALUE)
		{
			break;
		}
		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		// Xuwr lys looi
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			sprintf(Err, "Could not open pipe. GLE=%d\n", GetLastError());
			MessageBoxA(0, Err, "Tittle", MB_OK);
			return -1;
		}
		// Đợi Pipes khi nó bận chừng 20s
		if (!WaitNamedPipeA(NAMEPIPES, 20000))
		{
			//printf("Could not open pipe: 20 second wait timed out.");
			MessageBoxA(0, "Could not open pipe: 20 second wait timed out.", "Tittle", MB_OK);
			return -1;
		}
	}
	// Pipe đã được kết nối, chuyển chế độ READ tin nhắn
	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		sprintf(Err, "SetNamedPipeHandleState failed. GLE=%d\n", GetLastError());
		MessageBoxA(0, Err, "Tittle", MB_OK);
		return -1;
	}
	// Send a message to the pipe server. 

	cbToWrite = (lstrlenA(PathFile) + 1) * sizeof(TCHAR);
	while (1)
	{
		//printf("Sending %d byte message: \"%s\"\n", cbToWrite, PathFile);
		fSuccess = WriteFile(
			hPipe,                  // pipe handle 
			PathFile,             // message 
			cbToWrite,              // message length 
			&cbWritten,             // bytes written 
			NULL);                  // not overlapped 

		if (!fSuccess)

		{
			sprintf(Err, "WriteFile to pipe failed. GLE=%d\n", GetLastError());
			MessageBoxA(0, Err, "Tittle", MB_OK);
			return -1;
		}
		break;
	}
	//printf("\nMessage sent to server, receiving reply as follows:\n");
	char chBuf[BUFSIZE] = { 0 };
	do
	{
		// Read from the pipe. 

		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			BUFSIZE * sizeof(char),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

		//printf("\"%s\"\n", chBuf);
		MessageBoxA(0, chBuf, "Tittle", MB_OK);

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA
	CloseHandle(hPipe);
}
