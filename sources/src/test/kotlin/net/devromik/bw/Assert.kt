package net.devromik.bw

import net.devromik.bw.Color.*
import org.junit.Assert.assertEquals

/**
 * @author Shulnyaev Roman
 */
fun assert_FixedRootColorMaxWhiteMap_Equals(expected: FixedRootColorMaxWhiteMap, actual: FixedRootColorMaxWhiteMap) {
    assertEquals(expected.treeSize, actual.treeSize)

    for (black in 0..actual.treeSize) {
        assertEquals(expected[BLACK, black], actual[BLACK, black])
        assertEquals(expected[WHITE, black], actual[WHITE, black])
        assertEquals(expected[GRAY, black], actual[GRAY, black])
    }
}