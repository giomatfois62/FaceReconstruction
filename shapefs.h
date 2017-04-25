#ifndef SHAPEFS_H
#define SHAPEFS_H

#include <QWidget>

#include "mesh.h"
#include "facedatabasewidget.h"
#include "facedetector.h"

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix3x3>

#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QRadioButton>

class Shapefs: public QWidget
{
    Q_OBJECT
public:
    explicit Shapefs(Mesh *pmesh,
                     QWidget *parent =0);
    explicit Shapefs(Mesh *pmesh, Facedet *det,
                     QWidget *parent =0);
    explicit Shapefs(Mesh *pmesh, FaceDatabaseWidget *database,
                     QWidget *parent =0);
    explicit Shapefs(Mesh *pmesh, Facedet *det, FaceDatabaseWidget *db,
                     QWidget *parent =0);

    // apply photometric stereo using a face database to recover normal
    // and color of each vertex
    FaceDatabaseWidget *db;

    // menu plus items i need to declare here
    QGroupBox* menu();
    QRadioButton *filter1;
    QRadioButton *filter2;
    QRadioButton *filter3;

    // getter
    float threshold() {return m_threshold;}
    int minImageNumber() {return m_minImageNumber;}
    int maxAzimuth() {return m_maxAzimuth;}
    int maxElevation() {return m_maxElevation;}

public slots:
    // setter
    void setFilter(QString filterType) {m_filterType = filterType;}
    void setFilter();
    void setThreshold(double value) { m_threshold=value; }
    void setMinImageNumber(int value) {m_minImageNumber=value; }
    void setMaxAzimuth(int value) {m_maxAzimuth=value; }
    void setMaxElevation(int value) {m_maxElevation=value; }
    // actions
    void execPhotometric();
    void execPhotometricSVD();
    void estimateLightDirection(cv::Mat &img, QVector4D &light);

signals:
    void meshUpdated();

private:
    Mesh *mesh;
    Facedet *detector;

    // threshold for treating shadows and specularities
    float m_threshold;
    // filter type (shadow, pError, none)
    QString m_filterType;
    // minimum image number to use after thresholding
    int m_minImageNumber;
    // maximum values for azimuth/elevation light angles to be used
    int m_maxAzimuth;
    int m_maxElevation;
    // init default values for class parameters
    void initDefault();

    // load sfs data from database and execute photometric stereo using data
    void loadSfsData(QVector<QVector<float>> &intensities,
                     QVector<QVector3D> &lightDirections);
    void execPS(QVector<QVector<float>> &intensities,
                QVector<QVector3D> &lightDirections);

    void execPS_SVD(QVector<QVector<float>> &intensities,
                    QVector<QVector3D> &lightDirections);

    // build and solve photometric Stereo linear system
    QMatrix3x3 buildSystemMatrix(QVector<QVector3D> &lightDirections);
    QVector3D buildSystemData(QVector<float> &intensities,
                              QVector<QVector3D> &lightDirections);
    QVector3D solveSystem(QMatrix3x3 &inverse, QVector3D &data);

    // matrix inversion funcs (these should be putted somewhere else)
    void invertQMatrix3x3(QMatrix3x3 &matrix);
    void invertQMatrix3x3_QR(QMatrix3x3 &matrix);
    void diagonalize(const float (&A)[3][3], float (&Q)[3][3], float (&D)[3][3]);

    // methods for computing error
    QMap<float,unsigned int> computeErrorMap(QVector3D &normal,
                                             QVector<QVector3D> &lightDirections,
                                             QVector<float> &intensities);
    float computeGlobalError(QVector3D &normal,
                             QList<QVector3D> &lightDirections,
                             QVector<float> &intensities);
};

#endif // SHAPEFS_H
