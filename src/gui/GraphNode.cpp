#include "GraphNode.h"
#include "GraphEdge.h"

#include <QBrush>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>

#include "BaseObject.h"
#include "Workspace.h"

GraphNode::GraphNode(qreal x, qreal y, const QString& label, ShapeType shape)
    : QGraphicsTextItem(label), m_shape(shape), m_obj(nullptr)
{
    setPos(x, y);
    setZValue(10);

    m_color = getShapeColor(shape);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    // setTextInteractionFlags(Qt::TextEditorInteraction); // enables editing on double click
    setTextInteractionFlags(Qt::NoTextInteraction); // editing disabled by default

    setDefaultTextColor(Qt::black);
}

GraphNode::GraphNode(qreal x, qreal y, CBaseObject* obj) : QGraphicsTextItem(), m_obj(obj)
{
    setPos(x, y);
    setZValue(10);

    setPlainText((m_obj != nullptr) ? m_obj->getLabel() : "nullptr");

    auto sc = (m_obj != nullptr)
        ? obj2shape2(m_obj)
        : std::pair<GraphNode::ShapeType, QColor>(ShapeType::Triangle, Qt::gray);
    m_shape = sc.first;
    m_color = sc.second;

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    setTextInteractionFlags(Qt::NoTextInteraction); // editing disabled by default

    setDefaultTextColor(Qt::black);
}

void GraphNode::setObject(CBaseObject* obj)
{
    m_obj = obj;

    if (m_obj != nullptr)
    {
        setPlainText(m_obj->getLabel());
    }
}

void GraphNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    setTextInteractionFlags(Qt::TextEditorInteraction); // enable editing
    setFocus(Qt::MouseFocusReason);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void GraphNode::focusOutEvent(QFocusEvent* event)
{
    setTextInteractionFlags(Qt::NoTextInteraction); // disable editing

    if (m_obj != nullptr)
    {
        m_obj->setLabel(toPlainText());
        CWorkspace::instance()->notifyObjectStateChanged(m_obj->id());
    }

    QGraphicsTextItem::focusOutEvent(event);
}

void GraphNode::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        clearFocus(); // finish editing and trigger focusOutEvent
    }
    else if (event->key() == Qt::Key_Escape)
    {
        if (m_obj != nullptr)
        {
            setPlainText(m_obj->getLabel()); // restore previous label
        }
        clearFocus();
    }
    else
    {
        QGraphicsTextItem::keyPressEvent(event);
    }
}

void GraphNode::addEdge(GraphEdge* e)
{
    m_edges.push_back(e);
}

QRectF GraphNode::boundingRect() const
{
    QRectF textRect = QGraphicsTextItem::boundingRect();
    return textRect.marginsAdded(QMarginsF(m_margin, m_margin, m_margin, m_margin));
}

QPainterPath GraphNode::shapePath() const
{
    QRectF r = boundingRect();
    QPainterPath path;

    auto addDiamond = [&path](const QRectF& rect) {
        QPolygonF diamond;
        diamond << rect.center() + QPointF(0, -rect.height() / 2)
                << rect.center() + QPointF(rect.width() / 2, 0)
                << rect.center() + QPointF(0, rect.height() / 2)
                << rect.center() + QPointF(-rect.width() / 2, 0)
                << rect.center() + QPointF(0, -rect.height() / 2);
        path.addPolygon(diamond);
    };

    auto addTriangle = [&path](const QRectF& rect) {
        QPolygonF tri;
        tri << rect.center() + QPointF(0, -rect.height() / 2)
            << rect.center() + QPointF(rect.width() / 2, rect.height() / 2)
            << rect.center() + QPointF(-rect.width() / 2, rect.height() / 2)
            << rect.center() + QPointF(0, -rect.height() / 2);
        path.addPolygon(tri);
    };

    switch (m_shape)
    {
    case ShapeType::Ellipse:
        path.addEllipse(r);
        break;
    case ShapeType::Rectangle:
        path.addRect(r);
        break;
    case ShapeType::Diamond:
        addDiamond(r);
        break;
    case ShapeType::Triangle:
        addTriangle(r);
        break;
    }
    return path;
}

QPainterPath GraphNode::shape() const
{
    return shapePath();
}

QColor GraphNode::getShapeColor(GraphNode::ShapeType shape)
{
    switch (shape)
    {
    case ShapeType::Ellipse:
        return Qt::yellow;
    case ShapeType::Rectangle:
        return Qt::cyan;
    case ShapeType::Diamond:
        return Qt::green;
    case ShapeType::Triangle:
    default:
        return Qt::gray;
    }
}

std::pair<GraphNode::ShapeType, QColor> GraphNode::obj2shape2(CBaseObject* obj)
{
    GraphNode::ShapeType shape = GraphNode::ShapeType::Ellipse;
    QColor color = Qt::yellow;

    if (obj->hasCategory(CBaseObject::Category::OBJECT))
    {
        switch (obj->type())
        {
        case CBaseObject::Type::MODEL:
            shape = GraphNode::ShapeType::Rectangle;
            color = QColor(Qt::cyan);
            break;
        case CBaseObject::Type::MOVEMENT:
            shape = GraphNode::ShapeType::Diamond;
            color = QColor(Qt::green);
            break;
        default:
            break;
        }
    }
    else
    {
        shape = GraphNode::ShapeType::Triangle;
        color = QColor(Qt::gray);
    }

    return { shape, color };
}

GraphNode::ShapeType GraphNode::obj2shape(CBaseObject* obj)
{
    GraphNode::ShapeType shape = GraphNode::ShapeType::Ellipse;
    if (obj->hasType(CBaseObject::Type::MODEL))
    {
        shape = GraphNode::ShapeType::Rectangle;
    }
    else if (obj->hasType(CBaseObject::Type::MOVEMENT))
    {
        shape = GraphNode::ShapeType::Diamond;
    }

    return shape;
}

void GraphNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(Qt::black);

    painter->save();
    painter->setBrush(m_color);
    painter->drawPath(shapePath());
    painter->restore();

    QPointF textOffset = boundingRect().center() - QGraphicsTextItem::boundingRect().center();
    painter->save();
    painter->translate(textOffset);
    QGraphicsTextItem::paint(painter, option, widget);
    painter->restore();
}

QPointF GraphNode::in_pos() const
{
    QRectF r = boundingRect();
    return pos() + QPointF(r.width() / 2.0 - m_margin, 0.0);
}

QPointF GraphNode::out_pos() const
{
    QRectF r = boundingRect();
    return pos() + QPointF(r.width() / 2.0 - m_margin, r.height() / 2.0 - m_margin);
}

QVariant GraphNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged)
    {
        for (auto edge : m_edges)
        {
            edge->updatePosition();
        }
    }
    return QGraphicsTextItem::itemChange(change, value);
}
