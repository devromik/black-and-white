#ifndef BW_STD_BW_TREE_COLORER_H
#define BW_STD_BW_TREE_COLORER_H

#include "BWTreeColorer.h"

namespace bw {

template<typename NodeType>
class StdBWTreeColorer : public BWTreeColorer<NodeType> {

public:

    typedef typename BWTreeColorer<NodeType>::TreeType TreeType;
    typedef typename TreeType::IsLeafPredicate IsLeafPredicate;
    typedef typename TreeType::template Iterator<IsLeafPredicate> TreeIterator;
    typedef typename BWTreeColorer<NodeType>::ColoringType ColoringType;
    typedef typename ColoringType::Color ColorType;

public:

    static const ColorType BLACK = ColoringType::BLACK;
    static const ColorType WHITE = ColoringType::WHITE;
    static const ColorType GRAY = ColoringType::GRAY;
    static const ColorType COLOR_COUNT = ColoringType::COLOR_COUNT;
    static const ColorType INVALID_COLOR = ColoringType::INVALID_COLOR;

    static const int INVALID_NODE_COUNT = -1;
    static const int INVALID_BLACK_COUNT = -1;
    static const int INVALID_WHITE_COUNT = -1;

public:

    StdBWTreeColorer() :
        tree(0),
        nodeCount(INVALID_NODE_COUNT),
        built(false),
        maxWhiteMap(0),
        coloring(0),
        blackCount(INVALID_BLACK_COUNT),
        whiteCount(INVALID_WHITE_COUNT) {}

    void setTree(TreeType* newTree) {
        clear();
        tree = newTree;
        nodeCount = tree != 0 ? newTree->getNodeCount() : INVALID_NODE_COUNT;
    }

    MaxWhiteMap* getMaxWhiteMap() {
        if (tree == 0 || tree->isEmpty()) {
            return 0;
        }

        if (!isBuilt()) {
            build();
        }

        return maxWhiteMap;
    }

    bool coloringExists(const int blackCount, const int whiteCount) {
        if (tree == 0 || tree->isEmpty()) {
            return false;
        }

        if (!isBuilt()) {
            build();
        }

        return maxWhiteMap->get(blackCount) >= whiteCount;
    }

    int getBlackCount() const {
        return blackCount;
    }

    int getWhiteCount() const {
        return whiteCount;
    }

    ColoringType* getColoring(const int blackCount, const int whiteCount) {
        if (tree == 0 || tree->isEmpty()) {
            return 0;
        }

        setColoringParameters(blackCount, whiteCount);

        if (coloring != 0) {
            delete coloring;
        }

        coloring = new ColoringType();

        if (!isBuilt()) {
            build();
        }

        buildColoring();
        return coloring;
    }

    virtual ~StdBWTreeColorer() {
        clear();
    }

private:

    void build() {
        if (maxWhiteMap != 0) {
            delete maxWhiteMap;
        }

        maxWhiteMap = new MaxWhiteMap(nodeCount);

        buildBody();
        setBuilt(true);
    }

    /**
     * Реализует стадию построения: формирует отображение Max_White
     * и выполняет подготовительную работу для метода getColoring(blackCount, whiteCount).
     */
    virtual void buildBody() = 0;

    virtual void buildColoring() = 0;

    void setBuilt(const bool newBuilt) {
        built = newBuilt;
    }

    bool isBuilt() const {
        return built;
    }

    void setColoringParameters(const int newBlackCount, const int newWhiteCount) {
        blackCount = newBlackCount;
        whiteCount = newWhiteCount;
    }

    void clear() {
        nodeCount = INVALID_NODE_COUNT;
        blackCount = INVALID_BLACK_COUNT;
        whiteCount = INVALID_WHITE_COUNT;
        tree = 0;
        setBuilt(false);

        if (maxWhiteMap != 0) {
            delete maxWhiteMap;
            maxWhiteMap = 0;
        }

        if (coloring != 0) {
            delete coloring;
            coloring = 0;
        }
    }

protected:

    // Дерево.
    TreeType* tree;

    // Количество узлов в дереве.
    int nodeCount;

    // Показывает, выполнена ли стадия построения.
    bool built;

    // Сопоставляет каждому целому blackCount неотрицательное целое Max_White(blackCount),
    // равное максимальному числу белых узлов по всем чёрно-белым раскраскам данного дерева таким,
    // что blackCount узлов этого дерева окрашено в чёрный цвет.
    MaxWhiteMap* maxWhiteMap;

    // Количество чёрных узлов в раскраске.
    int blackCount;

    // Количество белых узлов в раскраске.
    int whiteCount;

    // Чёрно-белая раскраска дерева, в которой число чёрных узлов равно blackCount, а число белых - whiteCount.
    ColoringType* coloring;
};

} // namespace bw.

#endif
