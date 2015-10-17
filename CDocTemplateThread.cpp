#include "stdafx.h"
#include "afxwin.h"

#include "CDocTemplateThread.h"

IMPLEMENT_DYNCREATE(CDocTemplateThread, CWinThread)

CDocTemplateThread::CDocTemplateThread()
{
}

CDocTemplateThread::~CDocTemplateThread()
{

}

void CDocTemplateThread::SetMDIClass(CRuntimeClass *pMainFrame, CDocTemplate *pDocTemplate)
{
	m_pMainFrame = pMainFrame;
	m_pDocTemplate = pDocTemplate;
}

void CDocTemplateThread::SetMDIClass(CRuntimeClass *pMainFrame, CMultiDocTemplate *pMultiDocTemplate)
{
	m_pMainFrame = pMainFrame;
	m_pMultiDocTemplate = pMultiDocTemplate;
}
