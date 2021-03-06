 
#include "stdafx.h"
#include "MovieNoise.h"
#include "MovieNoiseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
////////////////
typedef int(*lpAddFun)(char *, char *, int, char *);
typedef double (*lpProFun)( );
HANDLE  hSemaphoreSet;
HINSTANCE hDll; 
lpAddFun  addFun; 
lpProFun  proFun;
struct
{
  CString StringInput;
  CString StringOutput;
 } adding;
///////////////
CString RadioNum = "0.00%";
double  ratio = 0;
int     CMovieNoiseDlg::progress = 0;
int     FlagDone = 0;
///////////////
unsigned char buf[2048],buf1[2048];
unsigned char lastbit, last7bit;
long nbytes, fsize, readbytes;
FILE *InFile, *OutFile;

/***多线程处理函数1（处理视频）***/

/*********************************/
UINT ThreadFunc1(LPVOID lpParam)
{ 
  WaitForSingleObject(hSemaphoreSet, INFINITE);
  //char* sFile = (LPSTR)(LPCSTR)adding.StringInput;
  //char* dFile = (LPSTR)(LPCSTR)adding.StringOutput;  
  for(int i=0;i<200;i++)
  {
     nbytes =fread(buf,1,2048,InFile);
	 fwrite(buf,1,nbytes,OutFile);
	 readbytes += nbytes;
  }
  nbytes=fread(buf,1,2048,InFile);
  readbytes +=nbytes;
  do{
	 if(nbytes==2048)
	 {
	   for(int i=0; i<nbytes-1; i++)
		 buf1[i] = ((buf[i] & 0x7f)  << 1) + ((buf[i+1] & 0x80) >> 7);
	   last7bit = buf[nbytes-1] & 0x7f;				
	   nbytes=fread(buf,1,2048,InFile);
	   readbytes +=nbytes;
	   buf1[nbytes-1] = (last7bit << 1) + ((buf[0] & 0x80) >> 7);	
       fwrite(buf1, 1, nbytes, OutFile);
	 }
	 else
	 {
	   for(int i=0; i<nbytes; i++)
	     buf1[i] = ((buf[i] & 0x7f)  << 1) + ((buf[i+1] & 0x80) >> 7);
	   fwrite(buf1, 1, nbytes, OutFile);
	   ratio=1;
	   fclose(InFile);
	   fclose(OutFile);
	   FlagDone =1;
	   break;
	  }	
	}while(1);
  //如果是第一帧，需要申请内存，并初始化
  ReleaseSemaphore(hSemaphoreSet,1,NULL);
  return 0;
}
/***多线程处理函数2（加载进度条）***/

/***********************************/
UINT ThreadFunc2(LPVOID lpParam)
{
   WaitForSingleObject(hSemaphoreSet, INFINITE);
   //加载读取完成度函数
   //FlagDone=0表明还在处理视频，一直读取完成度
   while(!FlagDone)
   {
	 ratio=((double)readbytes)/fsize;
	 double ratio1 = ratio*100;
	 RadioNum.Format("%.2f%%",ratio1);
	 CMovieNoiseDlg::progress = (int)(ratio*1000);
	 if(CMovieNoiseDlg::progress >= 999)
		CMovieNoiseDlg::progress = 999;  	   
	 }
    ReleaseSemaphore(hSemaphoreSet,1,NULL);
    return 0;
}

CMovieNoiseDlg::CMovieNoiseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMovieNoiseDlg::IDD, pParent)
	, InputName(_T("E:\\11.mpg"))
	, OutputName(_T("E:\\11Denoising.mpg"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
void CMovieNoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Input, InputFile);
	DDX_Control(pDX, IDC_OUTPUT, OutputFile);
	DDX_Text(pDX, IDC_Input, InputName);
	DDX_Text(pDX, IDC_OUTPUT, OutputName);
}
BEGIN_MESSAGE_MAP(CMovieNoiseDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDAdding, &CMovieNoiseDlg::OnBnClickedAdding)
	ON_BN_CLICKED(IDC_OpenVideo, &CMovieNoiseDlg::OnBnClickedOpenvideo)
	ON_COMMAND(ID_ABOUT_ABOUTMOVIEPROCESS, &CMovieNoiseDlg::OnAboutAboutmovieprocess)
END_MESSAGE_MAP()
BOOL CMovieNoiseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);	
	return TRUE;  
}
void CMovieNoiseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
HCURSOR CMovieNoiseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
 }
IMPLEMENT_DYNAMIC(CProcess, CDialogEx)
CProcess::CProcess(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcess::IDD, pParent)
{
}
CProcess::~CProcess()
{
}
void CProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
}
BEGIN_MESSAGE_MAP(CProcess, CDialogEx)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()
// AboutMoiveProcess dialog

IMPLEMENT_DYNAMIC(AboutMoiveProcess, CDialogEx)

AboutMoiveProcess::AboutMoiveProcess(CWnd* pParent /*=NULL*/)
	: CDialogEx(AboutMoiveProcess::IDD, pParent)
{
}
AboutMoiveProcess::~AboutMoiveProcess()
{
}
void AboutMoiveProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(AboutMoiveProcess, CDialogEx)
END_MESSAGE_MAP()


// AboutMoiveProcess message handlers

/*******视频处理按键响应函数********/

/***********************************/
void CMovieNoiseDlg::OnBnClickedAdding()
{
   int ItemPoint1,ItemPoint2; 

   GetDlgItemText(IDC_Input,StringInput);
   GetDlgItemText(IDC_OUTPUT,StringOutput);
   for(int i=0; i<StringInput.GetLength();i++)
   {
	   if (StringInput[i]=='.')
		   ItemPoint1 = i;
   }
   for(int i=0; i<StringOutput.GetLength();i++)
   {
	   if (StringOutput[i]=='.')
		   ItemPoint2 = i;
   }
   //判断读取文件和输出文件是否为mpg格式
   CString ItemVideo  = StringInput.Mid(ItemPoint1+1,StringInput.GetLength()-ItemPoint1-1);
   CString ItemVideo1 = StringOutput.Mid(ItemPoint2+1,StringOutput.GetLength()-ItemPoint2-1);
   if((ItemVideo != _T("MPG"))&& (ItemVideo != _T("mpg"))&&(ItemVideo != _T("avi"))&&
	     (ItemVideo != _T("mp4"))&&(ItemVideo != _T("mvb"))&&(ItemVideo != _T("wma")))
	   MessageBox(StringInput+_T(" \n is not right format!"));
   else if((ItemVideo != _T("MPG"))&& (ItemVideo != _T("mpg"))&&(ItemVideo != _T("avi"))&&
	     (ItemVideo != _T("mp4"))&&(ItemVideo != _T("mvb"))&&(ItemVideo != _T("wma")))
       MessageBox(StringOutput+_T(" \n is not right format!"));
   //启动多线程处理
   else
   {
	   //判断是否存在输入文件
	   if(access(StringInput,4))
		   MessageBox(StringInput+_T(" \n is not exist!"));
	   else
	   {   
		   if(!(InFile = fopen(StringInput,"r+b"))|| !(OutFile = fopen(StringOutput,"w+b")))
	           MessageBox("open input file error");
		   //视频处理阶段
		   else
		   {
	           fseek(InFile,0L,SEEK_END);  
	           fsize=ftell(InFile);  
	           fseek(InFile,0L,SEEK_SET);  
			   adding.StringInput = StringInput;
		       adding.StringOutput= StringOutput;
	           readbytes = 0;
	           memset(buf,0,2048);
	           memset(buf1,0,2048);
		       //启动线程，重新置FlagDone为1。
		       FlagDone = 0;
		       //进度条起始为0
		       CMovieNoiseDlg::progress =0;
		       hSemaphoreSet = CreateSemaphore(NULL, 2, 2, NULL);
		       pThread[0]    = AfxBeginThread(ThreadFunc1,NULL);
		       pThread[1]    = AfxBeginThread(ThreadFunc2,NULL);
		       //CloseHandle(pThread[0]->m_hThread); //test
               dlg1.DoModal();
		       if(FlagDone == 1)
		       {
			   Sleep(10);
			   MessageBox(_T("Movie Denoising completed,\nPlease check under the output path!"),
				             _T("Tip"),MB_ICONINFORMATION);
			   }
		   }
	    }
    }
}
/***********打开文件操作************/

/***********************************/
CString CMovieNoiseDlg::OpenMPG( )
{
  bool isOpen = TRUE;      
  CString defaultDir = _T("E:\\");  
  CString fileName   = " ";      
  CString filter     = "Video (*.mpg;*.mp4;*.avi;*.wma;*.rmvb;*.3GP)|*.mpg;*.mp4;*.avi;*.wma;*.rmvb;*.3GP||";    
  CFileDialog openFileDlg(isOpen,defaultDir,fileName,OFN_HIDEREADONLY|OFN_READONLY,filter,NULL);  
  openFileDlg.GetOFN().lpstrInitialDir = "E:\\11.mpg";  
  INT_PTR result = openFileDlg.DoModal();  
  CString filePath = defaultDir + "\\11.mpg";  
  if(result == IDOK)  
    filePath = openFileDlg.GetPathName();  

  return filePath;
}
/*******打开视频按键响应函数********/

/***********************************/
void CMovieNoiseDlg::OnBnClickedOpenvideo()
{
  StringInput = OpenMPG();
  StringOutput= StringInput;
  StringOutput.Insert(StringInput.GetLength()-4,"Denoising"); 
  GetDlgItem(IDC_Input)->SetWindowText(StringInput);
  GetDlgItem(IDC_OUTPUT)->SetWindowText(StringOutput);
}
/********进度条处理函数函数*********/

/***********************************/
void CProcess::OnTimer(UINT_PTR nIDEvent)
{
  CDialogEx::OnTimer(nIDEvent);
  //设置进度条位置
  m_ctrlProgress.SetPos(CMovieNoiseDlg::progress);
  //更新完成度提示
  GetDlgItem(IDC_RADIO)->SetWindowText(RadioNum);
  //当FlagDone置1则关闭进度条
  if(FlagDone == 1)
  {
    this->EndDialog(true);
  }
}
/****开启计时，初始化进度条位置*****/

/***********************************/
void CProcess::OnShowWindow(BOOL bShow, UINT nStatus)
{
  CDialogEx::OnShowWindow(bShow, nStatus);
  this->KillTimer(1);
  this->SetTimer(1,10, NULL);
  //初始化进度条位置
  m_ctrlProgress.SetRange(1,1000);
}
/************About提示**************/

/***********************************/
void CMovieNoiseDlg::OnAboutAboutmovieprocess()
{
	 dlg2.DoModal();
 }
