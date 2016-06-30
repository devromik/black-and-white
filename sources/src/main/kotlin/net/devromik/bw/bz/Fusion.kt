package net.devromik.bw.bz

import net.devromik.bw.Color

/**
 * @author Shulnyaev Roman
 */
abstract class Fusion<D>(val leftFusion: Fusion<D>?, val rightFusion: Fusion<D>?) {
    abstract fun minGrayFor(rootColor: Color, black: Int): Int
}