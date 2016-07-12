package net.devromik.bw.bz

import net.devromik.bw.Color.*
import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.bw.INVALID_MAX_WHITE
import net.devromik.bw.assert_FixedRootColorMaxWhiteMap_Equals
import net.devromik.tree.Tree
import net.devromik.tree.TreeNode
import net.devromik.tree.root
import org.junit.Assert.*
import org.junit.Test

/**
 * @author Shulnyaev Roman
 */
class ResultImplTest {

    @Test fun initializesTreeResultCorrectly() {
        val tree: Tree<String> = Tree(
            root("root") {
                child("child_1") {
                    child("child_1_1")
                }
                child("child_2") {
                    child("child_2_1")
                }
            }
        )

        tree.index()
        val treeResult = initTreeResult(tree, tree.countSubtreeSizes())

        val root = tree.root
        val rootResult = treeResult[root]!!
        assertTrue(rootResult is InnerTreeNodeResult<String>)
        assertEquals(5, rootResult.subtreeSize())

        val child_1 = root.childAt(0)
        val child_1_Result = treeResult[child_1]!!
        assertTrue(child_1_Result is InnerTreeNodeResult<String>)
        assertEquals(2, child_1_Result.subtreeSize())

        val child_1_1 = child_1.childAt(0)
        val child_1_1_Result = treeResult[child_1_1]!!
        assertTrue(child_1_1_Result is LeafTreeNodeResult<String>)
        assertEquals(1, child_1_1_Result.subtreeSize())

        val child_2 = root.childAt(1)
        val child_2_Result = treeResult[child_2]!!
        assertTrue(child_2_Result is InnerTreeNodeResult<String>)
        assertEquals(2, child_2_Result.subtreeSize())

        val child_2_1 = child_2.childAt(0)
        val child_2_1_Result = treeResult[child_2_1]!!
        assertTrue(child_2_1_Result is LeafTreeNodeResult<String>)
        assertEquals(1, child_2_1_Result.subtreeSize())
    }

    @Test fun canSeparateSubtreeIntoSmallerOnes_EachOfWhichHasNoMoreThan_HalfTheInitialSubtree() {
        val tree: Tree<String> = Tree(
            root() {
                child("child_1") {
                    child()
                    child()
                }
                child() {
                    child()
                    child()
                }
            }
        )

        tree.index()
        val treeResult = initTreeResult(tree, tree.countSubtreeSizes())

        val root = tree.root
        var separator = separatorFor(root, treeResult)
        assertEquals(root, separator)

        val child_1_Result = treeResult[root.childAt(0)]!!
        child_1_Result.reduce(treeResult)

        separator = separatorFor(root, treeResult)
        assertEquals(root.childAt(1), separator)
    }
    
    @Test fun canMakePairMaxWhiteMap() {
        val childMaxWhiteMap = FixedRootColorMaxWhiteMap(treeSize = 2)

        childMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        childMaxWhiteMap[BLACK, 1] = 0
        childMaxWhiteMap[BLACK, 2] = 0

        childMaxWhiteMap[WHITE, 0] = 2
        childMaxWhiteMap[WHITE, 1] = INVALID_MAX_WHITE
        childMaxWhiteMap[WHITE, 2] = INVALID_MAX_WHITE

        childMaxWhiteMap[GRAY, 0] = 1
        childMaxWhiteMap[GRAY, 1] = 0
        childMaxWhiteMap[GRAY, 2] = INVALID_MAX_WHITE

        val childResult = InnerTreeNodeResult(node = TreeNode<String>(), initSubtreeSize = 2)
        childResult.subtreeMaxWhiteMap = childMaxWhiteMap

        val expectedPairMaxWhiteMap = FixedRootColorMaxWhiteMap(treeSize = 3)

        expectedPairMaxWhiteMap[BLACK, 0] = INVALID_MAX_WHITE
        expectedPairMaxWhiteMap[BLACK, 1] = 1
        expectedPairMaxWhiteMap[BLACK, 2] = 0
        expectedPairMaxWhiteMap[BLACK, 3] = 0

        expectedPairMaxWhiteMap[WHITE, 0] = 3
        expectedPairMaxWhiteMap[WHITE, 1] = 1
        expectedPairMaxWhiteMap[WHITE, 2] = INVALID_MAX_WHITE
        expectedPairMaxWhiteMap[WHITE, 3] = INVALID_MAX_WHITE

        expectedPairMaxWhiteMap[GRAY, 0] = 2
        expectedPairMaxWhiteMap[GRAY, 1] = 0
        expectedPairMaxWhiteMap[GRAY, 2] = 0
        expectedPairMaxWhiteMap[GRAY, 3] = INVALID_MAX_WHITE

        val pairMaxWhiteMap = parentResultForSingleChildResult(childResult)
        assert_FixedRootColorMaxWhiteMap_Equals(expectedPairMaxWhiteMap, pairMaxWhiteMap)
    }
}