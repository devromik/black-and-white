package net.devromik.bw.bz

import net.devromik.bw.FixedRootColorMaxWhiteMap
import net.devromik.tree.TreeInfo
import net.devromik.tree.TreeNode
import java.util.*
import java.util.concurrent.atomic.AtomicInteger

/**
 * @author Shulnyaev Roman
 */
class InnerTreeNodeResult<D>(override val node: TreeNode<D>, val initSubtreeSize: Int) : TreeNodeResult<D> {

    private val subtreeSize = AtomicInteger(initSubtreeSize)
    var subtreeMaxWhiteMap: FixedRootColorMaxWhiteMap? = null
    private val fusionStack = ArrayDeque<Fusion>()

    // ****************************** //

    override fun initSubtreeSize() = initSubtreeSize
    override fun subtreeSize() = subtreeSize.get()

    override fun subtreeMaxWhiteMap() = subtreeMaxWhiteMap!!

    override fun pushFusion(fusion: Fusion) = fusionStack.addLast(fusion)
    override fun topFusion() = fusionStack.peekLast()

    override fun reduce(treeResult: TreeInfo<D, TreeNodeResult<D>>) {
        // At this point only current thread can update subtreeSize of nodes of the subtree.
        // So this store is safe.
        subtreeSize.set(1)
        var currAncestor = node.parent

        while (currAncestor != null) {
            (treeResult[currAncestor] as InnerTreeNodeResult<D>).updateSubtreeSize(treeResult)
            currAncestor = currAncestor.parent
        }
    }

    private fun updateSubtreeSize(treeResult: TreeInfo<D, TreeNodeResult<D>>) {
        // In general, several threads can update subtreeSize of nodes of the ancestor tree.
        // So we have to take care of it.
        var prevSubtreeSize = subtreeSize.get()

        while (true) {
            var newSubtreeSize = 1

            for (child in node) {
                newSubtreeSize += treeResult[child]!!.subtreeSize()
            }

            if (subtreeSize.compareAndSet(prevSubtreeSize, newSubtreeSize)) {
                break
            }
            else {
                prevSubtreeSize = subtreeSize.get()
            }
        }
    }
}