#pragma once

class CPreViewDlg;
class CRgnGeneratorDlg : public CDialog
{
public:
	CRgnGeneratorDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_RGNGENERATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	


protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

private:
	void FindimageList(CString strPath);
	std::vector<CString> m_vImage;

	CRgnCreateInfo objRgn;

	CPreViewDlg * m_ViewDlg;
	void ClosePreView();	
};
