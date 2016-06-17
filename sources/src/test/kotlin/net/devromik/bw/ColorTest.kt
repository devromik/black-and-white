package net.devromik.bw

import net.devromik.bw.Color.*
import net.devromik.bw.Color.values
import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class ColorTest {

    @Test fun testCount() {
        assertEquals(3, COLOR_COUNT)
    }

    @Test fun testIndexes() {
        val indexes = values().map { it.index }.sorted().toIntArray()
        assertArrayEquals(indexes, (0..COLOR_COUNT - 1).toList().toIntArray())

        assertNull(colorByIndex(-1))
        assertEquals(BLACK, colorByIndex(0))
        assertEquals(WHITE, colorByIndex(1))
        assertEquals(GRAY, colorByIndex(2))
        assertNull(colorByIndex(COLOR_COUNT))
    }
}