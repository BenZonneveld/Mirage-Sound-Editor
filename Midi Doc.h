#pragma once

// CMidiDoc document
#include <deque>

using std::deque;
using std::string;

class CMidiDoc : public CDocument
{
protected:
	CMidiDoc();
	DECLARE_DYNCREATE(CMidiDoc)

public:
//	CMidiDoc();
	virtual ~CMidiDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void OnUpdateAllViews();
	void PutData(string InData,  BOOL IO_Dir);
	string GetData(int line);
	BOOL GetIO(int line);
	int GetSize();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();
	DECLARE_MESSAGE_MAP()

private:
	deque <string> m_MidiData;
	deque <BOOL> m_MidiIO;
};
