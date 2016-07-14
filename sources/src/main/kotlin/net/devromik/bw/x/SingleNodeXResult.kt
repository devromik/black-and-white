package net.devromik.bw.x

import net.devromik.bw.*
import net.devromik.bw.FixedRootColorMaxWhiteMap.Companion.forSingleNode
import net.devromik.tree.TreeNode

/**
 * @author Shulnyaev Roman
 */
class SingleNodeXResult<D>(val singleNode: TreeNode<D>) : XResult<D> {

    val maxWhiteMap = forSingleNode()

    // ****************************** //

    override fun maxWhiteMap() = maxWhiteMap

    override fun color(coloring: Coloring<D>, rootColor: Color, black: Int, maxWhite: Int) {
        coloring.color(singleNode, rootColor)
    }

    override fun subtreeSize() = 1
}