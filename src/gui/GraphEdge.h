#pragma once
#include <QGraphicsLineItem>

class GraphNode; // deklaracja wstêpna

class GraphEdge : public QGraphicsLineItem {
public:
    GraphEdge(GraphNode* a, GraphNode* b);
    void updatePosition();
private:
    GraphNode* m_a;
    GraphNode* m_b;
};

