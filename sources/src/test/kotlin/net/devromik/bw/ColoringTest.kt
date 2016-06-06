package net.devromik.bw

import net.devromik.bw.Color.*
import net.devromik.tree.Tree
import net.devromik.tree.root
import org.junit.Assert.*
import org.junit.Test

/**
 * @author Shulnyaev Roman
 */
class ColoringTest {

    @Test fun canColorNode() {
        val tree: Tree<String> = Tree(
            root("root") {
                child("child_1")
                child("child_2")
            }
        )

        tree.index()

        val root = tree.root
        val child_1 = root.childAt(0)
        val child_2 = root.childAt(1)

        // ****************************** //

        val coloring = Coloring(tree)

        assertFalse(coloring.isColored(root))
        assertNull(coloring.colorOf(root))

        assertFalse(coloring.isColored(child_1))
        assertNull(coloring.colorOf(child_1))

        assertFalse(coloring.isColored(child_2))
        assertNull(coloring.colorOf(child_2))

        // ****************************** //

        coloring.color(root, BLACK)
        coloring.color(child_2, WHITE)

        assertEquals(BLACK, coloring.colorOf(root))

        assertFalse(coloring.isColored(child_1))
        assertNull(coloring.colorOf(child_1))

        assertEquals(WHITE, coloring.colorOf(child_2))

        // ****************************** //

        coloring.color(child_1, GRAY)

        assertEquals(BLACK, coloring.colorOf(root))
        assertEquals(GRAY, coloring.colorOf(child_1))
        assertEquals(WHITE, coloring.colorOf(child_2))
    }

    @Test fun canProvideNumberOfNodesColoredGivenColor() {
        val tree: Tree<String> = Tree(
            root("root") {
                child("child_1")
                child("child_2")
                child("child_3")
                child("child_4")
                child("child_5")
            }
        )

        tree.index()

        val root = tree.root
        val child_1 = root.childAt(0)
        val child_2 = root.childAt(1)
        val child_3 = root.childAt(2)
        val child_4 = root.childAt(3)
        val child_5 = root.childAt(4)

        // ****************************** //

        val coloring = Coloring(tree)

        assertEquals(0, coloring.blackCount())
        assertEquals(0, coloring.whiteCount())
        assertEquals(0, coloring.grayCount())

        // ****************************** //

        coloring.color(child_1, BLACK)
        coloring.color(child_2, BLACK)
        coloring.color(child_3, WHITE)
        coloring.color(child_4, GRAY)
        coloring.color(child_5, GRAY)

        assertEquals(2, coloring.blackCount())
        assertEquals(1, coloring.whiteCount())
        assertEquals(2, coloring.grayCount())
    }
}