package net.devromik.bw.bz

import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.tree.TreeInfo
import net.devromik.tree.TreeNode

/**
 * @author Shulnyaev Roman
 */
class LeafTreeNodeResult<D>(override val node: TreeNode<D>) : TreeNodeResult<D> {
    override fun initSubtreeSize() = 1
    override fun subtreeSize() = 1

    override fun subtreeMaxWhiteMap() = FixedRootColorMaxWhiteMap.forSingleNode()

    override fun pushFusion(fusion: Fusion) = throw UnsupportedOperationException()
    override fun topFusion() = throw UnsupportedOperationException()

    override fun reduce(treeResult: TreeInfo<D, TreeNodeResult<D>>) {}
}