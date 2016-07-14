package net.devromik.bw.x

import net.devromik.bw.Color
import net.devromik.bw.Color.*
import net.devromik.bw.Coloring
import net.devromik.bw.FixedRootColorMaxWhiteMap

/**
 * @author Shulnyaev Roman
 */
class SingleChildXResult<D>(
    val rootBlackResult: XResult<D>,
    val rootWhiteResult: XResult<D>,
    val rootGrayResult: XResult<D>) : XResult<D> {

    val results: Array<XResult<D>>
    val maxWhiteMaps: Array<FixedRootColorMaxWhiteMap>

    val maxWhiteMap: FixedRootColorMaxWhiteMap
    val subtreeSize = rootBlackResult.subtreeSize()

    // ****************************** //

    init {
        results = arrayOf(rootBlackResult, rootWhiteResult, rootGrayResult)
        maxWhiteMaps = arrayOf(rootBlackResult.maxWhiteMap(), rootWhiteResult.maxWhiteMap(), rootGrayResult.maxWhiteMap())

        maxWhiteMap = object : FixedRootColorMaxWhiteMap(subtreeSize()) {
            override fun get(rootColor: Color, black: Int) = maxWhiteMaps[rootColor.index][black]
        }
    }

    // ****************************** //

    override fun maxWhiteMap() = maxWhiteMap

    override fun color(coloring: Coloring<D>, rootColor: Color, black: Int, maxWhite: Int) {
        val maxWhiteMap = maxWhiteMaps[rootColor.index]

        val lastChainNodeColor =
            when (maxWhite) {
                maxWhiteMap[BLACK, black] -> BLACK
                maxWhiteMap[WHITE, black] -> WHITE
                else -> GRAY
            }

        val result = results[rootColor.index]
        result.color(coloring, lastChainNodeColor, black, maxWhite)
    }

    override fun subtreeSize() = subtreeSize
}