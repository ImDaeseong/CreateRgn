#pragma once

class CRgnCreateInfo
{
public:
	CRgnCreateInfo(void);
	~CRgnCreateInfo(void);

	bool SaveToFileImage(CString strPath, int nImage);
	void CreateImageRgn(CString strPath);
	void FindImageInfo(CString strPath, CString &strImgPath, CString &strRgnPath);
	void SetRemoveColorRGB(int nRed, int nGreen, int nBlue);

private:
	void CreateFolder();
	CString GetModulePath();
	CString GetFileName(CString strFilename);
	int GetEncoderClsid(const WCHAR *format, CLSID *pClsid);

	HRGN CreateRgnFromBitmap(Bitmap* pBitmap, int nXPos, int nYPos, int nImgWidth, int nImgHeight, COLORREF cfTransparent);
	void CreateRgn(CString strPath, CRgn *pRgn);
	
	COLORREF m_clrColor;
};
