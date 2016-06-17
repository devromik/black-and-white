package net.devromik.bw

import org.junit.Assert.*
import org.junit.Test

/**
 * @author Shulnyaev Roman
 */
class MaxWhiteMapTest {

    val TREE_SIZE = 10

    // ****************************** //

    @Test fun numberOfBlackNodes_ShouldBe_From_0_To_SizeOfTree() {
        val maxWhiteMap = MaxWhiteMap(TREE_SIZE)
        assertEquals(TREE_SIZE, maxWhiteMap.treeSize)

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap[-1])
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap[Int.MIN_VALUE])

        (0..TREE_SIZE).forEach {
            maxWhiteMap[it] = it
            assertEquals(it, maxWhiteMap[it])
        }

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap[TREE_SIZE + 1])
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap[Int.MAX_VALUE])
    }

    @Test(expected = IllegalStateException::class)
    fun shouldBeInitialized() {
        val maxWhiteMap = MaxWhiteMap(TREE_SIZE)
        maxWhiteMap[0]
    }

    @Test(expected = IllegalArgumentException::class)
    fun maxNumberOfWhiteNodes_ShouldBe_NonNegative_Or_INVALID_MAX_WHITE() {
        val maxWhiteMap = MaxWhiteMap(TREE_SIZE)
        maxWhiteMap[0] = NOT_INIT_MAX_WHITE
    }

    @Test fun INVALID_MAX_WHITE_ShouldBe_Negative() {
        assertTrue(INVALID_MAX_WHITE < 0)
    }

    @Test fun NOT_INIT_MAX_WHITE_ShouldBe_LessThan_INVALID_MAX_WHITE() {
        assertTrue(NOT_INIT_MAX_WHITE < INVALID_MAX_WHITE)
    }
}