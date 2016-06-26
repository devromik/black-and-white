package net.devromik.bw

/**
 * @author Shulnyaev Roman
 */
interface MaxWhiteMap {
    operator fun get(black: Int): Int
    val treeSize: Int
}