#pragma once

class CPreViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreViewDlg)

public:
	CPreViewDlg(CWnd* pParent = NULL);  
	virtual ~CPreViewDlg();

	enum { IDD = IDD_DIALOG_PREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	void SetImageInfo(CString strImagePath, CString strRgnPath);

private:
	void DrawSkin(CDC* pDC);
	BOOL LoadSkin();
	void LoadRgnData();
	CGdiPlusBitmapResource* m_pBackSkin;
	
	CString m_strImagePath;
	CString m_strRgnPath;
};
