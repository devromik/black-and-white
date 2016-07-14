package net.devromik.bw.x

import net.devromik.bw.Color.*
import net.devromik.bw.Coloring
import net.devromik.bw.INVALID_MAX_WHITE
import net.devromik.bw.checkColoring
import net.devromik.tree.Tree
import net.devromik.tree.root
import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class FixedRootColorSingleChildXResultTest {

    /**
     * root -> child -> child
     */
    @Test fun maxWhiteMap_1() {
        val tree = Tree<String>(
            root() {
                child {
                    child()
                }
            }
        )

        tree.index()
        val top = SingleNodeXResult(tree.root)

        // BLACK
        var result = FixedRootColorSingleChildXResult(tree.root, BLACK, top)
        var maxWhiteMap = result.maxWhiteMap()

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 4))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(1,                 maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 4))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 4))

        // WHITE
        result = FixedRootColorSingleChildXResult(tree.root, WHITE, top)
        maxWhiteMap = result.maxWhiteMap()

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(1,                 maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 4))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(3,                 maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 4))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(2,                 maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 4))

        // GRAY
        result = FixedRootColorSingleChildXResult(tree.root, GRAY, top)
        maxWhiteMap = result.maxWhiteMap()

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 4))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(2,                 maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 4))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(1,                 maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 4))
    }

    /**
     * root
     *     -> child
     *             -> child
     *             -> child
     */
    @Test fun maxWhiteMap_2() {
        val tree = Tree<String>(
            root() {
                child {
                    child()
                    child()
                }
            }
        )

        tree.index()
        val top = SingleNodeXResult(tree.root)

        // BLACK
        var result = FixedRootColorSingleChildXResult(tree.root, BLACK, top)
        var maxWhiteMap = result.maxWhiteMap()

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 3))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 3))

        // WHITE
        result = FixedRootColorSingleChildXResult(tree.root, WHITE, top)
        maxWhiteMap = result.maxWhiteMap()

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 3))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(2,                 maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(1,                 maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 3))

        // GRAY
        result = FixedRootColorSingleChildXResult(tree.root, GRAY, top)
        maxWhiteMap = result.maxWhiteMap()

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 3))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(1,                 maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 3))
    }

    /**
     * root
     *     -> child
     *             -> child
     *                     -> child
     *                             -> child
     *                             -> child
     */
    @Test fun maxWhiteMap_3() {
        val tree = Tree<String>(
            root() {
                child {
                    child {
                        child {
                            child()
                            child()
                        }
                    }
                }
            }
        )

        tree.index()
        val top = SingleNodeXResult(tree.root)

        // BLACK
        var result = FixedRootColorSingleChildXResult(tree.root, BLACK, top)
        var maxWhiteMap = result.maxWhiteMap()

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 3))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 4))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 5))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(2,                 maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(1,                 maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 4))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 5))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(1,                 maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 4))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 5))

        // WHITE
        result = FixedRootColorSingleChildXResult(tree.root, WHITE, top)
        maxWhiteMap = result.maxWhiteMap()

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(2,                 maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(1,                 maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 4))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 5))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(4,                 maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 4))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 5))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(3,                 maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(1,                 maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 4))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 5))

        // GRAY
        result = FixedRootColorSingleChildXResult(tree.root, GRAY, top)
        maxWhiteMap = result.maxWhiteMap()

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(1,                 maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 4))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 5))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(3,                 maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(1,                 maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 4))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 5))

        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(2,                 maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 3))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 4))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 5))
    }

    /**
     * root -> child -> child
     */
    @Test fun coloring_1() {
        val tree = Tree<String>(
            root() {
                child {
                    child()
                }
            }
        )

        tree.index()
        val top = SingleNodeXResult(tree.root)
        val coloring = Coloring(tree)

        /* ***** Root color BLACK ***** */

        var result = FixedRootColorSingleChildXResult(tree.root, BLACK, top)

        // Last chain node color BLACK
        result.color(coloring, lastChainNodeColor = BLACK, black = 2, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 0)

        result.color(coloring, lastChainNodeColor = BLACK, black = 3, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 3, white = 0)

        // Last chain node color WHITE
        result.color(coloring, lastChainNodeColor = WHITE, black = 1, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 1)

        // Last chain node color GRAY
        result.color(coloring, lastChainNodeColor = GRAY, black = 1, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 0)

        result.color(coloring, lastChainNodeColor = GRAY, black = 2, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 0)

        /* ***** Root color WHITE ***** */

        result = FixedRootColorSingleChildXResult(tree.root, WHITE, top)

        // Last chain node color BLACK
        result.color(coloring, lastChainNodeColor = BLACK, black = 1, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 1)

        // Last chain node color WHITE
        result.color(coloring, lastChainNodeColor = WHITE, black = 0, maxWhite = 3)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 3)

        // Last chain node color GRAY
        result.color(coloring, lastChainNodeColor = GRAY, black = 0, maxWhite = 2)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 2)

        /* ***** Root color GRAY ***** */

        result = FixedRootColorSingleChildXResult(tree.root, GRAY, top)

        // Last chain node color BLACK
        result.color(coloring, lastChainNodeColor = BLACK, black = 1, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 0)

        result.color(coloring, lastChainNodeColor = BLACK, black = 2, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 0)

        // Last chain node color WHITE
        result.color(coloring, lastChainNodeColor = WHITE, black = 0, maxWhite = 2)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 2)

        // Last chain node color GRAY
        result.color(coloring, lastChainNodeColor = GRAY, black = 0, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 1)

        result.color(coloring, lastChainNodeColor = GRAY, black = 1, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 0)
    }

    /**
     * root
     *     -> child
     *             -> child
     *             -> child
     */
    @Test fun coloring_2() {
        val tree = Tree<String>(
            root() {
                child { // new root
                    child() // should be ignored
                    child() // should be ignored
                }
            }
        )

        tree.index()
        val top = SingleNodeXResult(tree.root)
        val coloring = Coloring(tree)

        /* ***** Root color BLACK ***** */

        var result = FixedRootColorSingleChildXResult(tree.root, BLACK, top)

        // Last chain node color BLACK
        result.color(coloring, lastChainNodeColor = BLACK, black = 2, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 0)

        // Last chain node color GRAY
        result.color(coloring, lastChainNodeColor = GRAY, black = 1, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 0)

        /* ***** Root color WHITE ***** */

        result = FixedRootColorSingleChildXResult(tree.root, WHITE, top)

        // Last chain node color WHITE
        result.color(coloring, lastChainNodeColor = WHITE, black = 0, maxWhite = 2)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 2)

        // Last chain node color GRAY
        result.color(coloring, lastChainNodeColor = GRAY, black = 0, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 1)

        /* ***** Root color GRAY ***** */

        result = FixedRootColorSingleChildXResult(tree.root, GRAY, top)

        // Last chain node color BLACK
        result.color(coloring, lastChainNodeColor = BLACK, black = 1, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 0)

        // Last chain node color WHITE
        result.color(coloring, lastChainNodeColor = WHITE, black = 0, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 1)

        // Last chain node color GRAY
        result.color(coloring, lastChainNodeColor = GRAY, black = 0, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 0)
    }

    /**
     * root
     *     -> child
     *             -> child
     *                     -> child
     *                             -> child
     *                             -> child
     */
    @Test fun coloring_3() {
        val tree = Tree<String>(
            root() {
                child {
                    child {
                        child { // new root
                            child() // should be ignored
                            child() // should be ignored
                        }
                    }
                }
            }
        )

        tree.index()
        val top = SingleNodeXResult(tree.root)
        val coloring = Coloring(tree)

        /* ***** Root color BLACK ***** */

        var result = FixedRootColorSingleChildXResult(tree.root, BLACK, top)

        // Last chain node color BLACK
        result.color(coloring, lastChainNodeColor = BLACK, black = 2, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 0)

        result.color(coloring, lastChainNodeColor = BLACK, black = 3, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 3, white = 0)

        result.color(coloring, lastChainNodeColor = BLACK, black = 4, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 4, white = 0)

        // Last chain node color WHITE
        result.color(coloring, lastChainNodeColor = WHITE, black = 1, maxWhite = 2)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 2)

        result.color(coloring, lastChainNodeColor = WHITE, black = 2, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 1)

        // Last chain node color GRAY
        result.color(coloring, lastChainNodeColor = GRAY, black = 1, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 1)

        result.color(coloring, lastChainNodeColor = GRAY, black = 2, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 0)

        result.color(coloring, lastChainNodeColor = GRAY, black = 3, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 3, white = 0)

        /* ***** Root color WHITE ***** */

        result = FixedRootColorSingleChildXResult(tree.root, WHITE, top)

        // Last chain node color BLACK
        result.color(coloring, lastChainNodeColor = BLACK, black = 1, maxWhite = 2)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 2)

        result.color(coloring, lastChainNodeColor = BLACK, black = 2, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 1)

        // Last chain node color WHITE
        result.color(coloring, lastChainNodeColor = WHITE, black = 0, maxWhite = 4)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 4)

        // Last chain node color GRAY
        result.color(coloring, lastChainNodeColor = GRAY, black = 0, maxWhite = 3)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 3)

        result.color(coloring, lastChainNodeColor = GRAY, black = 1, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 1)

        /* ***** Root color GRAY ***** */

        result = FixedRootColorSingleChildXResult(tree.root, GRAY, top)

        // Last chain node color BLACK
        result.color(coloring, lastChainNodeColor = BLACK, black = 1, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 1)

        result.color(coloring, lastChainNodeColor = BLACK, black = 2, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 0)

        result.color(coloring, lastChainNodeColor = BLACK, black = 3, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 3, white = 0)

        // Last chain node color WHITE
        result.color(coloring, lastChainNodeColor = WHITE, black = 0, maxWhite = 3)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 3)

        result.color(coloring, lastChainNodeColor = WHITE, black = 1, maxWhite = 1)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 1)

        // Last chain node color GRAY
        result.color(coloring, lastChainNodeColor = GRAY, black = 0, maxWhite = 2)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 0, white = 2)

        result.color(coloring, lastChainNodeColor = GRAY, black = 1, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 1, white = 0)

        result.color(coloring, lastChainNodeColor = GRAY, black = 2, maxWhite = 0)
        checkColoring(tree, result.subtreeSize, coloring = coloring, black = 2, white = 0)
    }

    /**
     * root -> child -> child
     */
    @Test fun subtreeSize_1() {
        val tree = Tree<String>(
            root() {
                child {
                    child()
                }
            }
        )

        tree.index()
        val top = SingleNodeXResult(tree.root)

        val result = FixedRootColorSingleChildXResult(tree.root, BLACK, top)
        assertEquals(3, result.subtreeSize())
        assertEquals(tree.root.childAt(0).childAt(0), result.lastChainNode)
    }

    /**
     * root
     *     -> child
     *             -> child
     *             -> child
     */
    @Test fun subtreeSize_2() {
        val tree = Tree<String>(
            root() {
                child { // new root
                    child() // should be ignored
                    child() // should be ignored
                }
            }
        )

        tree.index()
        val top = SingleNodeXResult(tree.root)

        val result = FixedRootColorSingleChildXResult(tree.root, BLACK, top)
        assertEquals(2, result.subtreeSize())
        assertEquals(tree.root.childAt(0), result.lastChainNode)
    }

    /**
     * root
     *     -> child
     *             -> child
     *                     -> child
     *                             -> child
     *                             -> child
     */
    @Test fun subtreeSize_3() {
        val tree = Tree<String>(
            root() {
                child {
                    child {
                        child { // new root
                            child() // should be ignored
                            child() // should be ignored
                        }
                    }
                }
            }
        )

        tree.index()
        val top = SingleNodeXResult(tree.root)

        val result = FixedRootColorSingleChildXResult(tree.root, BLACK, top)
        assertEquals(4, result.subtreeSize())
        assertEquals(tree.root.childAt(0).childAt(0).childAt(0), result.lastChainNode)
    }
}