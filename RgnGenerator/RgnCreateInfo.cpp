#include "StdAfx.h"
#include "RgnCreateInfo.h"

CRgnCreateInfo::CRgnCreateInfo(void)
{
	m_clrColor = RGB(255, 0, 255);
	CreateFolder();
}

CRgnCreateInfo::~CRgnCreateInfo(void)
{
}

void CRgnCreateInfo::CreateFolder()
{
	CString strPath;
	strPath.Format("%s\\PNG", GetModulePath());
	int nResult = _access(strPath, 0);
	if (nResult == -1)
		::CreateDirectory(strPath, NULL);
	
	strPath.Format("%s\\BMP", GetModulePath());
	nResult = _access(strPath, 0);
	if (nResult == -1)
		::CreateDirectory(strPath, NULL);
	
	strPath.Format("%s\\JPG", GetModulePath());
	nResult = _access(strPath, 0);
	if (nResult == -1)
		::CreateDirectory(strPath, NULL);
	
	strPath.Format("%s\\GIF", GetModulePath());
	nResult = _access(strPath, 0);
	if (nResult == -1)
		::CreateDirectory(strPath, NULL);
	
	strPath.Format("%s\\RGN", GetModulePath());
	nResult = _access(strPath, 0);
	if (nResult == -1)
		::CreateDirectory(strPath, NULL);
}

CString CRgnCreateInfo::GetModulePath()
{
	char cTemp[MAX_PATH];
	char *spzRealDirEnd;
	CString strModulePath;

	GetModuleFileName(NULL, cTemp, MAX_PATH);  
	spzRealDirEnd = strrchr(cTemp, '\\');
	*spzRealDirEnd = '\0';
	strModulePath = (CString)cTemp;
	return strModulePath;
}

CString CRgnCreateInfo::GetFileName(CString strFilename)
{	
	int nFindExt = 0;
	int i = 0;
	for (i = 0; i < strFilename.GetLength(); )
	{
		nFindExt = strFilename.Find('\\', nFindExt);
		if (nFindExt == -1) break;
		nFindExt++;
		i = nFindExt;
	}

	if (i > 0)
		return strFilename.Mid(i, strFilename.GetLength());
	else
		return strFilename;
}


int CRgnCreateInfo::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
	UINT num = 0;
	UINT size = 0;
	ImageCodecInfo * pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}

CStringW CSA2CSW(const CStringA &strA)
{
	if (strA.IsEmpty())
		return L"";

	const char *p = (LPCSTR)strA;
	int outlen = MultiByteToWideChar(CP_ACP, NULL, p, -1, NULL, 0);

	wchar_t *buf = new wchar_t[outlen];
	wmemset(buf, NULL, outlen);

	MultiByteToWideChar(CP_ACP, NULL, p, strA.GetLength(), buf, outlen);
	CStringW strW = buf;
	delete [] buf;

	return strW;
}

HRGN CRgnCreateInfo::CreateRgnFromBitmap(Bitmap* pBitmap, int nXPos, int nYPos, int nImgWidth, int nImgHeight, COLORREF cfTransparent)
{
	RECT rcMemDC = { 0, 0, nImgWidth, nImgHeight };

	HDC hDC = GetDC(GetDesktopWindow());
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, nImgWidth, nImgHeight);
	HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);

	HBRUSH hBrush = CreateSolidBrush(cfTransparent);
	FillRect(hMemDC, &rcMemDC, hBrush);
	DeleteObject(hBrush);

	SetStretchBltMode(hMemDC, HALFTONE);
	Graphics graphics(hMemDC);

	RectF gRect;
	gRect.X = (REAL)0;
	gRect.Y = (REAL)0;
	gRect.Width = (REAL)nImgWidth;
	gRect.Height = (REAL)nImgHeight;

	graphics.DrawImage(pBitmap, gRect, (REAL)nXPos, (REAL)nYPos, (REAL)nImgWidth, (REAL)nImgHeight, UnitPixel);

	int i = 0;
	int j = 0;
	POINT ptStart = { -1, -1 };
	POINT ptEnd = { -1, -1 };
	COLORREF color = 0;
	HRGN hImgRgn = CreateRectRgn(0, 0, nImgWidth, nImgHeight);
	for (i = 0; i < nImgWidth; i++)
	{
		for (j = 0; j < nImgHeight; j++)
		{
			color = GetPixel(hMemDC, i, j);
			if (color == cfTransparent)
			{
				if (ptStart.x == -1)
				{
					ptStart.x = i;
					ptStart.y = j;
				}
			}
			else
			{
				if (ptStart.x != -1)
				{
					ptEnd.x = i + 1;
					ptEnd.y = j;
					HRGN hTransparentRgn = CreateRectRgn(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
					CombineRgn(hImgRgn, hImgRgn, hTransparentRgn, RGN_DIFF);

					DeleteObject(hTransparentRgn);
					ptStart.x = -1;
				}
			}
		}
		if (ptStart.x != -1)
		{
			ptEnd.x = i + 1;
			ptEnd.y = j;
			HRGN hTransparentRgn = CreateRectRgn(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
			CombineRgn(hImgRgn, hImgRgn, hTransparentRgn, RGN_DIFF);

			DeleteObject(hTransparentRgn);
			ptStart.x = -1;
		}
	}

	SelectObject(hMemDC, hOldMemBmp);
	DeleteObject(hMemBmp);
	DeleteDC(hMemDC);
	::ReleaseDC(GetDesktopWindow(), hDC);

	return hImgRgn;
}

void CRgnCreateInfo::CreateRgn(CString strPath, CRgn *pRgn)
{	
	CFile f;
	CFileException e;
	HGLOBAL hmem;
	RGNDATA *pRd;

	int nSize;	
	if (!f.Open(strPath, CFile::modeCreate | CFile::modeWrite, &e))
	{
		return;
	}
	
	nSize = pRgn->GetRegionData(NULL, 0);
	ASSERT(nSize);

	if ((hmem = GlobalAlloc(GHND, nSize)) == NULL)
	{
		return;
	}

	if ((pRd = (RGNDATA *)GlobalLock(hmem)) == NULL)
	{
		return;
	}

	pRgn->GetRegionData(pRd, nSize);

	f.Write(pRd, nSize);

	GlobalUnlock(hmem);
	GlobalFree(hmem);
	f.Close();
}

bool CRgnCreateInfo::SaveToFileImage(CString strPath, int nImage)
{
	Bitmap *pBitmap;

#ifdef _UNICODE
	pBitmap = Bitmap::FromFile(strPath, TRUE);
#else
	pBitmap = Bitmap::FromFile(CSA2CSW(strPath), TRUE);
#endif
	
	CString strSavedPath;
	CString strFileName = GetFileName(strPath);
	CString strOnlyName = strFileName.Left(strFileName.ReverseFind('.'));

	if(nImage == 1)
		strSavedPath.Format("%s\\PNG\\%s.png", GetModulePath(), strOnlyName); 
	else if(nImage == 2)
		strSavedPath.Format("%s\\BMP\\%s.bmp", GetModulePath(), strOnlyName); 
    else if(nImage == 3)
		strSavedPath.Format("%s\\JPG\\%s.jpg", GetModulePath(), strOnlyName); 
    else if(nImage == 4)
		strSavedPath.Format("%s\\GIF\\%s.gif", GetModulePath(), strOnlyName); 	
	
	int nSizeCount = MultiByteToWideChar(CP_ACP, 0, strSavedPath, -1, NULL, 0);
	WCHAR * pWString = new WCHAR[nSizeCount];
	MultiByteToWideChar(CP_ACP, 0, strSavedPath, -1, pWString, nSizeCount);

	CLSID encoderClsid;

	if(nImage == 1)
		GetEncoderClsid(L"image/png", &encoderClsid);
	else if(nImage == 2)
		GetEncoderClsid(L"image/bmp", &encoderClsid);
    else if(nImage == 3)
		GetEncoderClsid(L"image/jpeg", &encoderClsid);
    else if(nImage == 4)
		GetEncoderClsid(L"image/gif", &encoderClsid);
	
	return pBitmap->Save(pWString, &encoderClsid, NULL) == Ok;
}

void CRgnCreateInfo::CreateImageRgn(CString strPath)
{	
	Bitmap *pBitmap;

#ifdef _UNICODE
	pBitmap = Bitmap::FromFile(strPath, TRUE);
#else
	pBitmap = Bitmap::FromFile(CSA2CSW(strPath), TRUE);
#endif
	
	CString strSavedPath;
	CString strFileName = GetFileName(strPath);
	CString strOnlyName = strFileName.Left(strFileName.ReverseFind('.'));

	strSavedPath.Format("%s\\RGN\\%s.rgn", GetModulePath(), strOnlyName); 

	if (pBitmap)
	{
		HRGN hRgn = CreateRgnFromBitmap(pBitmap, 0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), m_clrColor);
		CRgn *pRgn = CRgn::FromHandle(hRgn);
		CreateRgn(strSavedPath, pRgn);
	}
	delete pBitmap;
	pBitmap = NULL;	
}

void CRgnCreateInfo::FindImageInfo(CString strPath, CString &strImgPath, CString &strRgnPath)
{	
	CString strFileName = GetFileName(strPath);
	CString strOnlyName = strFileName.Left(strFileName.ReverseFind('.'));

	CString strGif;
	strGif.Format("%s\\GIF\\%s.gif", GetModulePath(), strOnlyName); 	

	CString strRgn;
	strRgn.Format("%s\\RGN\\%s.rgn", GetModulePath(), strOnlyName); 

	strImgPath = strGif;
	strRgnPath = strRgn;
}

void CRgnCreateInfo::SetRemoveColorRGB(int nRed, int nGreen, int nBlue)
{
	m_clrColor = RGB(nRed, nGreen, nBlue);
}