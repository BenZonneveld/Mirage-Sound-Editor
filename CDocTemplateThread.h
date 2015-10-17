#pragma once

#ifndef CDOCTEMPLATETHREAD_H
#define CDOCTEMPLATETHREAD_H

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

class CDocTemplateThread : public CWinThread
{
protected:
	CDocTemplateThread();
	DECLARE_DYNCREATE(CDocTemplateThread)
private:
	CRuntimeClass *m_pMainFrame;
	CDocTemplate	*m_pDocTemplate;
	CMultiDocTemplate *m_pMultiDocTemplate;

public:
	void SetMDIClass(CRuntimeClass *pMainFrame, CDocTemplate *pDocTemplate);
	void SetMDIClass(CRuntimeClass *pMainFrame, CMultiDocTemplate *pMultiDocTemplate);
	virtual ~CDocTemplateThread();
};

#endif