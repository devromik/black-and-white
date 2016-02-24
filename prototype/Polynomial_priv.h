#ifndef BW_POLYNOMIAL_PRIV_H
#define BW_POLYNOMIAL_PRIV_H

#include <math.h>
#include <complex>

namespace bw {
namespace priv {

typedef std::complex<double> Complex;

/**
 * Поразрядно-обратная перестановка.
 */
inline int permutation(const int num, const int lgN) {
    int result = 0;

    for (int i = 0; i < lgN; ++i) {
        if (num & (1 << i)) {
            result |= 1 << (lgN - 1 - i);
        }
    }

    return result;
}

/**
 * Быстрое преобразование Фурье.
 */
inline void fft(std::vector<Complex>& a, const bool invert) {
    static const int TWO_PI = 2.0 * 3.14159265358979323846;
    int n = (int)a.size();
    int lgN = 0;

    while ((1 << lgN) < n) {
        ++lgN;
    }

    for (int i = 0; i < n; ++i) {
        if (i < permutation(i, lgN)) {
            std::swap(a[i], a[permutation(i, lgN)]);
        }
    }

    for (int len = 2; len <= n; len <<= 1) {
        double angle = TWO_PI / (double)len;

        if (invert) {
            angle = -angle;
        }

        Complex wlen(cos(angle), sin(angle));

        for (int i = 0; i < n; i += len) {
            Complex w(1.0);

            for (int j = 0; j < len / 2; ++j) {
                Complex u = a[i + j],  v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    if (invert) {
        for (int i = 0; i < n; ++i) {
            a[i] /= n;
        }
    }
}

} // namespace priv.
} // namespace bw.

#endif
