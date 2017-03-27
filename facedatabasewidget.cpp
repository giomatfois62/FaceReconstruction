#include "facedatabasewidget.h"

FaceDatabaseWidget::FaceDatabaseWidget(QWidget *parent) : QWidget(parent)
{

}

void FaceDatabaseWidget::loadPrevious()
{
    if(!m_imageList.size() || !m_currentFrame)
        return;

    m_currentFrame--;

    QString dbPath = "../Database/"+m_name+"/";
    QString imgToLoadPath = dbPath + m_imageList.at(m_currentFrame);
    emit imageChanged(imgToLoadPath);
}

void FaceDatabaseWidget::loadNext()
{
    if(!m_imageList.size() || m_currentFrame==m_imageList.size()-1)
        return;

    m_currentFrame++;

    QString dbPath = "../Database/"+m_name+"/";
    QString imgToLoadPath = dbPath + m_imageList.at(m_currentFrame);
    emit imageChanged(imgToLoadPath);
}

void FaceDatabaseWidget::loadDatabase(int dbNumber)
{
    // no data for yaleB14, yaleB16
    if(dbNumber == 14 || dbNumber == 16)
       return;

    m_imageList.clear();
    m_lightDirections.clear();

    QString dbName = "yaleB"+QString::number(dbNumber);

    QString dbPath = "../Database/"+dbName+"/";
    QString imglistPath = dbPath+dbName+"_P00";
    QString lightlistPath = dbPath+ "lightDirections";

    QVecOperator::load(m_imageList,imglistPath);
    QVecOperator::load(m_lightDirections,lightlistPath);

    m_name = dbName;
    m_currentFrame = 0;

    QString imgToLoadPath = dbPath + m_imageList.at(m_currentFrame);
    emit imageChanged(imgToLoadPath);
}

void FaceDatabaseWidget::loadArtificial(int dbNumber)
{
    m_imageList.clear();
    m_lightDirections.clear();

    QString dbName = "artificialB"+QString::number(dbNumber);

    QString dbPath = "../Database/"+dbName+"/";
    QString imglistPath = dbPath+dbName+"_P00";
    QString lightlistPath = dbPath + "lightDirections";

    QVecOperator::load(m_imageList,imglistPath);
    QVecOperator::load(m_lightDirections,lightlistPath);

    m_name = dbName;
    m_currentFrame = 0;

    QString imgToLoadPath = dbPath + m_imageList.at(m_currentFrame);
    emit imageChanged(imgToLoadPath);
}



QGroupBox *FaceDatabaseWidget::menu()
{
    QGroupBox *m_menu = new QGroupBox("Database");

    QPushButton *nextBtm = new QPushButton("next");
    connect(nextBtm, SIGNAL(pressed()),this,SLOT(loadNext()));
    QPushButton *prevBtm = new QPushButton("prev");
    connect(prevBtm, SIGNAL(pressed()),this,SLOT(loadPrevious()));
    QHBoxLayout *btmLayout = new QHBoxLayout;
    btmLayout->addWidget(prevBtm);
    btmLayout->addWidget(nextBtm);

    QSpinBox *yaleSpin = new QSpinBox;
    yaleSpin->setMinimum(11);
    yaleSpin->setMaximum(39);
    yaleSpin->setValue(31);
    connect(yaleSpin, SIGNAL(valueChanged(int)),this,SLOT(loadDatabase(int)));
    QLabel *yaleLabel = new QLabel;
    yaleLabel->setText("yaleB");
    QHBoxLayout *yaleForm= new QHBoxLayout;
    yaleForm->addWidget(yaleLabel);
    yaleForm->addWidget(yaleSpin);

    QSpinBox *artSpin = new QSpinBox;
    artSpin->setMinimum(1);
    artSpin->setMaximum(8);
    artSpin->setValue(1);
    connect(artSpin, SIGNAL(valueChanged(int)),this,SLOT(loadArtificial(int)));
    QLabel *artLabel = new QLabel;
    artLabel->setText("artificial");
    QHBoxLayout *artForm= new QHBoxLayout;
    artForm->addWidget(artLabel);
    artForm->addWidget(artSpin);

    QGridLayout *menuLayout = new QGridLayout;
    menuLayout->addWidget(prevBtm,0,0);
    menuLayout->addWidget(nextBtm,0,1);
    menuLayout->addItem(yaleForm,1,0);
    menuLayout->addItem(artForm,2,0);

    m_menu->setLayout(menuLayout);
    m_menu->setSizePolicy(QSizePolicy( QSizePolicy::Fixed,QSizePolicy::Preferred));

    return m_menu;
}
