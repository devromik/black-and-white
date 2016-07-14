package net.devromik.bw.x

import net.devromik.bw.Color.*
import net.devromik.bw.Coloring
import net.devromik.bw.MaxWhiteMap
import net.devromik.bw.Result
import net.devromik.tree.Tree
import net.devromik.tree.TreeNode
import java.util.concurrent.Callable
import java.util.concurrent.ForkJoinPool
import java.util.concurrent.RecursiveTask

/**
 * @author Shulnyaev Roman
 */
class ResultImpl<D>(
    val tree: Tree<D>,
    private val forkJoinPool: ForkJoinPool) : Result<D> {

    val root = tree.root
    val subtreeSizes = tree.countSubtreeSizes()
    val treeSize = subtreeSizes[root]!!
    val result: XResult<D>
    val maxWhiteMap: MaxWhiteMap

    // ****************************** //

    init {
        if (treeSize == 1) {
            result = SingleNodeXResult(root)
        }
        else {
            val task = recursiveTaskFor { resultFor(root, 0, root.childCount - 1, top = SingleNodeXResult(root)) }
            result = forkJoinPool.invoke(task)
        }

        maxWhiteMap = result.maxWhiteMap()
    }

    private fun resultFor(
        subtreeRoot: TreeNode<D>,
        leftmostChildPos: Int,
        rightmostChildPos: Int,
        top: XResult<D>): XResult<D> {

        // The subtree consists exactly of two nodes.
        if (leftmostChildPos == rightmostChildPos && subtreeRoot.childAt(leftmostChildPos).isLeaf) {
            return PairXResult(subtreeRoot, top, leftmostChildPos)
        }
        // The subtree consists of more than two nodes and its root has only child.
        else if (leftmostChildPos == rightmostChildPos) {
            return singleChildResultFor(subtreeRoot, top, leftmostChildPos)
        }
        // The subtree consists of more than two nodes and its root has more than one child.
        else {
            val leftmostChild = subtreeRoot.childAt(leftmostChildPos)
            val rightmostChild = subtreeRoot.childAt(rightmostChildPos)

            if (subtreeSizes[leftmostChild]!! > subtreeSizes[rightmostChild]!!) {
                val task = recursiveTaskFor { resultFor(subtreeRoot, leftmostChildPos, rightmostChildPos - 1, top) }
                val leftResult = forkJoinPool.invoke(task)

                if (rightmostChild.isLeaf) {
                    return PairXResult(subtreeRoot, leftResult, rightmostChildPos)
                }
                else {
                    return singleChildResultFor(subtreeRoot, leftResult, rightmostChildPos)
                }
            }
            else {
                val task = recursiveTaskFor { resultFor(subtreeRoot, leftmostChildPos + 1, rightmostChildPos, top) }
                val rightResult = forkJoinPool.invoke(task)

                if (leftmostChild.isLeaf) {
                    return PairXResult(subtreeRoot, rightResult, leftmostChildPos)
                }
                else {
                    return singleChildResultFor(subtreeRoot, rightResult, leftmostChildPos)
                }
            }
        }
    }

    private fun singleChildResultFor(
        subtreeRoot: TreeNode<D>,
        top: XResult<D>,
        firstChainNodePos: Int): SingleChildXResult<D> {

        val tasks = listOf<Callable<XResult<D>>>(
            Callable {
                val b = FixedRootColorSingleChildXResult(subtreeRoot, BLACK, top, firstChainNodePos)
                if (b.lastChainNode.isLeaf) b else resultFor(b.lastChainNode, 0, b.lastChainNode.childCount - 1, b)
            },
            Callable {
                val w = FixedRootColorSingleChildXResult(subtreeRoot, WHITE, top, firstChainNodePos)
                if (w.lastChainNode.isLeaf) w else resultFor(w.lastChainNode, 0, w.lastChainNode.childCount - 1, w)
            },
            Callable {
                val g = FixedRootColorSingleChildXResult(subtreeRoot, GRAY, top, firstChainNodePos)
                if (g.lastChainNode.isLeaf) g else resultFor(g.lastChainNode, 0, g.lastChainNode.childCount - 1, g)
            }
        )

        val fixedRootColorResults = forkJoinPool.invokeAll(tasks)

        return SingleChildXResult(
            fixedRootColorResults[BLACK.index].get(),
            fixedRootColorResults[WHITE.index].get(),
            fixedRootColorResults[GRAY.index].get())
    }

    private fun recursiveTaskFor(resultFunc: () -> XResult<D>): RecursiveTask<XResult<D>> {
        return object : RecursiveTask<XResult<D>>() {
            override fun compute(): XResult<D> = resultFunc()
        }
    }

    // ****************************** //

    override fun maxWhiteMap() = maxWhiteMap

    override fun coloringExistsFor(black: Int, white: Int) = black in (0..treeSize) && maxWhiteMap[black] >= white

    override fun coloringFor(black: Int, white: Int): Coloring<D> {
        require(coloringExistsFor(black, white))

        val coloring = Coloring(tree)
        val maxWhite = maxWhiteMap[black]

        if (result.maxWhiteMap()[BLACK, black] == maxWhite) {
            result.color(coloring, BLACK, black, maxWhite)
        }
        else if (result.maxWhiteMap()[WHITE, black] == maxWhite) {
            result.color(coloring, WHITE, black, maxWhite)
        }
        else {
            result.color(coloring, GRAY, black, maxWhite)
        }

        if (coloring.whiteCount() > white) {
            val treeIter = tree.levelOrderIterator()

            while (treeIter.hasNext) {
                val node = treeIter.next()

                if (coloring.colorOf(node) == WHITE) {
                    coloring.color(node, GRAY)

                    if (coloring.whiteCount() == white) {
                        break
                    }
                }
            }
        }

        return coloring
    }
}