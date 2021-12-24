
// MFCApplication2412Dlg.h : header file
//

#pragma once


// CMFCApplication2412Dlg dialog
class CMFCApplication2412Dlg : public CDialogEx
{
// Construction
public:
	CMFCApplication2412Dlg(CWnd* pParent = nullptr);	// standard constructor
	afx_msg void OnBnClickedBtnadd();
	afx_msg void OnBnClickedBtndelete();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION2412_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

};
