#include "camera.h"

Camera::Camera()
{
    _position = QVector3D(0.0f, 0.0f, 1.0f);
    _aspect = 4.0f/3.0f;
    _zoom = 90.0f;

    updateView();
    updateProjection();
}

void Camera::updateView()
{
    _view.setToIdentity();
    _view.lookAt(_position,    // Camera Position
                 QVector3D(0.0f, 0.0f, 0.0f),    // Point camera looks towards
                 QVector3D(0.0f, 1.0f, 0.0f));   // Up vector*/

}

void Camera::updateProjection()
{
    _projection.setToIdentity();
    _projection.perspective( _zoom,         // field of vision
                             _aspect,     // aspect ratio
                             0.01f,          // near clipping plane
                             1000.0f);       // far clipping plane*/

}
