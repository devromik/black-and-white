package net.devromik.bw

import org.junit.Assert.*
import org.junit.Test

/**
 * @author Shulnyaev Roman
 */
class MaxWhiteTest {

    val TREE_SIZE = 10

    // ****************************** //

    @Test fun numberOfBlackNodes_ShouldBe_From_0_To_SizeOfTree() {
        val maxWhite = MaxWhite(treeSize = TREE_SIZE)
        assertEquals(TREE_SIZE, maxWhite.treeSize)

        assertEquals(INVALID_MAX_WHITE, maxWhite.forBlack(-1))
        assertEquals(INVALID_MAX_WHITE, maxWhite.forBlack(Int.MIN_VALUE))

        (0..TREE_SIZE).forEach {
            maxWhite.put(black = it, maxWhite = it)
            assertEquals(it, maxWhite.forBlack(it))
        }

        assertEquals(INVALID_MAX_WHITE, maxWhite.forBlack(TREE_SIZE + 1))
        assertEquals(INVALID_MAX_WHITE, maxWhite.forBlack(Int.MAX_VALUE))
    }

    @Test(expected = IllegalStateException::class)
    fun queryShouldBePrecededByFilling() {
        val maxWhite = MaxWhite(treeSize = TREE_SIZE)
        maxWhite.forBlack(0)
    }

    @Test(expected = IllegalArgumentException::class)
    fun maxNumberOfWhiteNodes_ShouldBe_NonNegative() {
        val maxWhite = MaxWhite(treeSize = TREE_SIZE)
        maxWhite.put(0, -1)
    }

    @Test fun INVALID_MAX_WHITE_ShouldBe_Negative() {
        assertTrue(INVALID_MAX_WHITE < 0)
    }
}