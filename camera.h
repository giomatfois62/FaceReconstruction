#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>

class Camera
{
public:
    Camera();

    QMatrix4x4& projection() { return _projection; }
    QMatrix4x4& view() { return _view; }

    QVector3D& position() {return _position;}
    float aspect() {return _aspect;}
    float zoom() {return _zoom;}

    void setPosition(const QVector3D& position) { _position=position; updateView(); }
    void setZoom(float zoom) { _zoom=zoom; updateProjection(); }
    void setAspect(float aspect) { _aspect=aspect; updateProjection(); }

    QMatrix4x4 _view;
private:
    QVector3D _position;
    float _aspect;
    float _zoom;

    void updateProjection();
    void updateView();

    QMatrix4x4 _projection;

};

#endif // CAMERA_H
