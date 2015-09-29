#ifndef __QROX_VECTOR_H__
#define __QROX_VECTOR_H__

#include <algorithm>
#include <cmath>
#include <type_traits>
#include "types.h"

template <typename T, u32 D>
class vec;

template <typename T, u32 D>
class __general_vector {
protected:
    __general_vector();

    T v[D];

public:
    __general_vector(T all);
    __general_vector(T const * _v);

    operator T const *() const;
    vec<T, D> operator +(__general_vector<T, D> const & o) const;
    vec<T, D> operator -(__general_vector<T, D> const & o) const;
    vec<T, D> operator *(T t) const;
    vec<T, D> operator /(T t) const;
    vec<T, D> operator -() const;

    vec<T, D> normalize() const;
    T dot(__general_vector<T, D> const & o) const;
    T sqr() const;
    T mod() const;
};

template <typename T, u32 D>
class vec : public __general_vector<T, D> {
public:
    vec(T all = 0);
    vec(T const * v);
};

template <typename T>
class vec<T, 2> : public __general_vector<T, 2> {
public:
    vec(T all = 0);
    vec(T const * v);
    vec(T v0, T v1);

    T cross(vec<T, 2> const & o) const;

    T x() const;
    T y() const;
};

template <typename T>
class vec<T, 3> : public __general_vector<T, 3> {
public:
    vec(T all = 0);
    vec(T const * v);
    vec(T v0, T v1, T v2);

    vec<T, 3> cross(vec<T, 3> const & o) const;

    vec<T, 3> rotate(vec<T, 3> const & axis) const;

    T x() const;
    T y() const;
    T z() const;
};

// for use to derived classes only
template <typename T, u32 D>
__general_vector<T, D>::__general_vector() {
}

template <typename T, u32 D>
__general_vector<T, D>::__general_vector(T all) {
    std::fill(v, v + D, all);
}

template <typename T, u32 D>
__general_vector<T, D>::__general_vector(T const * _v) {
    std::copy(_v, _v + D, v);
}

template <typename T, u32 D>
__general_vector<T, D>::operator const T *() const {
    return v;
}

template <typename T, u32 D>
vec<T, D> __general_vector<T, D>::operator +(__general_vector<T, D> const & o) const {
    T res[D];
    for (u32 i = 0; i < D; ++i) {
        res[i] = v[i] + o.v[i];
    }
    return vec<T, D>(res);
}

template <typename T, u32 D>
vec<T, D> __general_vector<T, D>::operator -(__general_vector<T, D> const & o) const {
    T res[D];
    for (u32 i = 0; i < D; ++i) {
        res[i] = v[i] - o.v[i];
    }
    return vec<T, D>(res);
}

template <typename T, u32 D>
vec<T, D> __general_vector<T, D>::operator *(T t) const {
    T res[D];
    for (u32 i = 0; i < D; ++i) {
        res[i] = v[i] * t;
    }
    return vec<T, D>(res);
}

template <typename T, u32 D>
vec<T, D> __general_vector<T, D>::operator /(T t) const {
    T res[D];
    for (u32 i = 0; i < D; ++i) {
        res[i] = v[i] / t;
    }
    return vec<T, D>(res);
}

template <typename T, u32 D>
vec<T, D> __general_vector<T, D>::operator -() const {
    T res[D];
    for (u32 i = 0; i < D; ++i) {
        res[i] = -v[i];
    }
    return vec<T, D>(res);
}

template <typename T, u32 D>
vec<T, D> __general_vector<T, D>::normalize() const {
    return operator /(mod());
}

template <typename T, u32 D>
T __general_vector<T, D>::dot(__general_vector<T, D> const & o) const {
    T res = 0;
    for (u32 i = 0; i < D; ++i) {
        res += v[i] * o.v[i];
    }
    return res;
}

template <typename T, u32 D>
T __general_vector<T, D>::sqr() const {
    return dot(*this);
}

template <typename T, u32 D>
T __general_vector<T, D>::mod() const {
    return sqrt(dot(*this));
}

template <typename T, u32 D>
vec<T, D>::vec(T all) : __general_vector<T, D>(all) {
}

template <typename T>
vec<T, 2>::vec(T all) : __general_vector<T, 2>(all) {
}

template <typename T>
vec<T, 3>::vec(T all) : __general_vector<T, 3>(all) {
}

template <typename T, u32 D>
vec<T, D>::vec(T const * v) : __general_vector<T, D>(v) {
}

template <typename T>
vec<T, 2>::vec(T const * v) : __general_vector<T, 2>(v) {
}

template <typename T>
vec<T, 3>::vec(T const * v) : __general_vector<T, 3>(v) {
}

template <typename T>
vec<T, 2>::vec(T v0, T v1) {
    this->v[0] = v0;
    this->v[1] = v1;
}

template <typename T>
T vec<T, 2>::cross(vec<T, 2> const & that) const {
    return this->v[0] * that.v[1] - this->v[1] * that.v[0];
}

template <typename T>
T vec<T, 2>::x() const {
    return this->v[0];
}

template <typename T>
T vec<T, 2>::y() const {
    return this->v[1];
}

template <typename T>
vec<T, 3>::vec(T v0, T v1, T v2) {
    this->v[0] = v0;
    this->v[1] = v1;
    this->v[2] = v2;
}

template <typename T>
vec<T, 3> vec<T, 3>::cross(vec<T, 3> const & that) const {
    return vec<T, 3>(this->v[1] * that.v[2] - this->v[2] * that.v[1],
                        this->v[2] * that.v[0] - this->v[0] * that.v[2],
                        this->v[0] * that.v[1] - this->v[1] * that.v[0]);
}

template <typename T>
vec<T, 3> vec<T, 3>::rotate(vec<T, 3> const & axis) const {
    //todo: when T is integral rather than float?
    T angle = axis.mod();
    if (angle == 0) return *this;
    vec<T, 3> eaxis = axis / angle;
    vec<T, 3> vph = eaxis.cross(*this);
    return (*this + vph * sin(angle) + vph.cross(eaxis) * (cos(angle) - 1));
}

template <typename T>
T vec<T, 3>::x() const {
    return this->v[0];
}

template <typename T>
T vec<T, 3>::y() const {
    return this->v[1];
}

template <typename T>
T vec<T, 3>::z() const {
    return this->v[2];
}

#endif
