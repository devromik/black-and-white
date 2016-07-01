package net.devromik.bw

import net.devromik.bw.Color.*
import java.lang.Math.max

/**
 * @author Shulnyaev Roman
 */
open class FixedRootColorMaxWhiteMap(override val treeSize: Int) : MaxWhiteMap {

    private val maps = Array(COLOR_COUNT, { WritableMaxWhiteMap(treeSize) })

    private fun mapFor(rootColor: Color) = maps[rootColor.index]

    // ****************************** //

    companion object {

        val SINGLE_NODE_FIXED_ROOT_COLOR_MAX_WHITE_MAP = object : FixedRootColorMaxWhiteMap(treeSize = 1) {

            override fun get(rootColor: Color, black: Int) =
                when (rootColor) {
                    BLACK -> if (black == 1) 0 else INVALID_MAX_WHITE
                    WHITE -> if (black == 0) 1 else INVALID_MAX_WHITE
                    GRAY -> if (black == 0) 0 else INVALID_MAX_WHITE
                }

            override fun set(rootColor: Color, black: Int, maxWhite: Int) {
                throw UnsupportedOperationException()
            }
        }

        fun forSingleNode() = SINGLE_NODE_FIXED_ROOT_COLOR_MAX_WHITE_MAP
    }

    // ****************************** //

    open operator fun set(rootColor: Color, black: Int, maxWhite: Int) {
        mapFor(rootColor)[black] = maxWhite
    }

    open operator fun get(rootColor: Color, black: Int): Int {
        return mapFor(rootColor)[black]
    }

    override operator fun get(black: Int): Int {
        val blackWhiteMax = max(get(BLACK, black), get(WHITE, black))
        return max(blackWhiteMax, get(GRAY, black))
    }
}