package net.devromik.bw

import net.devromik.bw.Color.*
import java.lang.Math.max

/**
 * @author Shulnyaev Roman
 */
open class FixedRootColorMaxWhiteMap(val treeSize: Int) {

    val maps = Array(COLOR_COUNT, { MaxWhiteMap(treeSize) })

    // ****************************** //

    operator fun set(rootColor: Color, black: Int, maxWhite: Int) {
        maxWhiteFor(rootColor)[black] = maxWhite
    }

    open operator fun get(rootColor: Color, black: Int): Int {
        return maxWhiteFor(rootColor)[black]
    }

    open operator fun get(black: Int): Int {
        val maxBlackWhite = max(get(BLACK, black), get(WHITE, black))
        return max(maxBlackWhite, get(GRAY, black))
    }

    fun checkColoringExists(rootColor: Color, black: Int, white: Int) {
        require(
            black in (0..treeSize),
            { "Number of black nodes should be in the range 0..<size of tree>" })

        val maxWhite = get(rootColor, black)

        require(
            white in (0..maxWhite),
            { "Impossible coloring: black = $black, white = $white" })
    }

    // ****************************** //

    private fun maxWhiteFor(rootColor: Color) = maps[rootColor.index]
}