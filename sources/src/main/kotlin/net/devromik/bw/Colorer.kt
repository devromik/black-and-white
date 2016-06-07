package net.devromik.bw

import net.devromik.tree.Tree

/**
 * @author Shulnyaev Roman
 */
interface Colorer {
    fun <D> resultFor(tree: Tree<D>): Result<D>
}