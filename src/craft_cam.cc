/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "craft_cam.h"

namespace mingfx {
    

CraftCam::CraftCam() : t_scale_(1.0), r_scale_(1.0), l_scale_(1.0), yaw_(0.0), pitch_(0.0) {
}

CraftCam::CraftCam(const Matrix4 &initialViewMatrix) :
    t_scale_(1.0), r_scale_(1.0), l_scale_(1.0), yaw_(0.0), pitch_(0.0)
{
}

CraftCam::~CraftCam()
{
}

    
void CraftCam::UpdateSimulation(double dt, GLFWwindow *window_ptr) {
    if ((glfwGetKey(window_ptr, GLFW_KEY_UP) == GLFW_PRESS) ||
        (glfwGetKey(window_ptr, GLFW_KEY_W) == GLFW_PRESS)) {
        WalkForward(dt);
    }
    if ((glfwGetKey(window_ptr, GLFW_KEY_DOWN) == GLFW_PRESS) ||
        (glfwGetKey(window_ptr, GLFW_KEY_Z) == GLFW_PRESS)) {
        WalkBackward(dt);
    }
    if ((glfwGetKey(window_ptr, GLFW_KEY_LEFT) == GLFW_PRESS) ||
        (glfwGetKey(window_ptr, GLFW_KEY_A) == GLFW_PRESS)) {
        RotateLeft(dt);
    }
    if ((glfwGetKey(window_ptr, GLFW_KEY_RIGHT) == GLFW_PRESS) ||
        (glfwGetKey(window_ptr, GLFW_KEY_S) == GLFW_PRESS)) {
        RotateRight(dt);
    }
}

    
void CraftCam::OnMouseMove(const Vector2 &normalized_mouse_delta) {
    LookWithMouse(normalized_mouse_delta);
}



void CraftCam::WalkForward(double dt) {
    base_head_ = Matrix4::Translation(3.0 * dt * t_scale_ * Vector3(0,0,1)) * base_head_;
}

void CraftCam::WalkBackward(double dt) {
    base_head_ = Matrix4::Translation(3.0 * dt * t_scale_ * Vector3(0,0,-1)) * base_head_;
}

void CraftCam::RotateLeft(double dt) {
    base_head_ = Matrix4::RotationY(-0.75 * dt * r_scale_) * base_head_;
}

void CraftCam::RotateRight(double dt) {
    base_head_ = Matrix4::RotationY(0.75 * dt * r_scale_) * base_head_;
}

void CraftCam::LookWithMouse(const Vector2 &mouse_delta) {
    yaw_ += l_scale_ * mouse_delta[0];
    pitch_ += l_scale_ * mouse_delta[1];
    added_rot_ = Matrix4::RotationX(-pitch_) * Matrix4::RotationY(yaw_);
}


Matrix4 CraftCam::view_matrix() {
    return added_rot_ * base_head_;
}

void CraftCam::set_view_matrix(Matrix4 view_matrix) {
    base_head_ = view_matrix;
    added_rot_ = Matrix4();
}

Point3 CraftCam::eye() {
    Matrix4 camMat = view_matrix().Inverse();
    return camMat.ColumnToPoint3(3);
}

Vector3 CraftCam::look() {
    Matrix4 camMat = view_matrix().Inverse();
    return -camMat.ColumnToVector3(2);
}


void CraftCam::set_translation_scale(float s) {
    t_scale_ = s;
}

void CraftCam::set_rotation_scale(float s) {
    r_scale_ = s;
}

void CraftCam::set_look_scale(float s) {
    l_scale_ = s;
}

    
float CraftCam::translation_scale() {
    return t_scale_;
}

float CraftCam::rotation_scale() {
    return r_scale_;
}

float CraftCam::look_scale() {
    return l_scale_;
}

void CraftCam::UpdateHeight(float new_y_value) {
    Vector3 offset = Vector3(0, new_y_value - eye()[1], 0);    
    base_head_ = Matrix4::Translation(-offset) * base_head_;
}

    
    
} // end namespace

