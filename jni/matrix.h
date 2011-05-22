/*
 * matrix.h
 *
 *  Created on: 2011-05-21
 *      Author: qrees
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <math.h>
#include <vector>

#define PI 3.1415926535897932384626433832795f

template<typename T> class Matrix:public RefCntObject {
    typedef AutoPtr< Matrix<T> > AMatrix;
private:
    T *_matrix;
public:
    Matrix(){
        LOGI("Matrix: new()");
        _matrix = new T[16];
    }
    ~Matrix(){
        LOGI("Matrix: delete");
        delete[] _matrix;
    }
    Matrix(const Matrix<T> & other){
        LOGI("Matrix: copy");
        _matrix = new T[16];
        mamcpy(_matrix, other._matrix, 16 * sizeof(T));
    }

    Matrix<T>& operator= (const Matrix<T> &other) {
        LOGI("Matrix: operator= ");
        for (int i = 0; i < 16; i++) {
            _matrix[i] = other._matrix[i];
        }
        return *this;
    }

    Matrix<T>& operator= (const T *other) {
        for (int i = 0; i < 16; i++) {
            _matrix[i] = other[i];
        }
        return *this;
    }
    
    bool operator== (const Matrix<T> &other) {
        for (int i = 0; i < 16; i++) {
            if (_matrix[i] != other._matrix[i]) {
                    return false;
            }
        }
        return true;
    }
    
    const Matrix<T> operator* (const Matrix<T> &other){
        LOGI("Matrix: operator* ");
        Matrix<T> dest = *this;
        dest._multiply(&other);
        LOGI("Matrix: end operator* ");
        return dest;
    }

    Matrix<T>& identity(){
        memset(_matrix, 0x0, 4*4*sizeof(T));
        _matrix[0] = _matrix[5] = _matrix[10] = _matrix[15] = 1;
        return *this;
    }

    Matrix<T>& scale(T sx, T sy, T sz) {
        _matrix[0 * 4 + 0] *= sx;
        _matrix[0 * 4 + 1] *= sx;
        _matrix[0 * 4 + 2] *= sx;
        _matrix[0 * 4 + 3] *= sx;

        _matrix[1 * 4 + 0] *= sy;
        _matrix[1 * 4 + 1] *= sy;
        _matrix[1 * 4 + 2] *= sy;
        _matrix[1 * 4 + 3] *= sy;

        _matrix[2 * 4 + 0] *= sz;
        _matrix[2 * 4 + 1] *= sz;
        _matrix[2 * 4 + 2] *= sz;
        _matrix[2 * 4 + 3] *= sz;
        return *this;
    }

    Matrix<T>& translate(T tx, T ty, T tz) {
        _matrix[3 * 4 + 0] += (_matrix[0 * 4 + 0] * tx + _matrix[1 * 4 + 0] * ty + _matrix[2 * 4 + 0] * tz);
        _matrix[3 * 4 + 1] += (_matrix[0 * 4 + 1] * tx + _matrix[1 * 4 + 1] * ty + _matrix[2 * 4 + 1] * tz);
        _matrix[3 * 4 + 2] += (_matrix[0 * 4 + 2] * tx + _matrix[1 * 4 + 2] * ty + _matrix[2 * 4 + 2] * tz);
        _matrix[3 * 4 + 3] += (_matrix[0 * 4 + 3] * tx + _matrix[1 * 4 + 3] * ty + _matrix[2 * 4 + 3] * tz);
        return *this;
    }

    Matrix<T>& rotation(T angle, T x, T y, T z){
        T sinAngle = sinf(angle * PI / 180.0f);
        T cosAngle = cosf(angle * PI / 180.0f);
        T oneMinusCos = 1.0f - cosAngle;
        T mag = sqrtf(x * x + y * y + z * z);
        
        if (mag != 0.0f && mag != 1.0f) {
            x /= mag;
            y /= mag;
            z /= mag;
        }
        
        T xx = x * x;
        T yy = y * y;
        T zz = z * z;
        T xy = x * y;
        T yz = y * z;
        T zx = z * x;
        T xs = x * sinAngle;
        T ys = y * sinAngle;
        T zs = z * sinAngle;
        
        _matrix[0] = (oneMinusCos * xx) + cosAngle;
        _matrix[1] = (oneMinusCos * xy) - zs;
        _matrix[2] = (oneMinusCos * zx) + ys;
        _matrix[3] = 0.0f; 
        
        _matrix[4] = (oneMinusCos * xy) + zs;
        _matrix[5] = (oneMinusCos * yy) + cosAngle;
        _matrix[6] = (oneMinusCos * yz) - xs;
        _matrix[7] = 0.0f;
        
        _matrix[8] = (oneMinusCos * zx) - ys;
        _matrix[9] = (oneMinusCos * yz) + xs;
        _matrix[10] = (oneMinusCos * zz) + cosAngle;
        _matrix[11] = 0.0f; 
        
        _matrix[12] = 0.0f;
        _matrix[13] = 0.0f;
        _matrix[14] = 0.0f;
        _matrix[15] = 1.0f;
        return *this;
    }

    Matrix<T>& rotate(T angle, T x, T y, T z) {
        Matrix<T> rot_mat = Matrix<T>().rotation(angle, x, y,z);
        _multiply(rot_mat);
        return *this;
    }

    Matrix<T>& frustum(T left, T right, T bottom, T top, T nearZ, T farZ) {
        T deltaX = right - left;
        T deltaY = top - bottom;
        T deltaZ = farZ - nearZ;

        if ((nearZ <= 0.0f) || (farZ <= 0.0f) || (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
            return;

        _matrix[0 * 4 + 0] = 2.0f * nearZ / deltaX;
        _matrix[0 * 4 + 1] = _matrix[0 * 4 + 2] = _matrix[0 * 4 + 3] = 0.0f;

        _matrix[1 * 4 + 1] = 2.0f * nearZ / deltaY;
        _matrix[1 * 4 + 0] = _matrix[1 * 4 + 2] = _matrix[1 * 4 + 3] = 0.0f;

        _matrix[2 * 4 + 0] = (right + left) / deltaX;
        _matrix[2 * 4 + 1] = (top + bottom) / deltaY;
        _matrix[2 * 4 + 2] = -(nearZ + farZ) / deltaZ;
        _matrix[2 * 4 + 3] = -1.0f;

        _matrix[3 * 4 + 2] = -2.0f * nearZ * farZ / deltaZ;
        _matrix[3 * 4 + 0] = _matrix[3 * 4 + 1] = _matrix[3 * 4 + 3] = 0.0f;
        return *this;
    }

    Matrix<T>& perspective(T fovy, T aspect, T nearZ, T farZ) {
        T frustumW, frustumH;

        frustumH = (T) tanf(fovy / 360.0 * PI) * nearZ;
        frustumW = frustumH * aspect;

        frustum(-frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
        return *this;
    }

    Matrix<T>& ortho(T left, T right, T bottom, T top, T nearZ, T farZ) {
        T deltaX = right - left;
        T deltaY = top - bottom;
        T deltaZ = farZ - nearZ;
        identity();

        if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f)){
            LOGI("Matrix: Invalid ortho arguments");
            return;
        }

        _matrix[0 * 4 + 0] = 2.0f / deltaX;
        _matrix[3 * 4 + 0] = -(right + left) / deltaX;
        _matrix[1 * 4 + 1] = 2.0f / deltaY;
        _matrix[3 * 4 + 1] = -(top + bottom) / deltaY;
        _matrix[2 * 4 + 2] = -2.0f / deltaZ;
        _matrix[3 * 4 + 2] = -(nearZ + farZ) / deltaZ;
        return *this;
    }

    void _multiply(Matrix<T> &matB) {
        int i;
        T*tmp = new T[16];
        
        for (i = 0; i < 4; i++) {
            tmp[i * 4 + 0] = (_matrix[i * 4 + 0] * matB._matrix[0 * 4 + 0])
                            + (_matrix[i * 4 + 1] * matB._matrix[1 * 4 + 0])
                            + (_matrix[i * 4 + 2] * matB._matrix[2 * 4 + 0])
                            + (_matrix[i * 4 + 3] * matB._matrix[3 * 4 + 0]);

            tmp[i * 4 + 1] = (_matrix[i * 4 + 0] * matB._matrix[0 * 4 + 1])
                            + (_matrix[i * 4 + 1] * matB._matrix[1 * 4 + 1])
                            + (_matrix[i * 4 + 2] * matB._matrix[2 * 4 + 1])
                            + (_matrix[i * 4 + 3] * matB._matrix[3 * 4 + 1]);

            tmp[i * 4 + 2] = (_matrix[i * 4 + 0] * matB._matrix[0 * 4 + 2])
                            + (_matrix[i * 4 + 1] * matB._matrix[1 * 4 + 2])
                            + (_matrix[i * 4 + 2] * matB._matrix[2 * 4 + 2])
                            + (_matrix[i * 4 + 3] * matB._matrix[3 * 4 + 2]);

            tmp[i * 4 + 3] = (_matrix[i * 4 + 0] * matB._matrix[0 * 4 + 3])
                            + (_matrix[i * 4 + 1] * matB._matrix[1 * 4 + 3])
                            + (_matrix[i * 4 + 2] * matB._matrix[2 * 4 + 3])
                            + (_matrix[i * 4 + 3] * matB._matrix[3 * 4 + 3]);
        }
        delete[] _matrix;
        _matrix = tmp;
    }

    vector<T> multiply(const vector<T> srcB) {
        assert(srcB.size() == 4, "Matrix: Vector size must be equal to 4");
        
        vector<T> tmp(4);
        for (int i = 0; i < 4; i++) {
            tmp[i] =  _matrix[i]     * srcB[0] +
                     _matrix[i + 4]  * srcB[1] +
                     _matrix[i + 8]  * srcB[2] +
                     _matrix[i + 12] * srcB[3];
        }
        return tmp;
    }

    #define _M(i,j) _matrix[i*4+j]
    T& operator()(int i, int j){
        return _M(i,j);
    }

    T operator()(int i, int j) const {
        return _M(i,j);
    }


    Matrix<T>& inverse() {
        int swap;
        T t;
        T temp[4][4];
        
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                temp[i][j] = _M(i, j);
            }
        }
        
        identity();
        
        for (int i = 0; i < 4; i++) {
            swap = i;
            for (int j = i + 1; j < 4; j++) {
                if (fabs(temp[j][i]) > fabs(temp[i][i])) {
                    swap = j;
                }
            }
                
            if (swap != i) {
                for (int k = 0; k < 4; k++) {
                    t = temp[i][k];
                    temp[i][k] = temp[swap][k];
                    temp[swap][k] = t;
                    
                    t = _M(i, k);
                    _M(i, k) = _M(swap, k);
                    _M(swap, k) = t;
                }
            }
            if (temp[i][i] == 0) {
                LOGI("Matrix: Matrix is singular, cannot invert.");
                return;
            }
            t = temp[i][i];
            for (int k = 0; k < 4; k++) {
                temp[i][k] /= t;
                _M(i, k) /= t;
            }
            for (int j = 0; j < 4; j++) {
                if (j != i) {
                    t = temp[j][i];
                    for (int k = 0; k < 4; k++) {
                        temp[j][k] -= temp[i][k] * t;
                        _M(j, k) -= _M(i, k) * t;
                    }
                }
            }
        }
    }

};


#endif /* MATRIX_H_ */
