#include "stdafx.h"
#include "RgnGenerator.h"
#include "PreViewDlg.h"

IMPLEMENT_DYNAMIC(CPreViewDlg, CDialog)

CPreViewDlg::CPreViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPreViewDlg::IDD, pParent)
{
	m_pBackSkin = NULL;
}

CPreViewDlg::~CPreViewDlg()
{
}

void CPreViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPreViewDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL CPreViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 0);
		
	if (!LoadSkin())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	CDC* pDC = GetDC();
	if (pDC)
	{
		DrawSkin(pDC);
		ReleaseDC(pDC);
	}

	return TRUE; 
}

void CPreViewDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawSkin(&dc);
}

void CPreViewDlg::DrawSkin(CDC* pDC)
{
	if (m_pBackSkin->m_pBitmap == NULL)
		return;

	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y), 0, 0, x, y, UnitPixel);
	SetWindowPos(NULL, 0, 0, x, y, SWP_NOZORDER | SWP_NOMOVE);

	//HRGN hImgRgn = m_pBackSkin->CreateRgnFromBitmap(m_pBackSkin->m_pBitmap, 0, 0, x, y, RGB(255, 0, 255));
	//SetWindowPos(NULL, 0, 0, x, y, SWP_NOZORDER | SWP_NOMOVE);
	//SetWindowRgn(hImgRgn, FALSE);
}

BOOL CPreViewDlg::LoadSkin()
{
	LoadRgnData();

	m_pBackSkin = new CGdiPlusBitmapResource;
	if (!m_pBackSkin->LoadPath(m_strImagePath))
	{
		delete m_pBackSkin;
		m_pBackSkin = NULL;
		return FALSE;
	}

	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow(((rcWorkArea.right - cx) / 2), ((rcWorkArea.bottom - cy) / 2), cx, cy);
	
	return TRUE;
}

void CPreViewDlg::LoadRgnData()
{
	CRgn rgn;
	CFile f;
	if (!f.Open(m_strRgnPath, CFile::modeRead | CFile::shareDenyWrite))
	{
		return ;
	}
	else
	{
		DWORD fileLength = f.GetLength();

		TCHAR *szBuffer = new TCHAR[fileLength];

		UINT nActual = 0;

		f.Seek(0, CFile::begin);

		nActual = f.Read(szBuffer, fileLength);

		f.Close();
		
		if (!rgn.CreateFromData(NULL, nActual, (LPRGNDATA)szBuffer))
			return;

		delete[] szBuffer;
	}
		
	if (!::SetWindowRgn(GetSafeHwnd(), (HRGN)rgn, TRUE))
		return;
}

void CPreViewDlg::SetImageInfo(CString strImagePath, CString strRgnPath)
{
	m_strImagePath = strImagePath;
	m_strRgnPath = strRgnPath;
}
