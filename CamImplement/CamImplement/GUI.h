#ifndef GUI_H
#define GUI_H

class GUI
{
private:

public:
	GUI();
	GUI(const GUI& obj);
	~GUI();

	void Initialize();
	void Render();

	void ReleaseCOM();
};

#endif