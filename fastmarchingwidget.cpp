#include "fastmarchingwidget.h"

FastMarchingWidget::FastMarchingWidget(QWidget *parent) : QWidget(parent)
{
    //fm = new Fastm;
    initMenu();
}


FastMarchingWidget::FastMarchingWidget(Mesh &pmesh, QWidget *parent):
    QWidget(parent)
{
      mesh = &pmesh;
      //fm = new Fastm;
      initMenu();
}




void FastMarchingWidget::initMenu()
{
   fmBtm = new QPushButton("exec FM");


   lambdaSpin = new QDoubleSpinBox;
   lambdaSpin->setValue(fm->lambda());
   connect(lambdaSpin,SIGNAL(valueChanged(double)),fm,SLOT(setLambda(double)));
   QLabel *lambdaLabel = new QLabel;
   lambdaLabel->setText("lambda: ");
   QFormLayout *form1 = new QFormLayout;
   form1->addRow(lambdaLabel,lambdaSpin);

   startSpin = new QSpinBox;
   startSpin->setValue(fm->initialPoint());
   connect(startSpin,SIGNAL(valueChanged(int)),fm,SLOT(setInitPoint(int)));
   QLabel *initLabel = new QLabel;
   initLabel->setText("starting vertex: ");
   QFormLayout *form2 = new QFormLayout;
   form2->addRow(initLabel,startSpin);

   QVBoxLayout *layout = new QVBoxLayout;
   layout->addWidget(fmBtm);
   layout->addItem(form1);
   layout->addItem(form2);
   //QSpacerItem *spacer = new QSpacerItem(1,120,QSizePolicy::Expanding, QSizePolicy::Minimum);
   //layout->addItem(spacer);




   menu = new QGroupBox("fm");
   menu->setLayout(layout);
   menu->setSizePolicy(QSizePolicy( QSizePolicy::Preferred,
                                     QSizePolicy::Fixed));

}
