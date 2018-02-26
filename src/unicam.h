/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_UNICAM_H_
#define SRC_UNICAM_H_

#include "quick_shapes.h"
#include "point2.h"
#include "point3.h"
#include "vector2.h"
#include "vector3.h"


namespace mingfx {

    
/** This implements a user interface for controlling the camera with the mouse.
 It is a special interface inspired by the "Unicam" technique developed by 
 Zeleznik et al.  
 
 The key feature is that this interface makes it possible to control camera pan, 
 dolly, and rotation with only a single mouse button.  That is quite useful 
 because it leaves the other mouse buttons free for pointing, sketching, or 
 other interaction techniques.

 The only downside of this technique is that it can take some time to learn.  In
 order to enjoy it, you will need to read these brief instructions on how to Pan,
 Dolly, Rotate, and Spin:
 
 - Pan: Click and drag horizontally with the mouse.  Once you make an initial
 horizontal movement you can than pan up and down as well, but the key to entering
 pan mode is to start with a horizontal movement.
 
 - Dolly: Click and drag vertically with the mouse.  The initial movement must
 be vertical.  If you click on some object in the scene, then the speed of dollying
 is set so that the object will come all the up to the camera lens if you drag
 the mouse to the bottom of the screen.
 
 - Rotate: Start with a quick click and release to set the center of rotation.  
 This is most useful if you click on some object in the scene.  You will see a
 black dot appear to mark the center of rotation.  If you click on the background
 then a center of rotation will be selected for you.  It will be a point straight
 ahead and at a depth 4.0 units away.  The depth can be adjusted for your application
 with set_default_depth().  Once your center of rotation is established, move 
 your mouse away a bit and then click and drag to do a trackball rotatation of
 the scene around this point.  Come to a stop before letting go of the mouse
 button in order to avoid entering the spin state!
 
 - Spin: For some fun, try "throwing" the scene so that it continues to rotate
 even after you let go.  To do this, start a rotation and then let go of the 
 mouse button while your mouse is still moving.  To stop spinning just click and
 release the mouse once to "catch" the scene.

 
 Example usage:
~~~
// Create a global or member variable in your MyGraphicsApp class:
UniCam unicam_;
 
 
void MyGraphicsApp::OnLeftMouseDown(const Point2 &pos) {
    Point2 mouse_xy = PixelsToNormalizedDeviceCoords(pos);
    float mouse_z = ReadZValueAtPixel(pos);
    unicam_.OnButtonDown(mouse_xy, mouse_z);
}

void MyGraphicsApp::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {
    Point2 mouse_xy = PixelsToNormalizedDeviceCoords(pos);
    unicam_.OnDrag(mouse_xy);
}

void MyGraphicsApp::OnLeftMouseUp(const Point2 &pos) {
    Point2 mouse_xy = PixelsToNormalizedDeviceCoords(pos);
    unicam_.OnButtonUp(mouse_xy);
}

void MyGraphicsApp::InitOpenGL() {
    projMatrix_ = Matrix4::perspective(30, aspect_ratio(), 1, 20);
    unicam_.set_view_matrix(Matrix4::lookAt(Point3(2.5,1,2.5), Point3(0,0,0), Vector3(0,1,0)););
}

void MyGraphicsApp::DrawOpenGL() {
    // draw your scene using the view matrix from UniCam
    Matrix4 proj_matrix = Matrix4::Perspective(60, aspect_ratio(), 0.001, 10);;
    Matrix4 view_matrix = uniCam.view_matrix();
    Matrix4 model_matrix = Matrix4::RotateY(to_radians(45.0));
    quickShapes.DrawCube(model_matrix, view_matirx, proj_matrix, Color(1,1,1));

    // tell unicam to draw itself (i.e., the small sphere that marks the center of
    // rotation when in rotation mode)
    unicam_.Draw(proj_matrix);
}
~~~
*/
class UniCam {
public:  
    
    /// Creates a UniCam object with an initial view matrix = identity.
    UniCam();

    /// Creates a UniCam object with the supplied initial view matrix.
    UniCam(const Matrix4 &initialViewMatrix);
    
    virtual ~UniCam();

    
    // To make the interaction work, the following set of functions need to be
    // called from your GraphicsApp or whatever main application class you use
    // to receive user input events and a draw callback.
    
    /// Attach this to whatever mouse button you wish, for example, call this
    /// from within GraphicsApp::OnRightMouseDown().  If your mousePos is reported
    /// in pixels, you will need to convert it to normalized device coordinates
    /// before passing it on to this routine.  The depth buffer value for the
    /// pixel under the mouse is also needed.  If you are using GraphicsApp, you
    /// can access both of these as follows:
    /// ~~~
    /// Point2 mouse_xy = PixelsToNormalizedDeviceCoords(mouse_in_pixels);
    /// float mouse_z = ReadZValueAtPixel(mouse_in_pixels);
    /// uniCam.OnButtonDown(mouse_xy, mouse_z);
    /// ~~~
    void OnButtonDown(const Point2 &normalizedMousePos, float mouseZ);
    
    /// Attach this to the corresponding mouse move event, for example, call this
    /// from within GraphicsApp::OnRightMouseDrag().  If your mousePos is reported
    /// in pixels, you will need to convert it to normalized device coordinates
    /// before passing it on to this routine.  Within GraphicsApp, use:
    /// ~~~
    /// Point2 mouse_xy = PixelsToNormalizedDeviceCoords(mouse_in_pixels);
    /// uniCam.OnDrag(mouse_xy);
    /// ~~~
    void OnDrag(const Point2 &normalizedMousePos);
    
    /// Attach this to the corresponding button up event, for example, call this
    /// from within GraphicsApp::OnRightMouseUp().  If your mousePos is reported
    /// in pixels, you will need to convert it to normalized device coordinates
    /// before passing it on to this routine.  Within GraphicsApp, use:
    /// ~~~
    /// Point2 mouse_xy = PixelsToNormalizedDeviceCoords(mouse_in_pixels);
    /// uniCam.OnButtonUp(mouse_xy);
    /// ~~~
    void OnButtonUp(const Point2 &normalizedMousePos);
    
    /// Attach this to a callback that can be used to control animation.  Within
    /// GraphicsApp::UpdateSimulation(), use:
    /// ~~~
    /// uniCam.AdvanceAnimation(dt);
    /// ~~~
    void AdvanceAnimation(double dt);

    /// Finally, attach this to your draw callback routine.  Within
    /// GraphicsApp::DrawUsingOpenGL(), use:
    /// ~~~
    /// uniCam.Draw(projMatrix);
    /// ~~~
    void Draw(const Matrix4 &projectionMatrix);
    
    
    /// Access the camera view matrix created by the UniCam interactions via
    /// this method and use it to draw the geometry in your scence.
    /// For example, within GraphicsApp::DrawUsingOpenGL(), you might have:
    /// ~~~
    /// Matrix4 P = Matrix4::Perspective(30, aspect_ratio(), 1, 20);
    /// Matrix4 V = unicam.view_matrix();
    /// Matrix4 M = Matrix4::RotateY(GfxMath::ToRadians(45.0));
    /// quick_shapes.DrawCube(M, V, P, Color(1,1,1));
    /// ~~~
    Matrix4 view_matrix();
    
    
    /// Returns the "eye" point (i.e., focal point) of the camera in world
    /// space coordinates.
    Point3 eye();
    
    /// Returns the look direction (i.e., -Z axis of the camera matrix) in world
    /// space coordinates.
    Vector3 look();
    
    
    // -------------
    
    /// This is not required, but you may use this if you wish to set an initial
    /// view matrix or reset the view matrix
    void set_view_matrix(Matrix4 viewMatrix);
    
    /// This sets the depth of the center of rotation for the case when the user's
    /// click does not intersect any geometry.  It defaults to 4 units, but the
    /// right value to use depends very much on the current scene.  For example,
    /// you could set a very good value by calculating the current centroid of
    /// your scene and the finding the depth of this point (the distance along
    /// the look vector) relative to the camera.
    void set_default_depth(float d);
    
    
private:
    
    void recalc_angular_vel();
    
    enum UniCamState {
        START,
        PAN_DOLLY_ROT_DECISION,
        PAN_DOLLY_DECISION,
        ROT_WAIT_FOR_SECOND_CLICK,
        PAN,
        DOLLY,
        ROT,
        SPINNING
    };
    UniCamState state_;
    
    Point2  mouseLast_;
    double  elapsedTime_;

    Point2  initialClickPos_;
    bool    hitGeometry_;
    Point3  hitPoint_;

    bool    rotInitialized_;
    Point3  rotLastIPoint_;
    float   boundingSphereRad_;
    Point3  boundingSphereCtr_;
    double  rotLastTime_;
    std::vector<std::pair<float, float>> rotAngularVelBuffer_;
    float rotAngularVel_;
    Vector3 rotAxis_;
    
    bool    dollyInitialized_;
    float   dollyFactor_;
    float   defaultDepth_;
    
    bool    showIcon_;
    QuickShapes quickShapes_;

    Matrix4 V_;
    Matrix4 Vstart_;

    // saved from the last draw call in order to unproject the mouse pos
    Matrix4 Pdraw_;
};

    
} // end namespace

#endif


