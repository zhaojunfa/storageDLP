// ����MFC������ʱ��ؼ����˿ؼ����������Ѽ������ṩ

#pragma once
#include "DuiEdit.h"

class CDlgBase;
class CMFCDateTime;

enum DATETIME_VALUES
{
	TIME_NULL,		//<<<δ����
	YESTODAY,		//<<<����
	NOW,			//<<<����
	TOMORROW		//<<<����
};

class CMFCMonthCalCtrl : public CMonthCalCtrl
{
public:
	CMFCMonthCalCtrl(CDuiObject* pParentObj, UINT msg, UINT id);
	virtual ~CMFCMonthCalCtrl();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnExLButtonDbClk(UINT id, long x, long y);
	BOOL SendMessageToParent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
protected:
	bool m_DbClick;					// ���˫��
	UINT m_nMsgClick;				// ѡ��ʱ����¼�
	CDuiObject* m_pParentObj;		// ���ؼ�
	UINT m_ControlID;				// ���ؼ�ID
};

// ʱ��ѡ��ؼ�
class CMFCDateTime : public CDuiEdit
{
	DUIOBJ_DECLARE_CLASS_NAME(CMFCDateTime, _T("mfcdatetime"))
public:
	CMFCDateTime(HWND hWnd, CDuiObject* pDuiObject);
	CMFCDateTime(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle = TEXT(""), BOOL bPassWord = FALSE, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual ~CMFCDateTime();

	SYSTEMTIME& GetTime();
	void SetTime(SYSTEMTIME* pst);

	virtual BOOL OnInit();
	virtual void SetControlRect(CRect rc);
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlTitle(CString strTitle);
	virtual BOOL SetControlFocus(BOOL bFocus);

	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HRESULT OnAttributeDateTimeValue(const CString& strValue, BOOL bLoading);

	BOOL OnMonthCalCtrlSelect(UINT nFlags, CPoint point);

protected:
	HFONT GetDateTimeFont();
	
	void ShowCMonthCalCtrl();
	void HideCMonthCalCtrl();
	void DeleteCMonthCalCtrl();

	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);

protected:
	SYSTEMTIME m_sysTime;
	int        m_nDTUpdateFlag;
	DATETIME_VALUES m_DateTime;		//<<<ʱ��
	CMFCMonthCalCtrl* m_pMouthCalCtrl;
	CRect  m_rcMounth;				// ����������ʱ�䴰�ڵ�λ��
	int  m_MouthCalCtrlHeight;		// ����ʱ�䴰�ڵĸ߶�
	int  m_MouthCalCtrlWidth;		// ����ʱ�䴰�ڵĿ���
	BOOL m_IsShowMouthCalCtrl;		// �Ƿ�������ʾ����ʱ�䵯������
	BOOL m_isVailidRect;			// ��������Ƿ�������ʱ�䴰�ڷ�Χ��
	BOOL m_isDefaultToday;			// �Ƿ�Ĭ��Ϊ��ǰʱ��

protected:
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_ENUM_ATTRIBUTE(_T("value"), DATETIME_VALUES, TRUE)
			DUI_ENUM_VALUE(_T("yestoday"), YESTODAY)
			DUI_ENUM_VALUE(_T("now")	 , NOW)
			DUI_ENUM_VALUE(_T("tomorrow"), TOMORROW)
		DUI_ENUM_END(m_DateTime)
		DUI_CUSTOM_ATTRIBUTE(_T("datetime"), OnAttributeDateTimeValue)
	DUI_DECLARE_ATTRIBUTES_END()
};