#ifndef __QROX_ADVANCED_MATH_H__
#define __QROX_ADVANCED_MATH_H__

#include <cmath>

double constexpr pi = 3.14159265358979323846;

template <typename T>
void solve_quadric_equation(T a, T b, T c, T * res) {
    if (a == 0) {
        res[0] = -c / b;
        res[1] = NAN;
    } else {
        T delta = b * b - a * c * 4;
        if (delta > 0) {
            T sqrtdelta = sqrt(delta);
            res[0] = (sqrtdelta + b) / a * -.5;
            res[1] = (sqrtdelta - b) / a * .5;
        } else if (delta == 0) {
            res[0] = b / a * -.5;
            res[1] = NAN;
        } else {
            res[0] = res[1] = NAN;
        }
    }
}

/*
 *        b³    bc    d
 * α = - ──── + ─── - ──
 *       27a³   9a²   3a
 *
 *       b²    c
 * β = - ─── + ──
 *       9a²   3a
 *
 * Δ = α² + β³
 *
 *                  b      ,─────;==     ,─────;==
 * if Δ > 0, x0 = - ── + ³√ α + √ Δ  + ³√ α - √ Δ , x1 & x2 are conjugate complex roots
 *                  3a
 *
 *                          b        _               b      _
 * if Δ = 0, α != 0, x0 = - ── + 2 ³√α , x1 = x2 = - ── - ³√α
 *                          3a                      3a
 *
 *                                   b
 * if Δ = 0, α = 0, x0 = x1 = x2 = - ──
 *                                   3a
 *
 *                              ┌       α   ┐                              ┌       α        ┐
 *                              │acos───────│                              │acos─────── ± 2π│
 *                  b      __   │    (-β)³'²│                  b      __   │    (-β)³'²     │
 * if Δ < 0, x0 = - ── + 2√-βcos│───────────│, x2, x3 = x0 = - ── + 2√-βcos│────────────────│
 *                  3a          └     3     ┘                              └        3       ┘
 */
template <typename T>
void solve_cubic_equation(T k0, T k1, T k2, T k3, T * res) {
    if (k0 == 0) {
        solve_quadric_equation(k1, k2, k3, res);
        res[2] = NAN;
    } else {
        T a, b, d; // alpha, beta, DELTA
        k0 *= 3.;
        k1 /= k0; k2 /= k0; k3 /= k0;
        b = -k1 * k1 + k2;
        a = b * k1 - k3;
        d = a * a + b * b * b;
        if (d > 0) {
            T sqrtd = sqrt(d);
            res[0] = pow(a + sqrtd, 1. / 3.) + pow(a - sqrtd, 1. / 3.) - k1;
            res[1] = res[2] = NAN;                  // conjugate complex root
        } else if (d == 0) {
            if (a != 0) {
                res[0] = pow(a, 1. / 3.) * 2 - k1;
                res[1] = -pow(a, 1. / 3.) - k1;     // double real root
                res[2] = NAN;
            } else {
                res[0] = -k1;                       // triple real root
                res[1] = res[2] = NAN;
            }
        } else {
            T sqrtnb = sqrt(-b), sqrtnb2 = sqrtnb * 2.;
            T acosblah = acos(a / (sqrtnb * sqrtnb * sqrtnb)) / 3.;
            res[0] = sqrtnb2 * cos(acosblah) - k1;
            res[1] = sqrtnb2 * cos(acosblah + (pi * 2. / 3.)) - k1;
            res[2] = sqrtnb2 * cos(acosblah - (pi * 2. / 3.)) - k1;
        }
    }
}

#endif
