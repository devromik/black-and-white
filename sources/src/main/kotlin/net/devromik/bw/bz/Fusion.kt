package net.devromik.bw.bz

import net.devromik.bw.Color

/**
 * @author Shulnyaev Roman
 */
abstract class Fusion(val leftFusion: Fusion?, val rightFusion: Fusion?) {
    abstract fun minGrayFor(rootColor: Color, black: Int): Int
}