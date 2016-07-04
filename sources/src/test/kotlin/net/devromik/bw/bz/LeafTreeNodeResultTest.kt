package net.devromik.bw.bz

import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.bw.assert_FixedRootColorMaxWhiteMap_Equals
import net.devromik.tree.TreeNode
import org.junit.Test
import org.junit.Assert.*

/**
 * @author Shulnyaev Roman
 */
class LeafTreeNodeResultTest {

    @Test fun initSubtreeSize_ShouldBe_EqualTo_1() {
        val result = LeafTreeNodeResult<String>(TreeNode())
        assertEquals(1, result.initSubtreeSize())
    }

    @Test fun subtreeSize_ShouldBe_EqualTo_1() {
        val result = LeafTreeNodeResult<String>(TreeNode())
        assertEquals(1, result.subtreeSize())
    }

    @Test fun subtreeMaxWhiteMap_ShouldBe_OneForSingleNode() {
        val result = LeafTreeNodeResult<String>(TreeNode())
        assert_FixedRootColorMaxWhiteMap_Equals(FixedRootColorMaxWhiteMap.forSingleNode(), result.subtreeMaxWhiteMap())
    }
}