#ifndef _MEMDC_H_
#define _MEMDC_H_

class CMemDC : public CDC
{
private:
	CBitmap		m_bitmap;		// Offscreen bitmap
	CBitmap*	m_oldBitmap;	// bitmap originaly found in CMemDC
	CDC*		m_pDC;			// Saves CDC passed in constructor
	CRect		m_updateRect_LP;	// Rectangle of drawing area to update in logical units
	CRect		m_clientRect_DP;	// Rectangle of drawing area in Device Units
	int			m_ZoomSize;		// Size of the Zoomed Rectangle
	BOOL		m_bMemDC;		// TRUE if CDC realy is a Memory DC
public:
	CMemDC(CDC* pDC, const CRect* pClientRect = NULL) : CDC()
	{
		ASSERT(pDC != NULL);
		ASSERT(pClientRect != NULL);

		// Some initialization
		m_pDC = pDC;
		m_oldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		// Get the client rect
		m_clientRect_DP = *pClientRect;

		// Get the area of the display that needs updating
		pDC->GetClipBox(&m_updateRect_LP);

		if (m_bMemDC)
		{
			// Create a Memory DC
			CreateCompatibleDC(pDC);

			// Create offscreen bitmap
			m_bitmap.CreateCompatibleBitmap(pDC, m_clientRect_DP.Width(),
												 m_clientRect_DP.Height());

			// Set mapmode
			SetMapMode(pDC->GetMapMode());

			// Set up the zoom
			SetWindowExt(pDC->GetWindowExt());
			SetViewportExt(pDC->GetViewportExt());

			// Store pointer to the bitmap
			m_oldBitmap = SelectObject(&m_bitmap);

			// Set the same viewport origin so they are aligned
			CPoint viewportOrg = pDC->GetViewportOrg();
			SetViewportOrg(viewportOrg);

			// Set the same window origin so the are aligned
			CPoint windowOrg = pDC->GetWindowOrg();
			SetWindowOrg(windowOrg);

			// Inflate to avoid rounding errors
			m_updateRect_LP.InflateRect(1,1);

			// Reset the clip region for the memore DC
			SelectClipRgn(NULL);

			// Update Clip region
			IntersectClipRect(&m_updateRect_LP);
		} else {
			// Make a copy of the relevant part of the current
			// DC for printing
			m_bPrinting	= pDC->m_bPrinting;
			m_hDC		= pDC->m_hDC;
			m_hAttribDC	= pDC->m_hAttribDC;
		}

		// Fill background
		FillSolidRect(m_updateRect_LP, pDC->GetBkColor());
	}

	~CMemDC()
	{
		if (m_bMemDC)
		{
			// Copy the offscreen bitmap onto the screen
			m_pDC->BitBlt(	m_updateRect_LP.left,
							m_updateRect_LP.top,
							m_updateRect_LP.Width(),
							m_updateRect_LP.Height(),
							this,
							m_updateRect_LP.left,
							m_updateRect_LP.top,
							SRCCOPY);

			// Swap back the original bitmap.
			SelectObject(m_oldBitmap);
		} else {
			// All we need to do is replace the DC with an illegal
			// value, this keeps us from accidentally deleting the
			// handles associated with the CDC that was passed to
			// the constructor.
			m_hDC = m_hAttribDC = NULL;
		}
	}

	// Allow usage as a pointer
	CMemDC*	operator->()
	{
		return this;
	}

	// Allow usage as a pointer
	operator CMemDC*()
	{
		return this;
	}
};

#endif