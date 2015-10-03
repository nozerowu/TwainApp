// TWainAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TWainApp.h"
#include "TWainAppDlg.h"

#include "RotateDialog.h"

#include "OpenCVImageHandle.h"
#include "PreViewFrame.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTWainAppDlg dialog

CTWainAppDlg::CTWainAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTWainAppDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTWainAppDlg)
	m_showUI = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_twainWrapper = new CTWainWrapper();

	m_map_cmdline.clear();
	m_pic_handles.clear();

	m_flag_insertscan = FALSE;

	m_current_rotation = 0;
	m_current_resolution = 0;

	m_minResolution = 0;
	m_maxResolution = 0;

	m_initPictureCtrl = FALSE;

	m_pTempCxImage = NULL;

	//��װGDI+ͼ�ο�
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

void CTWainAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTWainAppDlg)
	DDX_Control(pDX, IDC_BUTTON_INSCAN, m_btn_inscan);
	DDX_Control(pDX, IDC_BUTTON_SCAN, m_btn_scan);
	DDX_Control(pDX, IDC_BUTTON_QB, m_btn_qb);
	DDX_Control(pDX, IDC_BUTTON_JP, m_btn_jp);
	DDX_Control(pDX, IDC_BUTTON_QHB, m_btn_qhb);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_static_image);
	DDX_Control(pDX, IDC_LIST_IMAGE, m_ctrl_imagelist);
	DDX_Control(pDX, IDC_BUTTON_RIGHT90, m_btn_right);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btn_delete);
	DDX_Control(pDX, IDC_BUTTON_LEFT90, m_btn_left);
	DDX_Control(pDX, IDC_EDIT_RESOLUTION, m_edit_resolution);
	DDX_Control(pDX, IDC_EDIT_ROTATION, m_edit_rotation);
	DDX_Control(pDX, IDC_SPIN_ROTATION, m_spin_rotation);
	DDX_Control(pDX, IDC_COMBO_COLOR, m_combo_color);
	DDX_Control(pDX, IDC_BUTTON_MOVEDOWN, m_btn_movedown);
	DDX_Control(pDX, IDC_BUTTON_MOVEUP, m_btn_moveup);
	DDX_Control(pDX, IDC_COMBO_PAGE, m_combo_page);
	DDX_Control(pDX, IDC_EDIT_SOURCE, m_edit_sourcename);
	DDX_Check(pDX, IDC_CHECK_SHOW, m_showUI);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTWainAppDlg, CDialog)
	//{{AFX_MSG_MAP(CTWainAppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, OnButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, OnButtonCancle)
	ON_LBN_SELCHANGE(IDC_LIST_IMAGE, OnSelchangeListImage)
	ON_LBN_DBLCLK(IDC_LIST_IMAGE, OnDblclkListImage)
	ON_BN_CLICKED(IDC_BUTTON_INSCAN, OnButtonInscan)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ROTATION, OnDeltaposSpinRotation)
	ON_EN_CHANGE(IDC_EDIT_ROTATION, OnChangeEditRotation)
	ON_EN_UPDATE(IDC_EDIT_ROTATION, OnUpdateEditRotation)
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_IMAGE, OnItemchangedListImage)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_LEFT90, OnButtonLeft90)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT90, OnButtonRight90)
	ON_BN_CLICKED(IDC_BUTTON_MOVEUP, OnButtonMoveup)
	ON_BN_CLICKED(IDC_BUTTON_MOVEDOWN, OnButtonMovedown)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_QHB, OnButtonQhb)
	ON_BN_CLICKED(IDC_BUTTON_JP, OnButtonJp)
	ON_BN_CLICKED(IDC_BUTTON_QB, OnButtonQb)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTWainAppDlg message handlers

BOOL CTWainAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	//ShowWindow(SW_SHOWMAXIMIZED);

	// ��ʼ������ 
	m_combo_page.InsertString(0, "�� ��");
	m_combo_page.InsertString(1, "˫ ��");
	m_combo_page.SetCurSel(0);

	m_edit_rotation.SetWindowText("0");
	m_spin_rotation.SetRange(-359, 359);
	
	// ����CListCtrl������
	m_ctrl_imagelist.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrl_imagelist.SetIconSpacing(CSize(110, 100));
	
	// ��ʼ��CImageList�ߴ硢����
	m_imageList.Create(100, 100, ILC_COLOR32 | ILC_COLORDDB | ILC_MASK, 1, 1);
	m_imageList.SetBkColor(RGB(255,255,255));
	m_ctrl_imagelist.SetImageList(&m_imageList, LVSIL_NORMAL);
	
	//��ťͼƬ����
	HBITMAP moveupbmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_MOVEUP));
	m_btn_moveup.SetBitmap(moveupbmp);
	HBITMAP movedownbmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_MOVEDOWN));
	m_btn_movedown.SetBitmap(movedownbmp);
	HBITMAP deletebmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DELETE));
	m_btn_delete.SetBitmap(deletebmp);

	HBITMAP leftbmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_LEFT));
	m_btn_left.SetBitmap(leftbmp);
	HBITMAP rightbmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RIGHT));
	m_btn_right.SetBitmap(rightbmp);

	HBITMAP QHBbmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_QHB));
	m_btn_qhb.SetBitmap(QHBbmp);
	HBITMAP JPbmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_JP));
	m_btn_jp.SetBitmap(JPbmp);
	HBITMAP QBbmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_QB));
	m_btn_qb.SetBitmap(QBbmp);
	
	//EnableToolTips(TRUE);
	// ����ͣ����ʾ��Ϣ
	m_toolTip.Create(this);
	m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_MOVEUP), "��ǰ�ƶ�ѡ��ɨ��ҳ");
	m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_MOVEDOWN), "����ƶ�ѡ��ɨ��ҳ");
	m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_DELETE), "ɾ��ѡ��ҳ");
	m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_LEFT90), "˳ʱ����ת90��");
	m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_RIGHT90), "��ʱ����ת90��");
	m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_QHB), "ȥ���ڱ�");
	m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_JP), "ͼ���ƫ");
	m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_QB), "ɾ�����пհ�ɨ��ҳ");
	m_toolTip.SetDelayTime(500);
	m_toolTip.Activate(TRUE);
	
	//EXE·��
	TCHAR szFilePath[MAX_PATH + 1] = {0};
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	_tcsrchr(szFilePath, _T('\\'))[1] = 0;
	m_strExePath = szFilePath;
	
/*	//////////////////////////////////////////ͼ��������//////////////////////////////////////
	CString input = m_strExePath + "\\temp\\WHITE_TEMP.bmp";
	CString output = m_strExePath + "\\temp\\WHITE.bmp";

	string inputPath = input.GetBuffer(0);
	input.ReleaseBuffer();
	string outputPath = output.GetBuffer(0);
	output.ReleaseBuffer();

	BOOL ret = OpenCVImageHandle::cv_isAllWhite(inputPath);
*/
/*	CString input = m_strExePath + "\\temp\\QHB_TEMP_TEST.bmp";
	CString output = m_strExePath + "\\temp\\QHB.bmp";

	string inputPath = input.GetBuffer(0);
	input.ReleaseBuffer();
	string outputPath = output.GetBuffer(0);
	output.ReleaseBuffer();
	
	OpenCVImageHandle::cv_RemoveBorder(inputPath, outputPath);
*/
/*
	input = m_strExePath + "\\temp\\JP_TEMP_6.bmp";
	output = m_strExePath + "\\temp\\JP.bmp";
	
	inputPath = input.GetBuffer(0);
	input.ReleaseBuffer();
	outputPath = output.GetBuffer(0);
	output.ReleaseBuffer();

	int retValue = 0;
 	OpenCVImageHandle::cv_roteImgSelf(inputPath, outputPath, retValue);
*/	//////////////////////////////////////////////////////////////////////////////////////////////

	// ��ʼ��TWain
	if(!m_twainWrapper->InitTwain(m_hWnd))
	{
		MessageBox("TWain��ʼ��ʧ��", MESSAGEBOX_TITLE_ERROR);
		return FALSE;
	}

	// ѡ��Ĭ��Դ
	if(!m_twainWrapper->SelectDefaultSource())
	{
		MessageBox("ѡ��Ĭ��Դʧ��", MESSAGEBOX_TITLE_ERROR);
		return FALSE;
	}

	// ���Ĭ��Դ���ƣ����Խ�����ʾ
	TW_IDENTITY source = m_twainWrapper->GetSourceIdentity();
	m_edit_sourcename.SetWindowText(source.ProductName);

	/////////////////////////////////////////////////////////////////////////////

	// ��Դ
	if(!m_twainWrapper->OpenSource())
	{
		MessageBox("ѡ��Ĭ��Դʧ��", MESSAGEBOX_TITLE_ERROR);
		return FALSE;
	}

	// �����Զ�����װ��
	if(!m_twainWrapper->EnableAutofeed())
	{
		CTWainHelper::LogMessage("�����Զ�����װ��ʧ��");
	}

	// ���ɨ���ǵ�ǰ�����������ӵ�������
	GetCurrentSourceCapability();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTWainAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTWainAppDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTWainAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTWainAppDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_twainWrapper->CloseSource();
	m_twainWrapper->CloseDSM();
	
	//ж��GDI+ͼ�ο�
	GdiplusShutdown(m_gdiplusToken);
	
	CDialog::OnClose();
}

BOOL CTWainAppDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class	
	m_toolTip.RelayEvent(pMsg);
	
	// ��TWain������Ϣ���д���
	m_twainWrapper->ProcessMessage(*pMsg);
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CTWainAppDlg::SetParameter(std::map<CString, CString> mapCmdLine)
{
	m_map_cmdline = mapCmdLine;
}

BOOL CTWainAppDlg::GetCurrentSourceCapability()
{
	if(!m_twainWrapper->IsDSOpen())
	{
		MessageBox("��Դʧ��", MESSAGEBOX_TITLE_ERROR);
		return FALSE;
	}

	// ���֧�ֵĺ͵�ǰ��ɫ��ģʽ
	GetCapability_PixelType();
	// ���֧�ֵĺ͵�ǰ�ķֱ���
	GetCapability_Resolution();

	// ����Ƿ�֧��˫��ɨ������
	if(!GetCapability_Duplex())
	{
		m_combo_page.SetCurSel(0);
		m_combo_page.EnableWindow(FALSE);
	}
	else
	{
		m_combo_page.SetCurSel(0);
		m_combo_page.EnableWindow(TRUE);
	}

	//GetCapability_Rotation();
	
	return TRUE;
}

BOOL CTWainAppDlg::SetCurrentSourceCapability()
{
	//����ҳ������
	if(m_combo_page.IsWindowEnabled())
	{
		// �����û�ѡ��ȷ���ǵ���(FALSE)����˫��(TRUE)
		BOOL current_duplex = (BOOL)m_combo_page.GetCurSel();
		
		if(!SetCapability_Duplex(current_duplex))
		{
			MessageBox("����ҳ������ʧ��");
			return FALSE;
		}
	}

/*
	//������ת����
	if(m_edit_rotation.IsWindowEnabled())
	{
		CString strRotation = "0";
		m_edit_rotation.GetWindowText(strRotation);
		int nCurAngle = _ttoi(strRotation);

		//if(m_current_rotation != (BOOL)nCurAngle)
		
		if(SetCapability_Rotation(nCurAngle))
		{
			m_current_rotation = nCurAngle;
		}
		else
		{
			MessageBox("������ת����ʧ��");
			return FALSE;
		}
	}
*/

	//����ɫ��ģʽ
	int curSelect = m_combo_color.GetCurSel();
	
	CString curText;
	int len = m_combo_color.GetLBTextLen(curSelect);
	m_combo_color.GetLBText(curSelect, curText.GetBuffer(len));

	for(int i = 0; i < MAX_PIXELTYPE; i++)
	{
		char* pCurText = (LPSTR)(LPCTSTR)curText;
		if(strcmp(pCurText, PixelType[i].pszItemName) == 0)	//���
		{
			if(SetCapability_PixelType(PixelType[i].ItemId))
			{
				m_current_color = curSelect;
			}
			else
			{
				MessageBox("����ɫ��ģʽʧ��");
				return FALSE;
			}
			break;
		}
	}

	//���÷ֱ���
	if(m_edit_resolution.IsWindowEnabled())
	{
		CString strResolution = "0";
		m_edit_resolution.GetWindowText(strResolution);
		int nCurResolution = _ttoi(strResolution);

		if(nCurResolution < m_minResolution || nCurResolution > m_maxResolution)
		{
			CString info = "";
			info.Format("֧�ֵķֱ��ʵ�ȡֵ��ΧΪ %d - %d, ���벻�Ϸ�", m_minResolution, m_maxResolution);
			MessageBox((LPCTSTR)info);
			return FALSE;
		}
		else
		{
			if(SetCapabiliry_Resolution(nCurResolution))
			{
				m_current_resolution = nCurResolution;
			}
			else
			{
				MessageBox("���÷ֱ���ʧ��");
				return FALSE;
			}
		}
	}

	return TRUE;
}

void CTWainAppDlg::GetCapability_PixelType()
{
	m_combo_color.ResetContent();

	TW_CAPABILITY twCapability;
	m_twainWrapper->GetCapability(twCapability, ICAP_PIXELTYPE, TWON_ENUMERATION);
	
	pTW_ENUMERATION pEnumeration = (pTW_ENUMERATION)GlobalLock(twCapability.hContainer);
	
	//ɫ��ģʽ������
	int nNumItems = pEnumeration->NumItems;
	
	for (int index = 0; index < nNumItems; index++)
	{
		TW_UINT16 curItem = *(TW_UINT16*)&pEnumeration->ItemList[index*(sizeof(TW_UINT16))];
		for(int i = 0; i < MAX_PIXELTYPE; i++)
		{
			if(curItem == PixelType[i].ItemId)
			{
				m_combo_color.InsertString(index, PixelType[i].pszItemName);
				break;
			}
		}
	}
	
	m_combo_color.SetCurSel(pEnumeration->CurrentIndex);
	m_current_color = pEnumeration->CurrentIndex;
	
	GlobalUnlock(twCapability.hContainer);
	GlobalFree(twCapability.hContainer);
}

BOOL CTWainAppDlg::SetCapability_PixelType(TW_UINT16 value)
{
	return m_twainWrapper->SetCapability(ICAP_PIXELTYPE, value);
}

void CTWainAppDlg::GetCapability_Resolution()
{
	m_edit_resolution.EnableWindow(TRUE);

	TW_CAPABILITY twCapability_x;
	TW_CAPABILITY twCapability_y;

	if(!m_twainWrapper->GetCapability(twCapability_x, ICAP_XRESOLUTION, TWON_RANGE))
	{
		TW_STATUS status = m_twainWrapper->GetStatus();
		if(TWCC_CAPUNSUPPORTED == status.ConditionCode)
		{
			m_edit_resolution.EnableWindow(FALSE);
			return;
		}
		
		m_edit_resolution.EnableWindow(FALSE);
		MessageBox("��ȡ�ֱ���-X������ʧ��");
		return;
	}

	if(!m_twainWrapper->GetCapability(twCapability_y, ICAP_YRESOLUTION, TWON_RANGE))
	{
		TW_STATUS status = m_twainWrapper->GetStatus();
		if(TWCC_CAPUNSUPPORTED == status.ConditionCode)
		{
			m_edit_resolution.EnableWindow(FALSE);
			return;
		}
		
		m_edit_resolution.EnableWindow(FALSE);
		MessageBox("��ȡ�ֱ���-Y������ʧ��");
		return;
	}

	pTW_RANGE pRange_x = (pTW_RANGE)GlobalLock(twCapability_x.hContainer);
	pTW_RANGE pRange_y = (pTW_RANGE)GlobalLock(twCapability_y.hContainer);

	//Ҫ������ʵ��ӡ�����е���
	if(pRange_x->MinValue > pRange_x->MaxValue || pRange_y->MinValue > pRange_y->MaxValue)
	{
		m_current_resolution = pRange_x->DefaultValue;
		
		CString strResolution = "0";
		strResolution.Format("%d", m_current_resolution);
		m_edit_resolution.SetWindowText(strResolution);
		
		m_edit_resolution.EnableWindow(FALSE);

		GlobalUnlock(twCapability_x.hContainer);
		GlobalFree(twCapability_x.hContainer);
		GlobalUnlock(twCapability_y.hContainer);
		GlobalFree(twCapability_y.hContainer);

		return;
	}

	m_current_resolution = pRange_x->CurrentValue;

	if(pRange_x->MinValue > pRange_y->MinValue)
		m_minResolution = pRange_x->MinValue;
	else
		m_minResolution = pRange_y->MinValue;

	if(pRange_x->MaxValue > pRange_y->MaxValue)
		m_maxResolution = pRange_y->MaxValue;
	else
		m_maxResolution = pRange_x->MaxValue;

	CString strResolution = "0";
	strResolution.Format("%d", m_current_resolution);
	m_edit_resolution.SetWindowText(strResolution);

	GlobalUnlock(twCapability_x.hContainer);
	GlobalFree(twCapability_x.hContainer);
	GlobalUnlock(twCapability_y.hContainer);
	GlobalFree(twCapability_y.hContainer);
}

BOOL CTWainAppDlg::SetCapabiliry_Resolution(TW_UINT16 value)
{
	if(m_twainWrapper->IsDSOpen())
	{
		TW_CAPABILITY twCap;
		pTW_ONEVALUE pVal;
		BOOL ret_value_x = FALSE;
		BOOL ret_value_y = FALSE;
		
		twCap.Cap = ICAP_XRESOLUTION;
		twCap.ConType = TWON_ONEVALUE;
		twCap.hContainer = GlobalAlloc(GHND, sizeof(TW_ONEVALUE));
		
		if(twCap.hContainer)
		{
			pVal = (pTW_ONEVALUE)GlobalLock(twCap.hContainer);
			pVal->ItemType = TWTY_FIX32;
			pVal->Item = (TW_UINT32)value;
			
			GlobalUnlock(twCap.hContainer);
			ret_value_x = m_twainWrapper->SetCapability(twCap);
			GlobalFree(twCap.hContainer);
		}
		
		twCap.Cap = ICAP_YRESOLUTION;
		twCap.ConType = TWON_ONEVALUE;
		twCap.hContainer = GlobalAlloc(GHND, sizeof(TW_ONEVALUE));
		
		if(twCap.hContainer)
		{
			pVal = (pTW_ONEVALUE)GlobalLock(twCap.hContainer);
			pVal->ItemType = TWTY_FIX32;
			pVal->Item = (TW_UINT32)value;
			
			GlobalUnlock(twCap.hContainer);
			ret_value_y = m_twainWrapper->SetCapability(twCap);
			GlobalFree(twCap.hContainer);
		}
		return ret_value_x && ret_value_y;
	}
	return FALSE;
}

BOOL CTWainAppDlg::GetCapability_Duplex()
{
	TW_CAPABILITY twCapability;
	if(!m_twainWrapper->GetCapability(twCapability, CAP_DUPLEX, TWON_ONEVALUE))
	{
		TW_STATUS status = m_twainWrapper->GetStatus();
		if(TWCC_CAPUNSUPPORTED == status.ConditionCode)
		{
			return FALSE;
		}

		MessageBox("��ȡԴ����ʧ��");
		return FALSE;
	}
	
	pTW_ONEVALUE pValue = (pTW_ONEVALUE)GlobalLock(twCapability.hContainer);
	if(pValue->Item)
	{
		GlobalUnlock(twCapability.hContainer);
		GlobalFree(twCapability.hContainer);
		return TRUE;
	}

	GlobalUnlock(twCapability.hContainer);
	GlobalFree(twCapability.hContainer);
	return FALSE;
}

BOOL CTWainAppDlg::SetCapability_Duplex(BOOL isDuplex)
{
	return m_twainWrapper->SetCapability(CAP_DUPLEXENABLED, isDuplex);
}

void CTWainAppDlg::GetCapability_Rotation()
{
	TW_CAPABILITY twCapability;
	if(!m_twainWrapper->GetCapability(twCapability, ICAP_ROTATION, TWON_ONEVALUE))
	{
		TW_STATUS status = m_twainWrapper->GetStatus();
		if(TWCC_CAPUNSUPPORTED == status.ConditionCode)
		{
			m_edit_rotation.EnableWindow(FALSE);
			return;
		}
		
		m_edit_rotation.EnableWindow(FALSE);
		MessageBox("��ȡԴ����ʧ��");
		return;
	}
	
	pTW_ONEVALUE pValue = (pTW_ONEVALUE)GlobalLock(twCapability.hContainer);
	if(pValue->Item)
	{
		TW_UINT32 angle = pValue->Item;

		m_current_rotation = angle;

		CString strAngle= "0";
		strAngle.Format("%d", angle);
		m_edit_rotation.SetWindowText(strAngle);
		m_edit_rotation.EnableWindow(TRUE);
	
		GlobalUnlock(twCapability.hContainer);
		GlobalFree(twCapability.hContainer);
		return;
	}

	m_edit_rotation.EnableWindow(FALSE);
	GlobalUnlock(twCapability.hContainer);
	GlobalFree(twCapability.hContainer);
	return;
}

BOOL CTWainAppDlg::SetCapability_Rotation(TW_UINT16 value)
{
	return m_twainWrapper->SetCapability(ICAP_ROTATION, value);
}


void CTWainAppDlg::OnButtonSelect() 
{
	// TODO: Add your control notification handler code here
	m_twainWrapper->CloseSource();

	m_twainWrapper->SelectSource();
	TW_IDENTITY source = m_twainWrapper->GetSourceIdentity();
	m_edit_sourcename.SetWindowText(source.ProductName);

	//����ѡ���Դ
	if(!m_twainWrapper->OpenSource())
		MessageBox("ѡ��Դʧ��", MESSAGEBOX_TITLE_ERROR);

	//�����Զ�����װ��
	if(!m_twainWrapper->EnableAutofeed())
		CTWainHelper::LogMessage("�����Զ�����װ��ʧ��");

	//���ɨ���ǵ�ǰ�����������ӵ�������
	GetCurrentSourceCapability();
}

void CTWainAppDlg::OnButtonScan() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if(!m_twainWrapper->IsDSMOpen())
	{
		m_twainWrapper->OpenDSM();
	}

	if(!m_twainWrapper->IsDSOpen())
	{
		m_twainWrapper->OpenSource();
	}

	// ����Դ����
	if(!SetCurrentSourceCapability())
	{
		return;
	}

	m_flag_insertscan = FALSE;
	
	BOOL ret = m_twainWrapper->EnableSource(m_showUI);
	if(!ret)
		MessageBox("����ɨ�����ʧ��", MESSAGEBOX_TITLE_ERROR);

	if(m_showUI)
	{
		m_btn_scan.EnableWindow(TRUE);
		m_btn_inscan.EnableWindow(TRUE);
	}
	else
	{
		m_btn_scan.EnableWindow(FALSE);
		m_btn_inscan.EnableWindow(FALSE);
	}
}

void CTWainAppDlg::OnButtonInscan() 
{
	// TODO: Add your control notification handler code here
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(nSelectedIndex == -1)
	{
		MessageBox("δѡ��ҳ��", MESSAGEBOX_TITLE_ERROR);
		return;
	}
	
	UpdateData();
	
	if(!m_twainWrapper->IsDSMOpen())
	{
		m_twainWrapper->OpenDSM();
	}
	
	if(!m_twainWrapper->IsDSOpen())
	{
		m_twainWrapper->OpenSource();
	}

	// ����Դ����
	if(!SetCurrentSourceCapability())
	{
		return;
	}
	
	m_flag_insertscan = TRUE;

	BOOL ret = m_twainWrapper->EnableSource(m_showUI);
	if(!ret)
		MessageBox("����ɨ�����ʧ��", MESSAGEBOX_TITLE_ERROR);

	if(m_showUI)
	{
		m_btn_scan.EnableWindow(TRUE);
		m_btn_inscan.EnableWindow(TRUE);
	}
	else
	{
		m_btn_scan.EnableWindow(FALSE);
		m_btn_inscan.EnableWindow(FALSE);
	}
}

void CTWainAppDlg::OnButtonCancle() 
{
	// TODO: Add your control notification handler code here
	if(m_twainWrapper->IsDSEnable()){
		
		m_twainWrapper->CancelTransfer();

		BOOL ret = m_twainWrapper->DisableSource();
		if( !ret )
			MessageBox("�ر�ɨ�����ʧ��", MESSAGEBOX_TITLE_ERROR);
	}

	m_btn_scan.EnableWindow(TRUE);
	m_btn_inscan.EnableWindow(TRUE);
}

void CTWainAppDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	// /////////////////////////////////////////////////////
	// PDF��ʽת���ʹ洢
	if(!CTWainHelper::SaveAsPdf(".\\temp\\image.pdf", m_pic_handles))
	{
		MessageBox("����PDF�ļ�ʧ�ܣ�", MESSAGEBOX_TITLE_INFO);
		return;
	}

	// PDF�ļ��ϴ�
	// WinExec��ShellExecute��CreateProcess
	// Example : "httpUpload http://192.168.18.7:8080/BJXT/fileupload 3 zbxlh 123 ssmk bjxt yh hujq 1 scanfile.pdf C:\\image.pdf";
	CString strPara = "";
	strPara.Format("httpUpload %s %d tempfile %s 1 scanfile.pdf .\\temp\\image.pdf", 
		m_map_cmdline["URL"], 1, m_map_cmdline["TEMPFILE"]);

	CTWainHelper::LogMessage("�ļ��ϴ������У�");
	CTWainHelper::LogMessage((char*)(LPCTSTR)strPara);
	CTWainHelper::LogMessage("\r\n");

	if(CTWainHelper::Upload((LPCTSTR)strPara) != 0)
	{
		MessageBox("�ϴ�ɨ���ļ�ʧ�ܣ�", MESSAGEBOX_TITLE_INFO);
		return;
	}

	//::DeleteFile("C:\\image.pdf");

	//���ͼƬ�������

	//ֱ�ӵ���clear������������ڴ�
	//m_pic_handles.clear();

	int size = m_pic_handles.size();
	
	std::vector< HANDLE >::iterator it;
	for(it = m_pic_handles.begin(); it != m_pic_handles.end(); it++)
	{
		GlobalFree(*it);
	}

	m_pic_handles.erase(m_pic_handles.begin(), m_pic_handles.end());

	///////////////////////////////////////////////////////////////////////////
	for(int nIndex = 0; nIndex < m_imageList.GetImageCount(); nIndex++)
		m_imageList.Remove(nIndex);
	m_ctrl_imagelist.DeleteAllItems();

	m_static_image.SetBitmap(NULL);
	
	CRect lRect;
	m_static_image.GetClientRect(&lRect);
	m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));
	///////////////////////////////////////////////////////////////////////////

	MessageBox("�ļ��ϴ��ɹ���", MESSAGEBOX_TITLE_INFO);

	CDialog::OnOK();
}

void CTWainAppDlg::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
	int size = m_pic_handles.size();
	
	std::vector< HANDLE >::iterator it;
	for(it = m_pic_handles.begin(); it != m_pic_handles.end(); it++)
	{
		GlobalFree(*it);
	}
	
	m_pic_handles.erase(m_pic_handles.begin(), m_pic_handles.end());
	
	///////////////////////////////////////////////////////////////////////////
	for(int nIndex = 0; nIndex < m_imageList.GetImageCount(); nIndex++)
		m_imageList.Remove(nIndex);
	m_ctrl_imagelist.DeleteAllItems();
	
	m_static_image.SetBitmap(NULL);

	m_twainWrapper->CloseDSM();

//	_CrtDumpMemoryLeaks();

	CDialog::OnCancel();
}

LRESULT CTWainAppDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case PM_XFERDONE:
		// ȡ��
		if(wParam == NULL)
		{
			m_btn_scan.EnableWindow(TRUE);
			m_btn_inscan.EnableWindow(TRUE);
			
			m_ctrl_imagelist.Invalidate();
		
			break;
		}

		HandleImage((HANDLE)wParam);
		break;
	default:
		// TODO: Add any work else
		break;
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CTWainAppDlg::HandleImage(HANDLE image)
{
	if(m_flag_insertscan == FALSE)
	{
		//��ͼƬ������浽����������
		m_pic_handles.push_back(image);

		//������ʱҳ������
		char imageNum[32];
		sprintf(imageNum, "ɨ��Ԥ��_%d", m_pic_handles.size());

		//��ɨ��Ԥ����������ʾ����ͼ
		//ʹ��CxImage������ͼ�δ�������д���
		CxImage cxImage;
		cxImage.CreateFromHANDLE(image);
		
		//��ȡԭʼ�ĸߺͿ�
		int height = cxImage.GetHeight();
		int width = cxImage.GetWidth();
		
		//����CImageList�ĳߴ��ԭʼͼ��������Ŵ���
		int newWidth;
		int newHeight;
		CTWainHelper::GetProportionSize(width, height, 100, 100, &newWidth, &newHeight);
		
		//ͼ���������
		int x = (100 - newWidth) / 2;
		int y = (100 - newHeight) / 2;
		
		HBITMAP hBitmap = NULL;
		hBitmap = cxImage.MakeBitmap();
		
		// ʹ��GDI+������ͼ�δ�������д���
		// Bitmap����Ҫר���ͷ��ڴ�
		Bitmap bmp(hBitmap, NULL);
		Bitmap Thumbnail(100, 100);

		if (hBitmap)
		{
			DeleteObject(hBitmap);
		}
		
		//��ɫ������ˢ
		SolidBrush whiteBrush(Color::White);
		
		//��������ͼ�Ĵ����ͻ���
		Graphics* pGraphics = Graphics::FromImage(&Thumbnail);
		pGraphics->FillRectangle(&whiteBrush, 0, 0, 100, 100);
		
		pGraphics->DrawImage(&bmp, Rect(x, y, newWidth, newHeight));
		delete pGraphics;
		
		//���CBitmap���󣬴Ӷ���CImageList����������ͼ
		HBITMAP hBmp;
		Thumbnail.GetHBITMAP(RGB(255,255,255), &hBmp);
		CBitmap* pBitmap = CBitmap::FromHandle(hBmp);	//����ڴ���m_imageList����Ҫʹ�ò���ɾ��
	
		int imageID = m_imageList.Add(pBitmap, RGB(255,255,255));
		m_ctrl_imagelist.InsertItem(imageID, imageNum, imageID);

		m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
		m_ctrl_imagelist.SetItemState(imageID, LVIS_SELECTED, LVIS_SELECTED);
	}
	else
	{
		int curSelect = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
		
		int index = 0;
		std::vector< HANDLE >::iterator it;
		for(it = m_pic_handles.begin(); it != m_pic_handles.end(); it++)
		{
			if(index != curSelect)
			{
				index++;
				continue;
			}
			
			m_pic_handles.insert(it + 1, image);
			break;
		}
		
		char imageNum[32];
		sprintf(imageNum, "ɨ��Ԥ��_%d", m_pic_handles.size());

		//��������ͼ...

		//��ɨ��Ԥ����������ʾ����ͼ
		//ʹ��CxImage������ͼ�δ�������д���
		CxImage cxImage;
		cxImage.CreateFromHANDLE(image);
		
		//��ȡԭʼ�ĸߺͿ�
		int height = cxImage.GetHeight();
		int width = cxImage.GetWidth();
		
		//����CImageList�ĳߴ��ԭʼͼ��������Ŵ���
		int newWidth;
		int newHeight;
		CTWainHelper::GetProportionSize(width, height, 100, 100, &newWidth, &newHeight);
		
		//ͼ���������
		int x = (100 - newWidth) / 2;
		int y = (100 - newHeight) / 2;
		
		HBITMAP hBitmap = NULL;
		hBitmap = cxImage.MakeBitmap();
		
		//ʹ��GDI+������ͼ�δ�������д���
		Bitmap bmp(hBitmap, NULL);
		Bitmap Thumbnail(100, 100);

		if (hBitmap)
		{
			DeleteObject(hBitmap);
		}
		
		//��ɫ������ˢ
		SolidBrush whiteBrush(Color::White);
		
		//��������ͼ�Ĵ����ͻ���
		Graphics* pGraphics = Graphics::FromImage(&Thumbnail);
		pGraphics->FillRectangle(&whiteBrush, 0, 0, 100, 100);
		
		pGraphics->DrawImage(&bmp, Rect(x, y, newWidth, newHeight));
		delete pGraphics;
		
		//���CBitmap���󣬴Ӷ���CImageList����������ͼ
		HBITMAP hBmp;
		Thumbnail.GetHBITMAP(RGB(255,255,255), &hBmp);
		CBitmap* pBitmap = CBitmap::FromHandle(hBmp);

		///////////////////////////////////////////////////////////////

		int nImageCount = m_imageList.GetImageCount();
		m_imageList.SetImageCount(nImageCount + 1);
		
		int nIndex = nImageCount;
		LVITEM lvitem;
		for(; nIndex != curSelect + 1; nIndex--)
		{
			m_imageList.Copy(nIndex, nIndex - 1);

			lvitem.iItem = nIndex;
			lvitem.mask = LVIF_IMAGE;
			m_ctrl_imagelist.GetItem(&lvitem);
			lvitem.iImage = nIndex;
			m_ctrl_imagelist.SetItem(&lvitem);

			CString text = m_ctrl_imagelist.GetItemText(nIndex - 1, 0);
			m_ctrl_imagelist.SetItemText(nIndex, 0, text);

			if(nIndex == nImageCount)
				m_ctrl_imagelist.InsertItem(nIndex, text, nIndex);
		}

		m_imageList.Replace(nIndex, pBitmap, NULL);

		lvitem.iItem = nIndex;
		lvitem.mask = LVIF_IMAGE;
		lvitem.iImage = nIndex;

		m_ctrl_imagelist.SetItem(&lvitem);
		m_ctrl_imagelist.SetItemText(nIndex, 0, imageNum);

		m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
		m_ctrl_imagelist.SetItemState(curSelect + 1, LVIS_SELECTED, LVIS_SELECTED);
	}

	m_btn_scan.EnableWindow(TRUE);
	m_btn_inscan.EnableWindow(TRUE);

	m_ctrl_imagelist.Invalidate();
}

void CTWainAppDlg::OnSelchangeListImage() 
{
	// TODO: Add your control notification handler code here
	int currentIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(currentIndex == 0)
		m_btn_moveup.EnableWindow(FALSE);
	else
		m_btn_moveup.EnableWindow(TRUE);

	if(currentIndex == m_imageList.GetImageCount() - 1)
		m_btn_movedown.EnableWindow(FALSE);
	else
		m_btn_movedown.EnableWindow(TRUE);
}

void CTWainAppDlg::OnDblclkListImage() 
{
	// TODO: Add your control notification handler code here
	//��ȡ��������Ӧ��ͼƬ����
	int currentIndex = 0;//m_listbox_image.GetCurSel();
	HANDLE _hDib = m_pic_handles[currentIndex];

	//Create PreView Window
	if(!bPreviewing)
		CreatePreViewFrame(AfxGetInstanceHandle());

	//��Ԥ�����淢����Ϣ
	WPARAM wParam = (WPARAM)_hDib;
	::SendMessage(hWnd, USER_XFERDONE, wParam, 0);
}

void CTWainAppDlg::OnDeltaposSpinRotation(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pNMUpDown->iDelta == -1)	//��
	{
		CString curAngle = "0";
		m_edit_rotation.GetWindowText(curAngle);
		int nCurAngle = _ttoi(curAngle);

		if(nCurAngle == -359)
			return;

		curAngle.Format(("%d"), nCurAngle - 1);
		m_edit_rotation.SetWindowText(curAngle);
	}
	else
	{
		CString curAngle = "0";
		m_edit_rotation.GetWindowText(curAngle);
		int nCurAngle = _ttoi(curAngle);
		
		if(nCurAngle == 359)
			return;
		
		curAngle.Format(("%d"), nCurAngle + 1);
		m_edit_rotation.SetWindowText(curAngle);
	}

	*pResult = 0;
}

void CTWainAppDlg::OnChangeEditRotation() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString curAngle = "0";
	m_edit_rotation.GetWindowText(curAngle);
	int nCurAngle = _ttoi(curAngle);

	if(nCurAngle > 359)
	{
		curAngle.Format(("%d"), 359);
		m_edit_rotation.SetWindowText(curAngle);
	}
	if(nCurAngle < -359)
	{
		curAngle.Format(("%d"), -359);
		m_edit_rotation.SetWindowText(curAngle);
	}
}

void CTWainAppDlg::OnUpdateEditRotation() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CString curAngle = "0";
	m_edit_rotation.GetWindowText(curAngle);
	int loc = m_edit_rotation.GetSel();

	for(int i = 0; i < curAngle.GetLength(); i++)
	{
		TCHAR ch = curAngle.GetAt(i);
		if(ch > 57 || ch < 48)
		{
			if(ch != 45)
			{
				curAngle.Delete(i);
				m_edit_rotation.SetWindowText(curAngle);
				m_edit_rotation.SetSel(loc);

				break;
			}
		}
	}
}

// void CTWainAppDlg::GetProportionSize(int nSrcWidth, int nSrcHeight, int nMaxWidth, int nMaxHeight, int* nNewWidth, int* nNewHeight)
// {
// 	int w = nSrcWidth;
// 	int h = nSrcHeight;
// 	
// 	if(w > nMaxWidth)
// 	{
// 		*nNewWidth = nMaxWidth;
// 		*nNewHeight = h * nMaxWidth / w;
// 	}
// 	else
// 	{
// 		*nNewWidth = w;
// 		*nNewHeight = h;
// 	}
// 	
// 	w = *nNewWidth;
// 	h = *nNewHeight;
// 	
// 	if(h > nMaxHeight)
// 	{
// 		*nNewWidth = w * nMaxHeight / h;
// 		*nNewHeight = nMaxHeight;
// 	}
// }

void CTWainAppDlg::OnItemchangedListImage(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	if(pNMListView->uNewState == 0)
	{
		*pResult = 0;
		return;
	}

	//��õ�ǰѡ��ͼƬ������ֵ
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(nSelectedIndex == -1)
	{
		*pResult = 0;
		return;
	}

	m_static_image.SetBitmap(NULL);

	CRect lRect;
	m_static_image.GetClientRect(&lRect);
	m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));

	HANDLE image = m_pic_handles[nSelectedIndex];

	/*
	CxImage cxImage;
	cxImage.CreateFromHANDLE(image);

	int height = cxImage.GetHeight();
	int width = cxImage.GetWidth();

	int nNewWidth;
	int nNewHeight;
	GetProportionSize(width, height, lRect.Width(), lRect.Height(), &nNewWidth, &nNewHeight);
	
	cxImage.Resample(nNewWidth, nNewHeight);

	HBITMAP bmp = cxImage.MakeBitmap(m_static_image.GetDC()->m_hDC);
	m_static_image.SetBitmap(bmp);
	*/

	CxImage cxImage;
	cxImage.CreateFromHANDLE(image);
	
	int height = cxImage.GetHeight();
	int width = cxImage.GetWidth();
	
	int nNewWidth;
	int nNewHeight;
	CTWainHelper::GetProportionSize(width, height, lRect.Width(), lRect.Height(), &nNewWidth, &nNewHeight);

	//ͼ���������
	int x = (lRect.Width() - nNewWidth) / 2;
	int y = (lRect.Height() - nNewHeight) / 2;
	
	HBITMAP hBitmap = NULL;
	hBitmap = cxImage.MakeBitmap();
	
	// ʹ��GDI+������ͼ�δ�������д���
	// Bitmap����Ҫר���ͷ��ڴ�
	Bitmap bmp(hBitmap, NULL);
	Bitmap Thumbnail(lRect.Width(), lRect.Height());
	
	if (hBitmap)
	{
		DeleteObject(hBitmap);
	}
	
	//��ɫ������ˢ
	SolidBrush whiteBrush(Color::White);
	
	//��������ͼ�Ĵ����ͻ���
	Graphics* pGraphics = Graphics::FromImage(&Thumbnail);
	pGraphics->FillRectangle(&whiteBrush, 0, 0, lRect.Width(), lRect.Height());
	
	pGraphics->DrawImage(&bmp, Rect(x, y, nNewWidth, nNewHeight));
	delete pGraphics;
	
	//���CBitmap���󣬴Ӷ���CImageList����������ͼ
	HBITMAP hBmp;
	Thumbnail.GetHBITMAP(RGB(255,255,255), &hBmp);

	m_static_image.SetBitmap(hBmp);

	*pResult = 0;
}

HBRUSH CTWainAppDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if((pWnd->GetDlgCtrlID() == IDC_STATIC_IMAGE) && (nCtlColor == CTLCOLOR_STATIC))
	{
		if(!m_initPictureCtrl)
		{
			m_static_image.SetBitmap(NULL);
			
			CRect lRect;
			m_static_image.GetClientRect(&lRect);
			m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));
			
			m_initPictureCtrl = TRUE;
		}
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CTWainAppDlg::UpdateImageListCtrl()
{	
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(nSelectedIndex == -1)
	{
		return;
	}

	HANDLE imageHandle = m_pic_handles[nSelectedIndex];

	CxImage image;
	image.CreateFromHANDLE(imageHandle);

	int height = image.GetHeight();
	int width = image.GetWidth();

	int nNewWidth;
	int nNewHeight;
	CTWainHelper::GetProportionSize(width, height, 100, 100, &nNewWidth, &nNewHeight);

	int x = (100 - nNewWidth) / 2;
	int y = (100 - nNewHeight) / 2;
	
	HBITMAP hBitmap = NULL;
	hBitmap = image.MakeBitmap();
	
	Bitmap bmp(hBitmap, NULL);
	Bitmap Thumbnail(100, 100);

	if(hBitmap)
	{
		DeleteObject(hBitmap);
	}
	
	SolidBrush whiteBrush(Color::White);
	
	Graphics* pGraphics = Graphics::FromImage(&Thumbnail);
	pGraphics->FillRectangle(&whiteBrush, 0, 0, 100, 100);
	
	pGraphics->DrawImage(&bmp, Rect(x, y, nNewWidth, nNewHeight));
	delete pGraphics;
	
	HBITMAP hBmp;
	Thumbnail.GetHBITMAP(RGB(255,255,255), &hBmp);
	CBitmap* pBitmap = CBitmap::FromHandle(hBmp);

	m_imageList.Replace(nSelectedIndex, pBitmap, NULL);

	if(hBmp)
	{
		DeleteObject(hBmp);
	}

	m_ctrl_imagelist.Invalidate();
}

void CTWainAppDlg::OnButtonMoveup()
{
	// TODO: Add your control notification handler code here
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(nSelectedIndex == -1)
	{
		MessageBox("δѡ��ҳ��", MESSAGEBOX_TITLE_ERROR);
		return;
	}

	if(nSelectedIndex == 0)
	{
		MessageBox("�޷�����ҳ�������Ʋ�����", MESSAGEBOX_TITLE_ERROR);
		return;
	}

	//////////////////////////////////////////////////////////////////

	// CImageList �任
	IMAGEINFO CurImageInfo;
	m_imageList.GetImageInfo(nSelectedIndex, &CurImageInfo);
	CBitmap* pCurBitmap = CBitmap::FromHandle(CurImageInfo.hbmImage);
	
	IMAGEINFO UpperImageInfo;
	m_imageList.GetImageInfo(nSelectedIndex - 1, &UpperImageInfo);
	CBitmap* pUpperBitmap = CBitmap::FromHandle(UpperImageInfo.hbmImage);
	
	m_imageList.Replace(nSelectedIndex - 1, pCurBitmap, NULL);
	m_imageList.Replace(nSelectedIndex, pUpperBitmap, NULL);

	// CListCtrl ���Ʊ任
	CString Cur_text = m_ctrl_imagelist.GetItemText(nSelectedIndex, 0);
	CString Upper_text = m_ctrl_imagelist.GetItemText(nSelectedIndex - 1, 0);

	m_ctrl_imagelist.SetItemText(nSelectedIndex, 0, Upper_text);
	m_ctrl_imagelist.SetItemText(nSelectedIndex - 1, 0, Cur_text);

	//////////////////////////////////////////////////////////////////

	//��������
	HANDLE temp = m_pic_handles[nSelectedIndex - 1];
	m_pic_handles[nSelectedIndex - 1] = m_pic_handles[nSelectedIndex];
	m_pic_handles[nSelectedIndex] = temp;

	UpdateImageListCtrl();

	m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
	m_ctrl_imagelist.SetItemState(nSelectedIndex - 1, LVIS_SELECTED, LVIS_SELECTED);
}

void CTWainAppDlg::OnButtonMovedown() 
{
	// TODO: Add your control notification handler code here
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(nSelectedIndex == -1)
	{
		MessageBox("δѡ��ҳ��", MESSAGEBOX_TITLE_ERROR);
		return;
	}

	if(nSelectedIndex == m_ctrl_imagelist.GetItemCount() - 1)
	{
		MessageBox("�޷���βҳ�������Ʋ�����", MESSAGEBOX_TITLE_ERROR);
		return;
	}
	
	//////////////////////////////////////////////////////////////////
	IMAGEINFO CurImageInfo;
	m_imageList.GetImageInfo(nSelectedIndex, &CurImageInfo);
	CBitmap* pCurBitmap = CBitmap::FromHandle(CurImageInfo.hbmImage);
	
	IMAGEINFO LowerImageInfo;
	m_imageList.GetImageInfo(nSelectedIndex + 1, &LowerImageInfo);
	CBitmap* pLowerBitmap = CBitmap::FromHandle(LowerImageInfo.hbmImage);
	
	m_imageList.Replace(nSelectedIndex + 1, pCurBitmap, NULL);
	m_imageList.Replace(nSelectedIndex, pLowerBitmap, NULL);

	// CListCtrl ���Ʊ任
	CString Cur_text = m_ctrl_imagelist.GetItemText(nSelectedIndex, 0);
	CString Lower_text = m_ctrl_imagelist.GetItemText(nSelectedIndex + 1, 0);
	
	m_ctrl_imagelist.SetItemText(nSelectedIndex, 0, Lower_text);
	m_ctrl_imagelist.SetItemText(nSelectedIndex + 1, 0, Cur_text);
	
	//��������
	HANDLE temp = m_pic_handles[nSelectedIndex + 1];
	m_pic_handles[nSelectedIndex + 1] = m_pic_handles[nSelectedIndex];
	m_pic_handles[nSelectedIndex] = temp;
	
	UpdateImageListCtrl();

	m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
	m_ctrl_imagelist.SetItemState(nSelectedIndex + 1, LVIS_SELECTED, LVIS_SELECTED);
}

void CTWainAppDlg::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(nSelectedIndex == -1)
	{
		MessageBox("δѡ��ҳ��", MESSAGEBOX_TITLE_ERROR);
		return;
	}

	int nCount = m_pic_handles.size();

	//////////////////////////////////////////////////////////////////////

	int nImageCount = m_imageList.GetImageCount();

	LVITEM lvitem;
	for(int nIndex = 0; nIndex < nImageCount - 1; nIndex++)
	{
		if(nIndex < nSelectedIndex)
			continue;

		m_imageList.Copy(nIndex, nIndex + 1);

		lvitem.iItem = nIndex;
		lvitem.mask = LVIF_IMAGE;
		m_ctrl_imagelist.GetItem(&lvitem);
		lvitem.iImage = nIndex;
		m_ctrl_imagelist.SetItem(&lvitem);
		
		CString text = m_ctrl_imagelist.GetItemText(nIndex + 1, 0);
		m_ctrl_imagelist.SetItemText(nIndex, 0, text);
	}
	m_imageList.SetImageCount(nImageCount - 1);
	m_ctrl_imagelist.DeleteItem(nImageCount - 1);
	
	////////////////////////////////////////////////////////////////////////////////

	int num = 0;
	std::vector< HANDLE >::iterator it;
	for(it = m_pic_handles.begin(); it != m_pic_handles.end(); it++)
	{
		if(num < nSelectedIndex)
		{
			num++;
			continue;
		}
		
		if(num < nCount - 1)
			m_pic_handles[num] = m_pic_handles[num + 1];
		
		if(num == nCount - 1)
		{
			m_pic_handles.erase(it);
			//GlobalFree(*it);
			break;
		}
		
		num++;
	}

	if(m_pic_handles.size() == 0)
	{
		CRect lRect;
		m_static_image.GetClientRect(&lRect);
		m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));
	}
	else
	{
		if(nSelectedIndex > 0)
		{
			m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
			m_ctrl_imagelist.SetItemState(nSelectedIndex - 1, LVIS_SELECTED, LVIS_SELECTED);
		}
		else
		{
			m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
			m_ctrl_imagelist.SetItemState(nSelectedIndex, LVIS_SELECTED, LVIS_SELECTED);	
		}
	}

	m_ctrl_imagelist.Invalidate();
}

void CTWainAppDlg::OnButtonLeft90() 
{
	// TODO: Add your control notification handler code here
	m_static_image.SetBitmap(NULL);
	
	CRect lRect;
	m_static_image.GetClientRect(&lRect);
	m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));
	
	//��õ�ǰѡ��ͼƬ������
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	
	if(nSelectedIndex == -1)
	{
		return;
	}
	
	HANDLE image = m_pic_handles[nSelectedIndex];
	
	CxImage cxImage;
	cxImage.CreateFromHANDLE(image);
	
	cxImage.RotateRight();
	
	image = cxImage.CopyToHandle();
	UpdateImageAt(nSelectedIndex, image);
	
	///////////////////////////////////////////////////////////////////////
	
	int height = cxImage.GetHeight();
	int width = cxImage.GetWidth();
	
	int litter = height / lRect.Height();
	int newwidth = width * lRect.Height() / height;
	
	cxImage.Resample(newwidth, lRect.Height());
	
	HBITMAP bmp = cxImage.MakeBitmap(m_static_image.GetDC()->m_hDC);
	
	m_static_image.SetBitmap(bmp);
	
	UpdateImageListCtrl();
}

void CTWainAppDlg::OnButtonRight90() 
{
	// TODO: Add your control notification handler code here
	m_static_image.SetBitmap(NULL);
	
	CRect lRect;
	m_static_image.GetClientRect(&lRect);
	m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));
	
	//��õ�ǰѡ��ͼƬ������
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(nSelectedIndex == -1)
	{
		return;
	}
	
	HANDLE image = m_pic_handles[nSelectedIndex];
	
	CxImage cxImage;
	cxImage.CreateFromHANDLE(image);
	
	cxImage.RotateLeft();
	
	image = cxImage.CopyToHandle();
	UpdateImageAt(nSelectedIndex, image);
	
	///////////////////////////////////////////////////////////////////////
	
	int height = cxImage.GetHeight();
	int width = cxImage.GetWidth();
	
	int litter = height / lRect.Height();
	int newwidth = width * lRect.Height() / height;
	
	cxImage.Resample(newwidth, lRect.Height());
	
	HBITMAP bmp = cxImage.MakeBitmap(m_static_image.GetDC()->m_hDC);
	
	m_static_image.SetBitmap(bmp);
	
	UpdateImageListCtrl();
}

void CTWainAppDlg::OnButtonJp() 
{
	// TODO: Add your control notification handler code here
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(nSelectedIndex == -1)
	{
		MessageBox("δѡ��ҳ��", MESSAGEBOX_TITLE_ERROR);
		return;
	}

//	HANDLE image = m_pic_handles[nSelectedIndex];

	m_previous_rotatevalue = 0;

	CRotateDialog rotateDialog(this);
	rotateDialog.SetSelectedIndex(nSelectedIndex);
	rotateDialog.DoModal();

/*	�Զ���ƫ

	CxImage cxImage;
	cxImage.CreateFromHANDLE(image);

	//��ʱ�ļ�
	CString input = m_strExePath + "\\temp\\JP_TEMP.bmp";
	CString output = m_strExePath + "\\temp\\JP.bmp";
	
	string inputPath = input.GetBuffer(0);
	input.ReleaseBuffer();
	string outputPath = output.GetBuffer(0);
	output.ReleaseBuffer();

	cxImage.Save(input, CXIMAGE_FORMAT_BMP);

	int retValue = 0;
	OpenCVImageHandle::cv_roteImgSelf(inputPath, outputPath, retValue);

	if(retValue == -1)
	{
		return;
	}

	m_static_image.SetBitmap(NULL);
	
	CRect lRect;
	m_static_image.GetClientRect(&lRect);
	m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));

	CxImage newCxImage;
	newCxImage.Load(output, CXIMAGE_FORMAT_BMP);
	
	image = newCxImage.CopyToHandle();
	UpdateImageAt(nSelectedIndex, image);
	
	/////////////////////////////////////////////////////////////////////////
	int height = cxImage.GetHeight();
	int width = cxImage.GetWidth();
	
	int nNewWidth;
	int nNewHeight;
	GetProportionSize(width, height, lRect.Width(), lRect.Height(), &nNewWidth, &nNewHeight);

// 	int litter = height / lRect.Height();
// 	int newwidth = width * lRect.Height() / height;
	
	cxImage.Resample(nNewWidth, nNewHeight);
	
	HBITMAP bmp = cxImage.MakeBitmap(m_static_image.GetDC()->m_hDC);
	
	m_static_image.SetBitmap(bmp);
	
	if(IDYES != AfxMessageBox("�Ƿ�Ӧ�ø��ģ�", MB_YESNO))
	{
		UpdateImageAt(nSelectedIndex, image);
		
		height = cxImage.GetHeight();
		width = cxImage.GetWidth();
		
		int nNewWidth;
		int nNewHeight;
		GetProportionSize(width, height, lRect.Width(), lRect.Height(), &nNewWidth, &nNewHeight);

		cxImage.Resample(nNewWidth, nNewHeight);
		
		HBITMAP oldbmp = cxImage.MakeBitmap(m_static_image.GetDC()->m_hDC);
		m_static_image.SetBitmap(oldbmp);
	}

	UpdateImageListCtrl();
*/
}

void CTWainAppDlg::RotatePicture(int nImageIndex, int nRotateValue)
{
	HANDLE image = m_pic_handles[nImageIndex];

	// ʵ����ת�Ƕ�
	int ntempValue = nRotateValue;
	nRotateValue = nRotateValue - m_previous_rotatevalue;

	// ��תָ���Ƕ�
//	if(m_pTempCxImage == NULL)
//		m_pTempCxImage = new CxImage();
//	m_pTempCxImage->CreateFromHANDLE(image);
//	m_pTempCxImage->Rotate(nRotateValue);

	CxImage TempCxImage;
	TempCxImage.CreateFromHANDLE(image);
	TempCxImage.Rotate(nRotateValue);

	/////////////////////////////////////////////////////////////////////////
	//m_static_image.SetBitmap(NULL);
	
	HANDLE newImage = TempCxImage.CopyToHandle();
	UpdateImageAt(nImageIndex, newImage);
	
	m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
	m_ctrl_imagelist.SetItemState(nImageIndex, LVIS_SELECTED, LVIS_SELECTED);

	/////////////////////////////////////////////////////////////////////////
/*	CRect lRect;
	m_static_image.GetClientRect(&lRect);
	m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));

	int width = m_pTempCxImage->GetWidth();
	int height = m_pTempCxImage->GetHeight();
	
	int nNewWidth;
	int nNewHeight;
	GetProportionSize(width, height, lRect.Width(), lRect.Height(), &nNewWidth, &nNewHeight);
	
	CxImage tempCxImage(*m_pTempCxImage);
	tempCxImage.Resample(nNewWidth, nNewHeight);
	HBITMAP bmp = tempCxImage.MakeBitmap(m_static_image.GetDC()->m_hDC);

	m_static_image.SetBitmap(bmp);
	if(bmp)
	{
		DeleteObject(bmp);
	}
*/	
	UpdateImageListCtrl();

	m_previous_rotatevalue = ntempValue;
}

void CTWainAppDlg::IsSaveRotate(int nImageIndex, CxImage cxImage)
{
	HANDLE CurrImage = m_pic_handles[nImageIndex];
	
	if(IDYES != AfxMessageBox("�Ƿ�Ӧ�ø��ģ�", MB_YESNO))
	{
		HANDLE image = cxImage.CopyToHandle();
		UpdateImageAt(nImageIndex, image);
		
		m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
		m_ctrl_imagelist.SetItemState(nImageIndex, LVIS_SELECTED, LVIS_SELECTED);

		////////////////////////////////////////////////////////////////
/*		CRect lRect;
		m_static_image.GetClientRect(&lRect);
		m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));

		int height = cxImage.GetHeight();
		int width = cxImage.GetWidth();
		
		int nNewWidth;
		int nNewHeight;
		GetProportionSize(width, height, lRect.Width(), lRect.Height(), &nNewWidth, &nNewHeight);
		
		cxImage.Resample(nNewWidth, nNewHeight);
		
		HBITMAP bmp = cxImage.MakeBitmap(m_static_image.GetDC()->m_hDC);
		m_static_image.SetBitmap(bmp);

		if(bmp)
		{
			DeleteObject(bmp);
		}
*/
	}
	else
	{
		//HANDLE newImage = m_pTempCxImage->CopyToHandle();
		UpdateImageAt(nImageIndex, CurrImage);

		m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
		m_ctrl_imagelist.SetItemState(nImageIndex, LVIS_SELECTED, LVIS_SELECTED);

		//delete m_pTempCxImage;
		//m_pTempCxImage = NULL;
	}

	UpdateImageListCtrl();
}

void CTWainAppDlg::OnButtonQhb() 
{
	// TODO: Add your control notification handler code here
	m_static_image.SetBitmap(NULL);
	
	// ͼ����ʾ�ؼ��ı�׼��С
	CRect lRect;
	m_static_image.GetClientRect(&lRect);
	m_static_image.GetDC()->FillSolidRect(lRect.left, lRect.top, lRect.Width(), lRect.Height(), RGB(255,255,255));
	
	int nSelectedIndex = m_ctrl_imagelist.GetNextItem(-1, LVNI_SELECTED);
	if(nSelectedIndex == -1)
	{
		MessageBox("δѡ��ҳ��", MESSAGEBOX_TITLE_ERROR);
		return;
	}
	
	HANDLE image = m_pic_handles[nSelectedIndex];
	
	// ʹ��CxImage�������ͼ����
	CxImage cxImage;
	cxImage.CreateFromHANDLE(image);
	
	//��ʱ�ļ�
	CString input = m_strExePath + "\\temp\\QHB_TEMP.bmp";
	CString output = m_strExePath + "\\temp\\QHB.bmp";
	
	string inputPath = input.GetBuffer(0);
	input.ReleaseBuffer();
	string outputPath = output.GetBuffer(0);
	output.ReleaseBuffer();
	
	cxImage.Save(input, CXIMAGE_FORMAT_BMP);
	
	HANDLE hWaitThread = CreateThread(NULL, 0, WaitProc, NULL, 0, NULL);

	// ȥ�ڱߺ���
	OpenCVImageHandle::cv_RemoveBorder(inputPath, outputPath);

	TerminateThread(hWaitThread, 0);
	CloseHandle(hWaitThread);
	
	CxImage newCxImage;
	newCxImage.Load(output, CXIMAGE_FORMAT_BMP);
	HANDLE newImage = newCxImage.CopyToHandle();
	UpdateImageAt(nSelectedIndex, newImage);
	
	m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
	m_ctrl_imagelist.SetItemState(nSelectedIndex, LVIS_SELECTED, LVIS_SELECTED);

	/////////////////////////////////////////////////////////////////////////
	// ����ͼ�񣬴Ӷ��ܹ���Picture�ؼ����ʵ���ʾ����
//	int width = newCxImage.GetWidth();
//	int height = newCxImage.GetHeight();
	
//	int nNewWidth;
//	int nNewHeight;
//	GetProportionSize(width, height, lRect.Width(), lRect.Height(), &nNewWidth, &nNewHeight);
	
	// ������ʱCxImage������Ӧ�ø��ĺ󱣴�ͼ������
	//CxImage tempImage(newCxImage);
	
//	newCxImage.Resample(nNewWidth, nNewHeight);
//	HBITMAP bmp = newCxImage.MakeBitmap(m_static_image.GetDC()->m_hDC);
	
//	m_static_image.SetBitmap(bmp);
	
//	if(bmp)
//	{
//		DeleteObject(bmp);
//	}
	
	// ���Ч�������룬Ӧ����������
	if(IDYES != AfxMessageBox("�Ƿ�Ӧ�ø��ģ�", MB_YESNO))
	{
		UpdateImageAt(nSelectedIndex, image);
		
		m_ctrl_imagelist.SetItemState(-1, 0, LVIS_SELECTED);
		m_ctrl_imagelist.SetItemState(nSelectedIndex, LVIS_SELECTED, LVIS_SELECTED);

//		height = cxImage.GetHeight();
//		width = cxImage.GetWidth();
		
//		int nNewWidth;
//		int nNewHeight;
//		GetProportionSize(width, height, lRect.Width(), lRect.Height(), &nNewWidth, &nNewHeight);
		
//		cxImage.Resample(nNewWidth, nNewHeight);
		
//		HBITMAP oldbmp = cxImage.MakeBitmap(m_static_image.GetDC()->m_hDC);
//		m_static_image.SetBitmap(oldbmp);
		
//		if(oldbmp)
//		{
//			DeleteObject(bmp);
//		}
	}
	else
	{
		//CloseHandle(image);
	}
	
	UpdateImageListCtrl();
}

void CTWainAppDlg::OnButtonQb() 
{
	// TODO: Add your control notification handler code here
	int i = 0;
	while(i < m_pic_handles.size())
	{
		HANDLE image = m_pic_handles[i];
		
		CxImage cxImage;
		cxImage.CreateFromHANDLE(image);
		
		//��ʱ�ļ�
		CString input = m_strExePath + "\\temp\\WHITE_TEMP.bmp";
		CString output = m_strExePath + "\\temp\\WHITE.bmp";
		
		string inputPath = input.GetBuffer(0);
		input.ReleaseBuffer();
		string outputPath = output.GetBuffer(0);
		output.ReleaseBuffer();
		
		cxImage.Save(input, CXIMAGE_FORMAT_BMP);

		if(FALSE == OpenCVImageHandle::cv_isAllWhite(inputPath))
		{
			i++;
			continue;
		}
		
		int nSelectedIndex = i;
		int nCount = m_pic_handles.size();	
		int nImageCount = m_imageList.GetImageCount();
		
		LVITEM lvitem;
		for(int nIndex = 0; nIndex < nImageCount - 1; nIndex++)
		{
			if(nIndex < nSelectedIndex)
				continue;
			
			m_imageList.Copy(nIndex, nIndex + 1);
			
			lvitem.iItem = nIndex;
			lvitem.mask = LVIF_IMAGE;
			m_ctrl_imagelist.GetItem(&lvitem);
			lvitem.iImage = nIndex;
			m_ctrl_imagelist.SetItem(&lvitem);
			
			CString text = m_ctrl_imagelist.GetItemText(nIndex + 1, 0);
			m_ctrl_imagelist.SetItemText(nIndex, 0, text);
		}
		m_imageList.SetImageCount(nImageCount - 1);
		m_ctrl_imagelist.DeleteItem(nImageCount - 1);
		
		////////////////////////////////////////////////////////////////////////////////
		
		int num = 0;
		std::vector< HANDLE >::iterator it;
		for(it = m_pic_handles.begin(); it != m_pic_handles.end(); it++)
		{
			if(num < nSelectedIndex)
			{
				num++;
				continue;
			}
			
			if(num < nCount - 1)
				m_pic_handles[num] = m_pic_handles[num + 1];
			
			if(num == nCount - 1)
			{
				m_pic_handles.erase(it);
				//GlobalFree(*it);
				break;
			}
			
			num++;
		}
		
		m_ctrl_imagelist.Invalidate();
	}
	
}