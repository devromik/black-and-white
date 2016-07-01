package net.devromik.bw.bz

import net.devromik.bw.Color
import net.devromik.bw.Color.*
import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.bw.INVALID_MAX_WHITE
import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class MultiChildFusionTest {

    @Test fun minGrayCount() {
        // root -> child
        val rootMaxWhiteMap = FixedRootColorMaxWhiteMap(2)
        rootMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        rootMaxWhiteMap[BLACK, 1] = 0
        rootMaxWhiteMap[BLACK, 2] = 0

        rootMaxWhiteMap[Color.WHITE, 0] = 2
        rootMaxWhiteMap[Color.WHITE, 1] = INVALID_MAX_WHITE
        rootMaxWhiteMap[Color.WHITE, 2] = INVALID_MAX_WHITE

        rootMaxWhiteMap[Color.GRAY, 0] = 1
        rootMaxWhiteMap[Color.GRAY, 1] = 0
        rootMaxWhiteMap[Color.GRAY, 2] = INVALID_MAX_WHITE

        val minGrayToBlacksMap = MinGrayToBlacksMap(rootMaxWhiteMap)
        val leftFusion = SingleChildFusion(rootMaxWhiteMap)
        val rightFusion = SingleChildFusion(rootMaxWhiteMap)
        val fusion = MultiChildFusion(minGrayToBlacksMap.unitedWith(minGrayToBlacksMap), leftFusion, rightFusion)

        assertEquals(INVALID_MIN_GRAY, fusion.minGrayFor(rootColor = BLACK, black = 0))
        assertEquals(2,                fusion.minGrayFor(rootColor = BLACK, black = 1))
        assertEquals(1,                fusion.minGrayFor(rootColor = BLACK, black = 2))
        assertEquals(0,                fusion.minGrayFor(rootColor = BLACK, black = 3))
        assertEquals(INVALID_MIN_GRAY, fusion.minGrayFor(rootColor = BLACK, black = 4))

        assertEquals(0,                fusion.minGrayFor(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MIN_GRAY, fusion.minGrayFor(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MIN_GRAY, fusion.minGrayFor(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MIN_GRAY, fusion.minGrayFor(rootColor = WHITE, black = 3))
        assertEquals(INVALID_MIN_GRAY, fusion.minGrayFor(rootColor = WHITE, black = 4))

        assertEquals(1,                fusion.minGrayFor(rootColor = GRAY, black = 0))
        assertEquals(1,                fusion.minGrayFor(rootColor = GRAY, black = 1))
        assertEquals(1,                fusion.minGrayFor(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MIN_GRAY, fusion.minGrayFor(rootColor = GRAY, black = 3))
        assertEquals(INVALID_MIN_GRAY, fusion.minGrayFor(rootColor = GRAY, black = 4))
    }
}