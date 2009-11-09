#ifndef _MEMDC_H_
#define _MEMDC_H_

class CMemDC : public CDC
{
private:	
	CBitmap		m_bitmap;		// Offscreen bitmap
	CBitmap*	m_pbitmap;		// Offscreen bitmap

	CBitmap*	m_poldBitmap;	// bitmap originaly found in CMemDC
	CDC*		m_pDC;			// Saves CDC passed in constructor
	CRect		m_rect_dest;	// Rectangle of drawing area to update in logical units
	CRect		m_rect_source;
	BOOL		m_bMemDC;		// TRUE if CDC realy is a Memory DC

	HGDIOBJ		m_hOldBitmap;
	HGDIOBJ		m_hBitmap;

public:
	CMemDC(CDC* pDC, const CRect* pSourceRect = NULL, const CRect* pDestRect = NULL) : CDC()
	{
		ASSERT(pDC != NULL);
		//ASSERT(pClientRect != NULL);

		// Some initialization
		m_pDC = pDC;
		m_poldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		// Get the client rect
//		m_clientRect_DP = *pClientRect;

		// Get the rectangle to draw
		if (pDestRect == NULL )
		{
			pDC->GetClipBox(&m_rect_dest);
		} else {
			m_rect_dest = *pDestRect;
		}

		// Get the rectangle to draw
		if (pSourceRect == NULL )
		{
			pDC->GetClipBox(&m_rect_source);
		} else {
			m_rect_source = *pSourceRect;
		}

		if (m_bMemDC)
		{
			// Create a Memory DC
			CreateCompatibleDC(pDC);

			pDC->LPtoDP(&m_rect_dest);

			// Create offscreen bitmap
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect_dest.Width(),
												 m_rect_dest.Height());

			m_pbitmap = &m_bitmap;
			m_hBitmap = ((HBITMAP)m_pbitmap->GetSafeHandle() );

			m_poldBitmap = ((CBitmap*)SelectObject(&m_bitmap));

			m_hOldBitmap = ((HBITMAP)m_poldBitmap->GetSafeHandle() );

			// Set mapmode
			SetMapMode(pDC->GetMapMode());

			// Set up the zoom
			SetWindowExt(pDC->GetWindowExt());
			SetViewportExt(pDC->GetViewportExt());

			pDC->DPtoLP(&m_rect_dest);

			SetWindowOrg(m_rect_dest.left, m_rect_dest.top);

		} else {
			// Make a copy of the relevant part of the current
			// DC for printing
			m_bPrinting	= pDC->m_bPrinting;
			m_hDC		= pDC->m_hDC;
			m_hAttribDC	= pDC->m_hAttribDC;
		}

		// Fill background
		FillSolidRect(m_rect_dest, pDC->GetBkColor());
	}

	~CMemDC()
	{
		if (m_bMemDC)
		{
			m_pDC->BitBlt(m_rect_dest.left,
							m_rect_dest.top,
							m_rect_dest.Width(),
							m_rect_dest.Height(),
							this,
							/*m_rect_source.Width(),
							m_rect_source.Height(),*/
							m_rect_source.left,
							m_rect_source.top,
							SRCCOPY);

			// Copy the offscreen bitmap onto the screen
/*			m_pDC->StretchBlt(	m_rect_dest.left,
							m_rect_dest.top,
							m_rect_dest.Width(),
							m_rect_dest.Height(),
							this,
							m_rect_source.left,
							m_rect_source.top,
							m_rect_source.Width(),
							m_rect_source.Height(),
							SRCCOPY);
*/
			// Swap back the original bitmap.
			SelectObject(m_hOldBitmap);
			::DeleteObject(m_hBitmap);
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