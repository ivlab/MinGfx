/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_CRAFT_CAM_H_
#define SRC_CRAFT_CAM_H_

#include "point2.h"
#include "matrix4.h"
#include "vector2.h"


namespace mingfx {
    

/** This implements a user interface for controlling the camera with the mouse.
 This interface is appropriate for "first person" camera control, as in games
 like Minecraft.
 
 Use the arrow keys or A,S,W,Z keys to move around in the virtual world.
 UP/DOWN and W/Z move forward and back.  LEFT/RIGHT or A/S rotate your body
 to face left or right.  You can rotate and tilt your head to look 
 left/right/up/down by moving the mouse.  In Minecraft, that movement happens
 whenever you move the mouse, regardless of whether you are holding down a
 mouse button, but in some games you want to hold the camera still while you
 use the mouse to draw on the screen or do something else.  It's possible to
 use this interface both ways by calling the MouseMove() function either every
 time the mouse moves, or only when the mouse is in a dragging mode.
 
 Example usage:
 ~~~
 // Create a global or member variable in your MyGraphicsApp class:
 CraftCam cam_;
 
 
 // If you want to always rotate the view with the mouse, use this:
 void MyGraphicsApp::OnMouseMove(const Point2 &pos, const Vector2 &delta) {
    Vector2 delta_ndc = PixelsToNormalizedDeviceCoords(pos);
    cam_.OnMouseMove(delta_ndc);
 }

 // Alternatively, if you want to only rotate the view when the mouse button is
 // held down, use this instead.  Call cam_.OnMouseMove() in either one function
 // or the other, but not both!
 // void MyGraphicsApp::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {
 //     Vector2 delta_ndc = PixelsToNormalizedDeviceCoords(pos);
 //     cam_.OnMouseMove(delta_ndc);
 // }

 // This tells the camera to simulate walking based on the keyboard keys currently
 // pressed.  You need to pass a pointer to the underlying GLFW window created by
 // GraphicsApp.
 void MyGraphicsApp::UpdateSimulation(double dt)  {
     cam_.UpdateSimulation(dt, window());
 }
 
 void MyGraphicsApp::InitOpenGL() {
     cam_.set_view_matrix(Matrix4::lookAt(Point3(0,2,2), Point3(0,2,0), Vector3(0,1,0)););
 }
 
 void MyGraphicsApp::DrawOpenGL() {
     // draw your scene using the view matrix from the camera
     Matrix4 proj_matrix = Matrix4::perspective(60, aspect_ratio(), 1, 200);
     Matrix4 view_matrix = cam_.view_matrix();
     Matrix4 model_matrix = Matrix4::RotateY(to_radians(45.0));
     quick_shapes.DrawCube(model_matrix, view_matirx, proj_matrix, Color(1,1,1));
 }
 ~~~
 */
class CraftCam {
public:
    
    /// Creates a CraftCam object with an initial view matrix = identity.
    CraftCam();
    
    /// Creates a CraftCam object with the supplied initial view matrix.
    CraftCam(const Matrix4 &initial_view_matrix);
    
    virtual ~CraftCam();
    
    
    // To make the interaction work, the following set of functions need to be
    // called from your GraphicsApp or whatever main application class you use
    // to receive user input events and a draw callback.
    
    /// Call this from your app's UpdateSimulation() method.  This tells the
    /// camera to simulate walking based on the keyboard keys currently pressed.
    /// You need to pass a pointer to the underlying GLFW window created by
    /// GraphicsApp.  Example:
    /// ~~~
    /// void MyGraphicsApp::UpdateSimulation(double dt)  {
    ///     cam_.UpdateSimulation(dt, window());
    /// }
    /// ~~~
    void UpdateSimulation(double dt, GLFWwindow *window_ptr);
    

    /// Call this from your app's OnMouseMove() or On*MouseDrag() method.  Use
    /// OnMouseMove() if you want to always rotate the view with the mouse.
    /// Remember to convert the mouse coordinates (usually reported by window
    /// managers in pixels) into normalized device coordinates:
    /// ~~~
    /// void MyGraphicsApp::OnMouseMove(const Point2 &pos, const Vector2 &delta) {
    ///     Vector2 delta_ndc = PixelsToNormalizedDeviceCoords(pos);
    ///     cam_.OnMouseMove(delta_ndc);
    /// }
    /// ~~~
    /// Alternatively, if you want to only rotate the view when the mouse button is
    /// held down, use On*MouseDrag() instead:
    /// ~~~
    /// void MyGraphicsApp::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {
    ///     Vector2 delta_ndc = PixelsToNormalizedDeviceCoords(pos);
    ///     cam_.OnMouseMove(delta_ndc);
    /// }
    /// ~~~
    void OnMouseMove(const Vector2 &normalized_mouse_delta);

    
    /// Access the camera view matrix created by the CraftCam interactions via
    /// this method and use it to draw the geometry in your scence.
    /// For example, within GraphicsApp::DrawUsingOpenGL(), you might have:
    /// ~~~
    /// Matrix4 P = Matrix4::Perspective(30, aspect_ratio(), 1, 20);
    /// Matrix4 V = cam.view_matrix();
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
    

    /// Sets the y value of the camera (i.e., the height).  If you want to set
    /// the entire view matrix, then use set_view_matrix(), but if you just want
    /// to update the height, e.g., while walking around a bumpy terrain, then
    /// use this.
    void UpdateHeight(float new_y_value);

    
    // -------------
    
    /// This is not required, but you may use this if you wish to set an initial
    /// view matrix or reset the view matrix
    void set_view_matrix(Matrix4 view_matrix);

 
    /// This is the scale factor used to speed up / slow down forward/backward
    /// translation when walking for the UP / DOWN keys.  It defaults to 1.0,
    /// smaller values will make the camera walk slower, larger values will
    /// speed it up.
    float translation_scale();

    /// This is the scale factor used to speed up / slow down forward/backward
    /// translation when walking for the UP / DOWN keys.  It defaults to 1.0,
    /// smaller values will make the camera walk slower, larger values will
    /// speed it up.
    void set_translation_scale(float s);

    /// This is the scale factor used to speed up / slow down left/right
    /// rotation when walking for the LEFT / RIGHT keys.  It defaults to 1.0,
    /// smaller values will make the camera turn slower, larger values will
    /// speed it up.
    float rotation_scale();

    /// This is the scale factor used to speed up / slow down left/right
    /// rotation when walking for the LEFT / RIGHT keys.  It defaults to 1.0,
    /// smaller values will make the camera turn slower, larger values will
    /// speed it up.
    void set_rotation_scale(float s);
    
    /// This is the scale factor used to speed up / slow down looking around
    /// when moving the head with the mouse.  It defaults to 1.0, smaller values
    /// will make the camera turn slower, larger values will speed it up.
    float look_scale();

    /// This is the scale factor used to speed up / slow down looking around
    /// when moving the head with the mouse.  It defaults to 1.0, smaller values
    /// will make the camera turn slower, larger values will speed it up.
    void set_look_scale(float s);
    
private:
    
    void WalkForward(double dt);
    void WalkBackward(double dt);
    void RotateLeft(double dt);
    void RotateRight(double dt);
    void LookWithMouse(const Vector2 &mouse_delta);
    
    float t_scale_;
    float r_scale_;
    float l_scale_;
    float yaw_;
    float pitch_;
    Matrix4 base_head_;
    Matrix4 added_rot_;
};

    
} // end namespace

#endif


