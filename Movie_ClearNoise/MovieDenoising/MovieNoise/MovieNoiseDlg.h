
// MovieNoiseDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

//多线程函数处理
UINT ThreadFunc1(LPVOID lpParam);
UINT ThreadFunc2(LPVOID lpParam);

class CProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CProcess)
public:
	CProcess(CWnd* pParent = NULL);   
	virtual ~CProcess();

	enum { IDD = IDD_PROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_ctrlProgress;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
// AboutMoiveProcess dialog

class AboutMoiveProcess : public CDialogEx
{
	DECLARE_DYNAMIC(AboutMoiveProcess)
public:
	AboutMoiveProcess(CWnd* pParent = NULL);  
	virtual ~AboutMoiveProcess();
	enum { IDD = IDD_DIALOG1 };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);  
	DECLARE_MESSAGE_MAP()
};

class CMovieNoiseDlg : public CDialogEx
{
public:
	CMovieNoiseDlg(CWnd* pParent = NULL);	
	enum { IDD = IDD_MOVIENOISE_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit InputFile;
	CEdit OutputFile;
	afx_msg void OnBnClickedAdding();
	CString InputName;
	CString OutputName;
	CString StringInput;
    CString StringOutput;
	//////////////////
    CString OpenMPG( );
	afx_msg void OnBnClickedOpenvideo();
	CWinThread* pThread[2];
	static int progress; //控制进度条
	CProcess  dlg1;      //进度条界面
	AboutMoiveProcess dlg2; //about界面
	afx_msg void OnAboutAboutmovieprocess();
};



