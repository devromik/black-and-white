package net.devromik.bw

/**
 * @author Shulnyaev Roman
 */
val INVALID_MAX_WHITE = -1
val NOT_INIT_MAX_WHITE = INVALID_MAX_WHITE - 1

class MaxWhite(val treeSize: Int) {

    private val blackToMaxWhite = IntArray(treeSize + 1, { NOT_INIT_MAX_WHITE })

    // ****************************** //

    operator fun set(black: Int, maxWhite: Int) {
        require(
            maxWhite >= 0 || maxWhite == INVALID_MAX_WHITE,
            { "Max number of white nodes should be non-negative or INVALID_MAX_WHITE = $INVALID_MAX_WHITE" })

        blackToMaxWhite[black] = maxWhite
    }

    fun forBlack(black: Int): Int {
        if (black >= 0 && black <= treeSize) {
            val maxWhite = blackToMaxWhite[black]
            check(maxWhite != NOT_INIT_MAX_WHITE, { "Not filled" })

            return maxWhite
        }
        else {
            return INVALID_MAX_WHITE
        }
    }
}