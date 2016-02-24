#ifndef BW_SUMMATOR_H
#define BW_SUMMATOR_H

#include <memory>
#include <vector>
#include <algorithm>
#include "Polynomial.h"

namespace bw {

/**
 * Сумматор.
 */
class Summator {

public:

    /**
     * Вычисляет попарные суммы элементов из a и b. Результат сохраняет в s.
     * Предполагает, что a и b содержат только неотрицательные целые числа.
     */
    static void computePairwiseSums(const std::vector<int>& a, const std::vector<int>& b, std::vector<int>& s) {
        static const double EPSILON = 0.000000001;

        int maxA = *std::max_element(a.begin(), a.end());
        std::unique_ptr<Polynomial> polynomA(new Polynomial(maxA));

        for (int i = 0; i < a.size(); ++i) {
            polynomA->setCoefficient(a[i], 1.0);
        }

        int maxB = *std::max_element(b.begin(), b.end());
        std::unique_ptr<Polynomial> polynomB(new Polynomial(maxB));

        for (int i = 0; i < b.size(); ++i) {
            polynomB->setCoefficient(b[i], 1.0);
        }

        std::unique_ptr<Polynomial> productAB(multiply(polynomA.get(), polynomB.get()));

        for (int i = 0; i < productAB->getCoefficientCount(); ++i) {
            if (fabs(productAB->getCoefficient(i)) > EPSILON) {
                s.push_back(i);
            }
        }
    }

    static void computePairwiseSums(std::vector<int>* a, std::vector<int>* b, std::vector<int>* s) {
        computePairwiseSums(*a, *b, *s);
    }

    static void computePairwiseSums(std::vector<int>* a, std::vector<int>* b, std::vector<int>& s) {
        computePairwiseSums(*a, *b, s);
    }
};

} // namespace bw.

#endif
