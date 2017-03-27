#ifndef PSTEREOWIDGET_H
#define PSTEREOWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>

#include "shapefs.h"
#include "facedatabase.h"
#include "facedet.h"

class PStereoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PStereoWidget(QWidget *parent = 0);
    explicit PStereoWidget(Mesh &mesh, Facedet *det, QWidget *parent = 0);

    Shapefs *sfs;
    Mesh *mesh;
    FaceDatabase db;
    Facedet *detector;
    QGroupBox *menu;

public slots:
    void execPS();

private:
    void initMenu();

};

#endif // PSTEREOWIDGET_H
