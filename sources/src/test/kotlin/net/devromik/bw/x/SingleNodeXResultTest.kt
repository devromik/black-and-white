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
class SingleNodeXResultTest {

    val tree: Tree<String>
    val result: SingleNodeXResult<String>

    // ****************************** //

    init {
        tree = Tree(root())
        tree.index()

        result = SingleNodeXResult(tree.root)
    }

    // ****************************** //

    @Test fun maxWhiteMap() {
        val maxWhiteMap = result.maxWhiteMap()

        // BLACK
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = -1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 0))
        assertEquals(0,                 maxWhiteMap.get(rootColor = BLACK, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = BLACK, black = 2))

        // WHITE
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = -1))
        assertEquals(1,                 maxWhiteMap.get(rootColor = WHITE, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = WHITE, black = 2))

        // GRAY
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = -1))
        assertEquals(0,                 maxWhiteMap.get(rootColor = GRAY, black = 0))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 1))
        assertEquals(INVALID_MAX_WHITE, maxWhiteMap.get(rootColor = GRAY, black = 2))
    }

    @Test fun coloring() {
        val coloring = Coloring(tree)

        // BLACK
        result.color(coloring, rootColor = BLACK, black = 1, maxWhite = 0)
        assertEquals(BLACK, coloring.colorOf(tree.root))
        assertEquals(1, coloring.blackCount())
        assertEquals(0, coloring.whiteCount())
        assertEquals(0, coloring.grayCount())

        // WHITE
        result.color(coloring, rootColor = WHITE, black = 0, maxWhite = 1)
        assertEquals(WHITE, coloring.colorOf(tree.root))
        assertEquals(0, coloring.blackCount())
        assertEquals(1, coloring.whiteCount())
        assertEquals(0, coloring.grayCount())

        // GRAY
        result.color(coloring, rootColor = GRAY, black = 0, maxWhite = 0)
        assertEquals(GRAY, coloring.colorOf(tree.root))
        assertEquals(0, coloring.blackCount())
        assertEquals(0, coloring.whiteCount())
        assertEquals(1, coloring.grayCount())
    }

    @Test fun subtreeSize_ShouldBe_EqualTo_1() {
        assertEquals(1, result.subtreeSize())
    }
}