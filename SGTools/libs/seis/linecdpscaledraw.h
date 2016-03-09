#ifndef LINECDPSCALEDRAW_H
#define LINECDPSCALEDRAW_H

#include <QPainter>

#include <qwt_scale_draw.h>

#include "seisprofiledata.h"

struct WellPositionInScale
{
    QString wellName;
    int line;
    int cdp;
};

class LineCdpScaleDraw : public QwtScaleDraw
{
public:
    explicit LineCdpScaleDraw();

    explicit LineCdpScaleDraw(const SeisProfileData &seisProfileData,
                              const QList<WellPositionInScale> &wellPositions);

    void setSeisProfileData(const SeisProfileData &seisProfileData);

    void setWellPosition(const QList<WellPositionInScale> &wellPositions);

    void setMousePosition(double mousePos);

    virtual void draw( QPainter *painter, const QPalette &palette ) const;

    virtual QwtText label( double value ) const;

private:
    int trace(int line, int cdp) const
    {
        return mSeisProfileData.trace(line, cdp);
    }

private:
    SeisProfileData mSeisProfileData;
    QList<WellPositionInScale> mWellPositions;
    double mMousePos;
};

#endif // LINECDPSCALEDRAW_H
