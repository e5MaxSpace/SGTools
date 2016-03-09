#include "colortemplatedialog.h"
#include "ui_colortemplatedialog.h"

#include "colormanager.h"

#include <QInputDialog>
#include <QMessageBox>

ColorTemplateDialog::ColorTemplateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorTemplateDialog)
{
    ui->setupUi(this);

    connect(ui->widget, SIGNAL(colorDataChanged(ColorData)), SIGNAL(colorDataChanged(ColorData)));

    QStringList colorNames = ColorManager::getInstance().getColorNames();
    ui->comboTemplates->addItems(colorNames);
    connect(ui->comboTemplates, SIGNAL(currentIndexChanged(QString)), SLOT(slotCurrentColorNameChanged(QString)));

    connect(ui->newBtn, SIGNAL(clicked()), SLOT(slotNewTemplate()));
    connect(ui->delBtn, SIGNAL(clicked()), SLOT(slotDelTemplate()));
    connect(ui->saveBtn, SIGNAL(clicked()), SLOT(slotSaveTemplate()));
    connect(ui->saveAsBtn, SIGNAL(clicked()), SLOT(slotSaveAsTemplate()));
    connect(ui->reverseBtn, SIGNAL(clicked()), this, SLOT(slotReverseColorMark()));

    connect(ui->applyThresholdBtn, SIGNAL(clicked()), SLOT(slotApplyThresholdValue()));
    connect(ui->restoreThresholdBtn, SIGNAL(clicked()), SLOT(slotRestoreThresholdValue()));

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), SLOT(slotApply()));
}

ColorTemplateDialog::~ColorTemplateDialog()
{
    delete ui;
}

void ColorTemplateDialog::setData(double minValue, double maxValue, const ColorData &colorData, const QVector<float> &values)
{
    mOrgMinValue = minValue;
    mOrgMaxValue = maxValue;

    ui->minValueSpinBox->setValue(mOrgMinValue);
    ui->maxValueSpinBox->setValue(mOrgMaxValue);

    mValueDistributionCounter.setMinValue(mOrgMinValue);
    mValueDistributionCounter.setMaxValue(mOrgMaxValue);
    mValueDistributionCounter.setValues(values);

    ui->widget->setXInterval(QwtInterval(minValue, maxValue));
    ui->widget->setCurrentColor(colorData);
    ui->widget->setDistribution(mValueDistributionCounter.valueDistribution());
}

void ColorTemplateDialog::slotCurrentColorNameChanged(const QString &colorName)
{
    if (colorName.isEmpty())
        return;

    ColorData aColorData = ColorManager::getInstance().getColorData(colorName);
    ui->widget->setCurrentColor(aColorData);
    emit colorDataChanged(aColorData);
}

void ColorTemplateDialog::slotNewTemplate()
{
    bool ok = false;
    QString colorName = QInputDialog::getText(this, tr("新建颜色模板"), tr("颜色模板名称: "), QLineEdit::Normal, QString::null, &ok);
    if (ok && !colorName.isEmpty())
    {
        if (ColorManager::getInstance().createColorData(colorName))
        {
            ui->widget->setCurrentColor(ColorManager::getInstance().getColorData(colorName));
        }
    }
}

void ColorTemplateDialog::slotDelTemplate()
{
    int result = QMessageBox::question(this, tr("提示"), tr("颜色模板被删除后将不能恢复，是否继续？"),
                                       QMessageBox::Yes | QMessageBox::Cancel,
                                       QMessageBox::Cancel);
    if (result == QMessageBox::Cancel)
    {
        return;
    }

    QString msg = tr("失败");
    QString colorName = ui->comboTemplates->currentText();
    if (!colorName.isEmpty())
    {
        if (ColorManager::getInstance().removeColorData(colorName))
        {
            msg = tr("完毕");
            updateColorNameComboBox();
        }
    }
    QMessageBox::information(this, tr("提示"), tr("删除颜色模板%1。").arg(msg), QMessageBox::Ok);
}

void ColorTemplateDialog::slotSaveTemplate()
{
    QString msg = tr("失败");
    if (ColorManager::getInstance().saveColorData(ui->widget->getCurrentColor()))
    {
        msg = tr("完毕");
    }
    QMessageBox::information(this, tr("提示"), tr("保存颜色模板%1。").arg(msg), QMessageBox::Ok);
}

void ColorTemplateDialog::slotSaveAsTemplate()
{
    bool ok = false;
    QString colorName = QInputDialog::getText(this, tr("颜色模板另存为"), tr("颜色模板名称: "), QLineEdit::Normal, QString::null, &ok);
    if (ok && !colorName.isEmpty())
    {
        if (ColorManager::getInstance().isExist(colorName))
        {
            QMessageBox::warning(this, tr("提示!"), tr("颜色模板名称%1已经存在，请重新输入！").arg(colorName));
            return;
        }

        ColorData aColorData(ui->widget->getCurrentColor());
        aColorData.setColorName(colorName);
        if (ColorManager::getInstance().saveColorData(aColorData))
        {
            updateColorNameComboBox(ui->widget->getCurrentColor().getColorName());
        }
    }
}

void ColorTemplateDialog::slotReverseColorMark()
{
    ui->widget->reverseColor();
}

void ColorTemplateDialog::slotApplyThresholdValue()
{
    mValueDistributionCounter.setMinValue(ui->minValueSpinBox->value());
    mValueDistributionCounter.setMaxValue(ui->maxValueSpinBox->value());

    ui->widget->setXInterval(QwtInterval(ui->minValueSpinBox->value(), ui->maxValueSpinBox->value()));
    ui->widget->setDistribution(mValueDistributionCounter.valueDistribution());
}

void ColorTemplateDialog::slotRestoreThresholdValue()
{
    ui->minValueSpinBox->setValue(mOrgMinValue);
    ui->maxValueSpinBox->setValue(mOrgMaxValue);

    slotApplyThresholdValue();
}

void ColorTemplateDialog::slotApply()
{
    QMessageBox::information(this, tr("提示!"), tr("暂未实现。"), QMessageBox::Ok);
}

void ColorTemplateDialog::updateColorNameComboBox(const QString &currentColorName)
{
    QStringList colorNames = ColorManager::getInstance().getColorNames();
    ui->comboTemplates->clear();
    ui->comboTemplates->addItems(colorNames);

    if (!currentColorName.isEmpty() && ui->comboTemplates->count()>0)
    {
        int index = ui->comboTemplates->findText(currentColorName);
        if (index == -1)
            index = 0;
        ui->comboTemplates->setCurrentIndex(index);
    }
}
