#include "stdafx.h"
#include "RgnGenerator.h"
#include "RgnGeneratorDlg.h"
#include "PreViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRgnGeneratorDlg::CRgnGeneratorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRgnGeneratorDlg::IDD, pParent)
{
	m_ViewDlg = NULL;
}

void CRgnGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRgnGeneratorDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, &CRgnGeneratorDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CRgnGeneratorDlg::OnBnClickedButton2)
END_MESSAGE_MAP()

BOOL CRgnGeneratorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nScreenX = GetSystemMetrics(SM_CXSCREEN);
	int nScreenY = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(nScreenX - 200, nScreenY - 400, 140, 200);

	return TRUE;  
}

void CRgnGeneratorDlg::OnPaint()
{
	CPaintDC dc(this);
}

void CRgnGeneratorDlg::FindimageList(CString strPath)
{
	CFileFind finder;
	CString strWildcard = strPath;
	strWildcard += _T("\\*.*");

	CString strFullPath;
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			strFullPath = finder.GetFilePath();
			FindimageList(strFullPath);
		}
		else
		{
			CString strFullPath = finder.GetFilePath();
			CString strFileName = finder.GetFileName();
			CString strExtention = strFileName.Mid(strFileName.ReverseFind('.') + 1);

			if (lstrcmpi(strExtention, _T("gif")) == 0 || lstrcmpi(strExtention, _T("bmp")) == 0 || lstrcmpi(strExtention, _T("png")) == 0)
				m_vImage.push_back(strFullPath);
		}
	}
	finder.Close();
}

void CRgnGeneratorDlg::OnBnClickedButton1()
{
	CString strSelectPath;
	LPMALLOC pMalloc;

	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BROWSEINFO bi;
		char pszBuffer[MAX_PATH];
		LPITEMIDLIST pidl;

		bi.hwndOwner = GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = pszBuffer;
		bi.lpszTitle = _T("Select Folder");
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;

		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if (::SHGetPathFromIDList(pidl, pszBuffer))
			{
				strSelectPath = pszBuffer;
			}

			pMalloc->Free(pidl);
		}
		pMalloc->Release();
	}

	if (strSelectPath == "")
		return;

	m_vImage.clear();
	FindimageList(strSelectPath);


	BeginWaitCursor();

	CString	strImagePath;
	
	std::vector<CString>::iterator	iterImg;
	for (iterImg = m_vImage.begin(); iterImg != m_vImage.end(); iterImg++)
	{
		strImagePath.Format("%s", *iterImg);		
		objRgn.SaveToFileImage(strImagePath, 1);
		objRgn.SaveToFileImage(strImagePath, 2);
		objRgn.SaveToFileImage(strImagePath, 3);
		objRgn.SaveToFileImage(strImagePath, 4);
		objRgn.CreateImageRgn(strImagePath);
	}
	EndWaitCursor();
	AfxMessageBox(_T("complete"), MB_OK);
}

void CRgnGeneratorDlg::OnBnClickedButton2()
{
	ClosePreView();

	CString strImagePath;
	CString strImgPath;
	CString strRgnPath;

	TCHAR szFile[MAX_PATH];
	ZeroMemory(szFile, MAX_PATH);
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	ofn.hwndOwner = this->m_hWnd;
	ofn.lpstrFilter = _T("img Files Types(*.img)\0*.gif;*.png;*.bmp;*.jpg\0\0");
	ofn.lpstrTitle = _T("Load gif File");
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	if (IDOK == GetOpenFileName(&ofn))
		strImagePath = (LPCTSTR)szFile;
	
	objRgn.FindImageInfo(strImagePath, strImgPath, strRgnPath);

	if (!m_ViewDlg)
	{
		m_ViewDlg = new CPreViewDlg();
		m_ViewDlg->SetImageInfo(strImgPath, strRgnPath);
		m_ViewDlg->Create(IDD_DIALOG_PREVIEW);
		m_ViewDlg->ShowWindow(SW_SHOWNORMAL);
		m_ViewDlg->Invalidate();
	}
}

void CRgnGeneratorDlg::ClosePreView()
{
	if (m_ViewDlg)
	{
		m_ViewDlg->DestroyWindow();
		delete m_ViewDlg;
		m_ViewDlg = NULL;
	}
}