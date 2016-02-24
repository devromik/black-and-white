#ifndef BW_CONJECTURE_BW_TREE_COLORER_H
#define BW_CONJECTURE_BW_TREE_COLORER_H

#include <utility>
#include <math.h>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <QList>
#include "Tree.h"
#include "StdBWTreeColorer.h"
#include "Summator.h"

namespace bw {

/**
 * Алгоритм-гипотеза (fail, but works :)) чёрно-белой раскраски дерева.
 */
template<typename NodeType>
class ConjectureBWTreeColorer : public StdBWTreeColorer<NodeType> {

public:

    using StdBWTreeColorer<NodeType>::tree;
    using StdBWTreeColorer<NodeType>::nodeCount;
    using StdBWTreeColorer<NodeType>::coloring;
    using StdBWTreeColorer<NodeType>::maxWhiteMap;
    using StdBWTreeColorer<NodeType>::blackCount;
    using StdBWTreeColorer<NodeType>::whiteCount;

public:

    typedef typename StdBWTreeColorer<NodeType>::TreeType TreeType;
    typedef typename StdBWTreeColorer<NodeType>::IsLeafPredicate IsLeafPredicate;
    typedef typename StdBWTreeColorer<NodeType>::TreeIterator TreeIterator;
    typedef typename StdBWTreeColorer<NodeType>::ColoringType ColoringType;
    typedef typename StdBWTreeColorer<NodeType>::ColorType ColorType;

public:

    static const ColorType BLACK;
    static const ColorType WHITE;
    static const ColorType GRAY;
    static const ColorType COLOR_COUNT;
    static const ColorType INVALID_COLOR;

    static const int INVALID_MAX_WHITE;
    static const int INVALID_MIN_GRAY;

    static const int INVALID_ROOT_BRANCH_COUNT;

private:

    /**
     * Ветвь.
     */
    class Branch;

    /**
     * Раскраска ствола ветви.
     */
    class BranchTrunkColoring;

    /**
     * Информация о раскраске ветви.
     */
    class BranchColoringInfo;

    /**
     * Информация о раскраске составной подветви.
     * Cоставной подветвью называется объединение одной или нескольких простых подветвей.
     * При этом по крайней мере некоторые из образующих простых подветвей должны иметь различные начальные узлы стволов.
     */
    class CompositeSubbranchColoringInfo;
    class PrimeSubbranchColoringInfo;
    class SingleSubbranchColoringInfo;
    class TreeSubbranchColoringInfo;
    class PrefixSubbranchColoringInfo;
    class SuffixSubbranchColoringInfo;
    class PrefixSuffixSubbranchColoringInfo;

public:

    ConjectureBWTreeColorer() :
        rootBranchCount(INVALID_ROOT_BRANCH_COUNT),
        rootBranchesColoringInfo(0),
        fusionStack(0),
        topFusion(0),
        topMaxWhiteMap(0) {}

    ~ConjectureBWTreeColorer() {
        // Информация о раскраске корневых ветвей.
        if (rootBranchCount > 0) {
            for (int i = 0; i < rootBranchCount; ++i) {
                delete rootBranchesColoringInfo[i]->getBranch();
                delete rootBranchesColoringInfo[i];
            }

            delete[] rootBranchesColoringInfo;
        }

        // Стек объединений отображений Max_White.
        if (fusionStack != 0) {
            delete fusionStack;
        }

        // Объединение верхнего уровня.
        if (topFusion != 0) {
            delete topFusion;
        }
    }

protected:

    template<typename IsLeafPredicateType>
    TreeIterator treeIterator() const {
        return tree->template iterator<IsLeafPredicateType>();
    }

private:

    /**
     * Список, сопоставляемый некоторому числу серых узлов grayCount,
     * и содержащий всевозможные значения blackCount такие,
     * что существует оптимальная чёрно-белая раскраска дерева c grayCount серыми узлами и blackCount чёрными.
     */
    typedef std::vector<int> BlackList;

    /**
     * Подветвь и её позиция в родительской ветви.
     */
    typedef std::pair<Branch*, int> SubbranchInfo;

private:

    static const int NO_GRAY = -1;
    static const int LESS_THAN_MIN_MAX_WHITE = -1;
    static const int INVALID_NODE_COUNT = -1;

private:

    /**
     * Представление отображения Max_White в виде (color, grayCount) -> BlackList(color, grayCount).
     */
    class Gray2BlackListMap {

    public:

        Gray2BlackListMap() : maxGrayCount(-1), maxBlackCount(-1) {}

        Gray2BlackListMap(const int newMaxGrayCount) : maxGrayCount(newMaxGrayCount), maxBlackCount(-1) {
            initMap();
        }

        void set(const ColorType color, const int grayCount, BlackList* blackList) {
            if (get(color, grayCount) != 0) {
                delete get(color, grayCount);
            }

            (*(map[color]))[grayCount] = blackList;
        }

        BlackList* get(const ColorType color, const int grayCount) const {
            return grayCount <= maxGrayCount ? (*(map[color]))[grayCount] : 0;
        }

        void put(const ColorType color, const int grayCount, const int blackCount) {
            if (get(color, grayCount) == 0) {
                set(color, grayCount, new BlackList());
            }

            get(color, grayCount)->push_back(blackCount);

            if (blackCount > maxBlackCount) {
                maxBlackCount = blackCount;
            }
        }

        int getMaxGrayCount() const {
            return maxGrayCount;
        }

        int getMaxBlackCount() const {
            return maxBlackCount;
        }

        ~Gray2BlackListMap() {
            destroy();
        }

    private:

        void initMap() {
            map[BLACK] = new std::vector<BlackList*>(maxGrayCount + 1, (BlackList*)0);
            map[WHITE] = new std::vector<BlackList*>(maxGrayCount + 1, (BlackList*)0);
            map[GRAY] = new std::vector<BlackList*>(maxGrayCount + 1, (BlackList*)0);
        }

        void destroy() {
            // Чёрный цвет.
            std::vector<BlackList*>::iterator blackListIter = map[BLACK]->begin();

            while (blackListIter != map[BLACK]->end()) {
                delete *blackListIter;
                ++blackListIter;
            }

            delete map[BLACK];

            // Белый цвет.
            blackListIter = map[WHITE]->begin();

            while (blackListIter != map[WHITE]->end()) {
                delete *blackListIter;
                ++blackListIter;
            }

            delete map[WHITE];

            // Серый цвет.
            blackListIter = map[GRAY]->begin();

            while (blackListIter != map[GRAY]->end()) {
                delete *blackListIter;
                ++blackListIter;
            }

            delete map[GRAY];
        }

    private:

        // Представление отображения Max_White в виде (color, grayCount) -> BlackList(color, grayCount)
        std::vector<BlackList*>* map[COLOR_COUNT];

        // Максимальное количество серых узлов по всем чёрно-белым раскраскам.
        int maxGrayCount;

        // Максимальное количество чёрных узлов по всем чёрно-белым раскраскам.
        int maxBlackCount;
    };

    /**
     * Представление отображения Max_White в виде grayCount -> BlackList(grayCount).
     */
    class ColorlessGray2BlackListMap {

    public:

        ColorlessGray2BlackListMap() : maxGrayCount(-1), maxBlackCount(-1) {}

        ColorlessGray2BlackListMap(const int newMaxGrayCount) : maxGrayCount(newMaxGrayCount), maxBlackCount(-1) {
            initMap();
        }

        void set(const int grayCount, BlackList* blackList) {
            if (get(grayCount) != 0) {
                delete get(grayCount);
            }

            (*map)[grayCount] = blackList;
        }

        BlackList* get(const int grayCount) const {
            return grayCount <= maxGrayCount ? (*map)[grayCount] : 0;
        }

        void put(const int grayCount, const int blackCount) {
            if (get(grayCount) == 0) {
                set(grayCount, new BlackList());
            }

            get(grayCount)->push_back(blackCount);

            if (blackCount > maxBlackCount) {
                maxBlackCount = blackCount;
            }
        }

        int getMaxGrayCount() const {
            return maxGrayCount;
        }

        int getMaxBlackCount() const {
            return maxBlackCount;
        }

        ~ColorlessGray2BlackListMap() {
            destroy();
        }

    private:

        void initMap() {
            map = new std::vector<BlackList*>(maxGrayCount + 1, (BlackList*)0);
        }

        void destroy() {
            std::vector<BlackList*>::iterator blackListIter = map->begin();

            while (blackListIter != map->end()) {
                delete *blackListIter;
                ++blackListIter;
            }

            delete map;
        }

    private:

        // Представление отображения Max_White в виде (grayCount) -> BlackList(grayCount)
        std::vector<BlackList*>* map;

        // Максимальное количество серых узлов по всем чёрно-белым раскраскам.
        int maxGrayCount;

        // Максимальное количество чёрных узлов по всем чёрно-белым раскраскам.
        int maxBlackCount;
    };

    /**
     * Каждому цвету color чёрно-белой палитры, и каждому целому blackCount, сопоставляет элемент.
     * Элемент - это пара:
     *    1) неотрицательное целое, равное максимальному числу белых узлов по всем чёрно-белым раскраскам ветви,
     *       в которых начальный узел ствола ветви окрашен в цвет color, и ровно blackCount узлов ветви окрашено в чёрный цвет;
     *    2) раскраску ствола ветви, соответствующую некоторой оптимальной чёрно-белой раскраске ветви,
     *       в которой первый узел ствола ветви окрашен в цвет color, и число узлов ветви, окрашенных в чёрный цвет, равно blackCount.
     */
    class BranchMaxWhiteMap {

    public:

        /**
         * Элемент - это пара:
         *    1) неотрицательное целое, равное максимальному числу белых узлов по всем чёрно-белым раскраскам ветви,
         *       в которых начальный узел ствола ветви окрашен в цвет color, и ровно blackCount узлов ветви окрашено в чёрный цвет;
         *    2) раскраску ствола ветви, соответствующую некоторой оптимальной чёрно-белой раскраске ветви,
         *       в которой первый узел ствола ветви окрашен в цвет color, и число узлов ветви, окрашенных в чёрный цвет, равно blackCount.
         */
        class Element {

        public:

            Element() {}

            Element(const int newMaxWhite, const BranchTrunkColoring& newTrunkColoring) : maxWhite(newMaxWhite), trunkColoring(newTrunkColoring) {}

            void setMaxWhite(const int maxWhite) {
                this->maxWhite = maxWhite;
            }

            int getMaxWhite() const {
                return maxWhite;
            }

            void setBranchTrunkColoring(const BranchTrunkColoring& trunkColoring) {
                this->trunkColoring = trunkColoring;
            }

            BranchTrunkColoring getBranchTrunkColoring() const {
                return trunkColoring;
            }

        private:

            // Максимальное число белых узлов.
            int maxWhite;

            // Соответствующая раскраска ствола ветви.
            BranchTrunkColoring trunkColoring;
        };

    public:

        BranchMaxWhiteMap() : branchNodeCount(INVALID_NODE_COUNT) {}

        BranchMaxWhiteMap(const int newBranchNodeCount) : branchNodeCount(newBranchNodeCount) {
            initMap();
        }

        BranchMaxWhiteMap(const Gray2BlackListMap& source) : branchNodeCount(source.getMaxBlackCount()) {
            from(source);
        }

        BranchMaxWhiteMap(Gray2BlackListMap* source) : branchNodeCount(source->getMaxBlackCount()) {
            from(*source);
        }

        void put(const ColorType color, const int blackCount, const Element& element) {
            map[color][blackCount] = element;
        }

        Element get(const ColorType color, const int blackCount) const {
            return blackCount >= 0 && blackCount <= branchNodeCount ? map[color][blackCount] : Element(INVALID_MAX_WHITE, BranchTrunkColoring());
        }

        Element get(const int blackCount) const {
            ColorType maxColor = BLACK;

            if (get(WHITE, blackCount).getMaxWhite() > get(maxColor, blackCount).getMaxWhite()) {
                maxColor = WHITE;
            }

            if (get(GRAY, blackCount).getMaxWhite() > get(maxColor, blackCount).getMaxWhite()) {
                maxColor = GRAY;
            }

            return get(maxColor, blackCount);
        }

        int getBranchNodeCount() const {
            return branchNodeCount;
        }

        Gray2BlackListMap* toGray2BlackListMap() const {
            Gray2BlackListMap* gray2BlackListMap = new Gray2BlackListMap((int)log2(branchNodeCount) + 3);
            int grayCount;

            for (int b = 0; b <= branchNodeCount; ++b) {
                // Чёрный цвет.
                if (get(BLACK, b).getMaxWhite() != INVALID_MAX_WHITE) {
                    grayCount = branchNodeCount - (b + get(BLACK, b).getMaxWhite());
                    gray2BlackListMap->put(BLACK, grayCount, b);
                }

                // Белый цвет.
                if (get(WHITE, b).getMaxWhite() != INVALID_MAX_WHITE) {
                    grayCount = branchNodeCount - (b + get(WHITE, b).getMaxWhite());
                    gray2BlackListMap->put(WHITE, grayCount, b);
                }

                // Серый цвет.
                if (get(GRAY, b).getMaxWhite() != INVALID_MAX_WHITE) {
                    grayCount = branchNodeCount - (b + get(GRAY, b).getMaxWhite());
                    gray2BlackListMap->put(GRAY, grayCount, b);
                }
            }

            return gray2BlackListMap;
        }

        ColorlessGray2BlackListMap* toColorlessGray2BlackListMap() const {
            ColorlessGray2BlackListMap* colorlessGray2BlackListMap = new ColorlessGray2BlackListMap((int)log2(branchNodeCount) + 3);

            for (int b = 0; b <= branchNodeCount; ++b) {
                int maxWhite = get(b);

                if (maxWhite != INVALID_MAX_WHITE) {
                    int grayCount = branchNodeCount - (b + maxWhite);
                    colorlessGray2BlackListMap->put(grayCount, b);
                }
            }

            return colorlessGray2BlackListMap;
        }

        ColorlessGray2BlackListMap* toColorlessGray2BlackListMap(const ColorType color) const {
            ColorlessGray2BlackListMap* colorlessGray2BlackListMap = new ColorlessGray2BlackListMap((int)log2(branchNodeCount) + 3);

            for (int b = 0; b <= branchNodeCount; ++b) {
                int maxWhite = get(color, b).getMaxWhite();

                if (maxWhite != INVALID_MAX_WHITE) {
                    int grayCount = branchNodeCount - (b + maxWhite);
                    colorlessGray2BlackListMap->put(grayCount, b);
                }
            }

            return colorlessGray2BlackListMap;
        }

        void from(const Gray2BlackListMap& sourse) {
            initMap();

            for (int g = 0; g <= sourse.getMaxGrayCount(); ++g) {
                // Чёрный цвет.
                BlackList* blackList = sourse.get(BLACK, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(BLACK, *blackListIter, Element(branchNodeCount - (*blackListIter + g), BranchTrunkColoring()));
                    }
                }

                // Белый цвет.
                blackList = sourse.get(WHITE, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(WHITE, *blackListIter, Element(branchNodeCount - (*blackListIter + g), BranchTrunkColoring()));
                    }
                }

                // Серый цвет.
                blackList = sourse.get(GRAY, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(GRAY, *blackListIter, Element(branchNodeCount - (*blackListIter + g), BranchTrunkColoring()));
                    }
                }
            }
        }

    private:

        void destroy() {
            delete[] map[BLACK];
            delete[] map[WHITE];
            delete[] map[GRAY];
        }

        void initMap() {
            map[BLACK] = new Element[branchNodeCount + 1];
            map[WHITE] = new Element[branchNodeCount + 1];
            map[GRAY] = new Element[branchNodeCount + 1];

            for (int b = 0; b <= branchNodeCount; ++b) {
                map[BLACK][b].setMaxWhite(INVALID_MAX_WHITE);
                map[WHITE][b].setMaxWhite(INVALID_MAX_WHITE);
                map[GRAY][b].setMaxWhite(INVALID_MAX_WHITE);
            }
        }

    private:

         // Каждому цвету color чёрно-белой палитры, и каждому целому blackCount, сопоставляет элемент.
         // Элемент - это пара:
         //    1) неотрицательное целое, равное максимальному числу белых узлов по всем чёрно-белым раскраскам ветви,
         //       в которых начальный узел ствола ветви окрашен в цвет color, и ровно blackCount узлов ветви окрашено в чёрный цвет;
         //    2) раскраску ствола ветви, соответствующую некоторой оптимальной чёрно-белой раскраске ветви,
         //       в которой первый узел ствола ветви окрашен в цвет color, и число узлов ветви, окрашенных в чёрный цвет, равно blackCount.
        Element* map[COLOR_COUNT];

        // Количетво узлов в ветви.
        int branchNodeCount;
    };

    typedef typename BranchMaxWhiteMap::Element BranchMaxWhiteMapElement;

    /**
     * Каждому цвету color чёрно-белой палитры, и каждому целому blackCount, сопоставляет неотрицательное целое,
     * равное максимальному числу белых узлов по всем чёрно-белым раскраскам поддерева,
     * в которых корень поддерева окрашен в цвет color, и ровно blackCount узлов поддерева окрашено в чёрный цвет.
     */
    class ColorMaxWhiteMap {

    public:

        ColorMaxWhiteMap() : nodeCount(INVALID_NODE_COUNT) {}

        ColorMaxWhiteMap(const int newNodeCount) : nodeCount(newNodeCount) {
            initMap();
        }

        ColorMaxWhiteMap(const Gray2BlackListMap& source) : nodeCount(source.getMaxBlackCount()) {
            from(source);
        }

        ColorMaxWhiteMap(Gray2BlackListMap* source) : nodeCount(source->getMaxBlackCount()) {
            from(*source);
        }

        void put(const ColorType color, const int blackCount, const int maxWhite) {
            map[color][blackCount] = maxWhite;
        }

        int get(const ColorType color, const int blackCount) const {
            return blackCount >= 0 && blackCount <= nodeCount ? map[color][blackCount] : INVALID_MAX_WHITE;
        }

        int get(const int blackCount) const {
            return std::max(std::max(get(BLACK, blackCount), get(WHITE, blackCount)), get(GRAY, blackCount));
        }

        int getNodeCount() const {
            return nodeCount;
        }

        Gray2BlackListMap* toGray2BlackListMap() const {
            Gray2BlackListMap* gray2BlackListMap = new Gray2BlackListMap((int)log2(nodeCount) + 3);
            int grayCount;

            for (int b = 0; b <= nodeCount; ++b) {
                // Чёрный цвет.
                if (get(BLACK, b) != INVALID_MAX_WHITE) {
                    grayCount = nodeCount - (b + get(BLACK, b));
                    gray2BlackListMap->put(BLACK, grayCount, b);
                }

                // Белый цвет.
                if (get(WHITE, b) != INVALID_MAX_WHITE) {
                    grayCount = nodeCount - (b + get(WHITE, b));
                    gray2BlackListMap->put(WHITE, grayCount, b);
                }

                // Серый цвет.
                if (get(GRAY, b) != INVALID_MAX_WHITE) {
                    grayCount = nodeCount - (b + get(GRAY, b));
                    gray2BlackListMap->put(GRAY, grayCount, b);
                }
            }

            return gray2BlackListMap;
        }

        ColorlessGray2BlackListMap* toColorlessGray2BlackListMap() const {
            ColorlessGray2BlackListMap* colorlessGray2BlackListMap = new ColorlessGray2BlackListMap((int)log2(nodeCount) + 3);

            for (int b = 0; b <= nodeCount; ++b) {
                int maxWhite = get(b);

                if (maxWhite != INVALID_MAX_WHITE) {
                    int grayCount = nodeCount - (b + maxWhite);
                    colorlessGray2BlackListMap->put(grayCount, b);
                }
            }

            return colorlessGray2BlackListMap;
        }

        ColorlessGray2BlackListMap* toColorlessGray2BlackListMap(const ColorType color) const {
            ColorlessGray2BlackListMap* colorlessGray2BlackListMap = new ColorlessGray2BlackListMap((int)log2(nodeCount) + 3);

            for (int b = 0; b <= nodeCount; ++b) {
                int maxWhite = get(color, b);

                if (maxWhite != INVALID_MAX_WHITE) {
                    int grayCount = nodeCount - (b + maxWhite);
                    colorlessGray2BlackListMap->put(grayCount, b);
                }
            }

            return colorlessGray2BlackListMap;
        }

        void from(const Gray2BlackListMap& sourse) {
            initMap();

            for (int g = 0; g <= sourse.getMaxGrayCount(); ++g) {
                // Чёрный цвет.
                BlackList* blackList = sourse.get(BLACK, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(BLACK, *blackListIter, nodeCount - (*blackListIter + g));
                    }
                }

                // Белый цвет.
                blackList = sourse.get(WHITE, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(WHITE, *blackListIter, nodeCount - (*blackListIter + g));
                    }
                }

                // Серый цвет.
                blackList = sourse.get(GRAY, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(GRAY, *blackListIter, nodeCount - (*blackListIter + g));
                    }
                }
            }
        }

    private:

        void destroy() {
            delete[] map[BLACK];
            delete[] map[WHITE];
            delete[] map[GRAY];
        }

        void initMap() {
            map[BLACK] = new int[nodeCount + 1];
            map[WHITE] = new int[nodeCount + 1];
            map[GRAY] = new int[nodeCount + 1];

            for (int b = 0; b <= nodeCount; ++b) {
                map[BLACK][b] = INVALID_MAX_WHITE;
                map[WHITE][b] = INVALID_MAX_WHITE;
                map[GRAY][b] = INVALID_MAX_WHITE;
            }
        }

    private:

        // Каждому цвету color чёрно-белой палитры, и каждому целому blackCount, сопоставляет неотрицательное целое,
        // равное максимальному числу белых узлов по всем чёрно-белым раскраскам поддерева,
        // в которых корень поддерева окрашен в цвет color, и ровно blackCount узлов поддерева окрашено в чёрный цвет.
        int* map[COLOR_COUNT];

        // Количеcтво узлов в поддереве.
        int nodeCount;
    };

    /**
     * Каждому целому blackCount, сопоставляет неотрицательное целое,
     * равное максимальному числу белых узлов по всем чёрно-белым раскраскам
     * (возможно удовлетворяющим некоторому множеству условий) соответствующей совокупности узлов,
     * в которых ровно blackCount узлов совокупности окрашено в чёрный цвет.
     */
    class ColorlessMaxWhiteMap {

    public:

        ColorlessMaxWhiteMap() : nodeCount(INVALID_NODE_COUNT) {}

        ColorlessMaxWhiteMap(const int newNodeCount) : nodeCount(newNodeCount) {
            initMap();
        }

        ColorlessMaxWhiteMap(const ColorlessGray2BlackListMap& source) : nodeCount(source.getMaxBlackCount()) {
            from(source);
        }

        ColorlessMaxWhiteMap(const ColorlessGray2BlackListMap& source, const int newNodeCount) : nodeCount(newNodeCount) {
            from(source);
        }

        ColorlessMaxWhiteMap(ColorlessGray2BlackListMap* source) : nodeCount(source->getMaxBlackCount()) {
            from(*source);
        }

        ColorlessMaxWhiteMap(ColorlessGray2BlackListMap* source, const int newNodeCount) : nodeCount(newNodeCount) {
            from(*source);
        }

        void put(const int blackCount, const int maxWhite) {
            map[blackCount] = maxWhite;
        }

        int get(const int blackCount) const {
            return blackCount >= 0 && blackCount <= nodeCount ? map[blackCount] : INVALID_MAX_WHITE;
        }

        int getNodeCount() const {
            return nodeCount;
        }

        ColorlessGray2BlackListMap* toColorlessGray2BlackListMap() const {
            ColorlessGray2BlackListMap* colorlessGray2BlackListMap = new ColorlessGray2BlackListMap((int)log2(nodeCount) + 3);

            for (int b = 0; b <= nodeCount; ++b) {
                int maxWhite = get(b);

                if (maxWhite != INVALID_MAX_WHITE) {
                    int grayCount = nodeCount - (b + maxWhite);
                    colorlessGray2BlackListMap->put(grayCount, b);
                }
            }

            return colorlessGray2BlackListMap;
        }

        void from(const ColorlessGray2BlackListMap& sourse) {
            initMap();

            for (int g = 0; g <= sourse.getMaxGrayCount(); ++g) {
                BlackList* blackList = sourse.get(g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(*blackListIter, nodeCount - (*blackListIter + g));
                    }
                }
            }
        }

    private:

        void destroy() {
            delete[] map;
        }

        void initMap() {
            map = new int[nodeCount + 1];

            for (int b = 0; b <= nodeCount; ++b) {
                map[b] = INVALID_MAX_WHITE;
            }
        }

    private:

        // Каждому целому blackCount, сопоставляет неотрицательное целое,
        // равное максимальному числу белых узлов по всем чёрно-белым раскраскам
        // (возможно удовлетворяющим некоторому множеству условий) соответствующей совокупности узлов,
        // в которых ровно blackCount узлов совокупности окрашено в чёрный цвет.
        int* map;

        // Количетво узлов.
        int nodeCount;
    };

    /**
     * Слияние отображений Max_White множества подветвей некоторой ветви, у которых совпадают начальные узлы стволов.
     */
    class Fusion {

    public:

        Fusion(Fusion* newLeftFusion, Fusion* newRightFusion) : left(newLeftFusion), right(newRightFusion) {}

        void setLeftFusion(Fusion* newLeftFusion) {
            left = newLeftFusion;
        }

        Fusion* getLeftFusion() const {
            return left;
        }

        void setRightFusion(Fusion* newRightFusion) {
            right = newRightFusion;
        }

        Fusion* getRightFusion() const {
            return right;
        }

        virtual int getMinGrayCount(const ColorType color, const int blackCount) const = 0;

        virtual ~Fusion() {
            delete left;
            delete right;
        }

    private:

        // Левое слияние.
        Fusion* left;

        // Правое слияние.
        Fusion* right;
    };

    /**
     * Отображение Max_White подветви некоторой ветви, представленное в виде (color, blackCount) -> min{grayCount}.
     */
    class SingleChildFusion : public Fusion {

    public:

        SingleChildFusion(BranchColoringInfo* newSubbranchColoringInfo) :
            Fusion(0, 0),
            subbranchNodeCount(newSubbranchColoringInfo->getBranch()->getNodeCount()),
            info(newSubbranchColoringInfo) {}

        int getMinGrayCount(const ColorType color, const int blackCount) const {
            return blackCount >= 0 && blackCount <= subbranchNodeCount && info->getMaxWhite(color, blackCount) != INVALID_MAX_WHITE ? subbranchNodeCount - (blackCount + info->getMaxWhite(color, blackCount)) : INVALID_MIN_GRAY;
        }

        BranchColoringInfo* getSubbranchColoringInfo() const {
            return info;
        }

        virtual ~SingleChildFusion() {}

    private:

        // Количество узлов в подветви.
        int subbranchNodeCount;

        // Информация о раскраске подветви.
        BranchColoringInfo* info;
    };

    /**
     * Слияние отображений Max_White нескольких подветвей некоторой ветви, у которых совпадают начальные узлы стволов.
     */
    class MultiChildFusion : public Fusion {

    public:

        MultiChildFusion(const Gray2BlackListMap& gray2BlackListMap, Fusion* leftFusion, Fusion* rightFusion) : Fusion(leftFusion, rightFusion), maxBlackCount(gray2BlackListMap.getMaxBlackCount()) {
            from(gray2BlackListMap);
        }

        MultiChildFusion(Gray2BlackListMap* gray2BlackListMap, Fusion* leftFusion, Fusion* rightFusion) : Fusion(leftFusion, rightFusion), maxBlackCount(gray2BlackListMap->getMaxBlackCount()) {
            from(*gray2BlackListMap);
        }

        int getMinGrayCount(const ColorType color, const int blackCount) const {
            return blackCount >= 0 && blackCount <= maxBlackCount ? minGrayCount[color][blackCount] : INVALID_MIN_GRAY;
        }

        virtual ~MultiChildFusion() {
            delete[] minGrayCount[BLACK];
            delete[] minGrayCount[WHITE];
            delete[] minGrayCount[GRAY];
        }

    private:

        void initMinGrayCount() {
            minGrayCount[BLACK] = new int[maxBlackCount + 1];
            minGrayCount[WHITE] = new int[maxBlackCount + 1];
            minGrayCount[GRAY] = new int[maxBlackCount + 1];

            for (int b = 0; b <= maxBlackCount; ++b) {
                minGrayCount[BLACK][b] = INVALID_MIN_GRAY;
                minGrayCount[WHITE][b] = INVALID_MIN_GRAY;
                minGrayCount[GRAY][b] = INVALID_MIN_GRAY;
            }
        }

        void from(const Gray2BlackListMap& sourse) {
            initMinGrayCount();

            for (int g = 0; g <= sourse.getMaxGrayCount(); ++g) {
                // Чёрный цвет.
                BlackList* blackList = sourse.get(BLACK, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        minGrayCount[BLACK][*blackListIter] = g;
                    }
                }

                // Белый цвет.
                blackList = sourse.get(WHITE, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        minGrayCount[WHITE][*blackListIter] = g;
                    }
                }

                // Серый цвет.
                blackList = sourse.get(GRAY, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        minGrayCount[GRAY][*blackListIter] = g;
                    }
                }
            }
        }

    private:

        // Сопоставляет каждому цвету color чёрно-белой палитры, и каждому целому blackCount, неотрицательное целое Min_Gray_Count(color, blackCount),
        // равное числу серых узлов в (любой) оптимальной чёрно-белой раскраске соответствующей совокупности подветвей, при условии, что в этой раскраске
        // начальный узел ствола каждой из подветвей окрашен в цвет color и ровно blackCount узлов совокупности окрашено в чёрный цвет.
        int* minGrayCount[COLOR_COUNT];

        // Максимальное количество чёрных узлов.
        int maxBlackCount;
    };

private:

    typedef std::stack<Fusion*> FusionStack;

private:

    /**
     * Ветвь.
     *
     * Сначала введём понятие ствола ветви.
     *
     * Стволом ветви называется путь в дереве, определяемый парой (узел u, дочерний узел v узла u) следующим образом:
     * <u, v, First_Child(v), First_Child(First_Child(v)), ..., g, First_Child(g) in Leaf(T)>.
     * То есть, первыми двумя узлами этого пути будут узлы u и v.
     * Далее, по индукции: если g - k-ый узел ствола ветви, то k+1-ым узлом этого ствола будет узел First_Child(g).
     * Последний узел должен быть листом.
     *
     * В этом определении неявно фигурирует понятие упорядоченности дочерних узлов.
     * Имеется ввиду любое их упорядочение, заданное до начала работы алгоритма.
     * Например, если дочерние узлы хранятся в массиве,
     * k-ым дочерним узлом можно считать узел, расположенный в ячейке этого массива с индексом k.
     *
     * Ветвью называется ствол E в объединении со всеми ветвями дерева, у которых начальный узел ствола принадлежит множеству E \ First_Node(E).
     */
    class Branch {

    public:

        Branch() {}

        Branch(
            NodeType* newFirstTrunkNode,
            const int newChildPosition) :

            firstTrunkNode(newFirstTrunkNode),
            childPosition(newChildPosition),
            nodeCount(INVALID_NODE_COUNT),
            subbranches(0),
            uniqueSubbranchPositionCount(0),
            built(false) {

            // Подсчитаем длину ствола.
            calcTrunkLength();

            // Подсчитаем количество подветвей и количество уникальных позиций подветвей.
            calcSubbranchCount();
        }

        NodeType* getFirstTrunkNode() const {
            return firstTrunkNode;
        }

        int getChildPosition() const {
            return childPosition;
        }

        int getTrunkLength() const {
            return trunkLength;
        }

        const SubbranchInfo& getSubbranchInfo(const int i) const {
            return subbranches[i];
        }

        int getSubbranchCount() const {
            return subbranchCount;
        }

        int getUniqueSubbranchPositionCount() const {
            return uniqueSubbranchPositionCount;
        }

        int getNodeCount() {
            if (nodeCount != INVALID_NODE_COUNT) {
                return nodeCount;
            }

            build();
            nodeCount = trunkLength;

            for (int i = 0; i < subbranchCount; ++i) {
                nodeCount += subbranches[i].first->getNodeCount();
            }

            nodeCount -= subbranchCount;
            return nodeCount;
        }

        void build() {
            if (subbranchCount == 0) {
                built = true;
                return;
            }

            subbranches = new SubbranchInfo[subbranchCount];

            NodeType* currTrunkNode = firstTrunkNode->getChild(childPosition);
            int subbranchIndex = 0;
            int subbranchPosition = 1;

            while (!currTrunkNode->isLeaf()) {
                if (currTrunkNode->getChildCount() > 1) {
                    for (int i = 1; i < currTrunkNode->getChildCount(); ++i) {
                        subbranches[subbranchIndex] = SubbranchInfo(new Branch(currTrunkNode, i), subbranchPosition);
                        subbranches[subbranchIndex].first->build();
                        ++subbranchIndex;
                    }
                }

                ++subbranchPosition;
                currTrunkNode = currTrunkNode->getChild(0);
            }

            built = true;
        }

        bool isBuilt() const {
            return built;
        }

        ~Branch() {
            if (!isBuilt()) {
                return;
            }

            for (int i = 0; i < subbranchCount; ++i) {
               delete subbranches[i].first;
            }

            delete subbranches;
        }

    private:

        // Находит длину ствола.
        void calcTrunkLength() {
            if (firstTrunkNode->isLeaf()) {
                trunkLength = 1;
            }
            else {
                NodeType* currTrunkNode = firstTrunkNode->getChild(childPosition);
                trunkLength = 2;

                while (!currTrunkNode->isLeaf()) {
                    ++trunkLength;
                    currTrunkNode = currTrunkNode->getChild(0);
                }
            }
        }

        // Находит число подветвей и число уникальных позиций подветвей.
        void calcSubbranchCount() {
            subbranchCount = 0;
            uniqueSubbranchPositionCount = 0;

            if (!firstTrunkNode->isLeaf()) {
                NodeType* currTrunkNode = firstTrunkNode->getChild(childPosition);

                while (!currTrunkNode->isLeaf()) {
                    if (currTrunkNode->getChildCount() > 1) {
                        subbranchCount += currTrunkNode->getChildCount() - 1;
                        ++uniqueSubbranchPositionCount;
                    }

                    currTrunkNode = currTrunkNode->getChild(0);
                }
            }
        }

    private:

        // Начальный узел ствола.
        NodeType* firstTrunkNode;

        // Позиция второго узла ствола в наборе дочерних узлов первого узла ствола.
        int childPosition;

        // Длина ствола.
        int trunkLength;

        // Количество узлов.
        int nodeCount;

        // Подветви и их позиции.
        SubbranchInfo* subbranches;

        // Количество подветвей.
        int subbranchCount;

        // Количество уникальных позиций подветвей.
        int uniqueSubbranchPositionCount;

        // Показывает, выполнено ли построение?
        // Под построением понимается построение подветвей.
        bool built;
    };

    /**
     * Раскраска ствола ветви.
     * Определяется типом, позицией серого узла в стволе ветви и цветом первого узла ствола ветви.
     */
    class BranchTrunkColoring {

    public:

        /**
         * Тип раскраски.
         *
         * Всего существует 6 типов:
         *     a) все узлы чёрные;
         *     b) все узлы белые;
         *     c) чёрные узлы сверху от серого, белые - снизу;
         *     d) белые узлы сверху от серого, чёрные - снизу;
         *     e) чёрные узлы и сверху и снизу от серого;
         *     f) белые узлы и сверху и снизу от серого.
         */
        enum Type {
            ALL_BLACK,
            ALL_WHITE,
            BLACK_ABOVE,
            WHITE_ABOVE,
            BLACK_ABOVE_AND_BELOW,
            WHITE_ABOVE_AND_BELOW
        };

    public:

        BranchTrunkColoring() {}

        BranchTrunkColoring(
            const Type newType,
            const int newGrayPosition,
            const ColorType newFirstNodeColor) :

            type(newType),
            grayPosition(newGrayPosition),
            firstNodeColor(newFirstNodeColor) {}

        void setType(const Type type) {
            this->type = type;
        }

        Type getType() const {
            return type;
        }

        void setGrayPosition(const int grayPosition) {
            this->grayPosition = grayPosition;
        }

        int getGrayPosition() const {
            return grayPosition;
        }

        void setFirstNodeColor(const ColorType firstNodeColor) {
            this->firstNodeColor = firstNodeColor;
        }

        ColorType getFirstNodeColor() const {
            return firstNodeColor;
        }

        void apply(ColoringType* coloring, const Branch& branch) const {
            int childPosition = branch.getChildPosition();
            NodeType* currNode = branch.getFirstTrunkNode();

            if (type == ALL_BLACK || type == ALL_WHITE) {
                ColorType color = (type == ALL_BLACK) ? BLACK : WHITE;
                colorFirstNode(coloring, branch);

                if (currNode->isLeaf()) {
                    return;
                }

                currNode = currNode->getChild(childPosition);

                while (true) {
                    coloring->color(currNode, color);

                    if (currNode->isLeaf()) {
                        break;
                    }
                    else {
                        currNode = currNode->getChild(0);
                    }
                }

                return;
            }

            ColorType aboveColor;
            ColorType belowColor;

            if (type == BLACK_ABOVE) {
                aboveColor = BLACK;
                belowColor = WHITE;
            }
            else if (type == WHITE_ABOVE) {
                aboveColor = WHITE;
                belowColor = BLACK;
            }
            else if (type == BLACK_ABOVE_AND_BELOW) {
                aboveColor = BLACK;
                belowColor = BLACK;
            }
            else if (type == WHITE_ABOVE_AND_BELOW) {
                aboveColor = WHITE;
                belowColor = WHITE;
            }

            if (grayPosition == 0) {
                coloring->color(currNode, GRAY);

                if (currNode->isLeaf()) {
                    return;
                }

                currNode = currNode->getChild(childPosition);

                while (true) {
                    coloring->color(currNode, belowColor);

                    if (currNode->isLeaf()) {
                        break;
                    }
                    else {
                        currNode = currNode->getChild(0);
                    }
                }

                return;
            }

            colorFirstNode(coloring, branch);

            if (currNode->isLeaf()) {
                return;
            }

            currNode = currNode->getChild(childPosition);
            int currNodePosition = 1;

            while (currNodePosition < grayPosition) {
                coloring->color(currNode, aboveColor);
                currNode = currNode->getChild(0);
                ++currNodePosition;
            }

            coloring->color(currNode, GRAY);

            if (currNode->isLeaf()) {
                return;
            }

            currNode = currNode->getChild(0);

            while (true) {
                coloring->color(currNode, belowColor);

                if (currNode->isLeaf()) {
                    break;
                }
                else {
                    currNode = currNode->getChild(0);
                }
            }
        }

        void apply(ColoringType* coloring, Branch* branch) const {
            apply(coloring, *branch);
        }

    private:

        void colorFirstNode(ColoringType* coloring, const Branch& branch) const {
            coloring->color(branch.getFirstTrunkNode(), firstNodeColor);
        }

    private:

        // Тип.
        Type type;

        // Позиция серого узла в стволе ветви.
        int grayPosition;

        // Цвет первого узла ствола ветви.
        ColorType firstNodeColor;
    };

    /**
     * Информация о раскраске ветви.
     */
    class BranchColoringInfo {

    public:

        BranchColoringInfo() {}

        BranchColoringInfo(Branch* newBranch) :
            branch(newBranch),
            maxWhiteMap(0),
            primes(0),
            blackPrefixes(0),
            blackSuffixes(0),
            whitePrefixes(0),
            whiteSuffixes(0),
            bw(0),
            wb(0),
            bb(0),
            ww(0) {}

        Branch* getBranch() const {
            return branch;
        }

        void build() {
            int uniqueSubbranchPositionCount = branch->getUniqueSubbranchPositionCount();

            if (uniqueSubbranchPositionCount == 0) {
                buildBareTrunkMaxWhite();
            }
            else {
                buildPrimes();

                if (uniqueSubbranchPositionCount > 1) {
                    buildCompositions();
                }

                buildCompositeMaxWhite();
            }
        }

        void color(ColoringType* coloring, int blackCount, int whiteCount, const ColorType firstNodeColor) {
            int trunkLength = branch->getTrunkLength();
            int uniqueSubbranchPositionCount = branch->getUniqueSubbranchPositionCount();
            BranchTrunkColoring trunkColoring = maxWhiteMap->get(firstNodeColor, blackCount).getBranchTrunkColoring();
            typedef typename BranchTrunkColoring::Type Type;
            Type type = trunkColoring.getType();
            int grayPosition = trunkColoring.getGrayPosition();

            trunkColoring.apply(coloring, branch);

            if (uniqueSubbranchPositionCount == 1) {
                int primePosition = primes[0]->getPosition();
                ColorType acceptableFirstNodeColors;

                if (type == BranchTrunkColoring::ALL_BLACK) {
                    blackCount -= trunkLength - 1;
                    acceptableFirstNodeColors = BLACK;
                    primes[0]->color(coloring, branch, blackCount, whiteCount, acceptableFirstNodeColors, trunkColoring);

                    return;
                }

                if (type == BranchTrunkColoring::ALL_WHITE) {
                    whiteCount -= trunkLength - 1;
                    acceptableFirstNodeColors = WHITE;
                    primes[0]->color(coloring, branch, blackCount, whiteCount, acceptableFirstNodeColors, trunkColoring);

                    return;
                }

                ColorType aboveColor;
                ColorType belowColor;

                if (type == BranchTrunkColoring::BLACK_ABOVE) {
                    aboveColor = BLACK;
                    belowColor = WHITE;
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE) {
                    aboveColor = WHITE;
                    belowColor = BLACK;
                }
                else if (type == BranchTrunkColoring::BLACK_ABOVE_AND_BELOW) {
                    aboveColor = BLACK;
                    belowColor = BLACK;
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE_AND_BELOW) {
                    aboveColor = WHITE;
                    belowColor = WHITE;
                }

                if (aboveColor == BLACK && belowColor == WHITE) {
                    if (grayPosition == primePosition) {
                        blackCount -= grayPosition;
                        whiteCount -= trunkLength - grayPosition - 1;
                        acceptableFirstNodeColors = GRAY;
                    }
                    else if (grayPosition < primePosition) {
                        blackCount -= grayPosition;
                        whiteCount -= trunkLength - grayPosition - 2;
                        acceptableFirstNodeColors = WHITE;
                    }
                    else {
                        blackCount -= grayPosition - 1;
                        whiteCount -= trunkLength - grayPosition - 1;
                        acceptableFirstNodeColors = BLACK;
                    }
                }
                else if (aboveColor == WHITE && belowColor == BLACK) {
                    if (grayPosition == primePosition) {
                        whiteCount -= grayPosition;
                        blackCount -= trunkLength - grayPosition - 1;
                        acceptableFirstNodeColors = GRAY;
                    }
                    else if (grayPosition < primePosition) {
                        whiteCount -= grayPosition;
                        blackCount -= trunkLength - grayPosition - 2;
                        acceptableFirstNodeColors = BLACK;
                    }
                    else {
                        whiteCount -= grayPosition - 1;
                        blackCount -= trunkLength - grayPosition - 1;
                        acceptableFirstNodeColors = WHITE;
                    }
                }
                else if (aboveColor == BLACK && belowColor == BLACK) {
                    if (grayPosition == primePosition) {
                        blackCount -= trunkLength - 1;
                        acceptableFirstNodeColors = GRAY;
                    }
                    else {
                        blackCount -= trunkLength - 2;
                        acceptableFirstNodeColors = BLACK;
                    }
                }
                else if (aboveColor == WHITE && belowColor == WHITE) {
                    if (grayPosition == primePosition) {
                        whiteCount -= trunkLength - 1;
                        acceptableFirstNodeColors = GRAY;
                    }
                    else {
                        whiteCount -= trunkLength - 2;
                        acceptableFirstNodeColors = WHITE;
                    }
                }

                if (firstNodeColor == GRAY && grayPosition > 0) {
                    if (aboveColor == BLACK) {
                        ++blackCount;
                    }
                    else {
                        ++whiteCount;
                    }
                }

                primes[0]->color(coloring, branch, blackCount, whiteCount, acceptableFirstNodeColors, trunkColoring);
            }
            else if (uniqueSubbranchPositionCount > 1) {
                int firstPrimePosition = primes[0]->getPosition();
                int lastPrimePosition = primes[uniqueSubbranchPositionCount - 1]->getPosition();
                CompositeSubbranchColoringInfo* composition = getComposition(trunkColoring);

                if (type == BranchTrunkColoring::ALL_BLACK) {
                    blackCount -= trunkLength - lastPrimePosition - 1;
                }
                else if (type == BranchTrunkColoring::ALL_WHITE) {
                    whiteCount -= trunkLength - lastPrimePosition - 1;
                }
                else {
                    // Серый узел лежит в хвосте ветви.
                    if (grayPosition < firstPrimePosition) {
                        if (composition->getAboveColor() == BLACK) {
                            blackCount -= grayPosition;
                            whiteCount -= firstPrimePosition - grayPosition - 1;
                        }
                        else {
                            whiteCount -= grayPosition;
                            blackCount -= firstPrimePosition - grayPosition - 1;
                        }
                    }
                    else if (grayPosition > lastPrimePosition) {
                        if (composition->getAboveColor() == BLACK) {
                            blackCount -= grayPosition - lastPrimePosition - 1;
                            whiteCount -= trunkLength - grayPosition - 1;
                        }
                        else {
                            whiteCount -= grayPosition - lastPrimePosition - 1;
                            blackCount -= trunkLength - grayPosition - 1;
                        }
                    }
                    // Серый узел лежит не в хвосте ветви.
                    else {
                        int current = 0;

                        while (primes[current]->getPosition() < grayPosition) {
                            ++current;
                        }

                        int prev = (primes[current]->getPosition() == grayPosition) ? primes[current]->getPosition() : primes[current - 1]->getPosition();

                        if (prev == lastPrimePosition) {
                            if (composition->getAboveColor() == BLACK) {
                                blackCount -= lastPrimePosition - primes[uniqueSubbranchPositionCount - 2]->getPosition() - 1;
                            }
                            else {
                                whiteCount -= lastPrimePosition - primes[uniqueSubbranchPositionCount - 2]->getPosition() - 1;
                            }
                        }
                        else {
                            int next = (primes[current]->getPosition() == grayPosition) ? primes[current + 1]->getPosition() : primes[current]->getPosition();
                            int prevDistance = std::max(grayPosition - prev - 1, 0);
                            int nextDistance = next - grayPosition - 1;

                            if (composition->getAboveColor() == BLACK) {
                                blackCount -= prevDistance;
                                whiteCount -= nextDistance;
                            }
                            else {
                                whiteCount -= prevDistance;
                                blackCount -= nextDistance;
                            }
                        }
                    }
                }

                if (firstNodeColor == GRAY && grayPosition > 0) {
                    if (composition->getAboveColor() == BLACK) {
                        ++blackCount;
                    }
                    else {
                        ++whiteCount;
                    }
                }

                composition->color(coloring, branch, blackCount, whiteCount, trunkColoring);
            }
        }

        BranchMaxWhiteMap* getMaxWhiteMap() const {
            return maxWhiteMap;
        }

        int getMaxWhite(const ColorType color, const int blackCount) const {
            return maxWhiteMap->get(color, blackCount).getMaxWhite();
        }

        int getMaxWhite(const int blackCount) const {
            return maxWhiteMap->get(blackCount).getMaxWhite();
        }

        ~BranchColoringInfo() {
            if (maxWhiteMap != 0) {
                delete maxWhiteMap;
            }

            int uniqueSubbranchPositionCount = branch->getUniqueSubbranchPositionCount();

            if (primes != 0) {
                for (int i = 0; i < uniqueSubbranchPositionCount; ++i) {
                    delete primes[i];
                }

                delete primes;
            }

            if (blackPrefixes != 0) {
                for (int i = 0; i < uniqueSubbranchPositionCount; ++i) {
                    delete blackPrefixes[i];
                    delete blackSuffixes[i];
                    delete whitePrefixes[i];
                    delete whiteSuffixes[i];

                    if (i < uniqueSubbranchPositionCount - 1) {
                        delete bw[i];
                        delete wb[i];
                        delete bb[i];
                        delete ww[i];
                    }
                }

                delete primes;
                delete blackPrefixes;
                delete blackSuffixes;
                delete whitePrefixes;
                delete whiteSuffixes;
                delete bw;
                delete wb;
                delete bb;
                delete ww;
            }
        }

    private:

        CompositeSubbranchColoringInfo* getComposition(const BranchTrunkColoring& trunkColoring) const {
            int uniqueSubbranchPositionCount = branch->getUniqueSubbranchPositionCount();
            int firstPrimePosition = primes[0]->getPosition();
            int lastPrimePosition = primes[uniqueSubbranchPositionCount - 1]->getPosition();
            int grayPosition = trunkColoring.getGrayPosition();
            typedef typename BranchTrunkColoring::Type Type;
            Type type = trunkColoring.getType();

            if (type == BranchTrunkColoring::ALL_BLACK) {
                return blackPrefixes[uniqueSubbranchPositionCount - 1];
            }
            else if (type == BranchTrunkColoring::ALL_WHITE) {
                return whitePrefixes[uniqueSubbranchPositionCount - 1];
            }

            // Серый узел лежит в хвосте ветви.
            if (grayPosition < firstPrimePosition) {
                if (type == BranchTrunkColoring::BLACK_ABOVE) {
                    return whiteSuffixes[uniqueSubbranchPositionCount - 1];
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE) {
                    return blackSuffixes[uniqueSubbranchPositionCount - 1];
                }
                else if (type == BranchTrunkColoring::BLACK_ABOVE_AND_BELOW) {
                    return blackSuffixes[uniqueSubbranchPositionCount - 1];
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE_AND_BELOW) {
                    return whiteSuffixes[uniqueSubbranchPositionCount - 1];
                }
            }

            if (grayPosition > lastPrimePosition) {
                if (type == BranchTrunkColoring::BLACK_ABOVE) {
                    return blackPrefixes[uniqueSubbranchPositionCount - 1];
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE) {
                    return whitePrefixes[uniqueSubbranchPositionCount - 1];
                }
                else if (type == BranchTrunkColoring::BLACK_ABOVE_AND_BELOW) {
                    return blackPrefixes[uniqueSubbranchPositionCount - 1];
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE_AND_BELOW) {
                    return whitePrefixes[uniqueSubbranchPositionCount - 1];
                }
            }

            // Серый узел лежит не в хвосте ветви.
            int current = 0;

            while (primes[current]->getPosition() < grayPosition) {
                ++current;
            }

            int prev = (primes[current]->getPosition() == grayPosition) ? primes[current]->getPosition() : primes[current - 1]->getPosition();

            if (prev == lastPrimePosition) {
                if (type == BranchTrunkColoring::BLACK_ABOVE) {
                    return bw[uniqueSubbranchPositionCount - 2];
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE) {
                    return wb[uniqueSubbranchPositionCount - 2];
                }
                else if (type == BranchTrunkColoring::BLACK_ABOVE_AND_BELOW) {
                    return bb[uniqueSubbranchPositionCount - 2];
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE_AND_BELOW) {
                    return ww[uniqueSubbranchPositionCount - 2];
                }
            }
            else {
                int compositeIndex = (primes[current]->getPosition() == grayPosition) ? current : current - 1;

                if (type == BranchTrunkColoring::BLACK_ABOVE) {
                    return bw[compositeIndex];
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE) {
                    return wb[compositeIndex];
                }
                else if (type == BranchTrunkColoring::BLACK_ABOVE_AND_BELOW) {
                    return bb[compositeIndex];
                }
                else if (type == BranchTrunkColoring::WHITE_ABOVE_AND_BELOW) {
                    return ww[compositeIndex];
                }
            }
        }

        void buildBareTrunkMaxWhite() {
            int trunkLength = branch->getTrunkLength();
            maxWhiteMap = new BranchMaxWhiteMap(branch->getNodeCount());

            // Все узлы чёрные.
            maxWhiteMap->put(BLACK, trunkLength, BranchMaxWhiteMapElement(0, BranchTrunkColoring(BranchTrunkColoring::ALL_BLACK, NO_GRAY, BLACK)));

            // Все узлы белые.
            maxWhiteMap->put(WHITE, 0, BranchMaxWhiteMapElement(trunkLength, BranchTrunkColoring(BranchTrunkColoring::ALL_WHITE, NO_GRAY, WHITE)));

            // Обрабатываем остальные случаи.
            int blackCount;
            int maxWhite;
            ColorType firstTrunkNodeColor;

            for (int g = 0; g < trunkLength; ++g) {
                // Чёрные узлы сверху от серого, белые - снизу.
                blackCount = g;
                maxWhite = trunkLength - g - 1;
                firstTrunkNodeColor = (g == 0) ? GRAY : BLACK;

                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, g, firstTrunkNodeColor)));
                }

                if (g > 0) {
                    blackCount = g - 1;
                    maxWhite = trunkLength - g - 1;
                    firstTrunkNodeColor = GRAY;

                    if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                        maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, g, firstTrunkNodeColor)));
                    }
                }

                // Белые узлы сверху от серого, чёрные - снизу.
                blackCount = trunkLength - g - 1;
                maxWhite = g;
                firstTrunkNodeColor = (g == 0) ? GRAY : WHITE;

                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, g, firstTrunkNodeColor)));
                }

                if (g > 0) {
                    blackCount = trunkLength - g - 1;
                    maxWhite = g - 1;
                    firstTrunkNodeColor = GRAY;

                    if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                        maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, g, firstTrunkNodeColor)));

                    }
                }

                // Чёрные узлы и сверху и снизу от серого.
                blackCount = trunkLength - 1;
                maxWhite = 0;
                firstTrunkNodeColor = (g == 0) ? GRAY : BLACK;

                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));
                }

                if (g > 0) {
                    blackCount = trunkLength - 2;
                    maxWhite = 0;
                    firstTrunkNodeColor = GRAY;

                    if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                        maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));
                    }
                }

                // Белые узлы и сверху и снизу от серого.
                // Этот случай обрабатывать не имеет смысла, поскольку для нуля чёрных узлов
                // он даст заведомо немаксимальное значение числа белых.
            }
        }

        void buildPrimes() {
            int subbranchCount = branch->getSubbranchCount();
            int uniqueSubbranchPositionCount = branch->getUniqueSubbranchPositionCount();

            primes = new PrimeSubbranchColoringInfo*[uniqueSubbranchPositionCount];

            int left = 0;
            int primeIndex = 0;

            while (left < subbranchCount) {
                SubbranchInfo subbranch = branch->getSubbranchInfo(left);
                int rightNext = left + 1;

                while (rightNext < subbranchCount && branch->getSubbranchInfo(rightNext).second == subbranch.second) {
                    ++rightNext;
                }

                if (left == rightNext - 1) {
                    primes[primeIndex] = new SingleSubbranchColoringInfo(subbranch);
                }
                else {
                    primes[primeIndex] = new TreeSubbranchColoringInfo(branch, left, rightNext - 1);
                }

                primes[primeIndex]->build();

                left = rightNext;
                ++primeIndex;
            }
        }

        void buildCompositions() {
            int trunkLength = branch->getTrunkLength();
            int uniqueSubbranchPositionCount = branch->getUniqueSubbranchPositionCount();

            // Чёрные префиксы.
            blackPrefixes = new PrefixSubbranchColoringInfo*[uniqueSubbranchPositionCount];
            blackPrefixes[0] = new PrefixSubbranchColoringInfo(BLACK, 0, primes[0], primes[0]->getPosition());
            blackPrefixes[0]->build();

            for (int i = 1; i < uniqueSubbranchPositionCount; ++i) {
                blackPrefixes[i] = new PrefixSubbranchColoringInfo(BLACK, blackPrefixes[i - 1], primes[i], primes[i]->getPosition() - primes[i - 1]->getPosition() - 1);
                blackPrefixes[i]->build();
            }

            // Чёрные суффиксы.
            blackSuffixes = new SuffixSubbranchColoringInfo*[uniqueSubbranchPositionCount];
            blackSuffixes[0] = new SuffixSubbranchColoringInfo(WHITE, 0, primes[uniqueSubbranchPositionCount - 1], trunkLength - primes[uniqueSubbranchPositionCount - 1]->getPosition() - 1);
            blackSuffixes[0]->build();

            for (int i = 1; i < uniqueSubbranchPositionCount; ++i) {
                blackSuffixes[i] = new SuffixSubbranchColoringInfo(WHITE, blackSuffixes[i - 1], primes[uniqueSubbranchPositionCount - i - 1], primes[uniqueSubbranchPositionCount - i]->getPosition() - primes[uniqueSubbranchPositionCount - i - 1]->getPosition() - 1);
                blackSuffixes[i]->build();
            }

            // Белые префиксы.
            whitePrefixes = new PrefixSubbranchColoringInfo*[uniqueSubbranchPositionCount];
            whitePrefixes[0] = new PrefixSubbranchColoringInfo(WHITE, 0, primes[0], primes[0]->getPosition());
            whitePrefixes[0]->build();

            for (int i = 1; i < uniqueSubbranchPositionCount; ++i) {
                whitePrefixes[i] = new PrefixSubbranchColoringInfo(WHITE, whitePrefixes[i - 1], primes[i], primes[i]->getPosition() - primes[i - 1]->getPosition() - 1);
                whitePrefixes[i]->build();
            }

            // Белые суффиксы.
            whiteSuffixes = new SuffixSubbranchColoringInfo*[uniqueSubbranchPositionCount];
            whiteSuffixes[0] = new SuffixSubbranchColoringInfo(BLACK, 0, primes[uniqueSubbranchPositionCount - 1], trunkLength - primes[uniqueSubbranchPositionCount - 1]->getPosition() - 1);
            whiteSuffixes[0]->build();

            for (int i = 1; i < uniqueSubbranchPositionCount; ++i) {
                whiteSuffixes[i] = new SuffixSubbranchColoringInfo(BLACK, whiteSuffixes[i - 1], primes[uniqueSubbranchPositionCount - i - 1], primes[uniqueSubbranchPositionCount - i]->getPosition() - primes[uniqueSubbranchPositionCount - i - 1]->getPosition() - 1);
                whiteSuffixes[i]->build();
            }

            // Чёрные префиксы, белые суффиксы.
            // Белые префиксы, чёрные суффиксы.
            // Чёрные префиксы, чёрные суффиксы.
            // Белые префиксы, белые суффиксы.
            bw = new PrefixSuffixSubbranchColoringInfo*[uniqueSubbranchPositionCount - 1];
            wb = new PrefixSuffixSubbranchColoringInfo*[uniqueSubbranchPositionCount - 1];
            bb = new PrefixSuffixSubbranchColoringInfo*[uniqueSubbranchPositionCount - 1];
            ww = new PrefixSuffixSubbranchColoringInfo*[uniqueSubbranchPositionCount - 1];

            for (int i = 0; i < uniqueSubbranchPositionCount - 1; ++i) {
                bw[i] = new PrefixSuffixSubbranchColoringInfo(BLACK, WHITE, blackPrefixes[i], whiteSuffixes[uniqueSubbranchPositionCount - i - 2]);
                bw[i]->build();

                wb[i] = new PrefixSuffixSubbranchColoringInfo(WHITE, BLACK, whitePrefixes[i], blackSuffixes[uniqueSubbranchPositionCount - i - 2]);
                wb[i]->build();

                bb[i] = new PrefixSuffixSubbranchColoringInfo(BLACK, BLACK, blackPrefixes[i], blackSuffixes[uniqueSubbranchPositionCount - i - 2]);
                bb[i]->build();

                ww[i] = new PrefixSuffixSubbranchColoringInfo(WHITE, WHITE, whitePrefixes[i], whiteSuffixes[uniqueSubbranchPositionCount - i - 2]);
                ww[i]->build();
            }
        }

        void buildCompositeMaxWhite() {
            int trunkLength = branch->getTrunkLength();
            int branchNodeCount = branch->getNodeCount();
            int uniqueSubbranchPositionCount = branch->getUniqueSubbranchPositionCount();
            int blackCount;
            int maxWhite;
            ColorType firstTrunkNodeColor;

            maxWhiteMap = new BranchMaxWhiteMap(branchNodeCount);

            if (uniqueSubbranchPositionCount == 1) {
                PrimeSubbranchColoringInfo* prime = primes[0];
                int primeNodeCount = prime->getNodeCount();
                int primePosition = prime->getPosition();

                for (int b = 0; b <= primeNodeCount; ++b) {
                    // Все узлы чёрные.
                    blackCount = b + trunkLength - 1;
                    maxWhite = prime->getMaxWhite(BLACK, b);

                    if (maxWhite != INVALID_MAX_WHITE && maxWhite > maxWhiteMap->get(BLACK, blackCount).getMaxWhite()) {
                        maxWhiteMap->put(BLACK, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::ALL_BLACK, NO_GRAY, BLACK)));
                    }

                    // Все узлы белые.
                    blackCount = b;
                    maxWhite = prime->getMaxWhite(WHITE, b);

                    if (maxWhite != INVALID_MAX_WHITE && maxWhite + trunkLength - 1 > maxWhiteMap->get(WHITE, blackCount).getMaxWhite()) {
                        maxWhiteMap->put(WHITE, blackCount, BranchMaxWhiteMapElement(maxWhite + trunkLength - 1, BranchTrunkColoring(BranchTrunkColoring::ALL_WHITE, NO_GRAY, WHITE)));
                    }

                    // Обрабатываем остальные случаи.
                    for (int g = 0; g < trunkLength; ++g) {
                        // Чёрные узлы сверху от серого, белые - снизу.
                        if (primePosition == g && (maxWhite = prime->getMaxWhite(GRAY, b)) != INVALID_MAX_WHITE) {
                            blackCount = b + g;
                            maxWhite += trunkLength - g - 1;
                        }
                        else if (primePosition < g && (maxWhite = std::max(prime->getMaxWhite(BLACK, b), prime->getMaxWhite(GRAY, b))) != INVALID_MAX_WHITE) {
                            blackCount = b + g - 1;
                            maxWhite += trunkLength - g - 1;
                        }
                        else if (primePosition > g && (maxWhite = std::max(prime->getMaxWhite(WHITE, b), prime->getMaxWhite(GRAY, b))) != INVALID_MAX_WHITE) {
                            blackCount = b + g;
                            maxWhite += trunkLength - g - 2;
                        }

                        firstTrunkNodeColor = (g == 0) ? GRAY : BLACK;

                        if (maxWhite != INVALID_MAX_WHITE && maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                            maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, g, firstTrunkNodeColor)));
                        }

                        if (g > 0 && maxWhite != INVALID_MAX_WHITE) {
                            --blackCount;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhite != INVALID_MAX_WHITE && maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, g, firstTrunkNodeColor)));
                            }
                        }


                        // Белые узлы сверху от серого, чёрные - снизу.
                        if (primePosition == g && (maxWhite = prime->getMaxWhite(GRAY, b)) != INVALID_MAX_WHITE) {
                            blackCount = b + trunkLength - g - 1;
                            maxWhite += g;
                        }
                        else if (primePosition < g && (maxWhite = std::max(prime->getMaxWhite(WHITE, b), prime->getMaxWhite(GRAY, b))) != INVALID_MAX_WHITE) {
                            blackCount = b + trunkLength - g - 1;
                            maxWhite += g - 1;
                        }
                        else if (primePosition > g && (maxWhite = std::max(prime->getMaxWhite(BLACK, b), prime->getMaxWhite(GRAY, b))) != INVALID_MAX_WHITE) {
                            blackCount = b + trunkLength - g - 2;
                            maxWhite += g;
                        }

                        firstTrunkNodeColor = (g == 0) ? GRAY : WHITE;

                        if (maxWhite != INVALID_MAX_WHITE && maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                            maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, g, firstTrunkNodeColor)));
                        }

                        if (g > 0 && maxWhite != INVALID_MAX_WHITE) {
                            --maxWhite;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhite != INVALID_MAX_WHITE && maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, g, firstTrunkNodeColor)));
                            }
                        }

                        // Чёрные узлы и сверху и снизу от серого.
                        if (primePosition == g && (maxWhite = prime->getMaxWhite(GRAY, b)) != INVALID_MAX_WHITE) {
                            blackCount = b + trunkLength - 1;
                        }
                        else if (primePosition != g && (maxWhite = std::max(prime->getMaxWhite(BLACK, b), prime->getMaxWhite(GRAY, b))) != INVALID_MAX_WHITE) {
                            blackCount = b + trunkLength - 2;
                        }

                        firstTrunkNodeColor = (g == 0) ? GRAY : BLACK;

                        if (maxWhite != INVALID_MAX_WHITE && maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite)
                            maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));

                        if (g > 0 && maxWhite != INVALID_MAX_WHITE) {
                            --blackCount;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhite != INVALID_MAX_WHITE && maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));
                            }
                        }

                        // Белые узлы и сверху и снизу от серого.
                        blackCount = b;

                        if (primePosition == g && (maxWhite = prime->getMaxWhite(GRAY, b)) != INVALID_MAX_WHITE) {
                            maxWhite += trunkLength - 1;
                        }
                        else if (primePosition != g && (maxWhite = std::max(prime->getMaxWhite(WHITE, b), prime->getMaxWhite(GRAY, b))) != INVALID_MAX_WHITE) {
                            maxWhite += trunkLength - 2;
                        }

                        firstTrunkNodeColor = (g == 0) ? GRAY : WHITE;

                        if (maxWhite != INVALID_MAX_WHITE && maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                            maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));
                        }

                        if (g > 0 && maxWhite != INVALID_MAX_WHITE) {
                            --maxWhite;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhite != INVALID_MAX_WHITE && maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));
                            }
                        }
                    }
                }
            }
            else {
                int firstPrimePosition = primes[0]->getPosition();
                int lastPrimePosition = primes[uniqueSubbranchPositionCount - 1]->getPosition();

                // Все узлы чёрные.
                PrefixSubbranchColoringInfo* lastPrefix = blackPrefixes[uniqueSubbranchPositionCount - 1];
                int lastPrefixNodeCount = lastPrefix->getNodeCount();

                for (int b = 0; b <= lastPrefixNodeCount; ++b) {
                    blackCount = b + trunkLength - lastPrimePosition - 1;
                    maxWhite = lastPrefix->getMaxWhite(false, b);

                    if (maxWhite != INVALID_MAX_WHITE && maxWhite > maxWhiteMap->get(BLACK, blackCount).getMaxWhite()) {
                        maxWhiteMap->put(BLACK, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::ALL_BLACK, NO_GRAY, BLACK)));
                    }
                }

                // Все узлы белые.
                lastPrefix = whitePrefixes[uniqueSubbranchPositionCount - 1];
                lastPrefixNodeCount = lastPrefix->getNodeCount();

                for (int b = 0; b <= lastPrefixNodeCount; ++b) {
                    blackCount = b;
                    maxWhite = lastPrefix->getMaxWhite(false, b);

                    if (maxWhite != INVALID_MAX_WHITE && (maxWhite + trunkLength - lastPrimePosition - 1 > maxWhiteMap->get(WHITE, blackCount).getMaxWhite())) {
                        maxWhiteMap->put(WHITE, blackCount, BranchMaxWhiteMapElement(maxWhite + trunkLength - lastPrimePosition - 1, BranchTrunkColoring(BranchTrunkColoring::ALL_WHITE, NO_GRAY, WHITE)));
                    }
                }

                // Серый узел лежит в верхнем хвосте ветви.
                SuffixSubbranchColoringInfo* lastBlackSuffix = blackSuffixes[uniqueSubbranchPositionCount - 1];
                int lastSuffixNodeCount = lastBlackSuffix->getNodeCount();
                SuffixSubbranchColoringInfo* lastWhiteSuffix = whiteSuffixes[uniqueSubbranchPositionCount - 1];

                for (int g = 0; g < firstPrimePosition; ++g) {
                    for (int b = 0; b <= lastSuffixNodeCount; ++b) {
                        // Чёрные узлы сверху от серого, белые - снизу.
                        if ((maxWhite = lastWhiteSuffix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                            blackCount = b + g;
                            maxWhite += firstPrimePosition - g - 1;
                            firstTrunkNodeColor = (g == 0) ? GRAY : BLACK;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, g, firstTrunkNodeColor)));
                            }

                            if (g > 0) {
                                --blackCount;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, g, firstTrunkNodeColor)));
                                }
                            }
                        }

                        // Белые узлы сверху от серого, чёрные - снизу.
                        if ((maxWhite = lastBlackSuffix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                            blackCount = b + firstPrimePosition - g - 1;
                            maxWhite += g;
                            firstTrunkNodeColor = (g == 0) ? GRAY : WHITE;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, g, firstTrunkNodeColor)));
                            }

                            if (g > 0) {
                                --maxWhite;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, g, firstTrunkNodeColor)));
                                }
                            }
                        }

                        // Чёрные узлы и сверху и снизу от серого.
                        if ((maxWhite = lastBlackSuffix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                            blackCount = b + firstPrimePosition - 1;
                            firstTrunkNodeColor = (g == 0) ? GRAY : BLACK;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));
                            }

                            if (g > 0) {
                                --blackCount;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));
                                }
                            }
                        }

                        // Случай, когда белые узлы и сверху и снизу от серого, даст заведомо неоптимальное значение,
                        // поэтому мы его не обрабатываем.
                    }
                }

                // Серый узел лежит в нижнем хвосте ветви.
                PrefixSubbranchColoringInfo* lastBlackPrefix = blackPrefixes[uniqueSubbranchPositionCount - 1];
                lastPrefixNodeCount = lastBlackPrefix->getNodeCount();
                PrefixSubbranchColoringInfo* lastWhitePrefix = whitePrefixes[uniqueSubbranchPositionCount - 1];

                for (int g = lastPrimePosition + 1; g < trunkLength; ++g) {
                    for (int b = 0; b <= lastPrefixNodeCount; ++b) {
                        // Чёрные узлы сверху от серого, белые - снизу.
                        if ((maxWhite = lastBlackPrefix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                            blackCount = b + g - lastPrimePosition - 1;
                            maxWhite += trunkLength - g - 1;
                            firstTrunkNodeColor = BLACK;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, g, firstTrunkNodeColor)));
                            }

                            --blackCount;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, g, firstTrunkNodeColor)));
                            }
                        }

                        // Белые узлы сверху от серого, чёрные - снизу.
                        if ((maxWhite = lastWhitePrefix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                            blackCount = b + trunkLength - g - 1;
                            maxWhite += g - lastPrimePosition - 1;
                            firstTrunkNodeColor = WHITE;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, g, firstTrunkNodeColor)));
                            }

                            --maxWhite;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, g, firstTrunkNodeColor)));
                            }
                        }

                        // Чёрные узлы и сверху и снизу от серого.
                        if ((maxWhite = lastBlackPrefix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                            blackCount = b + trunkLength - lastPrimePosition - 2;
                            firstTrunkNodeColor = BLACK;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));
                            }

                            --blackCount;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, g, firstTrunkNodeColor)));
                            }
                        }

                        // Случай, когда белые узлы и сверху и снизу от серого, даст заведомо неоптимальное значение,
                        // поэтому мы его не обрабатываем.
                    }
                }

                // Обрабатываем случаи, когда серый узел не лежит в хвосте ветви.
                for (int i = 0; i < uniqueSubbranchPositionCount - 1; ++i) {
                    int distance = primes[i + 1]->getPosition() - primes[i]->getPosition() - 1;
                    int prefixSuffixNodeCount = bw[i]->getNodeCount();
                    PrefixSuffixSubbranchColoringInfo* blackPrefixWhiteSuffix = bw[i];
                    PrefixSuffixSubbranchColoringInfo* whitePrefixBlackSuffix = wb[i];
                    PrefixSuffixSubbranchColoringInfo* blackPrefixBlackSuffix = bb[i];
                    PrefixSuffixSubbranchColoringInfo* whitePrefixWhiteSuffix = ww[i];

                    for (int b = 0; b <= prefixSuffixNodeCount; ++b) {
                        // Чёрные узлы сверху от серого, белые - снизу.
                        if ((maxWhite = std::max(blackPrefixWhiteSuffix->getMaxWhite(true, true, b), blackPrefixWhiteSuffix->getMaxWhite(true, false, b))) != INVALID_MAX_WHITE) {
                            blackCount = b;
                            maxWhite += distance;
                            firstTrunkNodeColor = BLACK;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, primes[i]->getPosition(), firstTrunkNodeColor)));
                            }

                            --blackCount;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, primes[i]->getPosition(), firstTrunkNodeColor)));
                            }
                        }

                        // Белые узлы сверху от серого, чёрные - снизу.
                        if ((maxWhite = std::max(whitePrefixBlackSuffix->getMaxWhite(true, true, b), whitePrefixBlackSuffix->getMaxWhite(true, false, b))) != INVALID_MAX_WHITE) {
                            blackCount = b + distance;
                            firstTrunkNodeColor = WHITE;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, primes[i]->getPosition(), firstTrunkNodeColor)));
                            }

                            --maxWhite;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, primes[i]->getPosition(), firstTrunkNodeColor)));
                            }
                        }

                        // Чёрные узлы и сверху и снизу от серого.
                        if ((maxWhite = std::max(blackPrefixBlackSuffix->getMaxWhite(true, true, b), blackPrefixBlackSuffix->getMaxWhite(true, false, b))) != INVALID_MAX_WHITE) {
                            blackCount = b + distance;
                            firstTrunkNodeColor = BLACK;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, primes[i]->getPosition(), firstTrunkNodeColor)));
                            }

                            --blackCount;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, primes[i]->getPosition(), firstTrunkNodeColor)));
                            }
                        }

                        // Белые узлы и сверху и снизу от серого.
                        if ((maxWhite = std::max(whitePrefixWhiteSuffix->getMaxWhite(true, true, b), whitePrefixWhiteSuffix->getMaxWhite(true, false, b))) != INVALID_MAX_WHITE) {
                            blackCount = b;
                            maxWhite += distance;
                            firstTrunkNodeColor = WHITE;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE_AND_BELOW, primes[i]->getPosition(), firstTrunkNodeColor)));
                            }

                            --maxWhite;
                            firstTrunkNodeColor = GRAY;

                            if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE_AND_BELOW, primes[i]->getPosition(), firstTrunkNodeColor)));
                            }
                        }

                        for (int j = 0; j < distance; ++j) {
                            // Чёрные узлы сверху от серого, белые - снизу.
                            if ((maxWhite = blackPrefixWhiteSuffix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                                blackCount = b + j;
                                maxWhite += distance - j - 1;
                                firstTrunkNodeColor = BLACK;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, primes[i]->getPosition() + j + 1, firstTrunkNodeColor)));
                                }

                                --blackCount;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, primes[i]->getPosition() + j + 1, firstTrunkNodeColor)));
                                }
                            }

                            // Белые узлы сверху от серого, чёрные - снизу.
                            if ((maxWhite = whitePrefixBlackSuffix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                                blackCount = b + distance - j - 1;
                                maxWhite += j;
                                firstTrunkNodeColor = WHITE;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, primes[i]->getPosition() + j + 1, firstTrunkNodeColor)));
                                }

                                --maxWhite;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, primes[i]->getPosition() + j + 1, firstTrunkNodeColor)));
                                }
                            }

                            // Чёрные узлы и сверху и снизу от серого.
                            if ((maxWhite = blackPrefixBlackSuffix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                                blackCount = b + distance - 1;
                                firstTrunkNodeColor = BLACK;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, primes[i]->getPosition() + j + 1, firstTrunkNodeColor)));
                                }

                                --blackCount;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, primes[i]->getPosition() + j + 1, firstTrunkNodeColor)));
                                }
                            }

                            // Белые узлы и сверху и снизу от серого.
                            if ((maxWhite = whitePrefixWhiteSuffix->getMaxWhite(b)) != INVALID_MAX_WHITE) {
                                blackCount = b;
                                maxWhite += distance - 1;
                                firstTrunkNodeColor = WHITE;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE_AND_BELOW, primes[i]->getPosition() + j + 1, firstTrunkNodeColor)));
                                }

                                --maxWhite;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE_AND_BELOW, primes[i]->getPosition() + j + 1, firstTrunkNodeColor)));
                                }
                            }
                        }
                    }

                    if (i == uniqueSubbranchPositionCount - 2) {
                        for (int b = 0; b <= prefixSuffixNodeCount; ++b) {
                            // Чёрные узлы сверху от серого, белые - снизу.
                            if ((maxWhite = std::max(blackPrefixWhiteSuffix->getMaxWhite(true, true, b), blackPrefixWhiteSuffix->getMaxWhite(false, true, b))) != INVALID_MAX_WHITE) {
                                blackCount = b + distance;
                                firstTrunkNodeColor = BLACK;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, lastPrimePosition, firstTrunkNodeColor)));
                                }

                                --blackCount;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE, lastPrimePosition, firstTrunkNodeColor)));
                                }
                            }

                            // Белые узлы сверху от серого, чёрные - снизу.
                            if ((maxWhite = std::max(whitePrefixBlackSuffix->getMaxWhite(true, true, b), whitePrefixBlackSuffix->getMaxWhite(false, true, b))) != INVALID_MAX_WHITE) {
                                blackCount = b;
                                maxWhite += distance;
                                firstTrunkNodeColor = WHITE;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, lastPrimePosition, firstTrunkNodeColor)));
                                }

                                --maxWhite;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE, lastPrimePosition, firstTrunkNodeColor)));
                                }
                            }

                            // Чёрные узлы и сверху и снизу от серого.
                            if ((maxWhite = std::max(blackPrefixBlackSuffix->getMaxWhite(true, true, b), blackPrefixBlackSuffix->getMaxWhite(false, true, b))) != INVALID_MAX_WHITE) {
                                blackCount = b + distance;
                                firstTrunkNodeColor = BLACK;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, lastPrimePosition, firstTrunkNodeColor)));
                                }

                                --blackCount;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::BLACK_ABOVE_AND_BELOW, lastPrimePosition, firstTrunkNodeColor)));
                                }
                            }

                            // Белые узлы и сверху и снизу от серого.
                            if ((maxWhite = std::max(whitePrefixWhiteSuffix->getMaxWhite(true, true, b), whitePrefixWhiteSuffix->getMaxWhite(false, true, b))) != INVALID_MAX_WHITE) {
                                blackCount = b;
                                maxWhite += distance;
                                firstTrunkNodeColor = WHITE;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE_AND_BELOW, lastPrimePosition, firstTrunkNodeColor)));
                                }

                                --maxWhite;
                                firstTrunkNodeColor = GRAY;

                                if (maxWhiteMap->get(firstTrunkNodeColor, blackCount).getMaxWhite() < maxWhite) {
                                    maxWhiteMap->put(firstTrunkNodeColor, blackCount, BranchMaxWhiteMapElement(maxWhite, BranchTrunkColoring(BranchTrunkColoring::WHITE_ABOVE_AND_BELOW, lastPrimePosition, firstTrunkNodeColor)));
                                }
                            }
                        }
                    }
                }
            }
        }

    private:

        // Ветвь.
        Branch* branch;

        // Отображение Max_White.
        BranchMaxWhiteMap* maxWhiteMap;

        // Информация о раскраске подветвей.
        PrimeSubbranchColoringInfo** primes;

        // Чёрные префиксы.
        PrefixSubbranchColoringInfo** blackPrefixes;

        // Чёрные суффиксы.
        SuffixSubbranchColoringInfo** blackSuffixes;

        // Белые префиксы.
        PrefixSubbranchColoringInfo** whitePrefixes;

        // Белые суффиксы.
        SuffixSubbranchColoringInfo** whiteSuffixes;

        // Чёрные префиксы, белые суффиксы.
        PrefixSuffixSubbranchColoringInfo** bw;

        // Белые префиксы, чёрные суффиксы.
        PrefixSuffixSubbranchColoringInfo** wb;

        // Чёрные префиксы, чёрные суффиксы.
        PrefixSuffixSubbranchColoringInfo** bb;

        // Белые префиксы, белые суффиксы.
        PrefixSuffixSubbranchColoringInfo** ww;
    };

    /**
     * Информация о раскраске подветви.
     */
    class SubbranchColoringInfo {

    public:

        virtual void build() = 0;

        virtual int getNodeCount() = 0;

        virtual int getMaxWhite(const int blackCount) const = 0;

        virtual ~SubbranchColoringInfo() {}
    };

    /**
     * Информация о раскраске совокупности простых подветвей, у которых совпадают начальные узлы стволов.
     * Простой подветвью называется подветвь, у которой есть ровно один узел, принадлежащий стволу родительской ветви.
     */
    class PrimeSubbranchColoringInfo : public SubbranchColoringInfo {

    public:

        PrimeSubbranchColoringInfo(const int newPosition) : position(newPosition) {}

        int getPosition() const {
            return position;
        }

        virtual int getMaxWhite(const ColorType color, const int blackCount) const = 0;

        int getMaxWhite(const int blackCount) const {
            return std::max(std::max(getMaxWhite(BLACK, blackCount), getMaxWhite(WHITE, blackCount)), getMaxWhite(GRAY, blackCount));
        }

        virtual void color(
            ColoringType* coloring,
            Branch* branch,
            const int blackCount,
            const int whiteCount,
            const ColorType acceptableFirstNodeColors,
            const BranchTrunkColoring& trunkColoring) = 0;

        virtual ~PrimeSubbranchColoringInfo() {}

    protected:

        // Позиция первого узла в стволе родительской ветви.
        int position;
    };

    /**
     * Информация о раскраске одной простой подветви.
     */
    class SingleSubbranchColoringInfo : public PrimeSubbranchColoringInfo {

    public:

        SingleSubbranchColoringInfo(const SubbranchInfo& subbranchInfo) :
            PrimeSubbranchColoringInfo(subbranchInfo.second),
            subbranch(subbranchInfo.first),
            info(0) {}

        BranchColoringInfo* getSubbranchColoringInfo() const {
            return info;
        }

        void build() {
            info = new BranchColoringInfo(subbranch);
            info->build();
        }

        int getNodeCount() {
            return subbranch->getNodeCount();
        }

        int getMaxWhite(const ColorType color, const int blackCount) const {
            return info->getMaxWhite(color, blackCount);
        }

        BranchMaxWhiteMap* getMaxWhiteMap() const {
            return info->getMaxWhiteMap();
        }

        void color(
            ColoringType* coloring,
            Branch* branch,
            const int blackCount,
            const int whiteCount,
            const ColorType acceptableFirstNodeColors,
            const BranchTrunkColoring& trunkColoring) {

            ColorType firstNodeColor = GRAY;

            if (acceptableFirstNodeColors == BLACK && getMaxWhite(BLACK, blackCount) > getMaxWhite(GRAY, blackCount)) {
                firstNodeColor = BLACK;
            }
            else if (acceptableFirstNodeColors == WHITE && getMaxWhite(WHITE, blackCount) > getMaxWhite(GRAY, blackCount)) {
                firstNodeColor = WHITE;
            }

            info->color(coloring, blackCount, whiteCount, firstNodeColor);
        }

        ~SingleSubbranchColoringInfo() {
            if (info != 0) {
                delete info;
            }
        }

    private:

        // Подветвь.
        Branch* subbranch;

        // Информация о раскраске подветви.
        BranchColoringInfo* info;
    };

    /**
     * Информация о раскраске совокупности нескольких простых подветвей,
     * у которых совпадают начальные узлы стволов.
     */
    class TreeSubbranchColoringInfo : public PrimeSubbranchColoringInfo {

    public:

        TreeSubbranchColoringInfo(
            Branch* newBranch,
            const int newLeft,
            const int newRight) :

            PrimeSubbranchColoringInfo(newBranch->getSubbranchInfo(newLeft).second),
            branch(newBranch),
            left(newLeft),
            right(newRight),
            primes(0),
            nodeCount(INVALID_NODE_COUNT),
            maxWhiteMap(0),
            fusionStack(0),
            topFusion(0) {

            primeCount = getPrimeCount();
        }

        int getBranch() const {
            return branch;
        }

        int getLeft() const {
            return left;
        }

        int getRight() const {
            return right;
        }

        void build() {
            primes = new BranchColoringInfo*[primeCount];

            for (int i = 0; i < primeCount; ++i) {
                primes[i] = new BranchColoringInfo(getSubbranch(i));
                primes[i]->build();
            }

            std::pair<Gray2BlackListMap*, Fusion*>* currentMerging = new std::pair<Gray2BlackListMap*, Fusion*>[primeCount];

            for (int i = 0; i < primeCount; ++i) {
                currentMerging[i] =
                    std::pair<Gray2BlackListMap*, Fusion*>(primes[i]->getMaxWhiteMap()->toGray2BlackListMap(), new SingleChildFusion(primes[i]));
            }

            Gray2BlackListMap* topGray2BlackListMap = merge(fusionStack, topFusion, getNodeCount(), currentMerging, 0, primeCount - 1).first;
            maxWhiteMap = new ColorMaxWhiteMap(topGray2BlackListMap);
            delete topGray2BlackListMap;
            delete currentMerging;
        }

        int getNodeCount() {
            if (nodeCount != INVALID_NODE_COUNT) {
                return nodeCount;
            }

            nodeCount = 0;

            for (int i = 0; i < primeCount; ++i) {
                nodeCount += getSubbranch(i)->getNodeCount();
            }

            return nodeCount -= right - left;
        }

        int getMaxWhite(const ColorType color, const int blackCount) const {
            return maxWhiteMap->get(color, blackCount);
        }

        ColorMaxWhiteMap* getMaxWhiteMap() const {
            return maxWhiteMap;
        }

        void color(
            ColoringType* coloring,
            Branch* branch,
            const int blackCount,
            const int whiteCount,
            const ColorType acceptableFirstNodeColors,
            const BranchTrunkColoring& trunkColoring) {

            this->coloring = coloring;
            this->acceptableFirstNodeColors = acceptableFirstNodeColors;

            nodeCount = getNodeCount();
            int grayCount = nodeCount - (blackCount + whiteCount);
            findColoring(blackCount, grayCount);
        }

        ~TreeSubbranchColoringInfo() {
            if (primes != 0) {
                for (int i = 0; i < primeCount; ++i) {
                    delete primes[i];
                }

                delete primes;
            }

            delete maxWhiteMap;
            delete fusionStack;
            delete topFusion;
            topFusion = 0;
        }

        static Gray2BlackListMap* createUnion(const int nodeCount, Gray2BlackListMap* first, Gray2BlackListMap* second) {
            int nodeCountLog2 = (int)log2(nodeCount);
            int maxBlackCountUp = nodeCount + 2;
            int maxGrayCountUp = nodeCountLog2 + 4;

            Gray2BlackListMap* result = new Gray2BlackListMap(maxGrayCountUp);

            int blackCount2GrayCountLength = maxBlackCountUp * maxGrayCountUp;
            bool* blackCount2GrayCount = new bool[blackCount2GrayCountLength];

            std::vector<int> sums;
            std::vector<int>::iterator sumIter;

            int* minGrayCount = new int[maxBlackCountUp];

            // Чёрный цвет.
            for (int i = 0; i < blackCount2GrayCountLength; ++i) {
                blackCount2GrayCount[i] = false;
            }

            for (int b = 0; b < maxBlackCountUp; ++b) {
                minGrayCount[b] = -1;
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (first->get(BLACK, j) == 0 || second->get(BLACK, i - j) == 0) {
                        continue;
                    }

                    sums.clear();
                    Summator::computePairwiseSums(first->get(BLACK, j), second->get(BLACK, i - j), sums);

                    for (sumIter = sums.begin(); sumIter != sums.end(); ++sumIter) {
                        if (minGrayCount[*sumIter] == -1 || minGrayCount[*sumIter] > i) {
                            blackCount2GrayCount[(*sumIter) * maxGrayCountUp + i] = true;
                            minGrayCount[*sumIter] = i;
                        }
                    }
                }
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j < maxBlackCountUp; ++j) {
                    if (blackCount2GrayCount[j * maxGrayCountUp + i]) {
                        result->put(BLACK, i, j - 1);
                    }
                }
            }

            // Белый цвет.
            for (int i = 0; i < blackCount2GrayCountLength; ++i) {
                blackCount2GrayCount[i] = false;
            }

            for (int b = 0; b < maxBlackCountUp; ++b) {
                minGrayCount[b] = -1;
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (first->get(WHITE, j) == 0 || second->get(WHITE, i - j) == 0) {
                        continue;
                    }

                    sums.clear();
                    Summator::computePairwiseSums(first->get(WHITE, j), second->get(WHITE, i - j), sums);

                    for (sumIter = sums.begin(); sumIter != sums.end(); ++sumIter) {
                        if (minGrayCount[*sumIter] == -1 || minGrayCount[*sumIter] > i) {
                            blackCount2GrayCount[(*sumIter) * maxGrayCountUp + i] = true;
                            minGrayCount[*sumIter] = i;
                        }
                    }
                }
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j < maxBlackCountUp; ++j) {
                    if (blackCount2GrayCount[j * maxGrayCountUp + i]) {
                        result->put(WHITE, i, j);
                    }
                }
            }

            // Серый цвет.
            for (int i = 0; i < blackCount2GrayCountLength; ++i) {
                blackCount2GrayCount[i] = false;
            }

            for (int b = 0; b < maxBlackCountUp; ++b) {
                minGrayCount[b] = -1;
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (first->get(GRAY, j) == 0 || second->get(GRAY, i - j) == 0) {
                        continue;
                    }

                    sums.clear();
                    Summator::computePairwiseSums(first->get(GRAY, j), second->get(GRAY, i - j), sums);

                    for (sumIter = sums.begin(); sumIter != sums.end(); ++sumIter) {
                        if (minGrayCount[*sumIter] == -1 || minGrayCount[*sumIter] > i) {
                            blackCount2GrayCount[(*sumIter) * maxGrayCountUp + i] = true;
                            minGrayCount[*sumIter] = i;
                        }
                    }
                }
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j < maxBlackCountUp; ++j) {
                    if (blackCount2GrayCount[j * maxGrayCountUp + i]) {
                        result->put(GRAY, i - 1, j);
                    }
                }
            }

            delete[] blackCount2GrayCount;
            delete[] minGrayCount;

            return result;
        }

        static std::pair<Gray2BlackListMap*, Fusion*> merge(const int nodeCount, const std::pair<Gray2BlackListMap*, Fusion*>& left, const std::pair<Gray2BlackListMap*, Fusion*>& right) {
            Gray2BlackListMap* unitedGray2BlackListMap = createUnion(nodeCount, left.first, right.first);
            delete left.first;
            delete right.first;

            return std::pair<Gray2BlackListMap*, Fusion*>(unitedGray2BlackListMap, new MultiChildFusion(unitedGray2BlackListMap, left.second, right.second));
        }

        static std::pair<Gray2BlackListMap*, Fusion*> merge(FusionStack*& fusionStack, Fusion*& topFusion, const int nodeCount, std::pair<Gray2BlackListMap*, Fusion*>* currentMerging, const int l, const int r) {
            if (l == r) {
                return currentMerging[l];
            }

            if (r - l == 1) {
                std::pair<Gray2BlackListMap*, Fusion*> result = merge(nodeCount, currentMerging[l], currentMerging[r]);
                pushFusion(fusionStack, topFusion, result.second);

                return result;
            }
            else {
                std::pair<Gray2BlackListMap*, Fusion*> leftMerging = merge(fusionStack, topFusion, nodeCount, currentMerging, l, (l + r) / 2);
                std::pair<Gray2BlackListMap*, Fusion*> rightMerging = merge(fusionStack, topFusion, nodeCount, currentMerging, (l + r) / 2 + 1, r);

                std::pair<Gray2BlackListMap*, Fusion*> result = merge(nodeCount, leftMerging, rightMerging);
                pushFusion(fusionStack, topFusion, result.second);

                return result;
            }
        }

    private:

        int getPrimeCount() const {
            return right - left + 1;
        }

        Branch* getSubbranch(const int i) const {
            return branch->getSubbranchInfo(left + i).first;
        }

        int getSubbranchPosition(const int i) const {
            return branch->getSubbranchInfo(left + i).second;
        }

        void pushFusion(Fusion* fusion) {
            if (fusionStack == 0) {
                fusionStack = new FusionStack();
            }

            fusionStack->push(fusion);
            topFusion = fusion;
        }

        static void pushFusion(FusionStack*& fusionStack, Fusion*& topFusion, Fusion* fusion) {
            if (fusionStack == 0) {
                fusionStack = new FusionStack();
            }

            fusionStack->push(fusion);
            topFusion = fusion;
        }

        Fusion* getTopFusion() const {
            return !fusionStack->empty() ? fusionStack->top() : 0;
        }

        static Fusion* getTopFusion(FusionStack*& fusionStack) {
            return !fusionStack->empty() ? fusionStack->top() : 0;
        }

        void popFusion() {
            if (fusionStack != 0 && !fusionStack->empty()) {
                fusionStack->pop();
            }
        }

        void findColoring(const int blackCount, const int grayCount) {
            findColoring(
                0,
                primeCount - 1,
                blackCount,
                grayCount,
                0,
                false);
        }

        void findColoring(const int first, const int last, int blackCount, int grayCount, Fusion* fusion, const bool isFirstNodeColored) {
            ColorType firstNodeColor;

            if (isFirstNodeColored) {
                firstNodeColor = coloring->getColor(primes[0]->getBranch()->getFirstTrunkNode());
            }
            else {
                firstNodeColor = GRAY;

                if (acceptableFirstNodeColors == BLACK && getMaxWhite(BLACK, blackCount) > getMaxWhite(GRAY, blackCount)) {
                    firstNodeColor = BLACK;
                }
                else if (acceptableFirstNodeColors == WHITE && getMaxWhite(WHITE, blackCount) > getMaxWhite(GRAY, blackCount)) {
                    firstNodeColor = WHITE;
                }

                coloring->color(primes[0]->getBranch()->getFirstTrunkNode(), firstNodeColor);
            }

            if (last - first == 0) {
                primes[first]->color(
                    coloring,
                    blackCount,
                    primes[first]->getBranch()->getNodeCount() - (blackCount + grayCount),
                    firstNodeColor);

                return;
            }

            if (fusion == 0) {
                fusion = getTopFusion();
                popFusion();
            }

            if (firstNodeColor == BLACK) {
                ++blackCount;
            }
            else if (firstNodeColor == GRAY) {
                ++grayCount;
            }

            Fusion* leftFusion = fusion->getLeftFusion();
            Fusion* rightFusion = fusion->getRightFusion();

            for (int b = 0; b <= blackCount; ++b) {
                if (
                   (leftFusion->getMinGrayCount(firstNodeColor, b) != INVALID_MIN_GRAY) &&
                   (rightFusion->getMinGrayCount(firstNodeColor, blackCount - b) != INVALID_MIN_GRAY) &&
                   (leftFusion->getMinGrayCount(firstNodeColor, b) + rightFusion->getMinGrayCount(firstNodeColor, blackCount - b) <= grayCount)) {

                    findColoring(
                        first,
                        (first + last) / 2,
                        b,
                        leftFusion->getMinGrayCount(firstNodeColor, b),
                        leftFusion,
                        true);

                    findColoring(
                        (first + last) / 2 + 1,
                        last,
                        blackCount - b,
                        rightFusion->getMinGrayCount(firstNodeColor, blackCount - b),
                        rightFusion,
                        true);

                    break;
                }
            }
        }

    private:

        // Ветвь.
        Branch* branch;

        // Границы диапазона образующих простых подветвей.
        int left;
        int right;

        // Информация о раскраске образующих подветвей.
        BranchColoringInfo** primes;

        // Количество образующих подветвей.
        int primeCount;

        // Количество узлов.
        int nodeCount;

        // Отображение Max_White.
        ColorMaxWhiteMap* maxWhiteMap;

        // Стек объединений отображений Max_White.
        FusionStack* fusionStack;

        // Объединение верхнего уровня.
        Fusion* topFusion;

        // Раскраска дерева.
        ColoringType* coloring;

        // Допустимые цвета начального узла стволов подветвей.
        ColorType acceptableFirstNodeColors;
    };

    /**
     * Информация о раскраске составной подветви.
     * Cоставной подветвью называется объединение одной или нескольких простых подветвей.
     * При этом по крайней мере некоторые из образующих простых подветвей должны иметь различные начальные узлы стволов.
     */
    class CompositeSubbranchColoringInfo : public SubbranchColoringInfo {

    public:

        CompositeSubbranchColoringInfo(const ColorType newAboveColor) : aboveColor(newAboveColor) {}

        ColorType getAboveColor() const {
            return aboveColor;
        }

        virtual void color(
            ColoringType* coloring,
            Branch* branch,
            const int blackCount,
            const int whiteCount,
            const BranchTrunkColoring& trunkColoring) = 0;

    protected:

        // Цвет первых узлов верхних простых подветвей и узлов, расположенных между ними.
        ColorType aboveColor;
    };

    /**
     * Информация о раскраске префикса ветви.
     */
    class PrefixSubbranchColoringInfo : public CompositeSubbranchColoringInfo {

    public:

        using CompositeSubbranchColoringInfo::aboveColor;

    public:

        PrefixSubbranchColoringInfo(
            const ColorType newAboveColor,
            PrefixSubbranchColoringInfo* newPrefix,
            PrimeSubbranchColoringInfo* newSuffix,
            const int newAdditionalNodeCount) :

            CompositeSubbranchColoringInfo(newAboveColor),
            prefix(newPrefix),
            suffix(newSuffix),
            additionalNodeCount(newAdditionalNodeCount),
            aboveColorMaxWhiteMap(0),
            grayMaxWhiteMap(0),
            maxWhiteMap(0) {

            calcNodeCount();
        }

        PrefixSubbranchColoringInfo* getPrefix() const {
            return prefix;
        }

        PrimeSubbranchColoringInfo* getSuffix() const {
            return suffix;
        }

        void build() {
            mergeMaxWhiteMaps(aboveColorMaxWhiteMap, aboveColor);
            mergeMaxWhiteMaps(grayMaxWhiteMap, GRAY);

            maxWhiteMap = new ColorlessMaxWhiteMap(nodeCount);

            for (int b = 0; b <= nodeCount; ++b) {
                maxWhiteMap->put(b, std::max(aboveColorMaxWhiteMap->get(b), grayMaxWhiteMap->get(b)));
            }
        }

        int getNodeCount() {
            return nodeCount;
        }

        ColorlessMaxWhiteMap* getMaxWhiteMap(const bool isCornerGray) const {
            return isCornerGray ? grayMaxWhiteMap : aboveColorMaxWhiteMap;
        }

        ColorlessMaxWhiteMap* getMaxWhiteMap() const {
            return maxWhiteMap;
        }

        int getMaxWhite(const int blackCount) const {
            return maxWhiteMap->get(blackCount);
        }

        int getMaxWhite(const bool isCornerGray, const int blackCount) const {
            return isCornerGray ? grayMaxWhiteMap->get(blackCount) : aboveColorMaxWhiteMap->get(blackCount);
        }

        void color(
            ColoringType* coloring,
            Branch* branch,
            const int blackCount,
            const int whiteCount,
            const BranchTrunkColoring& trunkColoring) {

            color(coloring, branch, blackCount, whiteCount, true, trunkColoring);
        }

        void color(
            ColoringType* coloring,
            Branch* branch,
            int blackCount,
            int whiteCount,
            const bool isAboveColorAcceptableForCorner,
            const BranchTrunkColoring& trunkColoring) {

            if (aboveColor == BLACK) {
                blackCount -= additionalNodeCount;
            }
            else {
                whiteCount -= additionalNodeCount;
            }

            if (prefix == 0) {
                suffix->color(coloring, branch, blackCount, whiteCount, isAboveColorAcceptableForCorner ? aboveColor : GRAY, trunkColoring);
                return;
            }

            for (int b = 0; b <= blackCount; ++b) {
                int prefixMaxWhite = prefix->getMaxWhite(b);
                int suffixMaxWhite =
                    isAboveColorAcceptableForCorner ? std::max(suffix->getMaxWhite(aboveColor, blackCount - b), suffix->getMaxWhite(GRAY, blackCount - b)) : suffix->getMaxWhite(GRAY, blackCount - b);

                if (prefixMaxWhite != INVALID_MAX_WHITE && suffixMaxWhite != INVALID_MAX_WHITE && (prefixMaxWhite + suffixMaxWhite >= whiteCount)) {
                    prefix->color(coloring, branch, b, prefixMaxWhite, trunkColoring);
                    suffix->color(coloring, branch, blackCount - b, suffixMaxWhite, isAboveColorAcceptableForCorner ? aboveColor : GRAY, trunkColoring);

                    break;
                }
            }
        }

        ~PrefixSubbranchColoringInfo() {
            delete aboveColorMaxWhiteMap;
            delete grayMaxWhiteMap;
            delete maxWhiteMap;
        }

    private:

        void calcNodeCount() {
            nodeCount = (prefix != 0) ? prefix->getNodeCount() + additionalNodeCount + suffix->getNodeCount() : additionalNodeCount + suffix->getNodeCount();
        }

        void mergeMaxWhiteMaps(ColorlessMaxWhiteMap*& mergedMaxWhiteMap, const ColorType cornerColor) const {
            if (prefix == 0) {
                mergedMaxWhiteMap = new ColorlessMaxWhiteMap(nodeCount);

                if (aboveColor == BLACK) {
                    for (int b = additionalNodeCount; b <= nodeCount; ++b) {
                        mergedMaxWhiteMap->put(b, suffix->getMaxWhite(cornerColor, b - additionalNodeCount));
                    }
                }
                else {
                    for (int b = 0; b < nodeCount - additionalNodeCount; ++b) {
                        if (suffix->getMaxWhite(cornerColor, b) != INVALID_MAX_WHITE) {
                            mergedMaxWhiteMap->put(b, suffix->getMaxWhite(cornerColor, b) + additionalNodeCount);
                        }
                    }
                }

                return;
            }

            ColorlessGray2BlackListMap* prefixGray2BlackListMap = prefix->getMaxWhiteMap()->toColorlessGray2BlackListMap();
            ColorlessGray2BlackListMap* suffixGray2BlackListMap;

            if (dynamic_cast<SingleSubbranchColoringInfo*>(suffix) != 0) {
                suffixGray2BlackListMap = dynamic_cast<SingleSubbranchColoringInfo*>(suffix)->getMaxWhiteMap()->toColorlessGray2BlackListMap(cornerColor);
            }
            else {
                suffixGray2BlackListMap = dynamic_cast<TreeSubbranchColoringInfo*>(suffix)->getMaxWhiteMap()->toColorlessGray2BlackListMap(cornerColor);
            }

            int nodeCountLog2 = (int)log2(nodeCount);
            int maxBlackCountUp = nodeCount + 2;
            int maxGrayCountUp = nodeCountLog2 + 4;

            ColorlessGray2BlackListMap* mergedGray2BlackListMap = new ColorlessGray2BlackListMap(maxGrayCountUp);

            int blackCount2GrayCountLength = maxBlackCountUp * maxGrayCountUp;
            bool* blackCount2GrayCount = new bool[blackCount2GrayCountLength];

            std::vector<int> sums;
            std::vector<int>::iterator sumIter;

            int* minGrayCount = new int[maxBlackCountUp];

            for (int i = 0; i < blackCount2GrayCountLength; ++i) {
                blackCount2GrayCount[i] = false;
            }

            for (int b = 0; b < maxBlackCountUp; ++b) {
                minGrayCount[b] = -1;
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (prefixGray2BlackListMap->get(i - j) == 0 || suffixGray2BlackListMap->get(j) == 0) {
                        continue;
                    }

                    sums.clear();
                    Summator::computePairwiseSums(prefixGray2BlackListMap->get(i - j), suffixGray2BlackListMap->get(j), sums);

                    for (sumIter = sums.begin(); sumIter != sums.end(); ++sumIter) {
                        if (minGrayCount[*sumIter] == -1 || minGrayCount[*sumIter] > i) {
                            blackCount2GrayCount[(*sumIter) * maxGrayCountUp + i] = true;
                            minGrayCount[*sumIter] = i;
                        }
                    }
                }
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j < maxBlackCountUp; ++j) {
                    if (blackCount2GrayCount[j * maxGrayCountUp + i]) {
                        mergedGray2BlackListMap->put(i, (aboveColor == BLACK) ? j + additionalNodeCount : j);
                    }
                }
            }

            mergedMaxWhiteMap = new ColorlessMaxWhiteMap(mergedGray2BlackListMap, nodeCount);

            delete prefixGray2BlackListMap;
            delete suffixGray2BlackListMap;
            delete[] blackCount2GrayCount;
            delete[] minGrayCount;
        }

    private:

        // Информация о раскраске объединения верхних простых подветвей.
        PrefixSubbranchColoringInfo* prefix;

        // Информация о раскраске нижней простой подветви.
        PrimeSubbranchColoringInfo* suffix;

        // Дополнительное количество узлов, окрашенных в цвет aboveColor.
        int additionalNodeCount;

        // Количество узлов.
        int nodeCount;

        // Отображения Max_White.
        ColorlessMaxWhiteMap* aboveColorMaxWhiteMap;
        ColorlessMaxWhiteMap* grayMaxWhiteMap;
        ColorlessMaxWhiteMap* maxWhiteMap;
    };

    /**
     * Информация о раскраске суффикса ветви.
     */
    class SuffixSubbranchColoringInfo : public CompositeSubbranchColoringInfo {

    public:

        SuffixSubbranchColoringInfo(
            const ColorType newAboveColor,
            SuffixSubbranchColoringInfo* newSuffix,
            PrimeSubbranchColoringInfo* newPrefix,
            const int newAdditionalNodeCount) :

            CompositeSubbranchColoringInfo(newAboveColor),
            belowColor((newAboveColor == BLACK) ? WHITE : BLACK),
            suffix(newSuffix),
            prefix(newPrefix),
            additionalNodeCount(newAdditionalNodeCount),
            belowColorMaxWhiteMap(0),
            grayMaxWhiteMap(0),
            maxWhiteMap(0) {

            calcNodeCount();
        }

        PrimeSubbranchColoringInfo* getPrefix() const {
            return prefix;
        }

        SuffixSubbranchColoringInfo* getSuffix() const {
            return suffix;
        }

        void build() {
            mergeMaxWhiteMaps(belowColorMaxWhiteMap, belowColor);
            mergeMaxWhiteMaps(grayMaxWhiteMap, GRAY);

            maxWhiteMap = new ColorlessMaxWhiteMap(nodeCount);

            for (int b = 0; b <= nodeCount; ++b) {
                maxWhiteMap->put(b, std::max(belowColorMaxWhiteMap->get(b), grayMaxWhiteMap->get(b)));
            }
        }

        int getNodeCount() {
            return nodeCount;
        }

        ColorlessMaxWhiteMap* getMaxWhiteMap(const bool isCornerGray) const {
            return isCornerGray ? grayMaxWhiteMap : belowColorMaxWhiteMap;
        }

        ColorlessMaxWhiteMap* getMaxWhiteMap() const {
            return maxWhiteMap;
        }

        int getMaxWhite(const int blackCount) const {
            return maxWhiteMap->get(blackCount);
        }

        int getMaxWhite(const bool isCornerGray, const int blackCount) const {
            return isCornerGray ? grayMaxWhiteMap->get(blackCount) : belowColorMaxWhiteMap->get(blackCount);
        }

        void color(
            ColoringType* coloring,
            Branch* branch,
            const int blackCount,
            const int whiteCount,
            const BranchTrunkColoring& trunkColoring) {

            color(coloring, branch, blackCount, whiteCount, true, trunkColoring);
        }

        void color(
            ColoringType* coloring,
            Branch* branch,
            int blackCount,
            int whiteCount,
            const bool isBelowColorAcceptableForCorner,
            const BranchTrunkColoring& trunkColoring) {

            if (belowColor == BLACK) {
                blackCount -= additionalNodeCount;
            }
            else {
                whiteCount -= additionalNodeCount;
            }

            if (suffix == 0) {
                prefix->color(coloring, branch, blackCount, whiteCount, isBelowColorAcceptableForCorner ? belowColor : GRAY, trunkColoring);
                return;
            }

            for (int b = 0; b <= blackCount; ++b) {
                int suffixMaxWhite = suffix->getMaxWhite(b);
                int prefixMaxWhite =
                    isBelowColorAcceptableForCorner ? std::max(prefix->getMaxWhite(belowColor, blackCount - b), prefix->getMaxWhite(GRAY, blackCount - b)) : prefix->getMaxWhite(GRAY, blackCount - b);

                if (suffixMaxWhite != INVALID_MAX_WHITE && prefixMaxWhite != INVALID_MAX_WHITE && (suffixMaxWhite + prefixMaxWhite >= whiteCount)) {
                    suffix->color(coloring, branch, b, suffixMaxWhite, trunkColoring);
                    prefix->color(coloring, branch, blackCount - b, prefixMaxWhite, isBelowColorAcceptableForCorner ? belowColor : GRAY, trunkColoring);

                    break;
                }
            }
        }

        ~SuffixSubbranchColoringInfo() {
            delete belowColorMaxWhiteMap;
            delete grayMaxWhiteMap;
            delete maxWhiteMap;
        }

    private:

        void calcNodeCount() {
            nodeCount = (suffix != 0) ? prefix->getNodeCount() + additionalNodeCount + suffix->getNodeCount() : prefix->getNodeCount() + additionalNodeCount;
        }

        void mergeMaxWhiteMaps(ColorlessMaxWhiteMap*& mergedMaxWhiteMap, const ColorType cornerColor) const {
            if (suffix == 0) {
                mergedMaxWhiteMap = new ColorlessMaxWhiteMap(nodeCount);

                if (belowColor == BLACK) {
                    for (int b = additionalNodeCount; b <= nodeCount; ++b) {
                        mergedMaxWhiteMap->put(b, prefix->getMaxWhite(cornerColor, b - additionalNodeCount));
                    }
                }
                else {
                    for (int b = 0; b < nodeCount - additionalNodeCount; ++b) {
                        if (prefix->getMaxWhite(cornerColor, b) != INVALID_MAX_WHITE) {
                            mergedMaxWhiteMap->put(b, prefix->getMaxWhite(cornerColor, b) + additionalNodeCount);
                        }
                    }
                }

                return;
            }

            ColorlessGray2BlackListMap* suffixGray2BlackListMap = suffix->getMaxWhiteMap()->toColorlessGray2BlackListMap();
            ColorlessGray2BlackListMap* prefixGray2BlackListMap;

            if (dynamic_cast<SingleSubbranchColoringInfo*>(prefix) != 0) {
                prefixGray2BlackListMap = dynamic_cast<SingleSubbranchColoringInfo*>(prefix)->getMaxWhiteMap()->toColorlessGray2BlackListMap(cornerColor);
            }
            else {
                prefixGray2BlackListMap = dynamic_cast<TreeSubbranchColoringInfo*>(prefix)->getMaxWhiteMap()->toColorlessGray2BlackListMap(cornerColor);
            }

            int nodeCountLog2 = (int)log2(nodeCount);
            int maxBlackCountUp = nodeCount + 2;
            int maxGrayCountUp = nodeCountLog2 + 4;

            ColorlessGray2BlackListMap* mergedGray2BlackListMap = new ColorlessGray2BlackListMap(maxGrayCountUp);

            int blackCount2GrayCountLength = maxBlackCountUp * maxGrayCountUp;
            bool* blackCount2GrayCount = new bool[blackCount2GrayCountLength];

            std::vector<int> sums;
            std::vector<int>::iterator sumIter;

            int* minGrayCount = new int[maxBlackCountUp];

            for (int i = 0; i < blackCount2GrayCountLength; ++i) {
                blackCount2GrayCount[i] = false;
            }

            for (int b = 0; b < maxBlackCountUp; ++b) {
                minGrayCount[b] = -1;
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (suffixGray2BlackListMap->get(i - j) == 0 || prefixGray2BlackListMap->get(j) == 0) {
                        continue;
                    }

                    sums.clear();
                    Summator::computePairwiseSums(suffixGray2BlackListMap->get(i - j), prefixGray2BlackListMap->get(j), sums);

                    for (sumIter = sums.begin(); sumIter != sums.end(); ++sumIter) {
                        if (minGrayCount[*sumIter] == -1 || minGrayCount[*sumIter] > i) {
                            blackCount2GrayCount[(*sumIter) * maxGrayCountUp + i] = true;
                            minGrayCount[*sumIter] = i;
                        }
                    }
                }
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j < maxBlackCountUp; ++j) {
                    if (blackCount2GrayCount[j * maxGrayCountUp + i]) {
                        mergedGray2BlackListMap->put(i, (belowColor == BLACK) ? j + additionalNodeCount : j);
                    }
                }
            }

            mergedMaxWhiteMap = new ColorlessMaxWhiteMap(mergedGray2BlackListMap, nodeCount);

            delete suffixGray2BlackListMap;
            delete prefixGray2BlackListMap;
            delete[] blackCount2GrayCount;
            delete[] minGrayCount;
        }

    private:

        // Цвет первых узлов нижних простых подветвей и узлов, расположенных между ними.
        ColorType belowColor;

        // Информация о раскраске объединения нижних простых подветвей.
        SuffixSubbranchColoringInfo* suffix;

        // Информация о раскраске верхней простой подветви.
        PrimeSubbranchColoringInfo* prefix;

        // Дополнительное количество узлов, окрашенных в цвет belowColor.
        const int additionalNodeCount;

        // Количество узлов.
        int nodeCount;

        // Отображения Max_White.
        ColorlessMaxWhiteMap* belowColorMaxWhiteMap;
        ColorlessMaxWhiteMap* grayMaxWhiteMap;
        ColorlessMaxWhiteMap* maxWhiteMap;
    };

    /**
     * Информация о раскраске составной подветви, которую составляют ближайшие друг у другу префиксная и суффиксная подветви.
     */
    class PrefixSuffixSubbranchColoringInfo : public CompositeSubbranchColoringInfo {

    public:

        using CompositeSubbranchColoringInfo::aboveColor;

    public:

        PrefixSuffixSubbranchColoringInfo(
            const ColorType newAboveColor,
            const ColorType newBelowColor,
            PrefixSubbranchColoringInfo* newPrefix,
            SuffixSubbranchColoringInfo* newSuffix) :

            CompositeSubbranchColoringInfo(newAboveColor),
            belowColor(newBelowColor),
            prefix(newPrefix),
            suffix(newSuffix) {

            calcNodeCount();
        }

        ColorType getBelowColor() const {
            return belowColor;
        }

        void build() {
            mergeMaxWhiteMaps(aboveBelowMaxWhiteMap, aboveColor, belowColor);
            mergeMaxWhiteMaps(aboveGrayMaxWhiteMap, aboveColor, GRAY);
            mergeMaxWhiteMaps(grayBelowMaxWhiteMap, GRAY, belowColor);
            mergeMaxWhiteMaps(grayGrayMaxWhiteMap, GRAY, GRAY);

            maxWhiteMap = new ColorlessMaxWhiteMap(nodeCount);

            for (int b = 0; b <= nodeCount; ++b) {
                maxWhiteMap->put(b, std::max(std::max(std::max(aboveBelowMaxWhiteMap->get(b), aboveGrayMaxWhiteMap->get(b)), grayBelowMaxWhiteMap->get(b)), grayGrayMaxWhiteMap->get(b)));
            }
        }

        int getNodeCount() {
            return nodeCount;
        }

        int getMaxWhite(const int blackCount) const {
            return maxWhiteMap->get(blackCount);
        }

        int getMaxWhite(const bool isPrefixCornerGray, const bool isSuffixCornerGray, const int blackCount) const {
            if (isPrefixCornerGray && isSuffixCornerGray) {
                return grayGrayMaxWhiteMap->get(blackCount);
            }
            else if (isPrefixCornerGray && !isSuffixCornerGray) {
                return grayBelowMaxWhiteMap->get(blackCount);
            }
            else if (!isPrefixCornerGray && isSuffixCornerGray) {
                return aboveGrayMaxWhiteMap->get(blackCount);
            }
            else {
                return aboveBelowMaxWhiteMap->get(blackCount);
            }
        }

        void color(
            ColoringType* coloring,
            Branch* branch,
            const int blackCount,
            const int whiteCount,
            const BranchTrunkColoring& trunkColoring) {

            int grayPosition = trunkColoring.getGrayPosition();

            if (grayPosition == prefix->getSuffix()->getPosition()) {
                if (grayBelowMaxWhiteMap->get(blackCount) >= whiteCount) {
                    for (int b = 0; b <= blackCount; ++b) {
                        int prefixMaxWhite = prefix->getMaxWhite(true, b);
                        int suffixMaxWhite = suffix->getMaxWhite(false, blackCount - b);

                        if (prefixMaxWhite != INVALID_MAX_WHITE && suffixMaxWhite != INVALID_MAX_WHITE && (prefixMaxWhite + suffixMaxWhite >= whiteCount)) {
                            prefix->color(coloring, branch, b, prefixMaxWhite, false, trunkColoring);
                            suffix->color(coloring, branch, blackCount - b, suffixMaxWhite, true, trunkColoring);

                            break;
                        }
                    }
                }
                else if (grayGrayMaxWhiteMap->get(blackCount) >= whiteCount) {
                    for (int b = 0; b <= blackCount; ++b) {
                        int prefixMaxWhite = prefix->getMaxWhite(true, b);
                        int suffixMaxWhite = suffix->getMaxWhite(true, blackCount - b);

                        if (prefixMaxWhite != INVALID_MAX_WHITE && suffixMaxWhite != INVALID_MAX_WHITE && (prefixMaxWhite + suffixMaxWhite >= whiteCount)) {
                            prefix->color(coloring, branch, b, prefixMaxWhite, false, trunkColoring);
                            suffix->color(coloring, branch, blackCount - b, suffixMaxWhite, false, trunkColoring);

                            break;
                        }
                    }
                }
            }
            else if (grayPosition == suffix->getPrefix()->getPosition()) {
                if (aboveGrayMaxWhiteMap->get(blackCount) >= whiteCount) {
                    for (int b = 0; b <= blackCount; ++b) {
                        int prefixMaxWhite = prefix->getMaxWhite(false, b);
                        int suffixMaxWhite = suffix->getMaxWhite(true, blackCount - b);

                        if (prefixMaxWhite != INVALID_MAX_WHITE && suffixMaxWhite != INVALID_MAX_WHITE && (prefixMaxWhite + suffixMaxWhite >= whiteCount)) {
                            prefix->color(coloring, branch, b, prefixMaxWhite, true, trunkColoring);
                            suffix->color(coloring, branch, blackCount - b, suffixMaxWhite, false, trunkColoring);

                            break;
                        }
                    }
                }
                else if (grayGrayMaxWhiteMap->get(blackCount) >= whiteCount) {
                    for (int b = 0; b <= blackCount; ++b) {
                        int prefixMaxWhite = prefix->getMaxWhite(true, b);
                        int suffixMaxWhite = suffix->getMaxWhite(true, blackCount - b);

                        if (prefixMaxWhite != INVALID_MAX_WHITE && suffixMaxWhite != INVALID_MAX_WHITE && (prefixMaxWhite + suffixMaxWhite >= whiteCount)) {
                            prefix->color(coloring, branch, b, prefixMaxWhite, false, trunkColoring);
                            suffix->color(coloring, branch, blackCount - b, suffixMaxWhite, false, trunkColoring);

                            break;
                        }
                    }
                }
            }
            else {
                if (aboveBelowMaxWhiteMap->get(blackCount) >= whiteCount) {
                    for (int b = 0; b <= blackCount; ++b) {
                        int prefixMaxWhite = prefix->getMaxWhite(false, b);
                        int suffixMaxWhite = suffix->getMaxWhite(false, blackCount - b);

                        if (prefixMaxWhite != INVALID_MAX_WHITE && suffixMaxWhite != INVALID_MAX_WHITE && (prefixMaxWhite + suffixMaxWhite >= whiteCount)) {
                            prefix->color(coloring, branch, b, prefixMaxWhite, true, trunkColoring);
                            suffix->color(coloring, branch, blackCount - b, suffixMaxWhite, true, trunkColoring);

                            break;
                        }
                    }
                }
                else if (aboveGrayMaxWhiteMap->get(blackCount) >= whiteCount) {
                    for (int b = 0; b <= blackCount; ++b) {
                        int prefixMaxWhite = prefix->getMaxWhite(false, b);
                        int suffixMaxWhite = suffix->getMaxWhite(true, blackCount - b);

                        if (prefixMaxWhite != INVALID_MAX_WHITE && suffixMaxWhite != INVALID_MAX_WHITE && (prefixMaxWhite + suffixMaxWhite >= whiteCount)) {
                            prefix->color(coloring, branch, b, prefixMaxWhite, true, trunkColoring);
                            suffix->color(coloring, branch, blackCount - b, suffixMaxWhite, false, trunkColoring);

                            break;
                        }
                    }
                }
                else if (grayBelowMaxWhiteMap->get(blackCount) >= whiteCount) {
                    for (int b = 0; b <= blackCount; ++b) {
                        int prefixMaxWhite = prefix->getMaxWhite(true, b);
                        int suffixMaxWhite = suffix->getMaxWhite(false, blackCount - b);

                        if (prefixMaxWhite != INVALID_MAX_WHITE && suffixMaxWhite != INVALID_MAX_WHITE && (prefixMaxWhite + suffixMaxWhite >= whiteCount)) {
                            prefix->color(coloring, branch, b, prefixMaxWhite, false, trunkColoring);
                            suffix->color(coloring, branch, blackCount - b, suffixMaxWhite, true, trunkColoring);

                            break;
                        }
                    }
                }
                else if (grayGrayMaxWhiteMap->get(blackCount) >= whiteCount) {
                    for (int b = 0; b <= blackCount; ++b) {
                        int prefixMaxWhite = prefix->getMaxWhite(true, b);
                        int suffixMaxWhite = suffix->getMaxWhite(true, blackCount - b);

                        if (prefixMaxWhite != INVALID_MAX_WHITE && suffixMaxWhite != INVALID_MAX_WHITE && (prefixMaxWhite + suffixMaxWhite >= whiteCount)) {
                            prefix->color(coloring, branch, b, prefixMaxWhite, false, trunkColoring);
                            suffix->color(coloring, branch, blackCount - b, suffixMaxWhite, false, trunkColoring);

                            break;
                        }
                    }
                }
            }
        }

        ~PrefixSuffixSubbranchColoringInfo() {
            delete aboveBelowMaxWhiteMap;
            delete aboveGrayMaxWhiteMap;
            delete grayBelowMaxWhiteMap;
            delete grayGrayMaxWhiteMap;

            delete maxWhiteMap;
        }

    private:

        void calcNodeCount() {
            nodeCount = prefix->getNodeCount() + suffix->getNodeCount();
        }

        void mergeMaxWhiteMaps(ColorlessMaxWhiteMap*& mergedMaxWhiteMap, const ColorType prefixCornerColor, const ColorType suffixCornerColor) const {
            ColorlessGray2BlackListMap* prefixGray2BlackListMap =
                prefix->getMaxWhiteMap(prefixCornerColor == aboveColor ? false : true)->toColorlessGray2BlackListMap();
            ColorlessGray2BlackListMap* suffixGray2BlackListMap =
                suffix->getMaxWhiteMap(suffixCornerColor == belowColor ? false : true)->toColorlessGray2BlackListMap();

            int nodeCountLog2 = (int)log2(nodeCount);
            int maxBlackCountUp = nodeCount + 2;
            int maxGrayCountUp = nodeCountLog2 + 4;

            ColorlessGray2BlackListMap* mergedGray2BlackListMap = new ColorlessGray2BlackListMap(maxGrayCountUp);

            int blackCount2GrayCountLength = maxBlackCountUp * maxGrayCountUp;
            bool* blackCount2GrayCount = new bool[blackCount2GrayCountLength];

            std::vector<int> sums;
            std::vector<int>::iterator sumIter;

            int* minGrayCount = new int[maxBlackCountUp];

            for (int i = 0; i < blackCount2GrayCountLength; ++i) {
                blackCount2GrayCount[i] = false;
            }

            for (int b = 0; b < maxBlackCountUp; ++b) {
                minGrayCount[b] = -1;
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (prefixGray2BlackListMap->get(j) == 0 || suffixGray2BlackListMap->get(i - j) == 0) {
                        continue;
                    }

                    sums.clear();
                    Summator::computePairwiseSums(prefixGray2BlackListMap->get(j), suffixGray2BlackListMap->get(i - j), sums);

                    for (sumIter = sums.begin(); sumIter != sums.end(); ++sumIter) {
                        if (minGrayCount[*sumIter] == -1 || minGrayCount[*sumIter] > i) {
                            blackCount2GrayCount[(*sumIter) * maxGrayCountUp + i] = true;
                            minGrayCount[*sumIter] = i;
                        }
                    }
                }
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j < maxBlackCountUp; ++j) {
                    if (blackCount2GrayCount[j * maxGrayCountUp + i]) {
                        mergedGray2BlackListMap->put(i, j);
                    }
                }
            }

            mergedMaxWhiteMap = new ColorlessMaxWhiteMap(mergedGray2BlackListMap, nodeCount);

            delete prefixGray2BlackListMap;
            delete suffixGray2BlackListMap;
            delete[] blackCount2GrayCount;
            delete[] minGrayCount;
        }

    private:

        // Цвет первых узлов подветвей суффикса и узлов, расположенных между ними.
        ColorType belowColor;

        // Префикс.
        PrefixSubbranchColoringInfo* prefix;

        // Суффикс.
        SuffixSubbranchColoringInfo* suffix;

        // Количество узлов.
        int nodeCount;

        // Отображения Max_White.
        ColorlessMaxWhiteMap* aboveBelowMaxWhiteMap;
        ColorlessMaxWhiteMap* aboveGrayMaxWhiteMap;
        ColorlessMaxWhiteMap* grayBelowMaxWhiteMap;
        ColorlessMaxWhiteMap* grayGrayMaxWhiteMap;

        ColorlessMaxWhiteMap* maxWhiteMap;
    };

private:

    int getRootBranchCount() {
        if (rootBranchCount != INVALID_ROOT_BRANCH_COUNT) {
            return rootBranchCount;
        }

        if (tree == 0 || tree->getRoot() == 0 || tree->getRoot()->getChildCount() == 0) {
            rootBranchCount = 0;
        }
        else {
            rootBranchCount = tree->getRoot()->getChildCount();
        }

        return rootBranchCount;
    }

    void pushFusion(Fusion* fusion) {
        if (fusionStack == 0) {
            fusionStack = new FusionStack();
        }

        fusionStack->push(fusion);
        topFusion = fusion;
    }

    Fusion* getTopFusion() const {
        return !fusionStack->empty() ? fusionStack->top() : 0;
    }

    void popFusion() {
        if (fusionStack != 0 && !fusionStack->empty()) {
            fusionStack->pop();
        }
    }

    void findColoring_priv(const int blackCount, const int grayCount) {
        findColoring_priv(
            0,
            rootBranchCount - 1,
            blackCount,
            grayCount,
            0,
            false);
    }

    void findColoring_priv(const int first, const int last, int blackCount, int grayCount, Fusion* fusion, const bool isFirstNodeColored) {
        ColorType firstNodeColor;

        if (isFirstNodeColored) {
            firstNodeColor = coloring->getColor(rootBranchesColoringInfo[0]->getBranch()->getFirstTrunkNode());
        }
        else {
            firstNodeColor = GRAY;

            if (topMaxWhiteMap->get(BLACK, blackCount) > topMaxWhiteMap->get(firstNodeColor, blackCount)) {
                firstNodeColor = BLACK;
            }

            if (topMaxWhiteMap->get(WHITE, blackCount) > topMaxWhiteMap->get(firstNodeColor, blackCount)) {
                firstNodeColor = WHITE;
            }

            coloring->color(rootBranchesColoringInfo[0]->getBranch()->getFirstTrunkNode(), firstNodeColor);
        }

        if (last - first == 0) {
            rootBranchesColoringInfo[first]->color(
                coloring,
                blackCount,
                rootBranchesColoringInfo[first]->getBranch()->getNodeCount() - (blackCount + grayCount),
                firstNodeColor);

            return;
        }

        if (fusion == 0) {
            fusion = getTopFusion();
            popFusion();
        }

        if (firstNodeColor == BLACK) {
            ++blackCount;
        }
        else if (firstNodeColor == GRAY) {
            ++grayCount;
        }

        Fusion* leftFusion = fusion->getLeftFusion();
        Fusion* rightFusion = fusion->getRightFusion();

        for (int b = 0; b <= blackCount; ++b) {
            if (
               (leftFusion->getMinGrayCount(firstNodeColor, b) != INVALID_MIN_GRAY) &&
               (rightFusion->getMinGrayCount(firstNodeColor, blackCount - b) != INVALID_MIN_GRAY) &&
               (leftFusion->getMinGrayCount(firstNodeColor, b) + rightFusion->getMinGrayCount(firstNodeColor, blackCount - b) <= grayCount)) {

                findColoring_priv(
                    first,
                    (first + last) / 2,
                    b,
                    leftFusion->getMinGrayCount(firstNodeColor, b),
                    leftFusion,
                    true);

                findColoring_priv(
                    (first + last) / 2 + 1,
                    last,
                    blackCount - b,
                    rightFusion->getMinGrayCount(firstNodeColor, blackCount - b),
                    rightFusion,
                    true);

                break;
            }
        }
    }

    void buildBody() {
        if (tree->getRoot()->isLeaf()) {
            maxWhiteMap = new MaxWhiteMap(1);

            maxWhiteMap->set(0, 1);
            maxWhiteMap->set(1, 0);

            return;
        }

        rootBranchCount = getRootBranchCount();
        rootBranchesColoringInfo = new BranchColoringInfo*[rootBranchCount];

        for (int i = 0; i < rootBranchCount; ++i) {
            Branch* rootBranch = new Branch(tree->getRoot(), i);
            rootBranch->build();
            rootBranchesColoringInfo[i] = new BranchColoringInfo(rootBranch);
            rootBranchesColoringInfo[i]->build();
        }

        std::pair<Gray2BlackListMap*, Fusion*>* currentMerging = new std::pair<Gray2BlackListMap*, Fusion*>[rootBranchCount];

        for (int i = 0; i < rootBranchCount; ++i) {
            currentMerging[i] =
                std::pair<Gray2BlackListMap*, Fusion*>(rootBranchesColoringInfo[i]->getMaxWhiteMap()->toGray2BlackListMap(), new SingleChildFusion(rootBranchesColoringInfo[i]));
        }

        Gray2BlackListMap* topGray2BlackListMap = TreeSubbranchColoringInfo::merge(fusionStack, topFusion, nodeCount, currentMerging, 0, rootBranchCount - 1).first;
        topMaxWhiteMap = new ColorMaxWhiteMap(topGray2BlackListMap);

        for (int b = 0; b <= nodeCount; ++b) {
            int maxWhite = (topMaxWhiteMap->get(BLACK, b) != INVALID_MAX_WHITE) ? topMaxWhiteMap->get(BLACK, b) : INVALID_MAX_WHITE;

            if ((topMaxWhiteMap->get(WHITE, b) != INVALID_MAX_WHITE) && (topMaxWhiteMap->get(WHITE, b) > maxWhite)) {
                maxWhite = topMaxWhiteMap->get(WHITE, b);
            }

            if ((topMaxWhiteMap->get(GRAY, b) != INVALID_MAX_WHITE) && (topMaxWhiteMap->get(GRAY, b) > maxWhite)) {
                maxWhite = topMaxWhiteMap->get(GRAY, b);
            }

            maxWhiteMap->set(b, maxWhite);
        }

        delete topGray2BlackListMap;
        delete currentMerging;
    }

    void buildColoring() {
        NodeType* root = tree->getRoot();
        int g = nodeCount - (blackCount + whiteCount);

        if (root->isLeaf()) {
            if (blackCount == 1) {
                coloring->color(root, BLACK);
            }
            else if (g == 1) {
                coloring->color(root, GRAY);
            }
            else {
                coloring->color(root, WHITE);
            }

            return;
        }

        int maxWhite = maxWhiteMap->get(blackCount);
        g = nodeCount - (blackCount + maxWhite);

        findColoring_priv(blackCount, g);

        if (coloring->getWhiteCount() > whiteCount) {
            int excess = coloring->getWhiteCount() - whiteCount;
            TreeIterator treeIter = treeIterator<IsLeafPredicate>();

            while (treeIter.hasNext() && excess > 0) {
                NodeType* node = treeIter.getNext();

                if (coloring->getColor(node) == WHITE) {
                    coloring->color(node, GRAY);
                    --excess;
                }
            }
        }
    }

private:

    // Информация о раскраске корневых ветвей.
    BranchColoringInfo** rootBranchesColoringInfo;

    // Количество корневых ветвей.
    int rootBranchCount;

    // Стек объединений отображений Max_White.
    FusionStack* fusionStack;

    // Объединение верхнего уровня.
    Fusion* topFusion;

    // Отображние Max_White верхнего уровня.
    ColorMaxWhiteMap* topMaxWhiteMap;
};

template<typename NodeType> const typename ConjectureBWTreeColorer<NodeType>::ColorType ConjectureBWTreeColorer<NodeType>::BLACK = ConjectureBWTreeColorer<NodeType>::ColoringType::BLACK;
template<typename NodeType> const typename ConjectureBWTreeColorer<NodeType>::ColorType ConjectureBWTreeColorer<NodeType>::WHITE = ConjectureBWTreeColorer<NodeType>::ColoringType::WHITE;
template<typename NodeType> const typename ConjectureBWTreeColorer<NodeType>::ColorType ConjectureBWTreeColorer<NodeType>::GRAY = ConjectureBWTreeColorer<NodeType>::ColoringType::GRAY;
template<typename NodeType> const typename ConjectureBWTreeColorer<NodeType>::ColorType ConjectureBWTreeColorer<NodeType>::COLOR_COUNT = ConjectureBWTreeColorer<NodeType>::ColoringType::COLOR_COUNT;
template<typename NodeType> const typename ConjectureBWTreeColorer<NodeType>::ColorType ConjectureBWTreeColorer<NodeType>::INVALID_COLOR = ConjectureBWTreeColorer<NodeType>::ColoringType::INVALID_COLOR;

template<typename NodeType> const int ConjectureBWTreeColorer<NodeType>::INVALID_MAX_WHITE = -1;
template<typename NodeType> const int ConjectureBWTreeColorer<NodeType>::INVALID_MIN_GRAY = -1;
template<typename NodeType> const int ConjectureBWTreeColorer<NodeType>::INVALID_ROOT_BRANCH_COUNT = -1;

} // namespace bw.

#endif

