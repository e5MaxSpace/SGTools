#ifndef COLORTEMPLATEDIALOG_H
#define COLORTEMPLATEDIALOG_H

#include "colortemplate_global.h"

#include <QDialog>

#include "colordata.h"
#include "valuedistributioncounter.h"

namespace Ui {
class ColorTemplateDialog;
}

class COLORTEMPLATE_EXPORT ColorTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorTemplateDialog(QWidget *parent = 0);
    ~ColorTemplateDialog();

    void setData(double minValue, double maxValue, const ColorData &colorData, const QVector<float> &values);

signals:
    void colorDataChanged(const ColorData &colorData);

private slots:
    void slotCurrentColorNameChanged(const QString &colorName);

    void slotNewTemplate();
    void slotDelTemplate();
    void slotSaveTemplate();
    void slotSaveAsTemplate();
    void slotReverseColorMark();

    void slotApplyThresholdValue();
    void slotRestoreThresholdValue();

    void slotApply();

private:
    Ui::ColorTemplateDialog *ui;

    double mOrgMinValue;
    double mOrgMaxValue;
    ValueDistributionCounter mValueDistributionCounter;

    void updateColorNameComboBox(const QString &currentColorName = QString::null);
};

#endif // COLORTEMPLATEDIALOG_H
