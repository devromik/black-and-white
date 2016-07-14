package net.devromik.bw.x

import net.devromik.bw.Color.*
import net.devromik.bw.Coloring
import net.devromik.bw.INVALID_MAX_WHITE
import net.devromik.tree.Tree
import net.devromik.tree.root
import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class PairXResultTest {

    val tree: Tree<String>
    val top: SingleNodeXResult<String>
    val result: PairXResult<String>

    // ****************************** //

    init {
        tree = Tree(
            root() {
                child()
            }
        )

        tree.index()

        top = SingleNodeXResult(tree.root)
        result = PairXResult(tree.root, top)
    }

    // ****************************** //

    @Test fun maxWhiteMap() {
        val maxWhiteMap = result.maxWhiteMap()

        // BLACK
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 3))

        // WHITE
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(2,                 maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 3))

        // GRAY
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(1,                 maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 2))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 3))
    }

    @Test fun coloring() {
        val coloring = Coloring(tree)

        // BLACK
        result.color(coloring, rootColor = BLACK, black = 1, maxWhite = 0)
        assertEquals(BLACK, coloring.colorOf(tree.root))
        assertEquals(GRAY, coloring.colorOf(tree.root.childAt(0)))
        assertEquals(1, coloring.blackCount())
        assertEquals(0, coloring.whiteCount())
        assertEquals(1, coloring.grayCount())

        result.color(coloring, rootColor = BLACK, black = 2, maxWhite = 0)
        assertEquals(BLACK, coloring.colorOf(tree.root))
        assertEquals(BLACK, coloring.colorOf(tree.root.childAt(0)))
        assertEquals(2, coloring.blackCount())
        assertEquals(0, coloring.whiteCount())
        assertEquals(0, coloring.grayCount())

        // WHITE
        result.color(coloring, rootColor = WHITE, black = 0, maxWhite = 2)
        assertEquals(WHITE, coloring.colorOf(tree.root))
        assertEquals(WHITE, coloring.colorOf(tree.root.childAt(0)))
        assertEquals(0, coloring.blackCount())
        assertEquals(2, coloring.whiteCount())
        assertEquals(0, coloring.grayCount())

        // GRAY
        result.color(coloring, rootColor = GRAY, black = 0, maxWhite = 1)
        assertEquals(GRAY, coloring.colorOf(tree.root))
        assertEquals(WHITE, coloring.colorOf(tree.root.childAt(0)))
        assertEquals(0, coloring.blackCount())
        assertEquals(1, coloring.whiteCount())
        assertEquals(1, coloring.grayCount())

        result.color(coloring, rootColor = GRAY, black = 1, maxWhite = 0)
        assertEquals(GRAY, coloring.colorOf(tree.root))
        assertEquals(BLACK, coloring.colorOf(tree.root.childAt(0)))
        assertEquals(1, coloring.blackCount())
        assertEquals(0, coloring.whiteCount())
        assertEquals(1, coloring.grayCount())
    }

    @Test fun subtreeSize_ShouldBe_IncrementedSubtreeSizeOfTopResult() {
        assertEquals(2, result.subtreeSize())
    }
}