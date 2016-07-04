package net.devromik.bw.bz

import net.devromik.bw.Color.*
import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.bw.INVALID_MAX_WHITE
import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class SingleChildFusionTest {
    
    @Test fun doesNotHaveChildFusions() {
        val subtreeMaxWhiteMap = FixedRootColorMaxWhiteMap.forSingleNode()
        val fusion = SingleChildFusion(subtreeMaxWhiteMap)

        assertNull(fusion.leftFusion)
        assertNull(fusion.rightFusion)
    }  

    @Test fun providesMinGray() {
        // root -> child -> (child, child)
        val subtreeMaxWhiteMap = FixedRootColorMaxWhiteMap(4)
        subtreeMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        subtreeMaxWhiteMap[BLACK, 1] = 2
        subtreeMaxWhiteMap[BLACK, 2] = 0
        subtreeMaxWhiteMap[BLACK, 3] = 0
        subtreeMaxWhiteMap[BLACK, 4] = 0

        subtreeMaxWhiteMap[WHITE, 0] = 4
        subtreeMaxWhiteMap[WHITE, 1] = 1
        subtreeMaxWhiteMap[WHITE, 2] = 1
        subtreeMaxWhiteMap[WHITE, 3] = INVALID_MAX_WHITE
        subtreeMaxWhiteMap[WHITE, 4] = INVALID_MAX_WHITE

        subtreeMaxWhiteMap[GRAY, 0] = 3
        subtreeMaxWhiteMap[GRAY, 1] = 0
        subtreeMaxWhiteMap[GRAY, 2] = 0
        subtreeMaxWhiteMap[GRAY, 3] = 0
        subtreeMaxWhiteMap[GRAY, 4] = INVALID_MAX_WHITE

        val fusion = SingleChildFusion(subtreeMaxWhiteMap)

        assertEquals(fusion.minGrayFor(BLACK, 0), INVALID_MIN_GRAY)
        assertEquals(fusion.minGrayFor(BLACK, 1), 1)
        assertEquals(fusion.minGrayFor(BLACK, 2), 2)
        assertEquals(fusion.minGrayFor(BLACK, 3), 1)
        assertEquals(fusion.minGrayFor(BLACK, 4), 0)

        assertEquals(fusion.minGrayFor(WHITE, 0), 0)
        assertEquals(fusion.minGrayFor(WHITE, 1), 2)
        assertEquals(fusion.minGrayFor(WHITE, 2), 1)
        assertEquals(fusion.minGrayFor(WHITE, 3), INVALID_MIN_GRAY)
        assertEquals(fusion.minGrayFor(WHITE, 4), INVALID_MIN_GRAY)

        assertEquals(fusion.minGrayFor(GRAY, 0), 1)
        assertEquals(fusion.minGrayFor(GRAY, 1), 3)
        assertEquals(fusion.minGrayFor(GRAY, 2), 2)
        assertEquals(fusion.minGrayFor(GRAY, 3), 1)
        assertEquals(fusion.minGrayFor(GRAY, 4), INVALID_MIN_GRAY)
    }
}