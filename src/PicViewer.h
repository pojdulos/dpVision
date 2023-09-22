#pragma once

#include <QtWidgets/QWidget>

class QScrollArea;
class QVBoxLayout;

class ImageLabel;
class CImage;

class __declspec(dllexport) PicViewer : public QWidget
{
	Q_OBJECT

public:
	explicit PicViewer(QWidget* parent = 0);
	explicit PicViewer(CImage *im, QWidget* parent = 0);
	~PicViewer() {};

	void setImage(CImage* im);
	void addImage(CImage* im);
	void setImages(std::vector<CImage*> im);
	void addImages(std::vector<CImage*> im);
	void removeImages();
	void removeImage(int i);
	void reloadImage();

	int id();
private:
	std::vector<CImage*> m_images;
	int m_current;

	QScrollArea* scrollArea;
	ImageLabel* imageLabel;
	void setupUi(PicViewer* picViewer);

protected:
	virtual void resizeEvent(QResizeEvent* e) override;
};

