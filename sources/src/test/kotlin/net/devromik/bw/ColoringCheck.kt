package net.devromik.bw

import net.devromik.bw.Color.*
import net.devromik.tree.Tree
import kotlin.test.*

/**
 * @author Shulnyaev Roman
 */
fun checkColoring(
    tree: Tree<String>,
    treeSize: Int,
    coloring: Coloring<String>,
    black: Int,
    white: Int) {

    var actualBlack = 0
    var actualWhite = 0
    var actualGray = 0

    val treeIter = tree.levelOrderIterator()

    while (treeIter.hasNext) {
        val node = treeIter.next()
        val nodeColor = coloring.colorOf(node)

        when (nodeColor) {
            BLACK, WHITE -> {
                if (nodeColor == BLACK) ++actualBlack else ++actualWhite
                val incompatibleColor = if (nodeColor == BLACK) WHITE else BLACK

                if (!node.isRoot) {
                    assertNotEquals(incompatibleColor, coloring.colorOf(node.parent!!))
                }

                for (child in node) {
                    assertNotEquals(incompatibleColor, coloring.colorOf(child))
                }
            }
            GRAY -> {
                ++actualGray
            }
        }
    }

    assertEquals(black, actualBlack)
    assertEquals(white, actualWhite)
    assertEquals(treeSize - (black + white), actualGray)
}