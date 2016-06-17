package net.devromik.bw

/**
 * @author Shulnyaev Roman
 */
class FixedRootColorMaxWhite(val treeSize: Int) {

    val rootColorIndexToMaxWhite = Array(COLOR_COUNT, { MaxWhite(treeSize) })

    // ****************************** //

    operator fun set(rootColor: Color, black: Int, maxWhite: Int) {
        maxWhiteFor(rootColor)[black] = maxWhite
    }

    fun forBlack(rootColor: Color, black: Int): Int {
        return maxWhiteFor(rootColor).forBlack(black)
    }

    // ****************************** //

    private fun maxWhiteFor(rootColor: Color) = rootColorIndexToMaxWhite[rootColor.index]
}