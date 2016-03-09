#ifndef SEISPROFILEITEM_H
#define SEISPROFILEITEM_H

#include <QGraphicsItem>

class inSeisProfileData;
class SeisProfileItem : public QGraphicsItem
{
public:
    enum DisplayMode {VarDensity=0, Wave, VarArea, WaveVarArea, VarDensityWave,VarDensityVarArea, VarDensityWaveVarArea};
    enum TraceFillDirection{Left=0, Right};
    enum NormalizationMode {Profile=0, Average};
    enum ColorDistributeMode{Symmetric=0, Homogeneous};
private:
    enum TraceDrawMode {Curve=1, Fill=2};

public:
    explicit SeisProfileItem(QGraphicsItem *parent = 0);
    explicit SeisProfileItem(const QList<QVector<float> > &profileData, QGraphicsItem *parent = 0);
    ~SeisProfileItem();

    void setProfileData(const QList<QVector<float> > &profileData);

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    TraceFillDirection fillDirection() const;
    void setTraceFillDirection(TraceFillDirection direction);

    NormalizationMode normalizationMode() const;
    void setNormalizationMode(NormalizationMode mode);

    ColorDistributeMode colorDistributeMode() const;
    void setColorDistributeMode(ColorDistributeMode mode);

    size_t amplitueFactor();
    void setAmplitueFactor(size_t factor);

    Qt::TransformationMode transformationMode() const;
    void setTransformationMode(Qt::TransformationMode mode);

    const QPen &pen() const;
    void setPen(const QPen &pen);

    const QBrush &fillBrush() const;
    void setFillBrush(const QBrush &brush);

    const QVector<QRgb> colorMap();
    void setColorMap(const QVector<QRgb> &vecColor);

    double getUsedMaxAmp() const;
    void getColorMinMaxAmp(double &minAmp, double &maxAmp) const;

protected:
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    void drawImage(QPainter *painter, const inSeisProfileData &traceData,
                   int begTrace, int endTrace, int begTime, int endTime) const;
    void drawTraces(QPainter *painter, const inSeisProfileData &traceData,
                    int begTrace, int endTrace, int traceStep,
                    int begTime, int endTime, int timeStep, int traceDrawMode) const;

private:    
    Q_DISABLE_COPY(SeisProfileItem)

    class SeisProfileItemPrivate;
    SeisProfileItemPrivate *m_d;
};

#endif // SEISPROFILEITEM_H
