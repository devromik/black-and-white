package net.devromik.bw.bz

import net.devromik.bw.Color.*
import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.bw.INVALID_MAX_WHITE
import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class MultiChildFusionTest {

    @Test fun providesMinGray() {
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

        val minGrayToBlacksMap = MinGrayToBlacksMap(subtreeMaxWhiteMap)
        val leftFusion = SingleChildFusion(subtreeMaxWhiteMap)
        val rightFusion = SingleChildFusion(subtreeMaxWhiteMap)
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