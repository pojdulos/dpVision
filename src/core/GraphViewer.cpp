#include "GraphViewer.h"

#include <QGraphicsScene>

#include "GraphNode.h"
#include "GraphEdge.h"
#include "Object.h"

GraphViewer::GraphViewer(CBaseObject* root) : QGraphicsView(), m_obj(root) {
	m_scene = new QGraphicsScene(0, 0, 2400, 1200);
	GraphNode* n = new GraphNode((qreal)10.0, (qreal)10.0, m_obj);

	m_scene->addItem(n);
	layoutTree(n, (CObject*)m_obj, 0, 50, m_scene);

	// Na koñcu, po dodaniu wszystkich itemów i ustawieniu pozycji:
	double margines = 40.0;
	QRectF bounding = m_scene->itemsBoundingRect();
	bounding = bounding.marginsAdded(QMarginsF(margines, margines, margines, margines));
	m_scene->setSceneRect(bounding);

	this->setScene(m_scene);
	this->setAttribute(Qt::WA_DeleteOnClose); // automatycznie siê usunie po zamkniêciu
	this->resize(600, 400);
}

void GraphViewer::sceneAddKid(QGraphicsScene* scene, GraphNode* parentnode, CObject* kid)
{
	GraphNode* n = new GraphNode(10, 10, kid->getLabel());
	scene->addItem(n);
	scene->addItem(new GraphEdge(parentnode, n));

	for (auto k : kid->children()) {
		if (k.second->hasCategory(CBaseObject::Category::OBJECT))
			sceneAddKid(scene, n, (CObject*)k.second);
	}
}

int GraphViewer::layoutTree(GraphNode* node, CObject* obj, int level, int x, QGraphicsScene* scene, int deltaX, int deltaY)
{
	int myX = x;
	int nextX = x;

	auto kids = obj->children();
	std::vector<GraphNode*> kidNodes;
	for (auto& k : kids) {
		if (k.second->hasCategory(CBaseObject::Category::OBJECT)) {
			GraphNode* kidNode = new GraphNode(0, 0, k.second);

			scene->addItem(kidNode);
			scene->addItem(new GraphEdge(node, kidNode));
			kidNodes.push_back(kidNode);

			// Rekurencyjnie ustaw pozycjê dla dziecka (nextX przesuwa siê po ka¿dym poddrzewie)
			nextX = layoutTree(kidNode, (CObject*)k.second, level + 1, nextX, scene, deltaX, deltaY);
		}
	}

	// Jeœli s¹ dzieci, ustaw siebie po œrodku zakresu dzieci:
	if (!kidNodes.empty()) {
		int minX = kidNodes.front()->pos().x();
		int maxX = kidNodes.back()->pos().x();
		myX = (minX + maxX) / 2;
	}

	node->setPos(myX, level * deltaY + 20);
	return nextX + deltaX; // Zwraca miejsce na nastêpne poddrzewo w osi X
}
