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

    @Test fun canBeMadeOf_FixedRootColorMaxWhiteMap() {
        // root -> child -> (child, child)
        val rootMaxWhiteMap = FixedRootColorMaxWhiteMap(4)
        rootMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        rootMaxWhiteMap[BLACK, 1] = 2
        rootMaxWhiteMap[BLACK, 2] = 0
        rootMaxWhiteMap[BLACK, 3] = 0
        rootMaxWhiteMap[BLACK, 4] = 0

        rootMaxWhiteMap[WHITE, 0] = 4
        rootMaxWhiteMap[WHITE, 1] = 1
        rootMaxWhiteMap[WHITE, 2] = 1
        rootMaxWhiteMap[WHITE, 3] = INVALID_MAX_WHITE
        rootMaxWhiteMap[WHITE, 4] = INVALID_MAX_WHITE

        rootMaxWhiteMap[GRAY, 0] = 3
        rootMaxWhiteMap[GRAY, 1] = 0
        rootMaxWhiteMap[GRAY, 2] = 0
        rootMaxWhiteMap[GRAY, 3] = 0
        rootMaxWhiteMap[GRAY, 4] = INVALID_MAX_WHITE

        val minGrayToBlacksMap = MinGrayToBlacksMap(rootMaxWhiteMap)
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
    
    @Test fun canBeConvertedInto_FixedRootColorMaxWhiteMap() {
        // root -> child -> (child, child)
        val rootMaxWhiteMap = FixedRootColorMaxWhiteMap(4)
        rootMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        rootMaxWhiteMap[BLACK, 1] = 2
        rootMaxWhiteMap[BLACK, 2] = 0
        rootMaxWhiteMap[BLACK, 3] = 0
        rootMaxWhiteMap[BLACK, 4] = 0

        rootMaxWhiteMap[WHITE, 0] = 4
        rootMaxWhiteMap[WHITE, 1] = 1
        rootMaxWhiteMap[WHITE, 2] = 1
        rootMaxWhiteMap[WHITE, 3] = INVALID_MAX_WHITE
        rootMaxWhiteMap[WHITE, 4] = INVALID_MAX_WHITE

        rootMaxWhiteMap[GRAY, 0] = 3
        rootMaxWhiteMap[GRAY, 1] = 0
        rootMaxWhiteMap[GRAY, 2] = 0
        rootMaxWhiteMap[GRAY, 3] = 0
        rootMaxWhiteMap[GRAY, 4] = INVALID_MAX_WHITE

        val minGrayToBlacksMap = MinGrayToBlacksMap(rootMaxWhiteMap)

        assert_FixedRootColorMaxWhiteMap_Equals(
            rootMaxWhiteMap,
            minGrayToBlacksMap.toFixedRootColorMaxWhiteMap())
    }
    
    @Test fun mapsForTheSameRoot_CanBeUnited() {
        // root -> child
        val rootMaxWhiteMap = FixedRootColorMaxWhiteMap(2)
        rootMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        rootMaxWhiteMap[BLACK, 1] = 0
        rootMaxWhiteMap[BLACK, 2] = 0

        rootMaxWhiteMap[WHITE, 0] = 2
        rootMaxWhiteMap[WHITE, 1] = INVALID_MAX_WHITE
        rootMaxWhiteMap[WHITE, 2] = INVALID_MAX_WHITE

        rootMaxWhiteMap[GRAY, 0] = 1
        rootMaxWhiteMap[GRAY, 1] = 0
        rootMaxWhiteMap[GRAY, 2] = INVALID_MAX_WHITE

        val minGrayToBlacksMap_1 = MinGrayToBlacksMap(rootMaxWhiteMap)
        val minGrayToBlacksMap_2 = MinGrayToBlacksMap(rootMaxWhiteMap)

        // root -> (child, child)
        val united = minGrayToBlacksMap_1.unitedWith(minGrayToBlacksMap_2)

        assertEquals(3, united.subtreeSize)

        assertEquals(blackListOf(3), united[BLACK, 0])
        assertEquals(blackListOf(2), united[BLACK, 1])
        assertEquals(blackListOf(1), united[BLACK, 2])
        assertEquals(blackListOf(), united[BLACK, 3])

        assertEquals(blackListOf(0), united[WHITE, 0])
        assertEquals(blackListOf(), united[WHITE, 1])
        assertEquals(blackListOf(), united[WHITE, 2])
        assertEquals(blackListOf(), united[WHITE, 3])

        assertEquals(blackListOf(), united[GRAY, 0])
        assertEquals(blackListOf(0, 1, 2), united[GRAY, 1])
        assertEquals(blackListOf(), united[GRAY, 2])
        assertEquals(blackListOf(), united[GRAY, 3])
    }
}