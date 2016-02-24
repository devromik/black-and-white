#ifndef BW_POLYNOMIAL_H
#define BW_POLYNOMIAL_H

#include <math.h>
#include <vector>
#include "Polynomial_priv.h"

namespace bw {

/**
 * Многочлен.
 */
class Polynomial {

public:

    Polynomial(const int newDegree) : degree(newDegree), coeffs(newDegree + 1, 0.0), coeffCount(newDegree + 1) {}
    Polynomial(const Polynomial& source) : degree(source.degree), coeffs(source.coeffs), coeffCount(source.coeffCount) {}
    Polynomial(Polynomial* source) : degree(source->degree), coeffs(source->coeffs), coeffCount(source->coeffCount) {}

    int getDegree() const {
        return degree;
    }

    void setCoefficient(const int pos, const double newCoeff) {
        coeffs[pos] = newCoeff;
    }

    double getCoefficient(const int pos) const {
        return coeffs[pos];
    }

    int getCoefficientCount() const {
        return coeffCount;
    }

    void multiply(const Polynomial& other) {
        using namespace priv;

        int maxCoeffCount = std::max(coeffCount, other.coeffCount);
        int helpCoeffCount = 1;

        while (helpCoeffCount < maxCoeffCount) {
            helpCoeffCount <<= 1;
        }

        helpCoeffCount <<= 1;

        std::vector<Complex> a(coeffs.begin(), coeffs.end());
        a.resize(helpCoeffCount);
        fft(a, false);

        std::vector<Complex> b(other.coeffs.begin(), other.coeffs.end());
        b.resize(helpCoeffCount);
        fft(b, false);

        for (int i = 0; i < helpCoeffCount; ++i) {
            a[i] *= b[i];
        }

        fft(a, true);
        degree += other.degree;
        coeffCount = degree + 1;
        coeffs.resize(coeffCount);

        if (isIntCoefficients()) {
            for (int i = 0; i < coeffCount; ++i) {
                coeffs[i] = (int)(a[i].real() + 0.5);
            }
        }
        else {
            for (int i = 0; i < coeffCount; ++i) {
                coeffs[i] = a[i].real();
            }
        }
    }

    void multiply(Polynomial* other) {
        multiply(*other);
    }

private:

    bool isIntCoefficients() {
        static const double EPSILON = 0.0000001;
        double intPart;

        for (int i = 0; i < coeffCount; ++i) {
            if (fabs(modf(getCoefficient(i), &intPart)) > EPSILON) {
                return false;
            }
        }

        return true;
    }

private:

    // Степень.
    int degree;

    // Вектор коэффициентов.
    std::vector<double> coeffs;

    // Количество коэффициентов.
    int coeffCount;
};

inline Polynomial multiply(const Polynomial& a, const Polynomial& b) {
    Polynomial product(a);
    product.multiply(b);

    return product;
}

inline Polynomial* multiply(Polynomial* a, Polynomial* b) {
    Polynomial* product = new Polynomial(a);
    product->multiply(b);

    return product;
}

} // namespace bw.

#endif
