//
//  vector.h
//  dreamonway
//
//  Created by Kunal Parihar on 15/02/18.
//  Copyright © 2018 Tabish. All rights reserved.
//

//#ifndef vector_h
//#define vector_h


//#endif /* vector_h */
//
//  vector.h
//  dreamOnGo
//
//  Created by Kunal Parihar on 14/02/18.
//  Copyright © 2018 Tabish. All rights reserved.
//

#ifndef vector_h
#define vector_h


//#endif /* vector_h */
//#ifndef VECTOR_H
//#define VECTOR_H

/*! \brief Vector template class representing a 3 dimensional vector
 *
 *
 *  contains ctors,utility methods for normalizing,scaling,and various vector arithmetic operations
 *
 */

#include <cmath>
template <typename T>
class Vector3d {
public:
    T x;
    T y;
    T z;
    Vector3d();
    Vector3d(T defaultVal);
    Vector3d(T a, T b, T c);
    Vector3d(const Vector3d& a);
    Vector3d& operator=(const Vector3d &a);
    void normalize(); //!<  handles normalization of a vector.
    void scale(T a); //!< scales the vector by specified value.
    void add(const Vector3d& a); //!< utility method for performing vector addition.
    void subtract(const Vector3d& a); //!< utility method for performing vector subtraction
    T dot(const Vector3d& a); //!< utility method for performing vector dot multiplication
    
    
    T mod(); //!< returns the magnitude of the vector
    
    //!< utility for changing coordinates of vector to user specified parameters.
    /*!
     \param a,representing the x coordinate
     \param b,representing the y coordinate
     \param c,representing the z coordinate
     */
    
    void comp(T& a, T& b, T& c);
    
    //!< utility for setting vector's coordinates to coordinates of user specified vector
    /*!
     \param a,a Vector3d object
     \sa comp(T& a,T& b,T& c), dot(const Vector3d& a), subtract(const Vector3d& a),add(const Vector3d& a)
     */
    
    void comp(Vector3d& a); };

template <typename T>
Vector3d<T>::Vector3d()
{
    x = y = z = 0;
}

template <typename T>
Vector3d<T>::Vector3d(T defaultVal)
{
    x = y = z = defaultVal;
}

template <typename T>
Vector3d<T>::Vector3d(T a, T b, T c)
{
    x = a;
    y = b;
    z = c;
}

template <typename T>
Vector3d<T>::Vector3d(const Vector3d& a)
{
    x = a.x;
    y = a.y;
    z = a.z;
}

template<typename T>
Vector3d<T>& Vector3d<T>::operator=(const Vector3d& a)
{
    x = a.x;
    y = a.y;
    z = a.z;
    return *this;
}

template <typename T>
void Vector3d<T>::normalize()
{
    T tmp = sqrt(x * x + y * y + z * z);
    if(tmp == 0)
        return;
    x /= tmp;
    y /= tmp;
    z /= tmp;
}

template <typename T>
void Vector3d<T>::scale(T a)
{
    x *= a;
    y *= a;
    z *= a;
}

template <typename T>
void Vector3d<T>::add(const Vector3d& a)
{
    x += a.x;
    y += a.y;
    z += a.z;
}

template <typename T>
void Vector3d<T>::subtract(const Vector3d& a)
{
    x -= a.x;
    y -= a.y;
    z -= a.z;
}

template <typename T>
T Vector3d<T>::dot(const Vector3d& a)
{
    return (x * a.x + y * a.y + z * a.z);
}

template <typename T>
T Vector3d<T>::mod()
{
    return sqrt(x * x + y * y + z * z);
}

template <typename T>
void Vector3d<T>::comp(T& a, T& b, T& c)
{
    a = x;
    b = y;
    c = z;
}

template<typename T>
void Vector3d<T>::comp(Vector3d& a)
{
    a.x = x;
    a.y = y;
    a.z = z;
}
#endif // VECTOR_H
