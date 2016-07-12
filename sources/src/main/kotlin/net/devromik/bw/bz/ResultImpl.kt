package net.devromik.bw.bz

import net.devromik.bw.*
import net.devromik.bw.Color.*
import net.devromik.tree.*
import java.lang.Math.max
import java.util.*
import java.util.concurrent.Callable
import java.util.concurrent.ForkJoinPool
import java.util.concurrent.ForkJoinPool.commonPool

/**
 * @author Shulnyaev Roman
 */
class ResultImpl<D>(val tree: Tree<D>, private val forkJoinPool: ForkJoinPool = commonPool()) : Result<D> {
    private val root = tree.root
    private val subtreeSizes = tree.countSubtreeSizes()
    private val treeSize = subtreeSizes[root]!!

    internal val treeResult: TreeInfo<D, TreeNodeResult<D>>
    private fun resultFor(node: TreeNode<D>) = treeResult[node]!!

    // ****************************** //

    init {
        treeResult = initTreeResult(tree, subtreeSizes)
        buildResultFor(root)
    }

    private fun buildResultFor(subtreeRoot: TreeNode<D>) {
        val result = resultFor(subtreeRoot)

        if (result.subtreeSize() == 1) {
            return
        }

        if (result.subtreeSize() == 2) {
            val childResult = resultFor(subtreeRoot.childAt(0))
            val subtreeMaxWhiteMap = parentResultForSingleChildResult(childResult)
            (result as InnerTreeNodeResult<D>).subtreeMaxWhiteMap = subtreeMaxWhiteMap

            val fusion = SingleChildFusion(subtreeMaxWhiteMap)
            result.pushFusion(fusion)

            result.reduce(treeResult)

            return
        }

        val separator = separatorFor(subtreeRoot, treeResult)

        if (separator == subtreeRoot) {
            val childTasks = ArrayList<Callable<TreeNodeResult<D>>>(separator.childCount)

            for (child in separator) {
                val childTask = Callable {
                    buildResultFor(child)
                    resultFor(child)
                }

                childTasks.add(childTask)
            }

            // We make the results for the children visible for the current thread (happens-before).
            for (childResult in forkJoinPool.invokeAll(childTasks)) {
                childResult.get()
            }

            val merging = Array<Pair<MinGrayToBlacksMap, Fusion>>(separator.childCount, {
                val childResult = resultFor(separator.childAt(it))
                val childMaxWhiteMap = parentResultForSingleChildResult(childResult)
                MinGrayToBlacksMap(childMaxWhiteMap) to SingleChildFusion(childMaxWhiteMap)
            })

            val minGrayToBlacksMap = merge(
                merging,
                leftmostChildPos = 0,
                rightmostChildPos = separator.childCount - 1,
                separator = separator).first

            (result as InnerTreeNodeResult<D>).subtreeMaxWhiteMap = minGrayToBlacksMap.toFixedRootColorMaxWhiteMap()

            if (resultFor(separator).subtreeSize() != 1) {
                resultFor(separator).reduce(treeResult)
            }
        }
        else {
            buildResultFor(separator)

            if (resultFor(separator).subtreeSize() != 1) {
                resultFor(separator).reduce(treeResult)
            }

            buildResultFor(subtreeRoot)
        }
    }

    private fun merge(
        merging: Array<Pair<MinGrayToBlacksMap, Fusion>>,
        leftmostChildPos: Int,
        rightmostChildPos: Int,
        separator: TreeNode<D>) : Pair<MinGrayToBlacksMap, Fusion> {

        if (leftmostChildPos == rightmostChildPos) {
            return merging[leftmostChildPos]
        }
        else if (rightmostChildPos - leftmostChildPos == 1) {
            val merged = merge(merging[leftmostChildPos], merging[rightmostChildPos])
            resultFor(separator).pushFusion(merged.second)

            return merged
        }
        else {
            val middleChildPos = (leftmostChildPos + rightmostChildPos) / 2

            val leftMerging = merge(merging, leftmostChildPos, middleChildPos, separator)
            val rightMerging = merge(merging, middleChildPos + 1, rightmostChildPos, separator)

            val merged = merge(leftMerging, rightMerging)
            resultFor(separator).pushFusion(merged.second)

            return merged
        }
    }

    private fun merge(
        left: Pair<MinGrayToBlacksMap, Fusion>,
        right: Pair<MinGrayToBlacksMap, Fusion>) : Pair<MinGrayToBlacksMap, Fusion> {

        val unitedMinGrayToBlacksMap = left.first.unitedWith(right.first)
        return unitedMinGrayToBlacksMap to MultiChildFusion(unitedMinGrayToBlacksMap, left.second, right.second)
    }

    // ****************************** //

    override fun maxWhiteMap() = resultFor(root).subtreeMaxWhiteMap()

    override fun coloringExistsFor(black: Int, white: Int) = black in (0..treeSize) && maxWhiteMap()[black] >= white

    override fun coloringFor(black: Int, white: Int): Coloring<D> {
        require(coloringExistsFor(black, white))

        val maxWhite = resultFor(root).subtreeMaxWhiteMap()[black]
        val minGray = treeSize - (black + maxWhite)

        val coloring = Coloring(tree)

        color(
            root,
            leftmostChildPos = 0,
            rightmostChildPos = root.childCount - 1,
            inFusion = null,
            black = black,
            minGray = minGray,
            coloring = coloring)

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

    private fun color(
        subtreeRoot: TreeNode<D>,
        leftmostChildPos: Int,
        rightmostChildPos: Int,
        inFusion: Fusion?,
        black: Int,
        minGray: Int,
        coloring: Coloring<D>) {

        if (subtreeRoot.isLeaf) {
            colorLeaf(subtreeRoot, black, minGray, coloring)
        }
        else if (subtreeRoot.hasOnlyChild || rightmostChildPos - leftmostChildPos == 0) {
            colorSingleChild(subtreeRoot, leftmostChildPos, black, minGray, coloring)
        }
        else {
            val result = resultFor(subtreeRoot)
            val fusion = inFusion ?: result.topFusion()

            val color: Color
            val parentColor = if (!subtreeRoot.isRoot) coloring.colorOf(subtreeRoot.parent!!) else null
            var newBlack = black
            var newMinGray = minGray

            // try BLACK
            if (
                parentColor != WHITE &&
                fusion.minGrayFor(BLACK, black) != INVALID_MIN_GRAY &&
                fusion.minGrayFor(BLACK, black) <= minGray &&
                (!coloring.isColored(subtreeRoot) || coloring.colorOf(subtreeRoot)!! == BLACK)) {

                if (!coloring.isColored(subtreeRoot)) {
                    coloring.color(subtreeRoot, BLACK)
                }

                color = BLACK
                ++newBlack
            }
            // try WHITE
            else if (
                parentColor != BLACK &&
                fusion.minGrayFor(WHITE, black) != INVALID_MIN_GRAY &&
                fusion.minGrayFor(WHITE, black) <= minGray &&
                (!coloring.isColored(subtreeRoot) || coloring.colorOf(subtreeRoot)!! == WHITE)) {

                if (!coloring.isColored(subtreeRoot)) {
                    coloring.color(subtreeRoot, WHITE)
                }

                color = WHITE
            }
            // GRAY, sure! :)
            else {
                if (!coloring.isColored(subtreeRoot)) {
                    coloring.color(subtreeRoot, GRAY)
                }

                color = GRAY
                ++newMinGray
            }

            val leftFusion = fusion.leftFusion!!
            val rightFusion = fusion.rightFusion!!

            for (b in 0..newBlack) {
                var leftMinGray = leftFusion.minGrayFor(color, b)
                var rightMinGray = rightFusion.minGrayFor(color, newBlack - b)

                if (
                    leftMinGray != INVALID_MIN_GRAY &&
                    rightMinGray != INVALID_MIN_GRAY &&
                    leftMinGray + rightMinGray <= newMinGray) {

                    val middleChildPos = (leftmostChildPos + rightmostChildPos) / 2

                    color(
                        subtreeRoot,
                        leftmostChildPos,
                        middleChildPos,
                        leftFusion,
                        b,
                        leftMinGray,
                        coloring)

                    color(
                        subtreeRoot,
                        middleChildPos + 1,
                        rightmostChildPos,
                        rightFusion,
                        newBlack - b,
                        rightMinGray,
                        coloring)

                    break
                }

                // Does this branch improve the asymptotic of the coloring operation?
                // T(n) = T(a) + T(n − a + 1) + min(a, n - a + 1) instead of
                // T(n) = T(a) + T(n − a + 1) + n from the paper.
                leftMinGray = leftFusion.minGrayFor(color, newBlack - b)
                rightMinGray = rightFusion.minGrayFor(color, b)

                if (
                    leftMinGray != INVALID_MIN_GRAY &&
                    rightMinGray != INVALID_MIN_GRAY &&
                    leftMinGray + rightMinGray <= newMinGray) {

                    val middleChildPos = (leftmostChildPos + rightmostChildPos) / 2

                    color(
                        subtreeRoot,
                        leftmostChildPos,
                        middleChildPos,
                        leftFusion,
                        newBlack - b,
                        leftMinGray,
                        coloring)

                    color(
                        subtreeRoot,
                        middleChildPos + 1,
                        rightmostChildPos,
                        rightFusion,
                        b,
                        rightMinGray,
                        coloring)

                    break
                }
            }
        }
    }

    private fun colorLeaf(subtreeRoot: TreeNode<D>, black: Int, minGray: Int, coloring: Coloring<D>) {
        if (black == 1) {
            coloring.color(subtreeRoot, BLACK)
        }
        else if (minGray == 1) {
            coloring.color(subtreeRoot, GRAY)
        }
        else {
            coloring.color(subtreeRoot, WHITE)
        }
    }

    private fun colorSingleChild(
        subtreeRoot: TreeNode<D>,
        childPos: Int,
        black: Int,
        minGray: Int,
        coloring: Coloring<D>) {

        val child = subtreeRoot.childAt(childPos)

        if (coloring.isColored(subtreeRoot)) {
            val color = coloring.colorOf(subtreeRoot)
            var newBlack = black
            var newMinGray = minGray

            if (color == BLACK) {
                --newBlack
            }
            else if (color == GRAY) {
                --newMinGray
            }

            color(
                child,
                leftmostChildPos = 0,
                rightmostChildPos = child.childCount - 1,
                inFusion = null,
                black = newBlack,
                minGray = newMinGray,
                coloring = coloring)
        }
        else {
            val result = resultFor(subtreeRoot)
            val fusion = result.topFusion()

            var newBlack = black
            var newMinGray = minGray
            val parentColor = if (!subtreeRoot.isRoot) coloring.colorOf(subtreeRoot.parent!!) else null

            // try BLACK
            if (parentColor != WHITE && fusion.minGrayFor(BLACK, black) == minGray) {
                coloring.color(subtreeRoot, BLACK)
                --newBlack
            }
            // try WHITE
            else if (parentColor != BLACK && fusion.minGrayFor(WHITE, black) == minGray) {
                coloring.color(subtreeRoot, WHITE)
            }
            // GRAY, sure! :)
            else {
                coloring.color(subtreeRoot, GRAY)
                --newMinGray
            }

            color(
                child,
                leftmostChildPos = 0,
                rightmostChildPos = child.childCount - 1,
                inFusion = null,
                black = newBlack,
                minGray = newMinGray,
                coloring = coloring)
        }
    }
}

internal fun <D> initTreeResult(tree: Tree<D>, subtreeSizes: TreeInfo<D, Int>): TreeInfo<D, TreeNodeResult<D>> {
    val result = IndexedTreeInfo<D, TreeNodeResult<D>>(tree)

    tree.traverseInLevelOrder {
        result[it] = if (it.isLeaf) LeafTreeNodeResult(it) else InnerTreeNodeResult(it, subtreeSizes[it]!!)
    }

    return result
}

internal fun <D> separatorFor(subtreeRoot: TreeNode<D>, treeResult: TreeInfo<D, TreeNodeResult<D>>): TreeNode<D> {
    val subtreeSize = treeResult[subtreeRoot]!!.subtreeSize()
    val subtreeSizeHalf = subtreeSize / 2
    val treeIter = LevelOrderIterator(subtreeRoot) { treeResult[it]!!.subtreeSize() == 1 }

    treeTraverse@
    while (treeIter.hasNext) {
        val node = treeIter.next()
        val nodeResult = treeResult[node]!!

        if ((subtreeSize - nodeResult.subtreeSize()) > subtreeSizeHalf) {
            continue
        }

        for (child in node) {
            val childResult = treeResult[child]!!

            if (childResult.subtreeSize() > subtreeSizeHalf) {
                continue@treeTraverse
            }
        }

        return node
    }

    throw AssertionError()
}

internal fun <D> parentResultForSingleChildResult(childResult: TreeNodeResult<D>): FixedRootColorMaxWhiteMap {
    val childMaxWhiteMap = childResult.subtreeMaxWhiteMap()
    val subtreeSize = childResult.initSubtreeSize() + 1
    val maxWhiteMap = FixedRootColorMaxWhiteMap(subtreeSize)

    for (black in 0..subtreeSize) {
        // BLACK
        maxWhiteMap[BLACK, black] = max(
            childMaxWhiteMap[BLACK, black - 1],
            childMaxWhiteMap[GRAY, black - 1])

        // WHITE
        maxWhiteMap[WHITE, black] = max(
            childMaxWhiteMap[WHITE, black],
            childMaxWhiteMap[GRAY, black])

        if (maxWhiteMap[WHITE, black] != INVALID_MAX_WHITE) {
            ++maxWhiteMap[WHITE, black]
        }

        // GRAY
        val blackWhiteMax = max(childMaxWhiteMap[BLACK, black], childMaxWhiteMap[WHITE, black])
        maxWhiteMap[GRAY, black] = max(blackWhiteMax, childMaxWhiteMap[GRAY, black])
    }

    return maxWhiteMap
}