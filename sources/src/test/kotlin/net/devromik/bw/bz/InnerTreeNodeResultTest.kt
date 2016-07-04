package net.devromik.bw.bz

import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.tree.IndexedTreeInfo
import net.devromik.tree.Tree
import net.devromik.tree.TreeNode
import net.devromik.tree.root
import org.junit.Assert.*
import org.junit.Test

/**
 * @author Shulnyaev Roman
 */
class InnerTreeNodeResultTest {

    @Test fun isFusionStack() {
        val node = TreeNode<String>()
        val result = InnerTreeNodeResult(node, 1)

        val fusion_1 = SingleChildFusion(FixedRootColorMaxWhiteMap.forSingleNode())
        val fusion_2 = SingleChildFusion(FixedRootColorMaxWhiteMap.forSingleNode())
        val fusion_3 = SingleChildFusion(FixedRootColorMaxWhiteMap.forSingleNode())

        result.pushFusion(fusion_1)
        assertEquals(fusion_1, result.topFusion())

        result.pushFusion(fusion_2)
        assertEquals(fusion_2, result.topFusion())

        result.pushFusion(fusion_3)
        assertEquals(fusion_3, result.topFusion())
    }
    
    @Test fun canBeReduced() {
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
        val treeResult = IndexedTreeInfo<String, TreeNodeResult<String>>(tree)

        val root = tree.root
        val rootResult = InnerTreeNodeResult(root, 5)
        assertEquals(5, rootResult.initSubtreeSize())
        assertEquals(5, rootResult.subtreeSize())
        treeResult[root] = rootResult

        val child_1 = root.childAt(0)
        val child_1_Result = InnerTreeNodeResult(child_1, 2)
        assertEquals(2, child_1_Result.initSubtreeSize())
        assertEquals(2, child_1_Result.subtreeSize())
        treeResult[child_1] = child_1_Result

        val child_1_1 = child_1.childAt(0)
        val child_1_1_Result = LeafTreeNodeResult(child_1_1)
        assertEquals(1, child_1_1_Result.initSubtreeSize())
        assertEquals(1, child_1_1_Result.subtreeSize())
        treeResult[child_1_1] = child_1_1_Result

        val child_2 = root.childAt(1)
        val child_2_Result = InnerTreeNodeResult(child_2, 2)
        assertEquals(2, child_2_Result.initSubtreeSize())
        assertEquals(2, child_2_Result.subtreeSize())
        treeResult[child_2] = child_2_Result

        val child_2_1 = child_2.childAt(0)
        val child_2_1_Result = LeafTreeNodeResult(child_2_1)
        assertEquals(1, child_2_1_Result.initSubtreeSize())
        assertEquals(1, child_2_1_Result.subtreeSize())
        treeResult[child_2_1] = child_2_1_Result

        // ****************************** //

        child_1_Result.reduce(treeResult)

        assertEquals(5, rootResult.initSubtreeSize())
        assertEquals(4, rootResult.subtreeSize())

        assertEquals(2, child_1_Result.initSubtreeSize())
        assertEquals(1, child_1_Result.subtreeSize())

        assertEquals(1, child_1_1_Result.initSubtreeSize())
        assertEquals(1, child_1_1_Result.subtreeSize())

        assertEquals(2, child_2_Result.initSubtreeSize())
        assertEquals(2, child_2_Result.subtreeSize())

        assertEquals(1, child_2_1_Result.initSubtreeSize())
        assertEquals(1, child_2_1_Result.subtreeSize())

        // ****************************** //

        child_2_Result.reduce(treeResult)

        assertEquals(5, rootResult.initSubtreeSize())
        assertEquals(3, rootResult.subtreeSize())

        assertEquals(2, child_1_Result.initSubtreeSize())
        assertEquals(1, child_1_Result.subtreeSize())

        assertEquals(1, child_1_1_Result.initSubtreeSize())
        assertEquals(1, child_1_1_Result.subtreeSize())

        assertEquals(2, child_2_Result.initSubtreeSize())
        assertEquals(1, child_2_Result.subtreeSize())

        assertEquals(1, child_2_1_Result.initSubtreeSize())
        assertEquals(1, child_2_1_Result.subtreeSize())

        // ****************************** //

        rootResult.reduce(treeResult)

        assertEquals(5, rootResult.initSubtreeSize())
        assertEquals(1, rootResult.subtreeSize())

        assertEquals(2, child_1_Result.initSubtreeSize())
        assertEquals(1, child_1_Result.subtreeSize())

        assertEquals(1, child_1_1_Result.initSubtreeSize())
        assertEquals(1, child_1_1_Result.subtreeSize())

        assertEquals(2, child_2_Result.initSubtreeSize())
        assertEquals(1, child_2_Result.subtreeSize())

        assertEquals(1, child_2_1_Result.initSubtreeSize())
        assertEquals(1, child_2_1_Result.subtreeSize())
    }
}