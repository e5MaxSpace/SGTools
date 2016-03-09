#ifndef PROFILEMAINWINDOW_H
#define PROFILEMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class ProfileMainWindow;
}

class ColorData;
class QLineEdit;

class ProfileMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfileMainWindow(QWidget *parent = 0);
    ~ProfileMainWindow();

private slots:
    void slotGoBackNum();
    void slotGoNextNum();

    void slotDensity();
    void slotWave();

    void displaySegyData();
    void addSeisHorizon();
    void addWellColumn();

    void testColorTemplatePlot();
    void handleColorChange(const ColorData &colorData);

private:
    void createToolBar();
    void repaintProfile();

private:
    Ui::ProfileMainWindow *ui;

    QString m_fileName;
    int m_currentLine;
    QLineEdit *m_stepLineEditAct;
};

#endif // PROFILEMAINWINDOW_H
