package net.devromik.bw.x

import net.devromik.bw.*
import net.devromik.bw.Color.*
import net.devromik.tree.TreeNode
import java.lang.Math.max

/**
 * @author Shulnyaev Roman
 */
class PairXResult<D>(
    val root: TreeNode<D>,
    val top: XResult<D>,
    val childPos: Int = 0) : XResult<D> {

    val child = root.childAt(childPos)
    val subtreeSize = top.subtreeSize() + 1
    val topMaxWhiteMap = top.maxWhiteMap()
    val maxWhiteMap: FixedRootColorMaxWhiteMap

    // ****************************** //

    init {
        maxWhiteMap = FixedRootColorMaxWhiteMap(subtreeSize)

        for (black in (0..subtreeSize)) {
            // BLACK
            maxWhiteMap[BLACK, black] = max(
                topMaxWhiteMap[BLACK, black - 1],
                topMaxWhiteMap[BLACK, black])

            // WHITE
            var topMaxWhite = topMaxWhiteMap[WHITE, black]
            maxWhiteMap[WHITE, black] = if (topMaxWhite != INVALID_MAX_WHITE) topMaxWhite + 1 else INVALID_MAX_WHITE

            // GRAY
            topMaxWhite = topMaxWhiteMap[GRAY, black]
            maxWhiteMap[GRAY, black] = max(
                topMaxWhiteMap[GRAY, black - 1],
                if (topMaxWhite != INVALID_MAX_WHITE) topMaxWhite + 1 else INVALID_MAX_WHITE)
        }
    }

    // ****************************** //

    override fun maxWhiteMap() = maxWhiteMap

    override fun color(coloring: Coloring<D>, rootColor: Color, black: Int, maxWhite: Int) {
        coloring.color(root, rootColor)

        // BLACK
        if (rootColor == BLACK) {
            if (topMaxWhiteMap[BLACK, black - 1] > topMaxWhiteMap[BLACK, black]) {
                top.color(coloring, BLACK, black - 1, maxWhite)
                coloring.color(child, BLACK)
            }
            else {
                top.color(coloring, BLACK, black, maxWhite)
                coloring.color(child, GRAY)
            }
        }
        // WHITE
        else if (rootColor == WHITE) {
            top.color(coloring, WHITE, black, maxWhite - 1)
            coloring.color(child, WHITE)
        }
        // GRAY
        else {
            if (topMaxWhiteMap[GRAY, black - 1] > topMaxWhiteMap[GRAY, black]) {
                top.color(coloring, GRAY, black - 1, maxWhite)
                coloring.color(child, BLACK)
            }
            else {
                top.color(coloring, GRAY, black, maxWhite - 1)
                coloring.color(child, WHITE)
            }
        }
    }

    override fun subtreeSize() = subtreeSize
}