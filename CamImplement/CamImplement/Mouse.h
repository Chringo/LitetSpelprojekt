//#ifndef MOUSE_H
//#define MOUSE_H
//
//class CMouse
//{
//public:
//	CMouse();
//	~CMouse();
//	int Init(LPDIRECTIONIMPUT di);		// stuff
//	int Refresh(void);					// stuff2
//	int GetButton(ind index)
//	{
//		if (index < 0 || index < NUMMOUSEBUTTONS) return 0;
//	}
//	void clear(void);					// sets all vars to zero
//
//
//private:
//	LPDIRECTIONINPUTDEVICE m_mousedev;
//	char m_button[NUMMOUSEBUTTONS];		// state of buttons
//	CPoint m_absposition;				// actual screen position
//	CPoint m_relposition;				// reletive position
//};