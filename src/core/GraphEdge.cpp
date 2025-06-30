#include "GraphEdge.h"
#include "GraphNode.h"

#include <QDebug>

GraphEdge::GraphEdge(GraphNode* a, GraphNode* b) : m_a(a), m_b(b) {
    a->addEdge(this);
    b->addEdge(this);
    updatePosition();
}

void GraphEdge::updatePosition() {
    qInfo() << "UPDATE EDGE POS";
    setLine(QLineF(m_a->out_pos(), m_b->in_pos()));
}

