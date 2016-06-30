package net.devromik.bw

import java.lang.Integer.numberOfLeadingZeros

/**
 * @author Shulnyaev Roman
 */
fun log2(n: Int) = (Integer.SIZE - 1) - numberOfLeadingZeros(n)