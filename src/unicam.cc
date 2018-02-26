/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "unicam.h"

#include "gfxmath.h"

namespace mingfx {


UniCam::UniCam() : state_(START), defaultDepth_(4.0) {
}

UniCam::UniCam(const Matrix4 &initialViewMatrix) :
    state_(START), defaultDepth_(4.0), V_(initialViewMatrix)
{
}

UniCam::~UniCam()
{
}


void UniCam::recalc_angular_vel() {
    // update angular velocity
    float cutoff = elapsedTime_ - 0.2; // look just at the last 0.2 secs
    while ((rotAngularVelBuffer_.size()) && (rotAngularVelBuffer_[0].first < cutoff)) {
        rotAngularVelBuffer_.erase(rotAngularVelBuffer_.begin());
    }
    rotAngularVel_ = 0.0;
    if (rotAngularVelBuffer_.size()) {
        for (int i=0; i<rotAngularVelBuffer_.size(); i++) {
            rotAngularVel_ += rotAngularVelBuffer_[i].second;
        }
        rotAngularVel_ /= rotAngularVelBuffer_.size();
    }
    //std::cout << rotAngularVelBuffer_.size() << " " << rotAngularVel_ << std::endl;
}


void UniCam::OnButtonDown(const Point2 &mousePos, float mouseZ) {
    if (state_ == START) {
        initialClickPos_ = mousePos;
        mouseLast_ = mousePos;
        elapsedTime_ = 0.0;
        rotInitialized_ = false;
        dollyInitialized_ = false;
        
        hitGeometry_ = (mouseZ < 1.0);
        if (hitGeometry_) {
            hitPoint_ = GfxMath::ScreenToWorld(V_, Pdraw_, mousePos, mouseZ);
        }
        else {
            hitPoint_ = GfxMath::ScreenToDepthPlane(V_, Pdraw_, Point2(0,0), defaultDepth_);
        }
        showIcon_ = true;
        state_ = PAN_DOLLY_ROT_DECISION;
    }
    else if (state_ == ROT_WAIT_FOR_SECOND_CLICK) {
        // we have the second click now, and we will start the trackball rotate interaction
        state_ = ROT;
    }
    else if (state_ == SPINNING) {
        // this click is to "catch" the model, stopping it from spinning.
        state_ = START;
    }
    else {
        std::cerr << "UniCam::OnButtonDown() unexpected state." << std::endl;
    }
}

void UniCam::OnDrag(const Point2 &mousePos) {
    if (state_ == PAN_DOLLY_ROT_DECISION) {
        const double panMovementThreshold  = 0.01;
        const double dollyMovementThreshold = 0.01;
        if (fabs(mousePos[0] - initialClickPos_[0]) > panMovementThreshold) {
            // already lots of horizontal movement, we can go right to pan
            state_ = PAN;
            showIcon_ = false;
        }
        else if (fabs(mousePos[1] - initialClickPos_[1]) > dollyMovementThreshold) {
            // already lots of vertical movement, we can go right to dolly
            state_ = DOLLY;
            showIcon_ = false;
        }
        else if (elapsedTime_ > 1.0) {
            // timeout, this was not a quick click to set a center of rotation,
            // so there is no intent to rotate.  instead we will be doing either
            // pan or dolly.
            state_ = PAN_DOLLY_DECISION;
            showIcon_ = false;
        }
    }
    else if (state_ == PAN_DOLLY_DECISION) {
        const double panMovementThreshold  = 0.01;
        const double dollyMovementThreshold = 0.01;
        if (fabs(mousePos[0] - initialClickPos_[0]) > panMovementThreshold) {
            // lots of horizontal movement, go to pan
            state_ = PAN;
        }
        else if (fabs(mousePos[1] - initialClickPos_[1]) > dollyMovementThreshold) {
            // lots of vertical movement, go to dolly
            state_ = DOLLY;
        }
    }
    else if (state_ == PAN) {
        Matrix4 camMat = V_.Inverse();
        Point3 eye = camMat.ColumnToPoint3(3);
        Vector3 look = -camMat.ColumnToVector3(2);
        float depth = (hitPoint_ - eye).Dot(look);
        Point3 pWorld1 = GfxMath::ScreenToDepthPlane(V_, Pdraw_, mouseLast_, depth);
        Point3 pWorld2 = GfxMath::ScreenToDepthPlane(V_, Pdraw_, mousePos, depth);
        V_ = V_ * Matrix4::Translation(pWorld2 - pWorld1);
    }
    else if (state_ == DOLLY) {
        if (!dollyInitialized_) {
            // Setup dollyFactor so that if you move the mouse to the bottom of the screen, the point
            // you clicked on will be right on top of the camera.
            Matrix4 camMat = V_.Inverse();
            Point3 eye = camMat.ColumnToPoint3(3);
            Vector3 look = -camMat.ColumnToVector3(2);
            float depth = (hitPoint_ - eye).Dot(look);
            float deltaYToBottom = initialClickPos_[1] + 1;
            dollyFactor_ = depth / deltaYToBottom;
            dollyInitialized_ = true;
        }
        Vector3 d(0, 0, -dollyFactor_ * (mousePos[1] - mouseLast_[1]));
        V_ = Matrix4::Translation(d) * V_ ;
    }
    else if (state_ == ROT) {
        if (!rotInitialized_) {
            float depth = 0.0;
            if (hitGeometry_) {
                // if we hit some geometry, then make that the center of rotation
                boundingSphereCtr_ = hitPoint_;
                Matrix4 camMat = V_.Inverse();
                Point3 eye = camMat.ColumnToPoint3(3);
                Vector3 look = -camMat.ColumnToVector3(2);
                depth = (hitPoint_ - eye).Dot(look);
            }
            else {
                // if we did not hit any geometry, then center the bounding sphere in front of
                // the camera at a distance that can be configured by the user.
                boundingSphereCtr_ = GfxMath::ScreenToDepthPlane(V_, Pdraw_, Point2(0,0), defaultDepth_);
                depth = defaultDepth_;
            }
            
            // determine the size of the bounding sphere by projecting a screen-space
            // distance of 0.75 units to the depth of the sphere center
            Point3 pWorld1 = GfxMath::ScreenToDepthPlane(V_, Pdraw_, Point2(0,0), depth);
            Point3 pWorld2 = GfxMath::ScreenToDepthPlane(V_, Pdraw_, Point2(0.75,0), depth);
            boundingSphereRad_ = (pWorld2-pWorld1).Length();
            
            rotLastTime_ = elapsedTime_;
            rotAngularVelBuffer_.clear();
            rotInitialized_ = true;
        }
        else {
            // Do a trackball rotation based on the mouse movement and the bounding sphere
            // setup earlier.

            Matrix4 camMat = V_.Inverse();
            Point3 eye = camMat.ColumnToPoint3(3);
            
            // last mouse pos
            bool hit1 = false;
            Point3 mouse3D1 = GfxMath::ScreenToNearPlane(V_, Pdraw_, mouseLast_);
            Ray ray1(eye, mouse3D1 - eye);
            float t1;
            Point3 iPoint1;
            if (ray1.IntersectSphere(boundingSphereCtr_, boundingSphereRad_, &t1, &iPoint1)) {
                hit1 = true;
            }
            
            // current mouse pos
            bool hit2 = false;
            Point3 mouse3D2 = GfxMath::ScreenToNearPlane(V_, Pdraw_, mousePos);
            Ray ray2(eye, mouse3D2 - eye);
            float t2;
            Point3 iPoint2;
            if (ray2.IntersectSphere(boundingSphereCtr_, boundingSphereRad_, &t2, &iPoint2)) {
                hit2 = true;
            }
            rotLastIPoint_ = iPoint2;
            
            if (hit1 && hit2) {
                Vector3 v1 = (iPoint1 - boundingSphereCtr_).ToUnit();
                Vector3 v2 = (iPoint2 - boundingSphereCtr_).ToUnit();
                
                rotAxis_ = v1.Cross(v2).ToUnit();
                float angle = std::acos(v1.Dot(v2));

                if (std::isfinite(angle)) {
                    Matrix4 R = Matrix4::Rotation(boundingSphereCtr_, rotAxis_, angle);
                    R = R.Orthonormal();
                    V_ = V_ * R;
                    //V_ = V_.orthonormal();
                
                    // add a sample to the angular vel vector
                    double dt = elapsedTime_ - rotLastTime_;
                    double avel = angle / dt;
                    if (std::isfinite(avel)) {
                        rotAngularVelBuffer_.push_back(std::make_pair(elapsedTime_, avel));
                    }
                    rotLastTime_ = elapsedTime_;
                }
            }
            
            recalc_angular_vel();
        }
    }
    else if (state_ == START) {
        // picked up a little mouse movement after "catching" a spinning model
        // nothing to do, just wait for the button up.
    }
    else {
        std::cerr << "UniCam::OnDrag() unexpected state." << std::endl;
    }
    mouseLast_ = mousePos;
}

void UniCam::OnButtonUp(const Point2 &mousePos) {
    if (state_ == PAN_DOLLY_ROT_DECISION) {
        // here, we got a quick click of the mouse to indicate a center of rotation
        // so we now go into a mode of waiting for a second click to start rotating
        // around that point.
        state_ = ROT_WAIT_FOR_SECOND_CLICK;
    }
    else if (state_ == ROT) {
        showIcon_ = false;
        // if we are leaving the rotation state and the angular velocity is
        // greater than some thresold, then the user has "thrown" the model
        // keep rotating the same way by entering the spinning state.

        recalc_angular_vel();
        //std::cout << "check for spin: " << n-start << " " << rotAngularVel_ << " " << avel2 << std::endl;

        const float threshold = 0.2;
        if (std::fabs(rotAngularVel_) > threshold) {
            state_ = SPINNING;
        }
        else {
            state_ = START;
        }
    }
    else {
        showIcon_ = false;
        // all other cases go back to the start state
        state_ = START;
    }
}

void UniCam::AdvanceAnimation(double dt) {
    elapsedTime_ += dt;
    
    if (state_ == SPINNING) {
        double deltaT = elapsedTime_ - rotLastTime_;
        rotLastTime_ = elapsedTime_;
        double angle = rotAngularVel_ * deltaT;
        Matrix4 R = Matrix4::Rotation(boundingSphereCtr_, rotAxis_, angle);
        //R = R.orthonormal();
        V_ = V_ * R;
    }
}


void UniCam::Draw(const Matrix4 &projectionMatrix) {
    Pdraw_ = projectionMatrix;
    
    if (showIcon_) {
        Matrix4 camMat = V_.Inverse();
        Point3 eye = camMat.ColumnToPoint3(3);
        Vector3 look = -camMat.ColumnToVector3(2);
        float depth = (hitPoint_ - eye).Dot(look);
        Point3 pWorld1 = GfxMath::ScreenToDepthPlane(V_, Pdraw_, Point2(0,0), depth);
        Point3 pWorld2 = GfxMath::ScreenToDepthPlane(V_, Pdraw_, Point2(0.015,0), depth);
        float rad = (pWorld2 - pWorld1).Length();
        Matrix4 M = Matrix4::Translation(hitPoint_ - Point3::Origin()) * Matrix4::Scale(Vector3(rad, rad, rad));
        quickShapes_.DrawSphere(M, V_, Pdraw_, Color(0,0,0));
    }
}


Matrix4 UniCam::view_matrix() {
    return V_;
}

void UniCam::set_view_matrix(Matrix4 viewMatrix) {
    V_ = viewMatrix;
}

void UniCam::set_default_depth(float d) {
    defaultDepth_ = d;
}

Point3 UniCam::eye() {
    Matrix4 camMat = V_.Inverse();
    return camMat.ColumnToPoint3(3);
}

Vector3 UniCam::look() {
    Matrix4 camMat = V_.Inverse();
    return -camMat.ColumnToVector3(2);
}




} // end namespace
