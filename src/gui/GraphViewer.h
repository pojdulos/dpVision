#pragma once
#include <QGraphicsView>

class QGraphicsScene;
class GraphNode;
class CObject;
class CBaseObject;

class GraphViewer : public QGraphicsView
{
public:
	GraphViewer(std::shared_ptr<CBaseObject> root);
	~GraphViewer();

protected:
	void sceneAddKid(QGraphicsScene* scene, GraphNode* parentnode, CObject* kid);
	int layoutTree(GraphNode* node, CObject* obj, int level, int x, QGraphicsScene* scene, int deltaX = 60, int deltaY = 80);

private:
	std::weak_ptr<CBaseObject> m_obj;
	QGraphicsScene* m_scene;
};
