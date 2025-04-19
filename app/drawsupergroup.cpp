#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QCursor>
#include <QDebug>
#include <QGraphicsEffect>
#include <QMatrix4x4>
#include <QGraphicsTransform>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <cmath>


#include "drawsupergroup.h"

GraphicsSuperGroupItem::GraphicsSuperGroupItem(const QRect & rect , QGraphicsItem *parent)
    :GraphicsItem(parent)
    ,m_fRatioX(1/10.0)
    ,m_fRatioY(1/3.0)
{
    m_width = rect.width();
    m_height = rect.height();
    m_initialRect = rect;
    m_localRect = m_initialRect;
    m_localRect = rect;
    m_originPoint = QPointF(0,0);

    pItemGroup = new  QGraphicsItemGroup( this  );

    updatehandles();
}

//调整图形更新的面积
QRectF GraphicsSuperGroupItem::boundingRect() const
{
    QRectF rect ;
    rect = m_localRect;

    rect.translate( -pen().width(), -pen().width() );
    rect.setWidth(rect.width() + 2*pen().width() );
    rect.setHeight(rect.height() + 2*pen().width() );
    return rect ;

}

QPainterPath GraphicsSuperGroupItem::shape() const
{
    QPainterPath path;
    path.addRect(rect());

    return path;
}

void GraphicsSuperGroupItem::control(int dir, const QPointF & delta)
{
    prepareGeometryChange();
    updatehandles();
}

void GraphicsSuperGroupItem::stretch(int handle , double sx, double sy, const QPointF & origin)
{
//    qDebug( ) << "stretch origin :"  << origin <<  handle <<  sx  <<  sy ;
    switch (handle) {
        case Right:
        case Left:
            sy = 1;
            break;
        case Top:
        case Bottom:
            sx = 1;
            break;
        default:
            break;
        }

    //
        foreach (QGraphicsItem *item , pItemGroup->childItems()) {
             AbstractShape * ab = qgraphicsitem_cast<AbstractShape*>(item);
             if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab)){
                 ab->stretch(handle,sx,sy,ab->mapFromParent(origin));
             }
        }

        opposite_ = origin;

        QTransform trans  ;
        trans.translate(origin.x(),origin.y());
        trans.scale(sx,sy);
        trans.translate(-origin.x(),-origin.y());

        prepareGeometryChange();
        m_localRect = trans.mapRect(m_initialRect);
        m_width = m_localRect.width();
        m_height = m_localRect.height();
        updatehandles();

//    qDebug(  ) << "svg size: " <<  svgRect <<  "ratiosvg: " <<  ratiosvg ;
//    qDebug(  ) << "rect: " <<  m_localRect  ;

}

void GraphicsSuperGroupItem::updateCoordinate()
{
//    qDebug( ) << "updatecoordinate: " << m_localRect ;

    QPointF pt1,pt2,delta;
//    if (m_localRect.isNull() )
//        m_localRect = rect();

    pt1 = mapToScene(transformOriginPoint());
    pt2 = mapToScene(m_localRect.center());
    delta = pt1 - pt2;
    m_initialRect = m_localRect;
    m_width = m_localRect.width();
    m_height = m_localRect.height();
    setTransform(transform().translate(delta.x(),delta.y()));
    setTransformOriginPoint(m_localRect.center());
    moveBy(-delta.x(),-delta.y());

    foreach (QGraphicsItem *item , pItemGroup->childItems()) {
         AbstractShape * ab = qgraphicsitem_cast<AbstractShape*>(item);
         if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab)){
             ab->updateCoordinate();
             qDebug(  )<< "updateCoordinate: " <<  ab->displayName();
         }
    }
    updatehandles();

}

/////////////
/// \brief GraphicsSuperGroupItem::move
/// \param point
///     QPointF pt1,pt2,delta;
//    if (m_localRect.isNull() )
//        m_localRect = rect();


///
///
///
/// ///
///

void GraphicsSuperGroupItem::move(const QPointF &point)
{
    moveBy(point.x(),point.y());
}

QGraphicsItem *GraphicsSuperGroupItem::duplicate() const
{
    GraphicsSuperGroupItem * item = new GraphicsSuperGroupItem( rect().toRect() );
    item->m_width = width();
    item->m_height = height();
    item->setPos(pos().x(),pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue()+0.1);
    item->m_fRatioY = m_fRatioY;
    item->m_fRatioX = m_fRatioX;
    item->updateCoordinate();
    return item;
}

bool GraphicsSuperGroupItem::loadFromXml(QXmlStreamReader * xml )
{

    return true;
}

bool GraphicsSuperGroupItem::saveToXml(QXmlStreamWriter * xml)
{

    return true;
}

void GraphicsSuperGroupItem::addGroup( GraphicsItemGroup * group )
{
    if (group) {
        pItemGroup->addToGroup( group );

        // 使用m_localRect作为父对象实际矩形（boundingRect包含笔宽边距）
        QRectF parentRect = m_localRect;  // 修改前：this->boundingRect()
        
        // 获取传入组在父坐标系中的边界矩形
        QRectF groupRect = group->mapRectToParent(group->boundingRect());
        
        // 计算缩放比例时排除零宽度/高度
        double scaleX = groupRect.width() > 0 ? parentRect.width() / groupRect.width() : 1;
        double scaleY = groupRect.height() > 0 ? parentRect.height() / groupRect.height() : 1;
        
        double scaleFactor = qMin(scaleX, scaleY);
        
        // 应用缩放变换
        QTransform transform;
        transform.scale(scaleX, scaleY);
        group->setTransform(transform, true);
        
        // 获取缩放后的组位置（在父坐标系中）
        groupRect = group->mapRectToParent(group->boundingRect());
        
        // 计算左上角对齐偏移量（修改前：直接相减坐标导致中心对齐）
        QPointF offset = parentRect.topLeft() - groupRect.topLeft();
        group->moveBy(offset.x(), offset.y());
    }


    return ;


}

void GraphicsSuperGroupItem::updatehandles()
{
    const QRectF &geom = this->boundingRect();
    GraphicsItem::updatehandles();
}

//extern qt_graphicsItem_highlightSelected

void GraphicsSuperGroupItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen( Qt::black, 1 );
    painter->setPen( pen );
    QBrush brush ;
    brush.setStyle( Qt::NoBrush );
    painter->setBrush( brush );
    painter->drawRect(rect().toRect());


    painter->save();
    // 设置文字属性
    QFont font = painter->font();
    font.setPointSize(12);  // 默认12pt字号
    painter->setFont(font);
    painter->setPen(Qt::black);  // 默认黑色字体

    // 计算文字绘制区域（矩形中心位置）
    QRectF textRect = rect();
    textRect.adjust(5, 5, -5, -5);  // 留5像素边距

    // 绘制文字（自动换行+居中）
    painter->drawText(textRect, Qt::TextWordWrap | Qt::AlignCenter, "组合对象容器");
    painter->restore();



//    if (option->state & QStyle::State_Selected)
//        qt_graphicsItem_highlightSelected(this, painter, option);
}
