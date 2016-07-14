package net.devromik.bw.x

import net.devromik.bw.Colorer
import net.devromik.tree.Tree
import java.util.concurrent.ForkJoinPool
import java.util.concurrent.ForkJoinPool.commonPool

/**
 * @author Shulnyaev Roman
 */
class XColorer(private val forkJoinPool: ForkJoinPool = commonPool()) : Colorer {

    /**
     * The tree should be indexed.
     */
    override fun <D> resultFor(tree: Tree<D>) = ResultImpl(tree, forkJoinPool)
}