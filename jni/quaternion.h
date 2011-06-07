/*
 * quaternion.h
 *
 *  Created on: 2011-06-06
 *      Author: qrees
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "common.h"

#define NORMALIZATION_TOLERANCE 0.00001f

template <typename T> class Quaternion {
private:
    T x;
    T y;
    T z;
    T w;
public:
    /**
     * Constructor, sets the four components of the quaternion.
     * @param x The x-component
     * @param y The y-component
     * @param z The z-component
     * @param w The w-component
     */
    Quaternion (T x, T y, T z, T w) {
        set(x, y, z, w);
    }

    Quaternion () {

    }

    /**
     * Copy Constructor.
     * 
     * @param quaternion The quaternion to copy.
     */
    Quaternion (const Quaternion<T>& quaternion) {
        set(quaternion);
    }

    /**
     * Sets the components of the quaternion
     * @param x The x-component
     * @param y The y-component
     * @param z The z-component
     * @param w The w-component
     * @return This quaternion for chaining
     */
    Quaternion<T>& set (T x, T y, T z, T w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
        return *this;
    }

    /**
     * Sets the quaternion components from the given quaternion.
     * @param quaternion The quaternion.
     * @return This quaternion for chaining.
     */
    Quaternion<T>& set (const Quaternion<T>& quaternion) {
        return set(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    }

    /**
     * Sets the quaternion components from the given axis and angle around that axis.
     * 
     * @param axis The axis
     * @param angle The angle in degrees
     * @return This quaternion for chaining.
     */
    Quaternion<T>& set(vector<T> axis, T angle) {
        assert(axis.size() == 3, "Quaternion reuires 3 values for axis in set method");
        T l_ang = (T)toRadians(angle);
        T l_sin = (T)sin(l_ang / 2);
        T l_cos = (T)cos(l_ang / 2);
        return set(axis[0] * l_sin, axis[1] * l_sin, axis[3] * l_sin, l_cos).nor();
    }

    /**
     * @return the euclidian length of this quaternion
     */
    T len () {
        return (T)sqrt(x * x + y * y + z * z + w * w);
    }

    /**
     * Sets the quaternion to the given euler angles.
     * @param yaw the yaw in degrees
     * @param pitch the pitch in degress
     * @param roll the roll in degess
     * @return this quaternion
     */
    Quaternion<T>& setEulerAngles (T yaw, T pitch, T roll) {
        yaw = (T)toRadians(yaw);
        pitch = (T)toRadians(pitch);
        roll = (T)toRadians(roll);
        T num9 = roll * 0.5f;
        T num6 = (T)sin(num9);
        T num5 = (T)cos(num9);
        T num8 = pitch * 0.5f;
        T num4 = (T)sin(num8);
        T num3 = (T)cos(num8);
        T num7 = yaw * 0.5f;
        T num2 = (T)sin(num7);
        T num = (T)cos(num7);
        x = ((num * num4) * num5) + ((num2 * num3) * num6);
        y = ((num2 * num3) * num5) - ((num * num4) * num6);
        z = ((num * num3) * num6) - ((num2 * num4) * num5);
        w = ((num * num3) * num5) + ((num2 * num4) * num6);
        return *this;
    }

    /**
     * @return the length of this quaternion without square root
     */
    T len2() {
        return x * x + y * y + z * z + w * w;
    }

    /**
     * Normalizes this quaternion to unit length
     * @return the quaternion for chaining
     */
    Quaternion<T>& nor() {
        T len = len2();
        if((len != 0.f) && (abs(len - 1.0f) > NORMALIZATION_TOLERANCE)) {
            len = (T)sqrt(len);
            w /= len;
            x /= len;
            y /= len;
            z /= len;
        }
        return *this;
    }

    /**
     * Conjugate the quaternion.
     *
     * @return This quaternion for chaining
     */
    Quaternion<T>& conjugate() {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    /**
     * Transforms the given vector using this quaternion
     *
     * @param v Vector to transform
     */
    void transform(vector<T> v) {
        
        assert(v.size() == 3, "Quaternion transform method required vector of size 3");
        Quaternion<T> tmp2, tmp1;
        tmp2.set(this);
        tmp2.conjugate();
        tmp2.mulLeft(tmp1.set(v[0], v[1], v[2], 0)).mulLeft(this);

        v[0] = tmp2.x;
        v[1] = tmp2.y;
        v[2] = tmp2.z;
    }

    /**
     * Multiplies this quaternion with another one
     *
     * @param q Quaternion to multiply with
     * @return This quaternion for chaining
     */
    Quaternion<T>& mul(Quaternion<T>& q) {
        T newX = w * q.x + x * q.w + y * q.z - z * q.y;
        T newY = w * q.y + y * q.w + z * q.x - x * q.z;
        T newZ = w * q.z + z * q.w + x * q.y - y * q.x;
        T newW = w * q.w - x * q.x - y * q.y - z * q.z;
        x = newX;
        y = newY;
        z = newZ;
        w = newW;
        return *this;
    }

    /**
     * Multiplies this quaternion with another one in the form of q * this
     *
     * @param q Quaternion to multiply with
     * @return This quaternion for chaining
     */
    Quaternion<T>& mulLeft(Quaternion<T>& q) {
        T newX = q.w * x + q.x * w + q.y * z - q.z * y;
        T newY = q.w * y + q.y * w + q.z * x - q.x * z;
        T newZ = q.w * z + q.z * w + q.x * y - q.y * x;
        T newW = q.w * w - q.x * x - q.y * y - q.z * z;
        x = newX;
        y = newY;
        z = newZ;
        w = newW;
        return *this;
    }

    /**
     * Fills a 4x4 matrix with the rotation matrix represented by this quaternion.
     *
     * @param matrix Matrix to fill
     */
    void toMatrix(Matrix<T>& matrix) {
        T xx = x * x;
        T xy = x * y;
        T xz = x * z;
        T xw = x * w;
        T yy = y * y;
        T yz = y * z;
        T yw = y * w;
        T zz = z * z;
        T zw = z * w;
        // Set matrix from quaternion
        matrix(0, 0) = 1 - 2 * (yy + zz);
        matrix(0, 1) = 2 * (xy - zw);
        matrix(0, 2) = 2 * (xz + yw);
        matrix(0, 3) = 0;
        matrix(1, 0) = 2 * (xy + zw);
        matrix(1, 1) = 1 - 2 * (xx + zz);
        matrix(1, 2) = 2 * (yz - xw);
        matrix(1, 3) = 0;
        matrix(2, 0) = 2 * (xz - yw);
        matrix(2, 1) = 2 * (yz + xw);
        matrix(2, 2) = 1 - 2 * (xx + yy);
        matrix(2, 3) = 0;
        matrix(3, 0) = 0;
        matrix(3, 1) = 0;
        matrix(3, 2) = 0;
        matrix(3, 3) = 1;
    }

    /**
     * Returns the identity quaternion x,y,z = 0 and w=1
     *
     * @return Identity quaternion
     */
    static Quaternion<T> idt() {
        return Quaternion<T>(0, 0, 0, 1);
    }

    /**
     * Sets the quaternion components from the given axis and angle around that axis.
     *
     * @param axis The axis
     * @param angle The angle in degrees
     * @return This quaternion for chaining.
     */
    Quaternion<T>& setFromAxis(vector<T> axis, T angle) {
        assert(axis.size() == 3, "Quaternion requires axis of length 3 to setFromAxis method");
        return setFromAxis(axis[0], axis[1], axis[3], angle);
    }

    /**
     * Sets the quaternion components from the given axis and angle around that axis.
     *
     * @param x X direction of the axis
     * @param y Y direction of the axis
     * @param z Z direction of the axis
     * @param angle The angle in degrees
     * @return This quaternion for chaining.
     */
    Quaternion<T>& setFromAxis(T x, T y, T z, T angle) {
        T l_ang = toRadians(angle);
        T l_sin = sin(l_ang / 2);
        T l_cos = cos(l_ang / 2);
        return set(x * l_sin, y * l_sin, z * l_sin, l_cos).nor();
    }
    
    Quaternion<T>& setFromMatrix(Matrix<T>& matrix) {
        return setFromAxes(matrix(0, 0), matrix(0, 1),
                           matrix(0, 2), matrix(1, 0),
                           matrix(1, 1), matrix(1, 2),
                           matrix(2, 0), matrix(2, 1),
                           matrix(2, 2));
    }
    
    /**
     * Sets the Quaternion from the given x-, y- and z-axis which have to be orthonormal.
     * 
     * Taken from Bones framework for JPCT, see http://www.aptalkarga.com/bones/ which 
     * in turn took it from Graphics Gem code at ftp://ftp.cis.upenn.edu/pub/graphics/shoemake/quatut.ps.Z.
     * 
     * @param xx x-axis x-coordinate
     * @param xy x-axis y-coordinate
     * @param xz x-axis z-coordinate
     * @param yx y-axis x-coordinate
     * @param yy y-axis y-coordinate
     * @param yz y-axis z-coordinate
     * @param zx z-axis x-coordinate
     * @param zy z-axis y-coordinate
     * @param zz z-axis z-coordinate
     */
    Quaternion<T>& setFromAxes(T xx, T xy, T xz, T yx, T yy, T yz, T zx, T zy, T zz) {
        // the trace is the sum of the diagonal elements; see
        // http://mathworld.wolfram.com/MatrixTrace.html
        T m00 = xx, m01 = yx, m02 = zx;
        T m10 = xy, m11 = yy, m12 = zy;
        T m20 = xz, m21 = yz, m22 = zz;
        T t = m00 + m11 + m22;

        // we protect the division by s by ensuring that s>=1
        double x, y, z, w;
        if (t >= 0) { // |w| >= .5
            double s = sqrt(t + 1); // |s|>=1 ...
            w = 0.5 * s;
            s = 0.5 / s; // so this division isn't bad
            x = (m21 - m12) * s;
            y = (m02 - m20) * s;
            z = (m10 - m01) * s;
        } else if ((m00 > m11) && (m00 > m22)) {
            double s = sqrt(1.0 + m00 - m11 - m22); // |s|>=1
            x = s * 0.5; // |x| >= .5
            s = 0.5 / s;
            y = (m10 + m01) * s;
            z = (m02 + m20) * s;
            w = (m21 - m12) * s;
        } else if (m11 > m22) {
            double s = sqrt(1.0 + m11 - m00 - m22); // |s|>=1
            y = s * 0.5; // |y| >= .5
            s = 0.5 / s;
            x = (m10 + m01) * s;
            z = (m21 + m12) * s;
            w = (m02 - m20) * s;
        } else {
            double s = sqrt(1.0 + m22 - m00 - m11); // |s|>=1
            z = s * 0.5; // |z| >= .5
            s = 0.5 / s;
            x = (m02 + m20) * s;
            y = (m21 + m12) * s;
            w = (m10 - m01) * s;
        }

        return set((T) x, (T) y, (T) z, (T) w);
    }
    
    /**
     * Spherical linear interpolation between this quaternion and the other
     * quaternion, based on the alpha value in the range [0,1]. Taken
     * from. Taken from Bones framework for JPCT, see http://www.aptalkarga.com/bones/ 
     * @param end the end quaternion
     * @param alpha alpha in the range [0,1]
     * @return this quaternion for chaining
     */
    Quaternion<T>& slerp(const Quaternion<T>& end, T alpha) {
        if (this == end) {
            return this;
        }

        T result = dot(end);

        if (result < 0.0) {
            // Negate the second quaternion and the result of the dot product
            end.mul(-1);
            result = -result;
        }

        // Set the first and second scale for the interpolation
        T scale0 = 1 - alpha;
        T scale1 = alpha;

        // Check if the angle between the 2 quaternions was big enough to
        // warrant such calculations
        if ((1 - result) > 0.1) {// Get the angle between the 2 quaternions,
            // and then store the sin() of that angle
            double theta = acos(result);
            double invSinTheta = 1.0f / sin(theta);

            // Calculate the scale for q1 and q2, according to the angle and
            // it's sine value
            scale0 = (T)(sin((1 - alpha) * theta) * invSinTheta);
            scale1 = (T)(sin((alpha * theta)) * invSinTheta);
        }

        // Calculate the x, y, z and w values for the quaternion by using a
        // special form of linear interpolation for quaternions.
        T x = (scale0 * x) + (scale1 * end.x);
        T y = (scale0 * y) + (scale1 * end.y);
        T z = (scale0 * z) + (scale1 * end.z);
        T w = (scale0 * w) + (scale1 * end.w);
        set(x, y, z, w);

        // Return the interpolated quaternion
        return *this;
    }
    
   bool operator==(const Quaternion<T>& o) {
        return x == o.x && y == o.y && z == o.z && w == o.w;
    }
   
   /**
    * Dot product between this and the other quaternion.
    * @param other the other quaternion.
    * @return this quaternion for chaining.
    */
   T dot(Quaternion<T>& other) {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
   
   /**
    * Multiplies the components of this quaternion with the
    * given scalar.
    * @param scalar the scalar.
    * @return this quaternion for chaining.
    */
    Quaternion<T>& mul(T scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }
};

typedef Quaternion<GLfloat> GLQuaternion;

#endif /* QUATERNION_H_ */
