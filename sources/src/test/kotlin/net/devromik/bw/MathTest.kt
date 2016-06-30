package net.devromik.bw

import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class MathTest {

    @Test fun log2() {
        assertEquals(0, log2(1))
        assertEquals(1, log2(2))
        assertEquals(1, log2(3))
        assertEquals(2, log2(4))
        assertEquals(2, log2(5))
        assertEquals(2, log2(6))
        assertEquals(2, log2(7))
        assertEquals(3, log2(8))
        assertEquals(3, log2(9))
        assertEquals(3, log2(10))
        assertEquals(3, log2(11))
        assertEquals(3, log2(12))
        assertEquals(3, log2(13))
        assertEquals(3, log2(14))
        assertEquals(3, log2(15))
        assertEquals(4, log2(16))
    }
}