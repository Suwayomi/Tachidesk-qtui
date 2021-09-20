#pragma once

#include <QQuickItem>
#include <QLineF>
#include <QDebug>

/**
 * @brief The PinchZoomItem class
 * @author ARC
 */
class PinchZoomItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem* zoomTarget MEMBER mTarget)
    Q_PROPERTY(qreal wheelFactor    MEMBER mWheelFactor)
    Q_PROPERTY(qreal zoomFactor     MEMBER mFactor)
    Q_PROPERTY(qreal zoomMax        MEMBER mMax)
    Q_PROPERTY(qreal zoomMin        MEMBER mMin)
public:
    PinchZoomItem(QQuickItem *parent = 0) :
        QQuickItem(parent),
        mTarget(Q_NULLPTR),
        mWheelFactor(0.1),
        mCurrentFactor(1),
        mFactor(1),
        mMax(-1),
        mMin(0.5)
    {
        connect(this, &QQuickItem::widthChanged,    this, &PinchZoomItem::setDefaultMax);
        connect(this, &QQuickItem::heightChanged,   this, &PinchZoomItem::setDefaultMax);
    }

signals:

public slots:

protected:
    void touchEvent(QTouchEvent *touchEvent) Q_DECL_OVERRIDE {
        switch (touchEvent->type()) {
        case QTouchEvent::TouchBegin:
        case QTouchEvent::TouchUpdate:
        case QTouchEvent::TouchEnd: {
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            if(touchPoints.count() == 2) {
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                qreal s = QLineF(touchPoint0.pos(), touchPoint1.pos()).length() / \
                          QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
                if(touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                    mCurrentFactor = zoom(mCurrentFactor * s);
                } else {
                    zoom(mCurrentFactor * s);
                }
            }
        }
        default:
            break;
        }
    }

    void wheelEvent(QWheelEvent* wheelEvent) Q_DECL_OVERRIDE{
        mCurrentFactor = zoom(mCurrentFactor + (wheelEvent->angleDelta().y() > 0? 1:-1) * mWheelFactor);
    }

    void componentComplete() Q_DECL_OVERRIDE {
        QQuickItem::componentComplete();
        if(mTarget == Q_NULLPTR && !childItems().isEmpty())
            mTarget = childItems().first();
    }
private slots:
    void setDefaultMax(){
        if(mMax == -1){
            mMax = mTarget->parentItem()->width() / mTarget->width();
            if(mMax < mMin)
                qWarning()<<metaObject()->className()<<"Zoom min have to be smaller than max.";
        }
    }

private:
    inline qreal zoom(const qreal& f){
        if(mTarget != Q_NULLPTR && mMin < f && f < mMax){
            mTarget->setScale(f * mFactor);
            return f;
        } else {
            return mCurrentFactor;
        }
    }

    QQuickItem* mTarget;
    qreal       mWheelFactor;
    qreal       mCurrentFactor;
    qreal       mFactor;
    qreal       mMax;
    qreal       mMin;
};

