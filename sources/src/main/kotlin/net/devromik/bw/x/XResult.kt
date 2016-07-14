package net.devromik.bw.x

import net.devromik.bw.Color
import net.devromik.bw.Coloring
import net.devromik.bw.FixedRootColorMaxWhiteMap

/**
 * @author Shulnyaev Roman
 */
interface XResult<D> {
    fun maxWhiteMap(): FixedRootColorMaxWhiteMap
    fun color(coloring: Coloring<D>, rootColor: Color, black: Int, maxWhite: Int)

    fun subtreeSize(): Int
}