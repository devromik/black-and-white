package net.devromik.bw

/**
 * @author Shulnyaev Roman
 */
interface Result<D> {
    fun maxWhite(): MaxWhite

    fun coloringExistsFor(black: Int, white: Int): Boolean

    /**
     * @throws IllegalArgumentException if there does not exist a coloring with such parameters.
     */
    fun coloringFor(black: Int, white: Int): Coloring<D>
}