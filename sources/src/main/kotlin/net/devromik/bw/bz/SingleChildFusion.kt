package net.devromik.bw.bz

import net.devromik.bw.Color
import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.bw.INVALID_MAX_WHITE

/**
 * @author Shulnyaev Roman
 */
class SingleChildFusion(val subtreeMaxWhiteMap: FixedRootColorMaxWhiteMap) : Fusion(leftFusion = null, rightFusion = null) {

    val subtreeSize = subtreeMaxWhiteMap.treeSize

    // ****************************** //

    override fun minGrayFor(rootColor: Color, black: Int): Int {
        val maxWhite = subtreeMaxWhiteMap[rootColor, black]
        return if (maxWhite != INVALID_MAX_WHITE) subtreeSize - (black + maxWhite) else INVALID_MIN_GRAY
    }
}