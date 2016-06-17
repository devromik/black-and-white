package net.devromik.bw

/**
 * @author Shulnyaev Roman
 */
class FixedRootColorMaxWhiteMap(val treeSize: Int) {

    val maps = Array(COLOR_COUNT, { MaxWhiteMap(treeSize) })

    // ****************************** //

    operator fun set(rootColor: Color, black: Int, maxWhite: Int) {
        maxWhiteFor(rootColor)[black] = maxWhite
    }

    operator fun get(rootColor: Color, black: Int): Int {
        return maxWhiteFor(rootColor)[black]
    }

    // ****************************** //

    private fun maxWhiteFor(rootColor: Color) = maps[rootColor.index]
}