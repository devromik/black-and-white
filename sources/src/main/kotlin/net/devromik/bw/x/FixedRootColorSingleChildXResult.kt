@file:Suppress("PARAMETER_NAME_CHANGED_ON_OVERRIDE")

package net.devromik.bw.x

import net.devromik.bw.Color
import net.devromik.bw.Color.*
import net.devromik.bw.Coloring
import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.bw.INVALID_MAX_WHITE
import net.devromik.tree.TreeNode
import java.lang.Math.max

/**
 * The last chain node is treated as a new root.
 *
 * @author Shulnyaev Roman
 */
class FixedRootColorSingleChildXResult<D>(
    val root: TreeNode<D>,
    val rootColor: Color,
    val top: XResult<D>,
    val firstChainNodePos: Int = 0) : XResult<D> {

    val subtreeSize: Int
    val lastChainNode: TreeNode<D>
    val result: XResult<D>

    // ****************************** //

    init {
        var subtreeSizeIncrease = 1
        var currChainNode = root.childAt(firstChainNodePos)
        var currResult: XResult<D> = FirstChainNodeXResult(root, rootColor, top, currChainNode)

        while (currChainNode.hasOnlyChild) {
            ++subtreeSizeIncrease
            currChainNode = currChainNode.childAt(0)
            currResult = NotFirstChainNodeXResult(currChainNode, currResult)
        }

        subtreeSize = top.subtreeSize() + subtreeSizeIncrease
        lastChainNode = currChainNode
        result = currResult
    }

    /**
     * A MaxWhite map is built regarding the last chain node.
     */
    override fun maxWhiteMap() = result.maxWhiteMap()

    /**
     * Coloring is doing regarding the last chain node.
     */
    override fun color(coloring: Coloring<D>, lastChainNodeColor: Color, black: Int, maxWhite: Int) {
        result.color(coloring, lastChainNodeColor, black, maxWhite)
    }

    override fun subtreeSize() = subtreeSize
}

private class FirstChainNodeXResult<D>(
    val root: TreeNode<D>,
    val rootColor: Color,
    val top: XResult<D>,
    val firstChainNode: TreeNode<D>) : XResult<D> {

    val subtreeSize = top.subtreeSize() + 1
    val topMaxWhiteMap = top.maxWhiteMap()
    val maxWhiteMap: FixedRootColorMaxWhiteMap

    // ****************************** //

    init {
        maxWhiteMap = FixedRootColorMaxWhiteMap(subtreeSize)

        for (black in 0..subtreeSize) {
            if (rootColor == BLACK) {
                maxWhiteMap[BLACK, black] = topMaxWhiteMap[BLACK, black - 1]
                maxWhiteMap[WHITE, black] = INVALID_MAX_WHITE
                maxWhiteMap[GRAY, black] = topMaxWhiteMap[BLACK, black]
            }
            else if (rootColor == WHITE) {
                maxWhiteMap[BLACK, black] = INVALID_MAX_WHITE

                var topMaxWhite = topMaxWhiteMap[WHITE, black]
                maxWhiteMap[WHITE, black] = if (topMaxWhite != INVALID_MAX_WHITE) topMaxWhite + 1 else INVALID_MAX_WHITE

                topMaxWhite = topMaxWhiteMap[WHITE, black]
                maxWhiteMap[GRAY, black] = if (topMaxWhite != INVALID_MAX_WHITE) topMaxWhite else INVALID_MAX_WHITE
            }
            else {
                maxWhiteMap[BLACK, black] = topMaxWhiteMap[GRAY, black - 1]

                val topMaxWhite = topMaxWhiteMap[GRAY, black]
                maxWhiteMap[WHITE, black] = if (topMaxWhite != INVALID_MAX_WHITE) topMaxWhite + 1 else INVALID_MAX_WHITE

                maxWhiteMap[GRAY, black] = topMaxWhiteMap[GRAY, black]
            }
        }
    }

    /**
     * A MaxWhite map is built regarding the first node of the chain.
     */
    override fun maxWhiteMap() = maxWhiteMap

    override fun color(coloring: Coloring<D>, firstChainNodeColor: Color, black: Int, maxWhite: Int) {
        var newBlack = black
        var newMaxWhite = maxWhite

        coloring.color(firstChainNode, firstChainNodeColor)

        if (firstChainNodeColor == BLACK) {
            --newBlack
        }
        else if (firstChainNodeColor == WHITE) {
            --newMaxWhite
        }

        coloring.color(root, rootColor)

        if (newBlack > 0 || newMaxWhite > 0) {
            top.color(coloring, rootColor, newBlack, newMaxWhite)
        }
    }

    override fun subtreeSize() = subtreeSize
}

private class NotFirstChainNodeXResult<D>(
    val chainNode: TreeNode<D>,
    val prev: XResult<D>) : XResult<D> {

    val subtreeSize = prev.subtreeSize() + 1
    val prevMaxWhiteMap = prev.maxWhiteMap()
    val maxWhiteMap: FixedRootColorMaxWhiteMap

    // ****************************** //

    init {
        maxWhiteMap = FixedRootColorMaxWhiteMap(subtreeSize)

        for (black in 0..subtreeSize) {
            // BLACK
            maxWhiteMap[BLACK, black] = max(
                prevMaxWhiteMap[BLACK, black - 1],
                prevMaxWhiteMap[GRAY, black - 1])

            // WHITE
            maxWhiteMap[WHITE, black] = max(
                prevMaxWhiteMap[WHITE, black],
                prevMaxWhiteMap[GRAY, black])

            if (maxWhiteMap[WHITE, black] != INVALID_MAX_WHITE) {
                ++maxWhiteMap[WHITE, black]
            }

            // GRAY
            val blackWhiteMax = max(prevMaxWhiteMap[BLACK, black], prevMaxWhiteMap[WHITE, black])
            maxWhiteMap[GRAY, black] = max(blackWhiteMax, prevMaxWhiteMap[GRAY, black])
        }
    }

    /**
     * A MaxWhite map is built regarding the current node of the chain.
     */
    override fun maxWhiteMap() = maxWhiteMap

    override fun color(coloring: Coloring<D>, chainNodeColor: Color, black: Int, maxWhite: Int) {
        coloring.color(chainNode, chainNodeColor)

        if (chainNodeColor == BLACK) {
            if (prevMaxWhiteMap[BLACK, black - 1] > prevMaxWhiteMap[GRAY, black - 1]) {
                prev.color(coloring, BLACK, black - 1, maxWhite)
            }
            else {
                prev.color(coloring, GRAY, black - 1, maxWhite)
            }
        }
        else if (chainNodeColor == WHITE) {
            if (prevMaxWhiteMap[WHITE, black] > prevMaxWhiteMap[GRAY, black]) {
                prev.color(coloring, WHITE, black, maxWhite - 1)
            }
            else {
                prev.color(coloring, GRAY, black, maxWhite - 1)
            }
        }
        else if (chainNodeColor == GRAY) {
            when (maxWhiteMap[GRAY, black]) {
                prevMaxWhiteMap[BLACK, black] -> prev.color(coloring, BLACK, black, maxWhite)
                prevMaxWhiteMap[WHITE, black] -> prev.color(coloring, WHITE, black, maxWhite)
                else -> prev.color(coloring, GRAY, black, maxWhite)
            }
        }
    }

    override fun subtreeSize() = subtreeSize
}