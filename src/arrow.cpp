#include "arrow.h"
#include<iostream>
 
 DragBox::DragBox(double x, double y, QGraphicsItem *parent)
     : QGraphicsRectItem(parent),
     hoverOver(false)
 {
     setFlag(QGraphicsItem::ItemIsSelectable, false);
	 setFlag(QGraphicsItem::ItemIsMovable, true);
	 setAcceptsHoverEvents(true);
	 setZValue(1001.0);
	 double dimension = 15.0;
	 setRect(-dimension/2.0, -dimension/2.0, dimension, dimension);
	 setPos(x, y);
 }


 void DragBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
 {
	// A null event to prevent unwanted deselection
 }


 void DragBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
 {
	// A null event to prevent unwanted deselection
 }
  
 
 void DragBox::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
 {
	 Q_UNUSED(event);
	 hoverOver = true;
	 update();
 }

 
 void DragBox::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
 {
	 Q_UNUSED(event);
	 hoverOver = false;
	 update();
 }
  
 
 void DragBox::paint(QPainter *painter,
            const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
	if(!hoverOver) return;
	
	myPen.setWidthF(2.0); 	
 	myPen.setColor(Qt::black);
	painter->setPen(myPen);
 	painter->drawRect(rect());
 }


 Arrow::Arrow(double x, double y, DrawingInfo *info, QGraphicsItem *parent)
     : QGraphicsLineItem(parent),
     drawingInfo(info),
     hoverOver(false)
     {
         setFlag(QGraphicsItem::ItemIsSelectable, false);
    	 setAcceptsHoverEvents(true);
    	 setZValue(1000.0);
    	 myStartBox = new DragBox(x,y);
    	 myEndBox   = new DragBox(x,y);
     }

 
 void Arrow::updatePosition()
 {
     double angle = (line().length() == 0.0 ? 0.0 : acos(line().dx() / line().length()));
     double arrowSize = 0.1 * drawingInfo->scaleFactor();
	 setLine(QLineF(myStartBox->scenePos().x(),
			 	    myStartBox->scenePos().y(), 
			        myEndBox->scenePos().x(),
			        myEndBox->scenePos().y()));

     if (line().dy() >= 0) angle = (PI * 2) - angle;

     QPointF arrowP1 = line().p2() - QPointF(sin(angle + PI / 3.0) * arrowSize,
                                     cos(angle + PI / 3.0) * arrowSize);
     QPointF arrowP2 = line().p2() - QPointF(sin(angle + PI - PI / 3.0) * arrowSize,
                                     cos(angle + PI - PI / 3.0) * arrowSize);

     arrowHead.clear();
     arrowHead << line().p2() << arrowP1 << arrowP2;
     
     //TODO Stop the line at the butt of the arrowhead, not the tip
     if(line().length() > arrowSize){
		 setLine(QLineF(line().p1(), 
				 	    line().p2() + QPointF(-cos(angle)*arrowSize*sqrt(3.0)/2.0,
				 	                           sin(angle)*arrowSize*sqrt(3.0)/2.0)));
     }
 }
 
 
 void Arrow::paint(QPainter *painter,
            const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
	myPen.setWidthF(0.02 * drawingInfo->scaleFactor()); 	
 	myPen.setColor(Qt::black);
	painter->setPen(myPen);
 	painter->drawLine(line());
 	painter->setBrush(Qt::black);
 	myPen.setWidthF(0.001 * drawingInfo->scaleFactor());
 	painter->setPen(myPen);
 	painter->drawPolygon(arrowHead);
 }
 
 
  void Arrow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
  {
 	// A null event to prevent unwanted deselection
  }


  void Arrow::mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
 	// A null event to prevent unwanted deselection
  }
   
  
  void Arrow::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
  {
 	 Q_UNUSED(event);
 	 hoverOver = true;
 	 update();
  }

  
  void Arrow::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
  {
 	 Q_UNUSED(event);
 	 hoverOver = false;
 	 update();
  }
