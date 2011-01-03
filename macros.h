#define InitSlider(slider_ctrl,edit_ctrl,min,max,tick_freq,pagesize) \
	slider_ctrl.SetRange(min,max); \
	slider_ctrl.SetTicFreq(tick_freq); \
	slider_ctrl.SetPageSize(pagesize);
#define SetSlider(slider_ctrl,edit_ctrl) \
	m_str.Format(_T("%d"),(slider_ctrl.GetRangeMax()-slider_ctrl.GetPos())+slider_ctrl.GetRangeMin()); \
	edit_ctrl.SetWindowTextA(m_str);
#define SetSliderValue(slider_ctrl,edit_ctrl,value) \
	slider_ctrl.SetPos((slider_ctrl.GetRangeMax()-value)); \
	SetSlider(slider_ctrl,edit_ctrl);
#define SetSliderFromEdit(slider_ctrl,edit_ctrl) \
	edit_ctrl.GetWindowTextA(m_str); \
	slider_ctrl.SetPos(slider_ctrl.GetRangeMax()-atoi(m_str)); \
	SetSlider(slider_ctrl,edit_ctrl);