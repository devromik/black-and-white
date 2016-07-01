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
        val rootMaxWhiteMap = FixedRootColorMaxWhiteMap.forSingleNode()
        val fusion = SingleChildFusion(rootMaxWhiteMap)

        assertNull(fusion.leftFusion)
        assertNull(fusion.rightFusion)
    }  

    @Test fun minGrayCount() {
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

        val fusion = SingleChildFusion(rootMaxWhiteMap)

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