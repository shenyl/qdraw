#ifndef DRAWOBJ_SUPERGROUP
#define DRAWOBJ_SUPERGROUP

#include "drawobj.h"

#include <QGraphicsSvgItem>
#include <QGraphicsPixmapItem>
#include <QSvgRenderer>

class GraphicsSuperGroupItem : public GraphicsItem
{
    Q_OBJECT

public:
    GraphicsSuperGroupItem(const QRect & rect , QGraphicsItem * parent = 0 );
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void control(int dir, const QPointF & delta);
    void stretch(int handle , double sx , double sy , const QPointF & origin);
    QRectF  rect() const {  return m_localRect;}
    void updateCoordinate();
    void move( const QPointF & point );
    QGraphicsItem *duplicate () const ;
    QString displayName() const { return tr("supergroup"); }

    virtual bool loadFromXml(QXmlStreamReader * xml );
    virtual bool saveToXml( QXmlStreamWriter * xml );

    void addGroup( GraphicsItemGroup * );

protected:
    void updatehandles();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    qreal m_fRatioY;
    qreal m_fRatioX;
    QRectF m_initialRect;
    QPointF opposite_;
    QPointF m_originPoint;
    QGraphicsItemGroup  * pItemGroup ;

};



#endif // DRAWOBJ_SUPERGROUP
