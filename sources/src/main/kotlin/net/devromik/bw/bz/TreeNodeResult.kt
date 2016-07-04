package net.devromik.bw.bz

import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.tree.TreeInfo
import net.devromik.tree.TreeNode

/**
 * @author Shulnyaev Roman
 */
interface TreeNodeResult<D> {
    val node: TreeNode<D>

    fun initSubtreeSize(): Int
    fun subtreeSize(): Int

    fun subtreeMaxWhiteMap(): FixedRootColorMaxWhiteMap

    fun pushFusion(fusion: Fusion)
    fun topFusion(): Fusion

    fun reduce(treeResult: TreeInfo<D, TreeNodeResult<D>>)
}