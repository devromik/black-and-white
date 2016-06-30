package net.devromik.bw

import net.devromik.bw.Color.*
import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class FixedRootColorMaxWhiteMapTest {

    @Test fun canBuildMapForSingleNode() {
        val maxWhiteMap = FixedRootColorMaxWhiteMap.forSingleNode()

        // BLACK
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 2))

        // WHITE
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(1,                 maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))

        // GRAY
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 2))
    }
}