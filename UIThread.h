#include "resource.h"       // main symbols
#include "ProgressBar.h"

class CUIThread : public CWinThread
{
	DECLARE_DYNCREATE(CUIThread)
public:
	void SetParent(CWnd *pParent);
	void SetProgressText(LPCTSTR ProgressText);
	void SetProgressRange(unsigned int Range);
	CUIThread();
	virtual ~CUIThread();
	BOOL IsRunning();
	int Run();
	void Kill();

private:
	CWnd *m_pParent;
	BOOL m_bKill;
	BOOL m_bRunning;
	LPCTSTR m_Text;
	CProgressDialog m_Dlg;
	unsigned int m_Range;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUIThread)
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

protected:

	// Generated message map functions
	//{{AFX_MSG(CUIThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
