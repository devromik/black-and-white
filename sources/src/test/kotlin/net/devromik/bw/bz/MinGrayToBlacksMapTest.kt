package net.devromik.bw.bz

import com.carrotsearch.hppc.IntArrayList
import net.devromik.bw.Color.*
import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.bw.INVALID_MAX_WHITE
import net.devromik.bw.assert_FixedRootColorMaxWhiteMap_Equals
import org.junit.Assert.*
import org.junit.Test

/**
 * @author Shulnyaev Roman
 */
class MinGrayToBlacksMapTest {

    @Test fun canBeMadeOfSubtreeMaxWhiteMap() {
        // root -> child -> (child, child)
        val subtreeRootMaxWhiteMap = FixedRootColorMaxWhiteMap(4)
        subtreeRootMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        subtreeRootMaxWhiteMap[BLACK, 1] = 2
        subtreeRootMaxWhiteMap[BLACK, 2] = 0
        subtreeRootMaxWhiteMap[BLACK, 3] = 0
        subtreeRootMaxWhiteMap[BLACK, 4] = 0

        subtreeRootMaxWhiteMap[WHITE, 0] = 4
        subtreeRootMaxWhiteMap[WHITE, 1] = 1
        subtreeRootMaxWhiteMap[WHITE, 2] = 1
        subtreeRootMaxWhiteMap[WHITE, 3] = INVALID_MAX_WHITE
        subtreeRootMaxWhiteMap[WHITE, 4] = INVALID_MAX_WHITE

        subtreeRootMaxWhiteMap[GRAY, 0] = 3
        subtreeRootMaxWhiteMap[GRAY, 1] = 0
        subtreeRootMaxWhiteMap[GRAY, 2] = 0
        subtreeRootMaxWhiteMap[GRAY, 3] = 0
        subtreeRootMaxWhiteMap[GRAY, 4] = INVALID_MAX_WHITE

        val minGrayToBlacksMap = MinGrayToBlacksMap(subtreeRootMaxWhiteMap)
        assertEquals(4, minGrayToBlacksMap.subtreeSize)

        assertEquals(blackListOf(4), minGrayToBlacksMap[BLACK, 0])
        assertEquals(blackListOf(1, 3), minGrayToBlacksMap[BLACK, 1])
        assertEquals(blackListOf(2), minGrayToBlacksMap[BLACK, 2])
        assertEquals(blackListOf(), minGrayToBlacksMap[BLACK, 3])
        assertEquals(blackListOf(), minGrayToBlacksMap[BLACK, 4])

        assertEquals(blackListOf(0), minGrayToBlacksMap[WHITE, 0])
        assertEquals(blackListOf(2), minGrayToBlacksMap[WHITE, 1])
        assertEquals(blackListOf(1), minGrayToBlacksMap[WHITE, 2])
        assertEquals(blackListOf(), minGrayToBlacksMap[WHITE, 3])
        assertEquals(blackListOf(), minGrayToBlacksMap[WHITE, 4])

        assertEquals(blackListOf(), minGrayToBlacksMap[GRAY, 0])
        assertEquals(blackListOf(0, 3), minGrayToBlacksMap[GRAY, 1])
        assertEquals(blackListOf(2), minGrayToBlacksMap[GRAY, 2])
        assertEquals(blackListOf(1), minGrayToBlacksMap[GRAY, 3])
        assertEquals(blackListOf(), minGrayToBlacksMap[GRAY, 4])
    }

    fun blackListOf(vararg elements: Int): IntArrayList {
        val blackList = IntArrayList(elements.size)
        elements.forEach { blackList.add(it) }

        return blackList
    }
    
    @Test fun canBeConvertedIntoSubtreeMaxWhiteMap() {
        // root -> child -> (child, child)
        val subtreeRootMaxWhiteMap = FixedRootColorMaxWhiteMap(4)
        subtreeRootMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        subtreeRootMaxWhiteMap[BLACK, 1] = 2
        subtreeRootMaxWhiteMap[BLACK, 2] = 0
        subtreeRootMaxWhiteMap[BLACK, 3] = 0
        subtreeRootMaxWhiteMap[BLACK, 4] = 0

        subtreeRootMaxWhiteMap[WHITE, 0] = 4
        subtreeRootMaxWhiteMap[WHITE, 1] = 1
        subtreeRootMaxWhiteMap[WHITE, 2] = 1
        subtreeRootMaxWhiteMap[WHITE, 3] = INVALID_MAX_WHITE
        subtreeRootMaxWhiteMap[WHITE, 4] = INVALID_MAX_WHITE

        subtreeRootMaxWhiteMap[GRAY, 0] = 3
        subtreeRootMaxWhiteMap[GRAY, 1] = 0
        subtreeRootMaxWhiteMap[GRAY, 2] = 0
        subtreeRootMaxWhiteMap[GRAY, 3] = 0
        subtreeRootMaxWhiteMap[GRAY, 4] = INVALID_MAX_WHITE

        val minGrayToBlacksMap = MinGrayToBlacksMap(subtreeRootMaxWhiteMap)

        assert_FixedRootColorMaxWhiteMap_Equals(
            subtreeRootMaxWhiteMap,
            minGrayToBlacksMap.toFixedRootColorMaxWhiteMap())
    }
    
    @Test fun mapsForTheSameRootCanBeUnited() {
        // root -> child
        val subtreeMaxWhiteMap = FixedRootColorMaxWhiteMap(2)
        subtreeMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        subtreeMaxWhiteMap[BLACK, 1] = 0
        subtreeMaxWhiteMap[BLACK, 2] = 0

        subtreeMaxWhiteMap[WHITE, 0] = 2
        subtreeMaxWhiteMap[WHITE, 1] = INVALID_MAX_WHITE
        subtreeMaxWhiteMap[WHITE, 2] = INVALID_MAX_WHITE

        subtreeMaxWhiteMap[GRAY, 0] = 1
        subtreeMaxWhiteMap[GRAY, 1] = 0
        subtreeMaxWhiteMap[GRAY, 2] = INVALID_MAX_WHITE

        val minGrayToBlacksMap_1 = MinGrayToBlacksMap(subtreeMaxWhiteMap)
        val minGrayToBlacksMap_2 = MinGrayToBlacksMap(subtreeMaxWhiteMap)

        // root -> (child, child)
        val united = minGrayToBlacksMap_1.unitedWith(minGrayToBlacksMap_2)

        assertEquals(3, united.subtreeSize)

        assertEquals(blackListOf(3), united[BLACK, 0])
        assertEquals(blackListOf(2), united[BLACK, 1])
        assertEquals(blackListOf(1), united[BLACK, 2])
        assertEquals(blackListOf(),  united[BLACK, 3])

        assertEquals(blackListOf(0), united[WHITE, 0])
        assertEquals(blackListOf(),  united[WHITE, 1])
        assertEquals(blackListOf(),  united[WHITE, 2])
        assertEquals(blackListOf(),  united[WHITE, 3])

        assertEquals(blackListOf(),        united[GRAY, 0])
        assertEquals(blackListOf(0, 1, 2), united[GRAY, 1])
        assertEquals(blackListOf(),        united[GRAY, 2])
        assertEquals(blackListOf(),        united[GRAY, 3])
    }
}