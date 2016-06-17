package net.devromik.bw

import net.devromik.bw.Color.values

/**
 * @author Shulnyaev Roman
 */
enum class Color(val index: Int) {
    BLACK(0),
    WHITE(1),
    GRAY(2);
}

val COLOR_COUNT = values().size

fun colorByIndex(index: Int) = Color.values().firstOrNull { it.index == index }