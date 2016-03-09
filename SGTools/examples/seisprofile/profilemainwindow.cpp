#include "profilemainwindow.h"
#include "ui_profilemainwindow.h"

#include <QTime>
#include <QDebug>
#include <QMenuBar>
#include <QToolBar>
#include <QLineEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsView>

#include "qremotefile.h"
#include "segyread.h"
#include "seishorizonlineitem.h"
#include "profileseishorizonlayer.h"
#include "profilewellcolumnlayer.h"
#include "seishorizon.h"
#include "seisvolumn.h"
#include "linecdpscaledraw.h"
#include "colormanager.h"
#include "colortemplatedialog.h"

ProfileMainWindow::ProfileMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProfileMainWindow),
    m_fileName("F:/SeiSDATAS/BB2428/seisfile.segy"),
    m_currentLine(1900)
{
    ui->setupUi(this);

    ColorManager::getInstance().setColorTemplatePath("colortables");

    QAction *action = new QAction(tr("OpenSegy"), this);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(displaySegyData()));
    this->menuBar()->addAction(action);

    QAction *addHorAction = new QAction(tr("AddHorizon"), this);
    connect(addHorAction, SIGNAL(triggered(bool)), this, SLOT(addSeisHorizon()));
    this->menuBar()->addAction(addHorAction);

    QAction *addWellAction = new QAction(tr("AddWell"), this);
    connect(addWellAction, SIGNAL(triggered(bool)), this, SLOT(addWellColumn()));
    this->menuBar()->addAction(addWellAction);

    QAction *colorAction = new QAction(tr("Color"), this);
    connect(colorAction, SIGNAL(triggered(bool)), this, SLOT(testColorTemplatePlot()));
    this->menuBar()->addAction(colorAction);

    createToolBar();

    ui->graphicsView->setColorMap(ColorManager::getInstance().getColorData("LM_Profile").generateColorValues_igeoseis());
    ui->graphicsView->viewport()->setAutoFillBackground(false);
    ui->graphicsView->scale(5, 1);

    repaintProfile();
}

ProfileMainWindow::~ProfileMainWindow()
{
    delete ui;
}

void ProfileMainWindow::createToolBar()
{
    QToolBar *m_stepSizeToolBar = this->addToolBar(tr("toolBar"));
    m_stepSizeToolBar->setFloatable(false);
    QAction *backNumAct = m_stepSizeToolBar->addAction("back");
    connect(backNumAct,SIGNAL(triggered()),this,SLOT(slotGoBackNum()));

    m_stepLineEditAct = new QLineEdit(tr("10"));
    m_stepLineEditAct->setValidator(new QIntValidator());
    m_stepLineEditAct->setFixedWidth(35);
    m_stepSizeToolBar->addWidget(m_stepLineEditAct);

    QAction *nextNumAct = m_stepSizeToolBar->addAction("next");
    connect(nextNumAct,SIGNAL(triggered()),this,SLOT(slotGoNextNum()));

    m_stepSizeToolBar->addSeparator();
    QAction *densityAct = m_stepSizeToolBar->addAction("density");
    connect(densityAct,SIGNAL(triggered()),this,SLOT(slotDensity()));

    QAction *waveAct = m_stepSizeToolBar->addAction("wave");
    connect(waveAct,SIGNAL(triggered()),this,SLOT(slotWave()));
}

void ProfileMainWindow::slotGoBackNum()
{
    QTime time;
    time.start();

    m_currentLine -= m_stepLineEditAct->text().toInt();
    repaintProfile();

    qDebug()<<"slotGoPreviousNum time="<<time.elapsed();
}

void ProfileMainWindow::slotGoNextNum()
{
    QTime time;
    time.start();

    m_currentLine += m_stepLineEditAct->text().toInt();
    repaintProfile();

    qDebug()<<"slotGoNextNum time="<<time.elapsed();
}

void ProfileMainWindow::slotDensity()
{
    ui->graphicsView->varDensity();
}

void ProfileMainWindow::slotWave()
{
    ui->graphicsView->waveVarArea();
}

void ProfileMainWindow::repaintProfile()
{
    if (!QFile::exists(m_fileName))
        return;

    SeisVolumn aSeisVolumn(m_fileName);
    ui->graphicsView->setSeisProfileData(aSeisVolumn.getLine(m_currentLine));
    this->setWindowTitle(tr("数据体名称-(line=%1)").arg(m_currentLine));
}

void ProfileMainWindow::displaySegyData()
{
    QFileDialog dlg;
    m_fileName = dlg.getOpenFileName();
    if(m_fileName.isEmpty())
        return;

    repaintProfile();
}

void ProfileMainWindow::addSeisHorizon()
{
    //    QFileDialog dlg;
    //    QString fileName = dlg.getOpenFileName();
    //    if(fileName.isEmpty())
    //        return;

    //    Seis3DSurvey dataProvider;
    //    QList<float> datas = dataProvider.getSeisHorizonData(fileName).getLine(1900);
    //    QList<QPointF> line;
    //    for (int i=0; i<datas.size(); ++i)
    //    {
    //        line.append(QPointF(i, datas[i]));
    //    }

    //    qDebug()<<"line.size="<<line.size();
    //    SeisHorizonLineItem *item = new SeisHorizonLineItem(line);
    //    QIGLayer *horizonLayer = new ProfileSeisHorizonLayer();
    //    horizonLayer->addChild(item);
    //    mScene->addLayer(horizonLayer);
}

void ProfileMainWindow::addWellColumn()
{
    //    ProfileWellColumnLayer *wellLayer = new ProfileWellColumnLayer();
    //    QList<WellPositionInScale> wellPositions;
    //    for (int i=0; i<m_datas.traceCount(); i+=10)
    //    {
    //        WellPositionInScale aWell;
    //        aWell.wellName = QString("W%1").arg(i);
    //        aWell.line = m_datas.line(i);
    //        aWell.cdp  = m_datas.cdp(i);
    //        wellPositions.append(aWell);

    //        float startTime_ms = 0;
    //        float endTime_ms = 2000;
    //        wellLayer->addWell(i, startTime_ms, endTime_ms);
    //    }

    //    ui->graphicsView->setWellPosition(wellPositions);
    //    mScene->addLayer(wellLayer);
    //    ui->graphicsView->update();
}

void ProfileMainWindow::testColorTemplatePlot()
{
    ColorTemplateDialog colorTemplateDialog;
    const QList<QVector<float> > datas = ui->graphicsView->getSeisProfileData().profileData();
    QVector<float> profileDataValues;
    foreach (const QVector<float> &var, datas)
    {
        profileDataValues << var;
    }

    connect(&colorTemplateDialog, SIGNAL(colorDataChanged(ColorData)), SLOT(handleColorChange(ColorData)));

    float minValue = *std::min_element(profileDataValues.constBegin(), profileDataValues.constEnd());
    float maxValue = *std::max_element(profileDataValues.constBegin(), profileDataValues.constEnd());
    colorTemplateDialog.setData(minValue, maxValue,
                                ColorManager::getInstance().getColorData("LM_Profile"),
                                profileDataValues);
    colorTemplateDialog.exec();
}

void ProfileMainWindow::handleColorChange(const ColorData &colorData)
{
    ui->graphicsView->setColorMap(colorData.generateColorValues_igeoseis());
}
