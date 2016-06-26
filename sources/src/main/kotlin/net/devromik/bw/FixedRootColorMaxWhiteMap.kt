package net.devromik.bw

import net.devromik.bw.Color.*
import java.lang.Math.max

/**
 * @author Shulnyaev Roman
 */
open class FixedRootColorMaxWhiteMap(override val treeSize: Int) : MaxWhiteMap {

    val maps = Array(COLOR_COUNT, { WritableMaxWhiteMap(treeSize) })

    // ****************************** //

    operator fun set(rootColor: Color, black: Int, maxWhite: Int) {
        maxWhiteFor(rootColor)[black] = maxWhite
    }

    open operator fun get(rootColor: Color, black: Int): Int {
        return maxWhiteFor(rootColor)[black]
    }

    override operator fun get(black: Int): Int {
        val blackWhiteMax = max(get(BLACK, black), get(WHITE, black))
        return max(blackWhiteMax, get(GRAY, black))
    }

    // ****************************** //

    private fun maxWhiteFor(rootColor: Color) = maps[rootColor.index]
}