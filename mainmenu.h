#ifndef MAINMENU_H
#define MAINMENU_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>


class MainMenu
{
public:
    MainMenu();

    QPushButton *loadImg;
    QPushButton *drawLandmarks;
    QPushButton *startStream;
    QPushButton *stopStream;
    QPushButton *buildMeshOnly;
    QPushButton *buildTexture;

    QHBoxLayout *bar;
};

#endif // MAINMENU_H
