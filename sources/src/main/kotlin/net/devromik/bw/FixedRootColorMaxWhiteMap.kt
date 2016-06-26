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

        fun forSingleNode(): FixedRootColorMaxWhiteMap {
            val singleNodeMaxWhiteMap = FixedRootColorMaxWhiteMap(treeSize = 1)

            singleNodeMaxWhiteMap.set(rootColor = BLACK, black = 0, maxWhite = INVALID_MAX_WHITE)
            singleNodeMaxWhiteMap.set(rootColor = BLACK, black = 1, maxWhite = 0)

            singleNodeMaxWhiteMap.set(rootColor = WHITE, black = 0, maxWhite = 1)
            singleNodeMaxWhiteMap.set(rootColor = WHITE, black = 1, maxWhite = INVALID_MAX_WHITE)

            singleNodeMaxWhiteMap.set(rootColor = GRAY, black = 0, maxWhite = 0)
            singleNodeMaxWhiteMap.set(rootColor = GRAY, black = 1, maxWhite = INVALID_MAX_WHITE)

            return singleNodeMaxWhiteMap
        }
    }

    // ****************************** //

    operator fun set(rootColor: Color, black: Int, maxWhite: Int) {
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