#ifndef BW_BW_TREE_COLORING_H
#define BW_BW_TREE_COLORING_H

#include "TreeData.h"

namespace bw {

/**
 * Чёрно-белая раскраска дерева.
 */
template<typename NodeType>
class BWTreeColoring {

public:

    /**
     * Цвет узла в чёрно-белой раскраске дерева.
     */
    enum Color {
        BLACK,
        WHITE,
        GRAY,
        COLOR_COUNT,
        INVALID_COLOR
    };

public:

    BWTreeColoring() {
        init();
    }

    void color(NodeType* node, const Color newColor) {
        if (coloring.has(node)) {
            Color oldColor = coloring.get(node);
            count[oldColor] = count[oldColor] - 1;
        }

        coloring.set(node, newColor);
        count[newColor] = count[newColor] + 1;
    }

    bool isColored(NodeType* node) const {
        return coloring.has(node);
    }

    Color getColor(NodeType* node) const {
        return coloring.get(node);
    }

    int getBlackCount() const {
        return count[BLACK];
    }

    int getWhiteCount() const {
        return count[WHITE];
    }

    int getGrayCount() const {
         return count[GRAY];
    }

    void clear() {
        coloring.clear();
    }

private:

    void init() {
        initCount();
    }

    void initCount() {
        count[BLACK] = 0;
        count[WHITE] = 0;
        count[GRAY] = 0;
    }

private:

    // Раскраска.
    TreeData<NodeType, Color> coloring;

    // Отображение цвета на число узлов, окрашенных в этот цвет.
    int count[COLOR_COUNT];
};

} // namespace bw.

#endif

