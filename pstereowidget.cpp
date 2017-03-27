#include "pstereowidget.h"

PStereoWidget::PStereoWidget(QWidget *parent) : QWidget(parent)
{
    //sfs = new Shapefs;
    detector = new Facedet;
    initMenu();
}

PStereoWidget::PStereoWidget(Mesh &pmesh, Facedet *det, QWidget *parent) : QWidget(parent), detector(det)
{
    mesh = &pmesh;
    //sfs = new Shapefs;
    initMenu();
}


void PStereoWidget::execPS()
{

}

void PStereoWidget::initMenu()
{
    QPushButton *psBtm = new QPushButton("execPS");

    QSpinBox *maxAzimuthSpin = new QSpinBox;
    maxAzimuthSpin->setValue(sfs->maxAzimuth());
    QLabel *azLabel = new QLabel;
    azLabel->setText("max Azim: ");
    QFormLayout *form1 = new QFormLayout;
    form1->addRow(azLabel,maxAzimuthSpin);

    QSpinBox *maxElevationSpin = new QSpinBox;
    maxElevationSpin->setValue(sfs->maxElevation());
    QLabel *eleLabel = new QLabel;
    eleLabel->setText("max Elev: ");
    QFormLayout *form2 = new QFormLayout;
    form2->addRow(eleLabel,maxElevationSpin);

    QSpinBox *minImageNumberSpin = new QSpinBox;
    minImageNumberSpin->setValue(sfs->minImageNumber());
    QLabel *imgLabel = new QLabel;
    imgLabel->setText("min Img#: ");
    QFormLayout *form3 = new QFormLayout;
    form3->addRow(imgLabel,minImageNumberSpin);

    QLabel *filterLabel = new QLabel;
    filterLabel->setText("filter:");
    QRadioButton *filter1 = new QRadioButton("none");
    QRadioButton *filter2 = new QRadioButton("intensity");
    QRadioButton *filter3 = new QRadioButton("p-error");
    filter1->setChecked(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(psBtm);
    layout->addItem(form1);
    layout->addItem(form2);
    layout->addItem(form3);
    layout->addWidget(filterLabel);
    layout->addWidget(filter1);
    layout->addWidget(filter2);
    layout->addWidget(filter3);

    menu = new QGroupBox("sfs");
    menu->setLayout(layout);
    menu->setSizePolicy(QSizePolicy( QSizePolicy::Preferred,
                                      QSizePolicy::Fixed));
}
