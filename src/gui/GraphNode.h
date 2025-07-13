#pragma once
#include <QGraphicsTextItem>
#include <vector>
#include <QString>

class CBaseObject;
class GraphEdge;

class GraphNode : public QGraphicsTextItem {
public:
    enum class ShapeType { Ellipse, Rectangle, Diamond, Triangle };
    GraphNode(qreal x, qreal y, const QString& label, ShapeType shape = ShapeType::Ellipse);
    GraphNode(qreal x, qreal y, CBaseObject* obj);

    void setObject(CBaseObject* obj);;
    CBaseObject *getObject() { return m_obj; };

    void addEdge(GraphEdge* e);
    QRectF boundingRect() const override;
    QPainterPath shapePath() const;

    QPointF in_pos() const;
    QPointF out_pos() const;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void keyPressEvent(QKeyEvent* event) override;
    QPainterPath shape() const override;
    QColor getShapeColor(GraphNode::ShapeType shape);
    std::pair<GraphNode::ShapeType, QColor> obj2shape2(CBaseObject* obj);
    GraphNode::ShapeType obj2shape(CBaseObject* obj);
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    std::vector<GraphEdge*> m_edges;
    ShapeType m_shape;
    QColor m_color;

    qreal m_margin = 16.0; // margines na t³o
    CBaseObject* m_obj;
};
