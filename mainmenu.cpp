#include "mainmenu.h"

MainMenu::MainMenu()
{
    loadImg = new QPushButton("loadImage");
    drawLandmarks = new QPushButton("drawLandmarks");
    startStream = new QPushButton("startStream");
    stopStream = new QPushButton("stopStream");
    buildMeshOnly = new QPushButton("buildMesh");
    buildTexture = new QPushButton("applyTexture");

    bar = new QHBoxLayout;
    bar->addWidget(loadImg);
    bar->addWidget(drawLandmarks);
    bar->addWidget(startStream);
    bar->addWidget(stopStream);
    bar->addWidget(buildMeshOnly);
    bar->addWidget(buildTexture);
}

