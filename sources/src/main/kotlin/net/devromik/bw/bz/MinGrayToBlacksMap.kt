package net.devromik.bw.bz

import com.carrotsearch.hppc.IntArrayList
import net.devromik.bw.*
import net.devromik.bw.Color.*
import net.devromik.poly.pairwiseSumOfNonNegativeArrays

/**
 * @author Shulnyaev Roman
 */
val NO_BLACKS = IntArrayList()

class MinGrayToBlacksMap {

    val subtreeSize: Int
    val grayUpperEstimation: Int
    val map: Array<Array<IntArrayList>>

    // ****************************** //

    constructor(subtreeMaxWhiteMap: FixedRootColorMaxWhiteMap) {
        subtreeSize = subtreeMaxWhiteMap.treeSize
        grayUpperEstimation = log2(subtreeSize) + 2

        map = arrayOf(
            Array(grayUpperEstimation + 1, { IntArrayList() }),
            Array(grayUpperEstimation + 1, { IntArrayList() }),
            Array(grayUpperEstimation + 1, { IntArrayList() })
        )

        for (black in 0..subtreeSize) {
            distribute(subtreeMaxWhiteMap, rootColor = BLACK, black = black)
            distribute(subtreeMaxWhiteMap, rootColor = WHITE, black = black)
            distribute(subtreeMaxWhiteMap, rootColor = GRAY, black = black)
        }
    }

    private fun distribute(subtreeMaxWhiteMap: FixedRootColorMaxWhiteMap, rootColor: Color, black: Int) {
        val maxWhite = subtreeMaxWhiteMap[rootColor, black]

        if (maxWhite != INVALID_MAX_WHITE) {
            val minGray = subtreeSize - (black + maxWhite)

            /**
             * It is possible that the condition is false.
             * Consider a tree whose root has many enough children each of which is a leaf.
             * Let the rootColor be black and a number of black nodes is equal to 1.
             * Then an optimal number of gray nodes is subtreeSize - 1 that is greater than lg(subtreeSize) + 2.
             * Fortunately we do not have to take this case into account because such a coloring
             * cannot be a part of an optimal one for a whole tree according to the lemma 2 from the paper.
             */
            if (minGray <= grayUpperEstimation) {
                map[rootColor.index][minGray].add(black)
            }
        }
    }

    // ****************************** //

    operator fun get(rootColor: Color, minGray: Int) =
        if (minGray <= grayUpperEstimation) map[rootColor.index][minGray] else NO_BLACKS

    fun toFixedRootColorMaxWhiteMap(): FixedRootColorMaxWhiteMap {
        val maxWhiteMap = FixedRootColorMaxWhiteMap(subtreeSize)

        for (black in 0..subtreeSize) {
            maxWhiteMap[BLACK, black] = INVALID_MAX_WHITE
            maxWhiteMap[WHITE, black] = INVALID_MAX_WHITE
            maxWhiteMap[GRAY, black] = INVALID_MAX_WHITE
        }

        for (minGray in 0..grayUpperEstimation) {
            for (rootColor in Color.values()) {
                for (black in get(rootColor, minGray)) {
                    maxWhiteMap[rootColor, black.value] = subtreeSize - (black.value + minGray)
                }
            }
        }

        return maxWhiteMap
    }

    fun unitedWith(that: MinGrayToBlacksMap): MinGrayToBlacksMap {
        return MinGrayToBlacksMap(this, that)
    }

    private constructor(unionOp1: MinGrayToBlacksMap, unionOp2: MinGrayToBlacksMap) {
        subtreeSize = unionOp1.subtreeSize + unionOp2.subtreeSize - 1

        grayUpperEstimation = log2(subtreeSize) + 2
        val grayUpperBound = grayUpperEstimation + 1

        map = arrayOf(
            Array(grayUpperBound, { IntArrayList() }),
            Array(grayUpperBound, { IntArrayList() }),
            Array(grayUpperBound, { IntArrayList() })
        )

        // BLACK
        var blackToMinGray = blackToMinGrayFor(unionOp1, unionOp2, subtreeSize, BLACK)

        for (black in 0..subtreeSize + 1) {
            val minGray = blackToMinGray[black]

            if (minGray != INVALID_MIN_GRAY && minGray < grayUpperBound) {
                map[BLACK.index][minGray].add(black - 1)
            }
        }

        // WHITE
        blackToMinGray = blackToMinGrayFor(unionOp1, unionOp2, subtreeSize, WHITE)

        for (black in 0..subtreeSize + 1) {
            val minGray = blackToMinGray[black]

            if (minGray != INVALID_MIN_GRAY && minGray < grayUpperBound) {
                map[WHITE.index][minGray].add(black)
            }
        }

        // GRAY
        blackToMinGray = blackToMinGrayFor(unionOp1, unionOp2, subtreeSize, GRAY)

        for (black in 0..subtreeSize + 1) {
            val minGray = blackToMinGray[black]

            if (minGray != INVALID_MIN_GRAY && minGray < grayUpperBound) {
                map[GRAY.index][minGray - 1].add(black)
            }
        }
    }

    fun blackToMinGrayFor(
        unionOp1: MinGrayToBlacksMap,
        unionOp2: MinGrayToBlacksMap,
        subtreeSize: Int,
        rootColor: Color): IntArray {

        val blackToMinGray = IntArray(subtreeSize + 2, { INVALID_MIN_GRAY })

        for (gray in grayUpperEstimation downTo 0) {
            for (gray1 in 0..gray) {
                val blacks1 = unionOp1[rootColor, gray1]

                val gray2 = gray - gray1
                val blacks2 = unionOp2[rootColor, gray2]

                if (!blacks1.isEmpty && !blacks2.isEmpty) {
                    for (black in pairwiseSumOfNonNegativeArrays(blacks1, blacks2)) {
                        blackToMinGray[black.value] = gray
                    }
                }
            }
        }

        return blackToMinGray
    }
}