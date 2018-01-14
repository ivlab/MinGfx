#include "matrix4.h"

#include <math.h>

namespace MinGfx {

    
Matrix4::Matrix4() {
    m[0] = m[5]  = m[10] = m[15] = 1.0;
    m[1] = m[2]  = m[3]  = m[4]  = 0.0;
    m[6] = m[7]  = m[8]  = m[9]  = 0.0;
    m[11]= m[12] = m[13] = m[14] = 0.0;
}
  
  
Matrix4::Matrix4(const float* a) { 
    memcpy(m,a,16*sizeof(float));
}

  
Matrix4::Matrix4(const Matrix4& m2) { 
    memcpy(m,m2.m,16*sizeof(float));
}
  
Matrix4::~Matrix4() {
}
  
    
bool Matrix4::operator==(const Matrix4& m2) const {
    for (int i=0;i<16;i++) {
        if (fabs(m2.m[i] - m[i]) > MINGFX_MATH_EPSILON) {
            return false;
        }
    }
    return true;
}

bool Matrix4::operator!=(const Matrix4& m2) const {
    return !(*this == m2);
}
  
Matrix4& Matrix4::operator=(const Matrix4& m2) {
    memcpy(m,m2.m,16*sizeof(float));
    return *this;
}
    
const float * Matrix4::value_ptr() {
    return m;
}

float Matrix4::operator[](const int i) const {
    return m[i];
}

float& Matrix4::operator[](const int i) {
    return m[i];
}
    
float Matrix4::operator()(const int r, const int c) const { 
    return m[c*4+r];
}
  
float& Matrix4::operator()(const int r, const int c) { 
    return m[c*4+r];
}


    
Matrix4 Matrix4::scale(const Vector3& v) {
    return Matrix4::fromRowMajorElements(
        v[0],    0,    0,  0,
           0, v[1],    0,  0,
           0,    0, v[2],  0,
           0,    0,    0,  1
    );
}

    
Matrix4 Matrix4::translation(const Vector3& v) {
    return Matrix4::fromRowMajorElements(
        1, 0, 0, v[0],
        0, 1, 0, v[1],
        0, 0, 1, v[2],
        0, 0, 0,    1
    );
}

    
Matrix4 Matrix4::rotationX(const float radians) {
    const float cosTheta = cos(radians);
    const float sinTheta = sin(radians);
    return Matrix4::fromRowMajorElements(
        1, 0, 0, 0,
        0, cosTheta, -sinTheta, 0,
        0, sinTheta, cosTheta, 0,
        0, 0, 0, 1
    );
}

    
Matrix4 Matrix4::rotationY(const float radians) {
    const float cosTheta = cos(radians);
    const float sinTheta = sin(radians);
    return Matrix4::fromRowMajorElements(
        cosTheta, 0, sinTheta, 0,
        0, 1, 0, 0,
        -sinTheta, 0, cosTheta, 0,
        0, 0, 0, 1
    );
}

    
Matrix4 Matrix4::rotationZ(const float radians) {
    const float cosTheta = cos(radians);
    const float sinTheta = sin(radians);
    return Matrix4::fromRowMajorElements(
        cosTheta, -sinTheta, 0, 0,
        sinTheta, cosTheta, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

    
Matrix4 Matrix4::rotation(const Point3& p, const Vector3& v, const float a) {
    // Translate to origin from point p
    const float vZ = v[2];
    const float vX = v[0];
    const float theta = atan2(vZ, vX);
    const float phi   = -atan2((float)v[1], (float)sqrt(vX * vX + vZ * vZ));

    const Matrix4 transToOrigin = Matrix4::translation(-1.0*Vector3(p[0], p[1], p[2]));
    const Matrix4 A = Matrix4::rotationY(theta);
    const Matrix4 B = Matrix4::rotationZ(phi);
    const Matrix4 C = Matrix4::rotationX(a);
    const Matrix4 invA = Matrix4::rotationY(-theta);
    const Matrix4 invB = Matrix4::rotationZ(-phi);
    const Matrix4 transBack = Matrix4::translation(Vector3(p[0], p[1], p[2]));
  
    return transBack * invA * invB * C * B * A * transToOrigin;
}

    
Matrix4 Matrix4::lookAt(Point3 eye, Point3 target, Vector3 up) {
    Vector3 lookDir = (target - eye).normalize();

    // desired x,y,z for the camera itself
    Vector3 z = -lookDir;
    Vector3 x = up.cross(z).normalize();
    Vector3 y = z.cross(x);

    // for the view matrix rotation, we want the inverse of the rotation for the
    // camera, and the inverse of a rotation matrix is its transpose, so the
    // x,y,z colums become x,y,z rows.
    Matrix4 R = Matrix4::fromRowMajorElements(
        x[0], x[1], x[2], 0,
        y[0], y[1], y[2], 0,
        z[0], z[1], z[2], 0,
        0, 0, 0, 1
    );
    
    // also need to translate by -eye
    Matrix4 T = Matrix4::translation(Point3(0,0,0) - eye);

    return R * T;
}

Matrix4 Matrix4::perspective(float fovyInDegrees, float aspectRatio,
                             float near, float far)
{
    // https://www.khronos.org/opengl/wiki/GluPerspective_code
    float ymax, xmax;
    ymax = near * tanf(fovyInDegrees * M_PI / 360.0);
    // ymin = -ymax;
    // xmin = -ymax * aspectRatio;
    xmax = ymax * aspectRatio;
    return Matrix4::frustum(-xmax, xmax, -ymax, ymax, near, far);
}
    
    
Matrix4 Matrix4::frustum(float left, float right,
                         float bottom, float top,
                         float near, float far)
{
    return Matrix4::fromRowMajorElements(
        2.0*near/(right-left), 0, (right+left)/(right-left), 0,
        0, 2.0*near/(top-bottom), (top+bottom)/(top-bottom), 0,
        0, 0, -(far+near)/(far-near), -2.0*far*near/(far-near),
        0, 0, -1, 0
    );
}

    
Matrix4 Matrix4::fromRowMajorElements(
    const float r1c1, const float r1c2, const float r1c3, const float r1c4,
    const float r2c1, const float r2c2, const float r2c3, const float r2c4,
    const float r3c1, const float r3c2, const float r3c3, const float r3c4,
    const float r4c1, const float r4c2, const float r4c3, const float r4c4)
{
    float m[16];
    m[0]=r1c1; m[4]=r1c2;  m[8]=r1c3; m[12]=r1c4;
    m[1]=r2c1; m[5]=r2c2;  m[9]=r2c3; m[13]=r2c4;
    m[2]=r3c1; m[6]=r3c2; m[10]=r3c3; m[14]=r3c4;
    m[3]=r4c1; m[7]=r4c2; m[11]=r4c3; m[15]=r4c4;
    return Matrix4(m);
}
    

    

Matrix4 Matrix4::orthonormal() const {
    Vector3 x = getColumnAsVector3(0).normalize();
    Vector3 y = getColumnAsVector3(1);
    y = (y - y.dot(x)*x).normalize();
    Vector3 z = x.cross(y).normalize();
    return Matrix4::fromRowMajorElements(
        x[0], y[0], z[0], m[3],
        x[1], y[1], z[1], m[7],
        x[2], y[2], z[2], m[11],
        m[12], m[13], m[14], m[15]
    );
}


Matrix4 Matrix4::transpose() const {
    return Matrix4::fromRowMajorElements(
        m[0], m[1], m[2], m[3],
        m[4], m[5], m[6], m[7],
        m[8], m[9], m[10], m[11],
        m[12], m[13], m[14], m[15]
    );
}

    
    
    
// Returns the determinant of the 3x3 matrix formed by excluding the specified row and column
// from the 4x4 matrix.  The formula for the determinant of a 3x3 is discussed on
// page 705 of Hill & Kelley, but note that there is a typo within the m_ij indices in the 
// equation in the book that corresponds to the cofactor02 line in the code below.
float Matrix4::subDeterminant(int excludeRow, int excludeCol) const {
    // Compute non-excluded row and column indices
    int row[3];
    int col[3];

    int r=0;
    int c=0;
    for (int i=0; i<4; i++) {
        if (i != excludeRow) {
            row[r] = i;
            r++;
        }
        if (i != excludeCol) {
            col[c] = i;
            c++;
        }
    }
  
    // Compute the cofactors of each element in the first row
    float cofactor00 =    (*this)(row[1],col[1]) * (*this)(row[2],col[2])  -  (*this)(row[1],col[2]) * (*this)(row[2],col[1]);
    float cofactor01 = - ((*this)(row[1],col[0]) * (*this)(row[2],col[2])  -  (*this)(row[1],col[2]) * (*this)(row[2],col[0]));
    float cofactor02 =    (*this)(row[1],col[0]) * (*this)(row[2],col[1])  -  (*this)(row[1],col[1]) * (*this)(row[2],col[0]);
  
    // The determinant is then the dot product of the first row and the cofactors of the first row
    return (*this)(row[0],col[0])*cofactor00 + (*this)(row[0],col[1])*cofactor01 + (*this)(row[0],col[2])*cofactor02;
}

// Returns the cofactor matrix.  The cofactor matrix is a matrix where each element c_ij is the cofactor 
// of the corresponding element m_ij in M.  The cofactor of each element m_ij is defined as (-1)^(i+j) times 
// the determinant of the "submatrix" formed by deleting the i-th row and j-th column from M.
// See the definition in section A2.1.4 (page 705) in Hill & Kelley.   
Matrix4 Matrix4::cofactor() const {
    Matrix4 out;
    // We'll use i to incrementally compute -1^(r+c)
    int i = 1;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            // Compute the determinant of the 3x3 submatrix
            float det = subDeterminant(r, c);
            out(r,c) = i * det;
            i = -i;
        }
        i = -i;
    }
    return out;
}

// Returns the determinant of the 4x4 matrix
// See the hint in step 2 in Appendix A2.1.5 (page 706) in Hill & Kelley to learn how to compute this
float Matrix4::determinant() const {
    // The determinant is the dot product of any row of C (the cofactor matrix of m) with the corresponding row of m
    Matrix4 C = cofactor();
    return C(0,0)*(*this)(0,0) + C(0,1)*(*this)(0,1) + C(0,2)*(*this)(0,2) + C(0,3)*(*this)(0,3);
}

// Returns the inverse of the 4x4 matrix if it is nonsingular.  If it is singular, then returns the
// identity matrix. 
Matrix4 Matrix4::inverse() const {
    // Check for singular matrix
    float det = determinant();
    if (fabs(det) < 1e-8) {
        return Matrix4();
    }

    // m in nonsingular, so compute inverse using the 4-step procedure outlined in Appendix A2.1.5
    // (page 706) in Hill & Kelley
    // 1. Find cofactor matrix C
    Matrix4 C = cofactor();
    // 2. Find the determinant of M as the dot prod of any row of C with the corresponding row of M.
    // det = determinant(m);
    // 3. Transpose C to get Ctrans
    Matrix4 Ctrans = C.transpose();
    // 4. Scale each element of Ctrans by (1/det)
    return Ctrans * (1.0f / det);
}


Vector3 Matrix4::getColumnAsVector3(int c) const {
    return Vector3(m[c*4], m[c*4+1], m[c*4+2]);
}

Point3 Matrix4::getColumnAsPoint3(int c) const {
    return Point3(m[c*4], m[c*4+1], m[c*4+2]);
}

    


Matrix4 operator*(const Matrix4& m, const float& s) {
    Matrix4 result;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            result(r,c) = m(r,c) * s;
        }
    }
    return result;
}

Matrix4 operator*(const float& s, const Matrix4& m) {
    return m*s;
}

    
Point3 operator*(const Matrix4& m, const Point3& p) {
	// For our points, p[3]=1 and we don't even bother storing p[3], so need to homogenize
	// by dividing by w before returning the new point.
    const float winv = 1 / (p[0] * m(3,0) + p[1] * m(3,1) + p[2] * m(3,2) + 1.0 * m(3,3));
    return Point3(winv * (p[0] * m(0,0) + p[1] * m(0,1) + p[2] * m(0,2) + 1.0 * m(0,3)),
                  winv * (p[0] * m(1,0) + p[1] * m(1,1) + p[2] * m(1,2) + 1.0 * m(1,3)),
                  winv * (p[0] * m(2,0) + p[1] * m(2,1) + p[2] * m(2,2) + 1.0 * m(2,3)));

}

    
Vector3 operator*(const Matrix4& m, const Vector3& v) {
    // For a vector v[3]=0
    return Vector3(v[0] * m(0,0) + v[1] * m(0,1) + v[2] * m(0,2),
                   v[0] * m(1,0) + v[1] * m(1,1) + v[2] * m(1,2),
                   v[0] * m(2,0) + v[1] * m(2,1) + v[2] * m(2,2));

}


    
Matrix4 operator*(const Matrix4& m1, const Matrix4& m2) {
    Matrix4 m = Matrix4::fromRowMajorElements(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            for (int i = 0; i < 4; i++) {
                m(r,c) += m1(r,i) * m2(i,c);
            }
        }
    }
    return m;
}


std::ostream & operator<< ( std::ostream &os, const Matrix4 &m) {
    // format:  [[r1c1, r1c2, r1c3, r1c4], [r2c1, r2c2, r2c3, r2c4], etc.. ]
    return os << "[[" << m(0,0) << ", " << m(0,1) << ", " << m(0,2) << ", " << m(0,3) << "], "
              << "[" << m(1,0) << ", " << m(1,1) << ", " << m(1,2) << ", " << m(1,3) << "], "
              << "[" << m(2,0) << ", " << m(2,1) << ", " << m(2,2) << ", " << m(2,3) << "], "
              << "[" << m(3,0) << ", " << m(3,1) << ", " << m(3,2) << ", " << m(3,3) << "]]";
}

std::istream & operator>> ( std::istream &is, Matrix4 &m) {
    // format:  [[r1c1, r1c2, r1c3, r1c4], [r2c1, r2c2, r2c3, r2c4], etc.. ]
    char c;
    return is >> c >> c >> m(0,0) >> c >> m(0,1) >> c >> m(0,2) >> c >> m(0,3) >> c >> c
              >> c >> m(1,0) >> c >> m(1,1) >> c >> m(1,2) >> c >> m(1,3) >> c >> c
              >> c >> m(2,0) >> c >> m(2,1) >> c >> m(2,2) >> c >> m(2,3) >> c >> c
              >> c >> m(3,0) >> c >> m(3,1) >> c >> m(3,2) >> c >> m(3,3) >> c >> c;
}

} // end namespace
