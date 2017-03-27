#include "facedatabase.h"

FaceDatabase::FaceDatabase()
{

}


bool FaceDatabase::load(QString database)
{
    imageList.clear();
    lightDirections.clear();

    QString db = "../Database/"+database;
    QString imlist = db+"/"+database+"_P00";
    QString lightlist = db+ "/" + "lightDirections";

    bool imagesLoaded = QVecOperator::load(imageList,imlist);
    bool lightsLoaded = QVecOperator::load(lightDirections,lightlist);

    if(imagesLoaded && lightsLoaded)
    {
        name = database;
        currentFrame = 0;
        return true;
    }
    else
        return false;
}
