package net.devromik.bw.bz

import net.devromik.bw.Color

/**
 * @author Shulnyaev Roman
 */
class MultiChildFusion(
    minGrayToBlacksMap: MinGrayToBlacksMap,
    leftFusion: Fusion,
    rightFusion: Fusion) : Fusion(leftFusion, rightFusion) {

    val subtreeSize = minGrayToBlacksMap.subtreeSize
    val blackToMinGray: Array<IntArray>

    // ****************************** //

    init {
        blackToMinGray = arrayOf(
            IntArray(subtreeSize + 1, { INVALID_MIN_GRAY }),
            IntArray(subtreeSize + 1, { INVALID_MIN_GRAY }),
            IntArray(subtreeSize + 1, { INVALID_MIN_GRAY })
        )

        for (minGray in 0..minGrayToBlacksMap.grayUpperEstimation) {
            for (color in Color.values()) {
                for (black in minGrayToBlacksMap[color, minGray]) {
                    blackToMinGray[color.index][black.value] = minGray
                }
            }
        }
    }

    // ****************************** //

    override fun minGrayFor(rootColor: Color, black: Int) =
        if (black in 0..subtreeSize) blackToMinGray[rootColor.index][black] else INVALID_MIN_GRAY
}