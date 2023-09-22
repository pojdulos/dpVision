#pragma once
#include <QtWidgets/QMdiSubwindow>
#include <QtWidgets/QMdiArea>

class CImage;

class __declspec(dllexport) MdiChild : public QWidget
{
	Q_OBJECT

public:
	typedef enum {
		GL,
		Pic
	} Type;

	typedef enum {
		Minimized = 0,
		Normal = 1,
		Maximized = 2
	} Show;


	inline Type type() { return m_type; }
	inline bool hasType(Type t) { return t == m_type; }

	static QMdiSubWindow* create(Type t, QMdiArea* mdiArea, Show show = Normal);
	static QMdiSubWindow* create(CImage *im, QMdiArea* mdiArea);

	void updateView(bool buffered = true);
	//static void closeAllPic(int id);

	void fullScreen();

	void keyPressEvent(QKeyEvent* e) override;

	void resizeEvent(QResizeEvent* e) override;

	bool m_itWasMaximized;
	QWidget* m_widget;

protected:
	explicit MdiChild(Type t);
	explicit MdiChild(CImage* im);

	void mouseMoveEvent(QMouseEvent* event);

private:
	Type m_type;
};

