#ifndef FASTMARCHINGWIDGET_H
#define FASTMARCHINGWIDGET_H

#include <QWidget>
#include <glwidget.h>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>

#include "fastm.h"

class FastMarchingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FastMarchingWidget(QWidget *parent = 0);
    explicit FastMarchingWidget(Mesh &mesh, QWidget *parent = 0);

    Mesh *mesh;

    Fastm *fm;
    QGroupBox *menu;
    QPushButton *fmBtm;
    QDoubleSpinBox *lambdaSpin;
    QSpinBox *startSpin;

signals:

public slots:

private:
    void initMenu();
};

#endif // FASTMARCHINGWIDGET_H
