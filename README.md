# black-and-white

A library of algorithms for the Black-and-White tree coloring.

## Introduction

Let ```T``` be a tree.

```(b, w)```-coloring of ```T``` is a partial coloring of ```T``` such that:
* ```b``` nodes colored black
* ```w``` nodes colored white
* all other nodes left uncolored
* no black node and white node are adjacent

The library provides a set of algoritms each of which:
* takes a ```N```-node tree ```T``` (**input**)
* provides a map ```MaxWhite``` which, for each ```0 ≤ b ≤ N```,</br>
gives the maximal ```w``` such that there exists a ```(b, w)```-coloring of ```T``` (**output**)
* for each ```0 ≤ b ≤ N``` and each ```0 ≤ w ≤ MaxWhite(b)```</br>
can color ```b``` nodes of the tree black and ```w``` nodes of the tree white</br>
such that no black node and white node are adjacent (**output**)

The main (self-explaining) interfaces of the library are:

```kotlin
interface Colorer {
    fun <D> resultFor(tree: Tree<D>): Result<D>
}
```

```kotlin
interface Result<D> {
    fun maxWhiteMap(): MaxWhiteMap

    fun coloringExistsFor(black: Int, white: Int): Boolean

    /**
     * @throws IllegalArgumentException if there does not exist such a coloring.
     */
    fun coloringFor(black: Int, white: Int): Coloring<D>
}
```

## Supported algorithms

* **BerendZucker** (http://jgaa.info/accepted/2009/BerendZucker2009.13.2.pdf)</br>
```result = resultFor(tree)``` complexity: ```O(n^2*lg(n)^3)```</br>
```result.coloringFor(b, w)``` complexity: ```O(n^2)```
* **X** (I am writing an article)</br>
```result = resultFor(tree)``` complexity: ```O(n^3 * maxChainLength)```,</br>
```maxChainLenth``` is a length of a maximal path in the tree such that each its node has only child</br>
```result.coloringFor(b, w)``` complexity: ```O(n)```

## How to use

```kotlin
val tree = Tree<String>(
    root() {
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
)

// A tree should be indexed.
tree.index()

val colorer = BerendZuckerColorer()
val result = colorer.resultFor(tree)

/* ***** MaxWhiteMap ***** */

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

val treeSize = expectedMaxWhiteMap.size - 1

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
```

## Dependencies

* https://github.com/devromik/tree (Maven)
* https://github.com/devromik/poly (Maven)

## License

MIT
