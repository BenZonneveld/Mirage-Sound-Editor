#ifndef MYMDICHILDWND_H
#define MYMDICHILDWND_H

class CMyMDIChildWnd:  public CMDIChildWnd
{
private:
	using CMDIChildWnd::Create;

public:
	CMyMDIChildWnd();
	BOOL Create(LPCTSTR szTitle, LONG style = 0,
			const RECT& rect = rectDefault,
			CMDIFrameWnd* pParent = NULL);
// Attributes
public:
//	virtual BOOL DestroyWindow();
// Operations
public:

// Overrides

protected:
	static CMenu	NEAR	menu;

protected:
	DECLARE_MESSAGE_MAP()
};

class CMyWnd : public CWnd
{
private:
	using CWnd::Create;

	DECLARE_DYNAMIC(CMyWnd);

public:
	CMyWnd();
	BOOL Create(LPCTSTR szTitle, LONG style, const RECT& rect, CWnd* pParent);

protected:

	DECLARE_MESSAGE_MAP()
};

#endif /* MYMDICHILDWND_H */