package net.devromik.bw.bz

import net.devromik.bw.Color
import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.bw.INVALID_MAX_WHITE

/**
 * @author Shulnyaev Roman
 */
class SingleChildFusion<D>(val rootMaxWhiteMap: FixedRootColorMaxWhiteMap) : Fusion<D>(leftFusion = null, rightFusion = null) {

    val subtreeSize = rootMaxWhiteMap.treeSize

    // ****************************** //

    override fun minGrayFor(rootColor: Color, black: Int): Int {
        val maxWhite = rootMaxWhiteMap[rootColor, black]
        return if (maxWhite != INVALID_MAX_WHITE) subtreeSize - (black + maxWhite) else INVALID_MIN_GRAY
    }
}