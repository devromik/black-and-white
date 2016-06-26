package net.devromik.bw

import net.devromik.bw.Color.*

/**
 * The indexes must not be changed.
 *
 * @author Shulnyaev Roman
 */
enum class Color(val index: Int) {
    BLACK(0),
    WHITE(1),
    GRAY(2);
}

val COLOR_COUNT = values().size

fun colorByIndex(index: Int) =
    when (index) {
        BLACK.index -> BLACK
        WHITE.index -> WHITE
        GRAY.index -> GRAY
        else -> throw IllegalArgumentException("$index is not a color index")
    }