 ////////////////////////////////////////////////////////////////
 // 1998 Microsoft Systems Journal. 
 // If this code works, it was written by Paul DiLascia.
 // If not, I don't know who wrote it.
 // 
 
 ////////////////
 // Generic worker thread object. Use it to run a task in a separate thread.
 // To use:
 //  * Derive your own class from CThreadJob and implement DoWork:
 //     - periodically check m_bAbort
 //     - call OnProgress if you want to report progress
 //  * Create an instance of your class (not on stack!)
 //  * Call Begin to start, with CWnd and msg ID for OnProgress notifications.
 //  * Call Kill to abort
 //
 class CThreadJob : public CObject {
 private:
     static UINT ThreadProc(LPVOID pObj);
     CWinThread* m_pThread;  // running thread, if any
 
 protected:
     HWND    m_hWndOwner;    // HWND, *not* CWnd* of owner window
     UINT    m_ucbMsg;       // callback message for OnProgress
     UINT    m_uErr;         // thread error code
     BOOL    m_bAbort;       // whether to abort: DoWork must check this
 
     // Call this from DoWork to report progress.
     // Meaning of WPARAM/LPARAM is up to you.
     void OnProgress(WPARAM wp=0, LPARAM lp=0);
 
     // You must implement to do the work
     virtual UINT DoWork() = 0;
 
 public:
     virtual BOOL Begin(CWnd* pWndOwner=NULL, UINT ucbMsg=0);
     virtual void Kill();
     DECLARE_DYNAMIC(CThreadJob)
 };
