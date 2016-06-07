package net.devromik.bw

/**
 * @author Shulnyaev Roman
 */
val INVALID_MAX_WHITE = -1

class MaxWhite(val treeSize: Int) {

    private val blackToMaxWhite = IntArray(treeSize + 1, { INVALID_MAX_WHITE })

    // ****************************** //

    fun put(black: Int, maxWhite: Int) {
        require(maxWhite >= 0, { "Max number of white nodes should be non-negative" })
        blackToMaxWhite[black] = maxWhite
    }

    fun forBlack(black: Int): Int {
        if (black >= 0 && black <= treeSize) {
            val maxWhite = blackToMaxWhite[black]
            check(maxWhite != INVALID_MAX_WHITE, { "Not filled" })

            return maxWhite
        }
        else {
            return INVALID_MAX_WHITE
        }
    }
}