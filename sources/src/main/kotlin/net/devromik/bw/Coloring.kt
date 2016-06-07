package net.devromik.bw

import net.devromik.bw.Color.*
import net.devromik.tree.IndexedTreeInfo
import net.devromik.tree.Tree
import net.devromik.tree.TreeNode

/**
 * @author Shulnyaev Roman
 */
class Coloring<D>(tree: Tree<D>) {

    private val coloring = IndexedTreeInfo<D, Color>(tree)
    private val count = IntArray(COLOR_COUNT, { 0 })

    // ****************************** //

    fun color(node: TreeNode<D>, color: Color) {
        coloring[node] = color
        ++count[color.index]
    }

    fun isColored(node: TreeNode<D>) = coloring.has(node)
    fun colorOf(node: TreeNode<D>) = coloring[node]

    fun black() = count[BLACK.index]
    fun white() = count[WHITE.index]
    fun gray() = count[GRAY.index]
}