package net.devromik.bw

/**
 * @author Shulnyaev Roman
 */
val INVALID_MAX_WHITE = -1
val NOT_INIT_MAX_WHITE = INVALID_MAX_WHITE - 1

class WritableMaxWhiteMap(override val treeSize: Int) : MaxWhiteMap {

    private val map = IntArray(treeSize + 1, { NOT_INIT_MAX_WHITE })

    // ****************************** //

    operator fun set(black: Int, maxWhite: Int) {
        require(
            maxWhite >= 0 || maxWhite == INVALID_MAX_WHITE,
            { "Max number of white nodes should be non-negative or equal to INVALID_MAX_WHITE = $INVALID_MAX_WHITE" })

        map[black] = maxWhite
    }

    override operator fun get(black: Int): Int {
        if (black in 0..treeSize) {
            val maxWhite = map[black]
            check(maxWhite != NOT_INIT_MAX_WHITE, { "Not initialized" })

            return maxWhite
        }
        else {
            return INVALID_MAX_WHITE
        }
    }
}