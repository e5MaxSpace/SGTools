#ifndef PROFILEWELLCOLUMNLAYER_H
#define PROFILEWELLCOLUMNLAYER_H

#include "qiglayer.h"

class ProfileWellColumnLayer : public QIGLayer
{
    Q_OBJECT

public:
    explicit ProfileWellColumnLayer(QObject *parent = 0);

    void addWell(int pos, float startTime, float endTime);
};

#endif // PROFILEWELLCOLUMNLAYER_H
