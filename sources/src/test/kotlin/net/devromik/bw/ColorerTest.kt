package net.devromik.bw

import net.devromik.tree.Tree
import net.devromik.tree.TreeNode
import net.devromik.tree.root
import org.junit.Test
import kotlin.test.assertEquals
import kotlin.test.assertFalse
import kotlin.test.assertTrue

/**
 * @author Shulnyaev Roman
 */
abstract class ColorerTest(val colorer: Colorer) {

    @Test(expected = IllegalArgumentException::class)
    fun cannotProvideImpossibleColoring() {
        val tree = Tree(
            root("root") {
                child()
                child()
            }
        )

        tree.index()
        val result = colorer.resultFor(tree)

        assertFalse(result.coloringExistsFor(black = 3, white = 3))
        result.coloringFor(black = 3, white = 3)
    }

    // ****************************** //

    @Test fun test_1() {
        val root =
            root("root") {
                child()
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 2,
            1 to 0,
            2 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_2() {
        val root =
            root("root") {
                child()
                child()
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 3,
            1 to 1,
            2 to 0,
            3 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_3() {
        val root =
            root("root") {
                child() {
                    child()
                    child()
                }
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 4,
            1 to 2,
            2 to 1,
            3 to 0,
            4 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_4() {
        val root =
            root("root") {
                child()
                child()
                child()
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 4,
            1 to 2,
            2 to 1,
            3 to 0,
            4 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_5() {
        val root = root("root") {
            child {
                child()
                child()
                child()
            }
        }

        val expectedMaxWhiteMap = mapOf(
            0 to 5,
            1 to 3,
            2 to 2,
            3 to 1,
            4 to 0,
            5 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_7() {
        val root =
            root("root") {
                child {
                    child {
                        child {
                            child()
                        }
                    }
                }
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 5,
            1 to 3,
            2 to 2,
            3 to 1,
            4 to 0,
            5 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_8() {
        val root =
            root("root") {
                child {
                    child {
                        child {
                            child()
                            child()
                        }
                        child()
                    }
                    child()
                }
                child {
                    child {
                        child {
                            child()
                        }
                    }
                    child {
                        child {
                            child()
                        }
                    }
                }
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 15,
            1 to 13,
            2 to 12,
            3 to 11,
            4 to 10,
            5 to 9,
            6 to 8,
            7 to 7,
            8 to 6,
            9 to 5,
            10 to 4,
            11 to 3,
            12 to 2,
            13 to 1,
            14 to 0,
            15 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_9() {
        val root =
            root("root") {
                child()
                child {
                    child {
                        child {
                            child()
                        }
                    }
                    child {
                        child {
                            child()
                        }
                    }
                }
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 9,
            1 to 7,
            2 to 6,
            3 to 5,
            4 to 3,
            5 to 3,
            6 to 2,
            7 to 1,
            8 to 0,
            9 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_10() {
        val root =
            root("root") {
                child {
                    child {
                        child()
                    }
                }
                child {
                    child {
                        child()
                    }
                }
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 7,
            1 to 5,
            2 to 4,
            3 to 3,
            4 to 2,
            5 to 1,
            6 to 0,
            7 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_11() {
        val root =
            root("root") {
                child {
                    child {
                        child()
                    }
                }
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 4,
            1 to 2,
            2 to 1,
            3 to 0,
            4 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_12() {
        val root =
            root("root") {
                child()
                child {
                    child {
                        child()
                    }
                }
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 5,
            1 to 3 ,
            2 to 2,
            3 to 1,
            4 to 0,
            5 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_14() {
        val root =
            root("root") {
                child() {
                    child()
                }
                child {
                    child {
                        child()
                    }
                }
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 6,
            1 to 4 ,
            2 to 3,
            3 to 2,
            4 to 1,
            5 to 0,
            6 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_15() {
        val root =
            root("root") {
                child {
                    child {
                        child {
                            child()
                            child()
                        }
                        child()
                    }
                    child()
                }
                child {
                    child {
                        child {
                            child()
                        }
                    }
                    child {
                        child {
                            child()
                        }
                    }
                }
                child()
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 16,
            1 to 14,
            2 to 13,
            3 to 12,
            4 to 11,
            5 to 10,
            6 to 9,
            7 to 8,
            8 to 7,
            9 to 6,
            10 to 5,
            11 to 4,
            12 to 3,
            13 to 2,
            14 to 1,
            15 to 0,
            16 to 0)

        test(root, expectedMaxWhiteMap)
    }

    @Test fun test_16() {
        val root =
            root("root") {
                child {
                    child {
                        child {
                            child()
                            child()
                        }
                        child()
                    }
                    child()
                }
                child {
                    child {
                        child {
                            child()
                        }
                    }
                    child {
                        child {
                            child()
                        }
                    }
                }
                child {
                    child {
                        child {
                            child {
                                child()
                                child()
                            }
                            child()
                        }
                        child()
                    }
                    child {
                        child {
                            child {
                                child()
                            }
                        }
                        child {
                            child {
                                child()
                            }
                        }
                    }
                }
            }

        val expectedMaxWhiteMap = mapOf(
            0 to 30,
            1 to 28,
            2 to 27,
            3 to 26,
            4 to 25,
            5 to 24,
            6 to 23,
            7 to 22,
            8 to 20,
            9 to 19,
            10 to 18,
            11 to 17,
            12 to 16,
            13 to 15,
            14 to 15,
            15 to 14,
            16 to 12,
            17 to 11,
            18 to 10,
            19 to 9,
            20 to 8,
            21 to 7,
            22 to 7,
            23 to 6,
            24 to 5,
            25 to 4,
            26 to 3,
            27 to 2,
            28 to 1,
            29 to 0,
            30 to 0)

        test(root, expectedMaxWhiteMap)
    }

    // ****************************** //

    fun test(root: TreeNode<String>, expectedMaxWhiteMap: Map<Int, Int>) {
        val tree = Tree(root).index()
        val treeSize = expectedMaxWhiteMap.size - 1
        val result = colorer.resultFor(tree)

        /* ***** MaxWhiteMap ***** */

        val maxWhiteMap = result.maxWhiteMap()
        assertEquals(treeSize, maxWhiteMap.treeSize)

        expectedMaxWhiteMap.forEach {
            assertEquals(it.value, maxWhiteMap[it.key])
        }

        /* ***** Coloring ***** */

        for (black in 0..treeSize) {
            val maxWhite = maxWhiteMap[black]

            for (white in 0..maxWhite) {
                assertTrue(result.coloringExistsFor(black, white))

                val coloring = result.coloringFor(black, white)
                checkColoring(tree, treeSize, coloring, black, white)
            }

            assertFalse(result.coloringExistsFor(black, maxWhite + 1))
        }
    }
}