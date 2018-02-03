

#ifndef UNICAM_H_
#define UNICAM_H_

#include "draw.h"
#include "point2.h"
#include "point3.h"
#include "vector2.h"
#include "vector3.h"


namespace MinGfx {

/**
   This mouse navigation is derived from "Unicam" by Zeleznik et al.

   Interaction Description:
   
   - Start state: wait for mouse down then transition to PanDollyRotDecision
     - on mouse down, store an intersection point if the click intersected some geometry

   - PanDollyRotDecision state: 
     - if change in mouseX > threshold goto Pan
     - if change in mouseY > threshold goto Dolly
     - if mouse up, then go to RotWaitForSecondClick
     - if wait for mouse up time limit is reached then goto PanDollyDecision

   - PanDollyDecision state: 
     - if change in mouseX > threshold goto Pan
     - if change in mouseY > threshold goto Dolly

   - Pan state:
     - if geometry was clicked then act like we grabbed the geometry at that point
     and are dragging it in a plane parallel to the filmplane
     - otherwise, grab the point under the mouse on the filmplane

   - Dolly state:
     - vertical mouse movement only controls dollying.
     - if geometry was clicked then dollying is scaled such that moving the mouse to
     the bottom of the screen would pull that geometry right up to the filmplane no matter
     how far away it was when it started.
     - otherwise, a constant dolly factor is used to turn vertical mouse movement into
     translation in and out of the screen.
     
   - Rot state:
     - if we clicked on some geometry do a trackball rotate around the selected 3D point
     - otherwise rotate around the origin of RoomSpace
     - when we exit the state, check our angular velocity - if the mouse is still rotating
     the model around (angular vel > threshold) then goto Spinning

   - Spinning state:
     - continue to rotate the model with the same angular velocity and direction until
     the mouse is clicked once to stop it.
*/
class UniCam {
public:  
    
    UniCam();
    UniCam(const Matrix4 &initialViewMatrix);
    
    virtual ~UniCam();

    
    /// To make the interaction work, the following set of functions need to be
    /// called from your GraphicsApp or whatever main application class you use
    /// to receive user input events and a draw callback.
    
    /// Attach this to whatever mouse button you wish, for example, call this
    /// from within GraphicsApp::OnRightMouseDown().  If your mousePos is reported
    /// in pixels, you will need to convert it to normalized device coordinates
    /// before passing it on to this routine.  The depth buffer value for the
    /// pixel under the mouse is also needed.  If you are using GraphicsApp, you
    /// can access both of these as follows:
    /// ```
    /// Point2 normalizedMousePos = pixels_to_normalized_coordinates(mousePos);
    /// float mouseZ = z_value_at_pixel(mousePos);
    /// uniCam.OnButtonDown(normalizedMousePos, mouseZ);
    /// ```
    void OnButtonDown(const Point2 &normalizedMousePos, float mouseZ);
    
    /// Attach this to the corresponding mouse move event, for example, call this
    /// from within GraphicsApp::OnRightMouseDrag().  If your mousePos is reported
    /// in pixels, you will need to convert it to normalized device coordinates
    /// before passing it on to this routine.  Within GraphicsApp, use:
    /// ```
    /// Point2 normalizedMousePos = pixels_to_normalized_coordinates(mousePos);
    /// uniCam.OnDrag(normalizedMousePos);
    /// ```
    void OnDrag(const Point2 &normalizedMousePos);
    
    /// Attach this to the corresponding button up event, for example, call this
    /// from within GraphicsApp::OnRightMouseUp().  If your mousePos is reported
    /// in pixels, you will need to convert it to normalized device coordinates
    /// before passing it on to this routine.  Within GraphicsApp, use:
    /// ```
    /// Point2 normalizedMousePos = pixels_to_normalized_coordinates(mousePpos);
    /// ```
    void OnButtonUp(const Point2 &normalizedMousePos);
    
    /// Attach this to a callback that can be used to control animation, for
    /// example, call this within GraphicsApp::UpdateSimulation().
    void AdvanceAnimation(double dt);

    /// Finally, attach this to your draw callback routine, for example, call
    /// this from within GraphicsApp::DrawUsingOpenGL();
    void Draw(const Matrix4 &projectionMatrix);
    
    
    /// Access the camera view matrix created by the UniCam interactions via
    /// this method and use it to draw the geometry in your scence.
    Matrix4 view_matrix();
    
    
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
    class MinGfx::Draw quickShapes_;

    Matrix4 V_;
    Matrix4 Vstart_;

    // saved from the last draw call in order to unproject the mouse pos
    Matrix4 Pdraw_;
};

    
} // end namespace

#endif


