#pragma once
#include <QGraphicsLineItem>

class GraphNode; // deklaracja wst�pna

class GraphEdge : public QGraphicsLineItem {
public:
    GraphEdge(GraphNode* a, GraphNode* b);
    void updatePosition();
private:
    GraphNode* m_a;
    GraphNode* m_b;
};

