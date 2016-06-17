package net.devromik.bw

import net.devromik.bw.Color.*
import net.devromik.bw.Color.values
import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class ColorTest {

    @Test fun thereAreExactlyThreeColors() {
        assertEquals(3, COLOR_COUNT)
    }

    @Test fun colorsAreIndexed() {
        val indexes = values().map { it.index }.sorted().toIntArray()
        assertArrayEquals((0..COLOR_COUNT - 1).toList().toIntArray(), indexes)

        assertNull(colorByIndex(-1))
        assertEquals(BLACK, colorByIndex(0))
        assertEquals(WHITE, colorByIndex(1))
        assertEquals(GRAY, colorByIndex(2))
        assertNull(colorByIndex(COLOR_COUNT))
    }
}