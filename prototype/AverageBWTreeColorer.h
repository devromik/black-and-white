#ifndef BW_AVERAGE_BW_TREE_COLORER_H
#define BW_AVERAGE_BW_TREE_COLORER_H

#include <memory>
#include <math.h>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include "Tree.h"
#include "StdBWTreeColorer.h"
#include "Summator.h"
#include <QDebug>

namespace bw {

template<typename NodeType>
class AverageBWTreeColorer : public StdBWTreeColorer<NodeType> {

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

protected:

    /**
     * Список, сопоставляемый некоторому числу серых узлов grayCount,
     * и содержащий всевозможные значения blackCount такие,
     * что существует оптимальная чёрно-белая раскраска дерева c grayCount серыми узлами и blackCount чёрными.
     */
    typedef std::vector<int> BlackList;

protected:

    static const int INVALID_NODE_COUNT = -1;
    static const int INVALID_MAX_WHITE = -1;
    static const int INVALID_MIN_GRAY = -1;

protected:

    TreeIterator treeIterator() const {
        return tree->template iterator<IsLeafPredicate>();
    }

private:

    using StdBWTreeColorer<NodeType>::tree;
    using StdBWTreeColorer<NodeType>::nodeCount;
    using StdBWTreeColorer<NodeType>::coloring;
    using StdBWTreeColorer<NodeType>::maxWhiteMap;
    using StdBWTreeColorer<NodeType>::blackCount;
    using StdBWTreeColorer<NodeType>::whiteCount;

private:

    /**
     * Представление отображения Max_White в виде Black_List(Сolor(root), grayCount).
     */
    class GrayToBlackListMap {

    public:

        GrayToBlackListMap(const int newMaxGrayCount) :
            maxGrayCount(newMaxGrayCount),
            maxBlackCount(-1) {

            initMap();
        }

        GrayToBlackListMap(const int newMaxGrayCount, const int maxBlackCount) :
            maxGrayCount(newMaxGrayCount),
            maxBlackCount(maxBlackCount) {

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

        GrayToBlackListMap* makeUnion(GrayToBlackListMap* other, const int unionNodeCount) const {
            int unionNodeCountLog2 = (int)log2(unionNodeCount);
            int maxBlackCountUp = unionNodeCount + 2;
            int maxGrayCountUp = unionNodeCountLog2 + 4;

            GrayToBlackListMap* result = new GrayToBlackListMap(maxGrayCountUp, unionNodeCount);

            int blackInGrayListSize = maxBlackCountUp * maxGrayCountUp;
            bool* blackInGrayList = new bool[blackInGrayListSize];

            std::vector<int> sums;
            std::vector<int>::iterator sumIter;

            int* blackToMinGray = new int[maxBlackCountUp];

            // Цвет корня - чёрный.
            for (int i = 0; i < blackInGrayListSize; ++i) {
                blackInGrayList[i] = false;
            }

            for (int b = 0; b < maxBlackCountUp; ++b) {
                blackToMinGray[b] = -1;
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (get(BLACK, j) == 0 || other->get(BLACK, i - j) == 0) {
                        continue;
                    }

                    sums.clear();
                    Summator::computePairwiseSums(get(BLACK, j), other->get(BLACK, i - j), sums);

                    for (sumIter = sums.begin(); sumIter != sums.end(); ++sumIter) {
                        if (blackToMinGray[*sumIter] == -1 || blackToMinGray[*sumIter] > i) {
                            blackInGrayList[(*sumIter) * maxGrayCountUp + i] = true;
                            blackToMinGray[*sumIter] = i;
                        }
                    }
                }
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j < maxBlackCountUp; ++j) {
                    if (blackInGrayList[j * maxGrayCountUp + i]) {
                        result->put(BLACK, i, j - 1);
                    }
                }
            }

            // Цвет корня - белый.
            for (int i = 0; i < blackInGrayListSize; ++i) {
                blackInGrayList[i] = false;
            }

            for (int b = 0; b < maxBlackCountUp; ++b) {
                blackToMinGray[b] = -1;
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (get(WHITE, j) == 0 || other->get(WHITE, i - j) == 0) {
                        continue;
                    }

                    sums.clear();
                    Summator::computePairwiseSums(get(WHITE, j), other->get(WHITE, i - j), sums);

                    for (sumIter = sums.begin(); sumIter != sums.end(); ++sumIter) {
                        if (blackToMinGray[*sumIter] == -1 || blackToMinGray[*sumIter] > i) {
                            blackInGrayList[(*sumIter) * maxGrayCountUp + i] = true;
                            blackToMinGray[*sumIter] = i;
                        }
                    }
                }
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j < maxBlackCountUp; ++j) {
                    if (blackInGrayList[j * maxGrayCountUp + i]) {
                        result->put(WHITE, i, j);
                    }
                }
            }

            // Цвет корня - серый.
            for (int i = 0; i < blackInGrayListSize; ++i) {
                blackInGrayList[i] = false;
            }

            for (int b = 0; b < maxBlackCountUp; ++b) {
                blackToMinGray[b] = -1;
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (get(GRAY, j) == 0 || other->get(GRAY, i - j) == 0) {
                        continue;
                    }

                    sums.clear();
                    Summator::computePairwiseSums(get(GRAY, j), other->get(GRAY, i - j), sums);

                    for (sumIter = sums.begin(); sumIter != sums.end(); ++sumIter) {
                        if (blackToMinGray[*sumIter] == -1 || blackToMinGray[*sumIter] > i) {
                            blackInGrayList[(*sumIter) * maxGrayCountUp + i] = true;
                            blackToMinGray[*sumIter] = i;
                        }
                    }
                }
            }

            for (int i = 0; i < maxGrayCountUp; ++i) {
                for (int j = 0; j < maxBlackCountUp; ++j) {
                    if (blackInGrayList[j * maxGrayCountUp + i]) {
                        result->put(GRAY, i - 1, j);
                    }
                }
            }

            delete[] blackInGrayList;
            delete[] blackToMinGray;

            return result;
        }

        ~GrayToBlackListMap() {
            destroy();
        }

    private:

        void initMap() {
            map[BLACK] = new std::vector<BlackList*>(maxGrayCount + 1, (BlackList*)0);
            map[WHITE] = new std::vector<BlackList*>(maxGrayCount + 1, (BlackList*)0);
            map[GRAY] = new std::vector<BlackList*>(maxGrayCount + 1, (BlackList*)0);
        }

        void destroy() {
            // Цвет корня - черный.
            std::vector<BlackList*>::iterator blackListIter = map[BLACK]->begin();

            while (blackListIter != map[BLACK]->end()) {
                delete *blackListIter;
                ++blackListIter;
            }

            delete map[BLACK];

            // Цвет корня - белый.
            blackListIter = map[WHITE]->begin();

            while (blackListIter != map[WHITE]->end()) {
                delete *blackListIter;
                ++blackListIter;
            }

            delete map[WHITE];

            // Цвет корня - серый.
            blackListIter = map[GRAY]->begin();

            while (blackListIter != map[GRAY]->end()) {
                delete *blackListIter;
                ++blackListIter;
            }

            delete map[GRAY];
        }

    private:

        std::vector<BlackList*>* map[COLOR_COUNT];

        // Максимальное количество серых узлов по всем чёрно-белым раскраскам.
        int maxGrayCount;

        // Максимальное количество чёрных узлов по всем чёрно-белым раскраскам.
        int maxBlackCount;
    };

    /**
     * Каждому цвету color из множества {BLACK, WHITE, GRAY},
     * и каждому неотрицательному целому blackCount, не превышающему числа узлов в данном дереве,
     * сопоставляет неотрицательное целое, равное максимальному числу белых узлов по всем чёрно-белым раскраскам данного дерева,
     * в которых корень этого дерева окрашен в цвет color, и ровно blackCount его узлов окрашено в чёрный цвет.
     */
    class ColorMaxWhiteMap {

    public:

        ColorMaxWhiteMap(const int newNodeCount) : nodeCount(newNodeCount) {
            initMap();
        }

        ColorMaxWhiteMap(const GrayToBlackListMap& source) {
            from(source);
        }

        ColorMaxWhiteMap(GrayToBlackListMap* source) {
            from(*source);
        }

        void put(
            const ColorType color,
            const int blackCount,
            const int maxWhite) {

            map[color][blackCount] = maxWhite;
        }

        int get(
            const ColorType color,
            const int blackCount) const {

            return
                blackCount >= 0 && blackCount <= nodeCount ?
                map[color][blackCount] :
                INVALID_MAX_WHITE;
        }

        int get(const int blackCount) const {
            return
                std::max(
                    std::max(get(BLACK, blackCount), get(WHITE, blackCount)),
                    get(GRAY, blackCount));
        }

        int getNodeCount() const {
            return nodeCount;
        }

        GrayToBlackListMap* toGray2BlackListMap() const {
            GrayToBlackListMap* gray2BlackListMap = new GrayToBlackListMap((int)log2(nodeCount) + 3);

            for (int b = 0; b <= nodeCount; ++b) {
                // Чёрный цвет.
                if (get(BLACK, b) != INVALID_MAX_WHITE) {
                    int grayCount = nodeCount - (b + get(BLACK, b));
                    gray2BlackListMap->put(BLACK, grayCount, b);
                }

                // Белый цвет.
                if (get(WHITE, b) != INVALID_MAX_WHITE) {
                    int grayCount = nodeCount - (b + get(WHITE, b));
                    gray2BlackListMap->put(WHITE, grayCount, b);
                }

                // Серый цвет.
                if (get(GRAY, b) != INVALID_MAX_WHITE) {
                    int grayCount = nodeCount - (b + get(GRAY, b));
                    gray2BlackListMap->put(GRAY, grayCount, b);
                }
            }

            return gray2BlackListMap;
        }

        /**
         * @brief Возвращает объединение отображений Max_White в предположении,
         *        что оба операнда соответствуют поддеревьям одного и того же дерева,
         *        пересекающимся лишь в корне этого дерева.
         *
         */
        ColorMaxWhiteMap* makeUnion(ColorMaxWhiteMap* other) {
            const std::unique_ptr<GrayToBlackListMap> gray2BlackListMap(toGray2BlackListMap());
            const std::unique_ptr<GrayToBlackListMap> otherGray2BlackListMap(other->toGray2BlackListMap());
            const std::unique_ptr<GrayToBlackListMap> unitedGray2BlackListMap(
                gray2BlackListMap->makeUnion(
                    otherGray2BlackListMap.get(),
                    nodeCount + other->nodeCount - 1 /* Минус 1, поскольку корни поддеревьев "сливаются" в корне наддерева. */));

            return new ColorMaxWhiteMap(unitedGray2BlackListMap.get());
        }

        void print() {
            qDebug() << "***** Max_White Map *****";

            // Цвет корня - черный.
            qDebug() << "color(root) is black: ";

            for (int b = 0; b <= nodeCount; ++b) {
                qDebug() << "    " << QString::number(map[BLACK][b]);
            }

            // Цвет корня - белый.
            qDebug() << "color(root) is white: ";

            for (int b = 0; b <= nodeCount; ++b) {
                qDebug() << "    " << QString::number(map[WHITE][b]);
            }

            // Цвет корня - серый.
            qDebug() << "color(root) is gray:  ";

            for (int b = 0; b <= nodeCount; ++b) {
                qDebug() << "    " << QString::number(map[GRAY][b]);
            }

            qDebug() << "*************************";
        }

    private:

        void initMap() {
            clearMap();

            map[BLACK].resize(nodeCount + 1, static_cast<const int>(INVALID_MAX_WHITE));
            map[WHITE].resize(nodeCount + 1, static_cast<const int>(INVALID_MAX_WHITE));
            map[GRAY].resize(nodeCount + 1, static_cast<const int>(INVALID_MAX_WHITE));
        }

        void from(const GrayToBlackListMap& source) {
            nodeCount = source.getMaxBlackCount();            
            initMap();

            for (int g = 0; g <= source.getMaxGrayCount(); ++g) {
                // Цвет корня - черный.
                BlackList* blackList = source.get(BLACK, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin();
                         blackListIter != blackList->end();
                         ++blackListIter) {

                        put(
                            BLACK,
                            *blackListIter,
                            nodeCount - (*blackListIter + g));
                    }
                }

                // Цвет корня - белый.
                blackList = source.get(WHITE, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin();
                         blackListIter != blackList->end();
                         ++blackListIter) {

                        put(
                            WHITE,
                            *blackListIter,
                            nodeCount - (*blackListIter + g));
                    }
                }

                // Цвет корня - серый.
                blackList = source.get(GRAY, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin();
                         blackListIter != blackList->end();
                         ++blackListIter) {

                        put(
                            GRAY,
                            *blackListIter,
                            nodeCount - (*blackListIter + g));
                    }
                }
            }
        }

        void clearMap() {
            map[BLACK].clear();
            map[WHITE].clear();
            map[GRAY].clear();
        }

    private:

        std::vector<int> map[COLOR_COUNT];
        int nodeCount;
    };

    /**
     * Информация о дереве.
     */
    struct TreeInfo {

        TreeInfo() {}

        TreeInfo(TreeType* newTree) : tree(newTree) {}

        void setTree(TreeType* newTree) {
            tree = newTree;
        }

        TreeType* tree;
    };

    /**
     * Итератор по стволу дерева.
     */
    class TrunkIterator {

    public:

        static const int UNKNOWN_TRUNK_LENGTH = -1;

    public:

        TrunkIterator(
            NodeType* newRoot,
            NodeType* newWatch,
            bool newRootToWatchForward = true,
            bool newRootToWatchForwardLeft = true,
            bool newWatchToLeafForward = true,
            bool newWatchToLeafForwardLeft = true) :

            root(newRoot),
            watch(newWatch),
            trunkLength(UNKNOWN_TRUNK_LENGTH),
            rootToWatchForward(newRootToWatchForward),
            rootToWatchForwardLeft(newRootToWatchForwardLeft),
            watchToLeafForward(newWatchToLeafForward),
            watchToLeafForwardLeft(newWatchToLeafForwardLeft),
            forward(newRootToWatchForward),
            left(newRootToWatchForwardLeft),
            nextPos(0),
            next(newRoot) {}

        TrunkIterator(
            NodeType* newRoot,
            NodeType* newWatch,
            const int newTrunkLength,
            bool newRootToWatchForward = true,
            bool newRootToWatchForwardLeft = true,
            bool newWatchToLeafForward = true,
            bool newWatchToLeafForwardLeft = true) :

            root(newRoot),
            watch(newWatch),
            trunkLength(newTrunkLength),
            rootToWatchForward(newRootToWatchForward),
            rootToWatchForwardLeft(newRootToWatchForwardLeft),
            watchToLeafForward(newWatchToLeafForward),
            watchToLeafForwardLeft(newWatchToLeafForwardLeft),
            forward(newRootToWatchForward),
            left(newRootToWatchForwardLeft),
            nextPos(0),
            next(newRoot) {}

        bool hasNext() const {
            return next != 0;
        }

        NodeType* getNext() {
            if (next == 0) {
                return 0;
            }

            NodeType* current = next;

            if (trunkLength == UNKNOWN_TRUNK_LENGTH || nextPos < trunkLength - 1) {
                if (current == watch) {
                    forward = watchToLeafForward;
                    left = watchToLeafForwardLeft;
                }

                if (forward) {
                    next = !current->isLeaf() ? (left ? current->getLeftmostChild() : current->getRightmostChild()) : 0;
                }
                else {
                    next = current->hasParent() ? current->getParent() : 0;
                }

                ++nextPos;
            }
            else {
                next = 0;
            }

            return current;
        }

    private:

        NodeType* root;
        NodeType* watch;
        int trunkLength;
        bool rootToWatchForward;
        bool rootToWatchForwardLeft;
        bool watchToLeafForward;
        bool watchToLeafForwardLeft;
        bool forward;
        bool left;
        int nextPos;
        NodeType* next;
    };

    /**
     * Решение задачи.
     */
    class Solution {

    public:

        virtual const std::shared_ptr<ColorMaxWhiteMap>& getMaxWhiteMap() = 0;

        virtual ColoringType* getColoring(
            ColoringType*& coloring,
            int blackCount,
            int whiteCount) = 0;

        virtual ColoringType* getColoring(
            ColoringType*& coloring,
            int blackCount,
            int whiteCount,
            const ColorType watchColor) = 0;

        virtual ~Solution() {}
    };

    /**
     * Задача.
     */
    class Problem {

    public:

        Problem(
            NodeType* newRoot,
            NodeType* newWatch,
            const TreeInfo& newTreeInfo) :

            root(newRoot),
            watch(newWatch),
            treeInfo(newTreeInfo) {}

        virtual const std::shared_ptr<Solution>& solve() = 0;

        virtual ~Problem() {}

    protected:

        NodeType* root;
        NodeType* watch;
        const TreeInfo& treeInfo;
    };

    /**
     * Решение задачи типа "Chain".
     */
    class ChainSolution {

    public:

        ChainSolution(
            NodeType* newRoot,
            NodeType* newWatch,
            const int newWatchPos,
            bool newRootToWatchForward,
            bool newRootToWatchForwardLeft,
            bool newWatchToLeafForward,
            bool newWatchToLeafForwardLeft,
            const int newChainLength) :

            root(newRoot),
            watch(newWatch),
            watchPos(newWatchPos),
            rootToWatchForward(newRootToWatchForward),
            rootToWatchForwardLeft(newRootToWatchForwardLeft),
            watchToLeafForward(newWatchToLeafForward),
            watchToLeafForwardLeft(newWatchToLeafForwardLeft),
            chainLength(newChainLength) {}

        int getChainLength() const {
            return chainLength;
        }

        const std::shared_ptr<ColorMaxWhiteMap>& getMaxWhiteMap(const ColorType watchColor) {
            if (!maxWhiteMaps[watchColor]) {
                buildMaxWhiteMaps();
            }

            return maxWhiteMaps[watchColor];
        }

        ColoringType* getColoring(
            ColoringType*& coloring,
            int blackCount,
            int whiteCount,
            const ColorType watchColor) {

            std::shared_ptr<ColorMaxWhiteMap>& maxWhiteMap = maxWhiteMaps[watchColor];

            if (maxWhiteMap->get(BLACK, blackCount) >= whiteCount) {
                return getColoring(coloring, blackCount, whiteCount, BLACK, watchColor);
            }
            else if (maxWhiteMap->get(WHITE, blackCount) >= whiteCount) {
                return getColoring(coloring, blackCount, whiteCount, WHITE, watchColor);
            }
            else {
                return getColoring(coloring, blackCount, whiteCount, GRAY, watchColor);
            }
        }

        ColoringType* getColoring(
            ColoringType*& coloring,
            int blackCount,
            int whiteCount,
            const ColorType rootColor,
            const ColorType watchColor) {

            if (!maxWhiteMaps[watchColor]) {
                buildMaxWhiteMaps();
            }

            TrunkIterator chainIter(root, watch, chainLength, rootToWatchForward, true, watchToLeafForward, true);
            NodeType* chainNode;
            int chainNodePos;

            /* ***** watch имеет чёрный цвет. ***** */

            // root имеет чёрный цвет.
            if (watchColor == BLACK && rootColor == BLACK) {
                if (blackCount > watchPos) {
                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    if (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }

                    while (chainIter.hasNext()) {
                        if (whiteCount > 0) {
                            coloring->color(chainIter.getNext(), WHITE);
                            --whiteCount;
                        }
                        else {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                    }
                }
                else if (blackCount > (chainLength - watchPos)) {
                    chainNodePos = 0;

                    while (chainIter.hasNext()) {
                        if (chainNodePos == 0 || chainNodePos > (chainLength - blackCount)) {
                            coloring->color(chainIter.getNext(), BLACK);
                        }
                        else if (chainNodePos == 1 || chainNodePos == (chainLength - blackCount)) {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                        else {
                            if (whiteCount > 0) {
                                coloring->color(chainIter.getNext(), WHITE);
                                --whiteCount;
                            }
                            else {
                                coloring->color(chainIter.getNext(), GRAY);
                            }
                        }

                        ++chainNodePos;
                    }
                }
                else {
                    if (std::max(0, watchPos - blackCount - 1) + std::max(0, chainLength - (watchPos + 2)) >
                        std::max(0, watchPos - 3) + std::max(0, chainLength - watchPos - blackCount)) {

                        chainNodePos = 0;

                        while (chainIter.hasNext()) {
                            if (chainNodePos < (blackCount - 1) || chainNodePos == watchPos) {
                                coloring->color(chainIter.getNext(), BLACK);
                            }
                            else if (chainNodePos == (blackCount - 1) || chainNodePos == (watchPos - 1) || chainNodePos == (watchPos + 1)) {
                                coloring->color(chainIter.getNext(), GRAY);
                            }
                            else {
                                if (whiteCount > 0) {
                                    coloring->color(chainIter.getNext(), WHITE);
                                    --whiteCount;
                                }
                                else {
                                    coloring->color(chainIter.getNext(), GRAY);
                                }
                            }

                            ++chainNodePos;
                        }
                    }
                    else {
                        chainNodePos = 0;

                        while (chainIter.hasNext()) {
                            if (chainNodePos == 0 || (chainNodePos >= watchPos && chainNodePos < (watchPos + blackCount - 1))) {
                                coloring->color(chainIter.getNext(), BLACK);
                            }
                            else if (chainNodePos == 1 || chainNodePos == (watchPos - 1) || chainNodePos == (watchPos + blackCount - 1)) {
                                coloring->color(chainIter.getNext(), GRAY);
                            }
                            else {
                                if (whiteCount > 0) {
                                    coloring->color(chainIter.getNext(), WHITE);
                                    --whiteCount;
                                }
                                else {
                                    coloring->color(chainIter.getNext(), GRAY);
                                }
                            }

                            ++chainNodePos;
                        }
                    }
                }

                return coloring;
            }

            // root имеет белый цвет.
            if (watchColor == BLACK && rootColor == WHITE) {
                if (blackCount < (chainLength - watchPos)) {
                    chainNodePos = 0;

                    while (chainIter.hasNext()) {
                        if (chainNodePos < watchPos) {
                            if (whiteCount > 0) {
                                coloring->color(chainIter.getNext(), WHITE);
                                --whiteCount;
                            }
                            else {
                                coloring->color(chainIter.getNext(), GRAY);
                            }
                        }
                        else if (chainNodePos < watchPos + blackCount) {
                            coloring->color(chainIter.getNext(), BLACK);
                        }
                        else if (chainNodePos == watchPos + blackCount) {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                        else {
                            coloring->color(chainIter.getNext(), WHITE);
                        }

                        ++chainNodePos;
                    }
                }
                else {
                    chainNodePos = 0;

                    while (chainIter.hasNext()) {
                        if (chainNodePos < whiteCount) {
                            coloring->color(chainIter.getNext(), WHITE);
                        }
                        else if (chainNodePos < chainLength - blackCount) {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                        else {
                            coloring->color(chainIter.getNext(), BLACK);
                        }

                        ++chainNodePos;
                    }
                }

                return coloring;
            }

            // root имеет серый цвет.
            if (watchColor == BLACK && rootColor == GRAY) {
                if (blackCount > watchPos) {
                    coloring->color(chainIter.getNext(), GRAY);

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    if (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }

                    while (chainIter.hasNext()) {
                        if (whiteCount > 0) {
                            coloring->color(chainIter.getNext(), WHITE);
                            --whiteCount;
                        }
                        else {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                    }
                }
                else if (blackCount >= (chainLength - watchPos)) {
                    chainNodePos = 0;

                    while (chainIter.hasNext()) {
                        if (chainNodePos == 0 || chainNodePos == chainLength - blackCount) {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                        else if (chainNodePos < chainLength - blackCount) {
                            if (whiteCount > 0) {
                                coloring->color(chainIter.getNext(), WHITE);
                                --whiteCount;
                            }
                            else {
                                coloring->color(chainIter.getNext(), GRAY);
                            }
                        }
                        else {
                            coloring->color(chainIter.getNext(), BLACK);
                        }

                        ++chainNodePos;
                    }
                }
                else {
                    chainNodePos = 0;

                    while (chainIter.hasNext()) {
                        if (chainNodePos == 0 || chainNodePos == watchPos - 1 || chainNodePos == watchPos + blackCount) {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                        else if (chainNodePos < watchPos - 1 || chainNodePos > watchPos + blackCount) {
                            if (whiteCount > 0) {
                                coloring->color(chainIter.getNext(), WHITE);
                                --whiteCount;
                            }
                            else {
                                coloring->color(chainIter.getNext(), GRAY);
                            }
                        }
                        else {
                            coloring->color(chainIter.getNext(), BLACK);
                        }

                        ++chainNodePos;
                    }
                }

                return coloring;
            }

            /* ***** watch имеет белый цвет. ***** */

            // root имеет чёрный цвет.
            if (watchColor == WHITE && rootColor == BLACK) {
                if (blackCount < watchPos) {
                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    coloring->color(chainIter.getNext(), GRAY);

                    while (chainIter.hasNext()) {
                        if (whiteCount > 0) {
                            coloring->color(chainIter.getNext(), WHITE);
                            --whiteCount;
                        }
                        else {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                    }
                }
                else {
                    chainNode = chainIter.getNext();

                    while (chainNode != watch->getParent()) {
                        coloring->color(chainNode, BLACK);
                        chainNode = chainIter.getNext();
                        --blackCount;
                    }

                    coloring->color(chainNode, GRAY);

                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    coloring->color(chainIter.getNext(), GRAY);

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    while (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }

                return coloring;
            }

            // root имеет белый цвет.
            if (watchColor == WHITE && rootColor == WHITE) {
                if (watchPos == 0) {
                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    if (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    while (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }
                else {
                    if (blackCount == 0) {
                        while (whiteCount > 0) {
                            coloring->color(chainIter.getNext(), WHITE);
                            --whiteCount;
                        }

                        while (chainIter.hasNext()) {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                    }
                    else if (blackCount < (chainLength - (watchPos + 1))) {
                        chainNodePos = 0;

                        while ((chainNodePos + whiteCount - 1) < watchPos) {
                            coloring->color(chainIter.getNext(), GRAY);
                            ++chainNodePos;
                        }

                        while (whiteCount > 0) {
                            coloring->color(chainIter.getNext(), WHITE);
                            --whiteCount;
                        }

                        coloring->color(chainIter.getNext(), GRAY);

                        while (blackCount > 0) {
                            coloring->color(chainIter.getNext(), BLACK);
                            --blackCount;
                        }

                        while (chainIter.hasNext()) {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                    }
                    else if (blackCount < (watchPos - 2)) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                        coloring->color(chainIter.getNext(), GRAY);

                        while (blackCount > 0) {
                            coloring->color(chainIter.getNext(), BLACK);
                            --blackCount;
                        }

                        coloring->color(chainIter.getNext(), GRAY);

                        while (chainIter.hasNext()) {
                            if (whiteCount > 0) {
                                coloring->color(chainIter.getNext(), WHITE);
                                --whiteCount;
                            }
                            else {
                                coloring->color(chainIter.getNext(), GRAY);
                            }
                        }
                    }
                    else {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;

                        coloring->color(chainIter.getNext(), GRAY);

                        for (int i = 0; i < watchPos < 2; ++i) {
                            coloring->color(chainIter.getNext(), BLACK);
                            --blackCount;
                        }

                        coloring->color(chainIter.getNext(), GRAY);

                        while (whiteCount > 0) {
                            coloring->color(chainIter.getNext(), WHITE);
                            --whiteCount;
                        }

                        coloring->color(chainIter.getNext(), GRAY);

                        while (blackCount > 0) {
                            coloring->color(chainIter.getNext(), BLACK);
                            --blackCount;
                        }

                        while (chainIter.hasNext()) {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                    }
                }

                return coloring;
            }

            // root имеет серый цвет.
            if (watchColor == WHITE && rootColor == GRAY) {
                int topLength = std::max(0, watchPos - 2);
                int bottomLength = std::max(0, chainLength - watchPos - 2);

                if (blackCount <= topLength) {
                    coloring->color(chainIter.getNext(), GRAY);

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    coloring->color(chainIter.getNext(), GRAY);
                    chainNodePos = blackCount + 2;

                    while ((chainNodePos + whiteCount - 1) < watchPos) {
                        coloring->color(chainIter.getNext(), GRAY);
                        ++chainNodePos;
                    }

                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    while (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }
                else if (blackCount <= bottomLength) {
                    coloring->color(chainIter.getNext(), GRAY);
                    chainNodePos = 1;

                    while ((chainNodePos + whiteCount - 1) < watchPos) {
                        coloring->color(chainIter.getNext(), GRAY);
                        ++chainNodePos;
                    }

                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    coloring->color(chainIter.getNext(), GRAY);

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    while (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }
                else {
                    coloring->color(chainIter.getNext(), GRAY);
                    chainNode = chainIter.getNext();

                    while (chainNode != watch->getParent()) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    coloring->color(chainNode, GRAY);
                    coloring->color(chainNode = chainIter.getNext(), WHITE);
                    --whiteCount;

                    if (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    if (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    while (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }

                return coloring;
            }

            /* ***** watch имеет серый цвет. ***** */

            // root имеет чёрный цвет.
            if (watchColor == GRAY && rootColor == BLACK) {
                if (blackCount <= watchPos) {
                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    chainNode = chainIter.getNext();
                    coloring->color(chainNode, GRAY);

                    if (chainNode != watch) {
                        chainNode = chainIter.getNext();

                        while (chainNode != watch) {
                            if (whiteCount > 0) {
                                coloring->color(chainIter.getNext(), WHITE);
                                --whiteCount;
                            }
                            else {
                                coloring->color(chainIter.getNext(), GRAY);
                            }

                            chainNode = chainIter.getNext();
                        }
                    }

                    coloring->color(chainNode, GRAY);

                    while (chainIter.hasNext()) {
                        if (whiteCount > 0) {
                            coloring->color(chainIter.getNext(), WHITE);
                            --whiteCount;
                        }
                        else {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                    }
                }
                else {
                    chainNode = chainIter.getNext();

                    while (chainNode != watch) {
                        coloring->color(chainNode, BLACK);
                        --blackCount;
                        chainNode = chainIter.getNext();
                    }

                    coloring->color(chainNode, GRAY);

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    if (chainIter.hasNext()) {
                        coloring->color(chainNode, GRAY);
                    }

                    while (chainIter.hasNext()) {
                        if (whiteCount > 0) {
                            coloring->color(chainIter.getNext(), WHITE);
                            --whiteCount;
                        }
                        else {
                            coloring->color(chainIter.getNext(), GRAY);
                        }
                    }
                }

                return coloring;
            }

            // root имеет белый цвет.
            if (watchColor == GRAY && rootColor == WHITE) {
                chainNode = chainIter.getNext();

                while (chainNode != watch) {
                    if (whiteCount > 0) {
                        coloring->color(chainNode, WHITE);
                        --whiteCount;
                        chainNode = chainIter.getNext();
                    }
                    else {
                        coloring->color(chainNode, GRAY);
                        chainNode = chainIter.getNext();

                        while (chainNode != watch) {
                            coloring->color(chainNode, BLACK);
                            --blackCount;
                            chainNode = chainIter.getNext();
                        }
                    }
                }

                coloring->color(chainNode, GRAY);

                while (whiteCount > 0) {
                    coloring->color(chainIter.getNext(), WHITE);
                    --whiteCount;
                }

                if (chainIter.hasNext()) {
                    coloring->color(chainIter.getNext(), GRAY);
                }

                while (blackCount > 0) {
                    coloring->color(chainIter.getNext(), BLACK);
                    --blackCount;
                }

                return coloring;
            }

            // root имеет серый цвет.
            if (watchColor == GRAY && rootColor == GRAY) {
                int topLength = std::max(watchPos - 1, 0);
                int bottomLength = chainLength - watchPos - 1;

                if (watchPos == 0) {
                    coloring->color(chainIter.getNext(), GRAY);

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    if (blackCount > 0 && chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }

                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    while (chainIter.getNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }
                else if (blackCount == topLength) {
                    coloring->color(chainIter.getNext(), GRAY);

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    coloring->color(chainIter.getNext(), GRAY);

                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    while (chainIter.getNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }
                else if (blackCount == bottomLength) {
                    coloring->color(chainIter.getNext(), GRAY);

                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    if (watchPos != 0) {
                        chainNode = chainIter.getNext();

                        while (chainNode != watch) {
                            coloring->color(chainNode, GRAY);
                            chainNode = chainIter.getNext();
                        }

                        coloring->color(chainNode, GRAY);
                    }

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    while (chainIter.getNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }
                else if (blackCount < topLength) {
                    coloring->color(chainIter.getNext(), GRAY);

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    chainNode = chainIter.getNext();
                    coloring->color(chainNode, GRAY);
                    chainNode = chainIter.getNext();

                    while (chainNode != watch) {
                        coloring->color(chainNode, WHITE);
                        --whiteCount;
                    }

                    coloring->color(chainNode, GRAY);

                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    while (chainIter.getNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }
                else if (blackCount < bottomLength) {
                    chainNode = chainIter.getNext();
                    coloring->color(chainNode, GRAY);
                    chainNode = chainIter.getNext();

                    while (chainNode != watch) {
                        coloring->color(chainNode, WHITE);
                        --whiteCount;
                        chainNode = chainIter.getNext();
                    }

                    coloring->color(chainNode, GRAY);

                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    if (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    while (chainIter.getNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }
                else {
                    chainNode = chainIter.getNext();
                    coloring->color(chainNode, GRAY);
                    chainNode = chainIter.getNext();

                    while (chainNode != watch) {
                        coloring->color(chainNode, BLACK);
                        --blackCount;
                    }

                    coloring->color(chainNode, GRAY);

                    while (blackCount > 0) {
                        coloring->color(chainIter.getNext(), BLACK);
                        --blackCount;
                    }

                    if (chainIter.hasNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }

                    while (whiteCount > 0) {
                        coloring->color(chainIter.getNext(), WHITE);
                        --whiteCount;
                    }

                    while (chainIter.getNext()) {
                        coloring->color(chainIter.getNext(), GRAY);
                    }
                }

                return coloring;
            }
        }

    private:

        void buildMaxWhiteMaps() {
            /* ***** watch имеет чёрный цвет. ***** */
            std::shared_ptr<ColorMaxWhiteMap>& watchBlackMaxWhiteMap = maxWhiteMaps[BLACK];
            watchBlackMaxWhiteMap.reset(new ColorMaxWhiteMap(chainLength));

            // root имеет чёрный цвет.
            watchBlackMaxWhiteMap->put(BLACK, 0, INVALID_MAX_WHITE);

            if (watchPos != 0) {
                watchBlackMaxWhiteMap->put(BLACK, 1, INVALID_MAX_WHITE);
            }

            for (int b = (watchPos == 0 ? 1 : 2); b <= chainLength; ++b) {
                if (b > watchPos) {
                    watchBlackMaxWhiteMap->put(BLACK, b, std::max(0, chainLength - (b + 1)));
                }
                else if (b > (chainLength - watchPos)) {
                    watchBlackMaxWhiteMap->put(BLACK, b, std::max(0, chainLength - (b + 2)));
                }
                else {
                    watchBlackMaxWhiteMap->put(
                        BLACK,
                        b,
                        std::max(
                            std::max(0, watchPos - b - 1) + std::max(0, chainLength - (watchPos + 2)),
                            std::max(0, watchPos - 3) + std::max(0, chainLength - watchPos - b)));
                }
            }

            // root имеет белый цвет.
            if (watchPos < 2) {
                for (int b = 0; b <= chainLength; ++b) {
                    watchBlackMaxWhiteMap->put(WHITE, b, INVALID_MAX_WHITE);
                }
            }
            else {
                watchBlackMaxWhiteMap->put(WHITE, 0, INVALID_MAX_WHITE);
                watchBlackMaxWhiteMap->put(WHITE, chainLength - 1, INVALID_MAX_WHITE);
                watchBlackMaxWhiteMap->put(WHITE, chainLength, INVALID_MAX_WHITE);

                for (int b = 1; b < chainLength - 1; ++b) {
                    if (b < (chainLength - watchPos)) {
                        watchBlackMaxWhiteMap->put(WHITE, b, std::max(0, watchPos - 1) + std::max(0, chainLength - watchPos - b - 1));
                    }
                    else {
                        watchBlackMaxWhiteMap->put(WHITE, b, std::max(0, chainLength - b - 1));
                    }
                }
            }

            // root имеет серый цвет.
            if (root == watch) {
                for (int b = 0; b <= chainLength; ++b) {
                    watchBlackMaxWhiteMap->put(GRAY, b, INVALID_MAX_WHITE);
                }
            }
            else {
                watchBlackMaxWhiteMap->put(GRAY, 0, INVALID_MAX_WHITE);
                watchBlackMaxWhiteMap->put(GRAY, chainLength, INVALID_MAX_WHITE);

                for (int b = 1; b < chainLength; ++b) {
                    if (b >= watchPos) {
                        watchBlackMaxWhiteMap->put(GRAY, b, std::max(0, chainLength - b - 2));
                    }
                    else if (b >= (chainLength - watchPos)) {
                        watchBlackMaxWhiteMap->put(GRAY, b, std::max(0, chainLength - b - 2));
                    }
                    else {
                        watchBlackMaxWhiteMap->put(GRAY, b, std::max(0, watchPos - 2) + std::max(0, chainLength - watchPos - b - 1));
                    }
                }
            }

            /* ***** watch имеет белый цвет. ***** */
            std::shared_ptr<ColorMaxWhiteMap>& watchWhiteMaxWhiteMap = maxWhiteMaps[WHITE];
            watchWhiteMaxWhiteMap.reset(new ColorMaxWhiteMap(chainLength));

            // root имеет чёрный цвет.
            if (watchPos < 2) {
                for (int b = 0; b <= chainLength; ++b) {
                    watchWhiteMaxWhiteMap->put(BLACK, b, INVALID_MAX_WHITE);
                }
            }
            else {
                watchWhiteMaxWhiteMap->put(BLACK, 0, INVALID_MAX_WHITE);
                watchWhiteMaxWhiteMap->put(BLACK, chainLength - 1, INVALID_MAX_WHITE);
                watchWhiteMaxWhiteMap->put(BLACK, chainLength, INVALID_MAX_WHITE);

                if (watchPos != chainLength - 1) {
                    watchWhiteMaxWhiteMap->put(BLACK, chainLength - 2, INVALID_MAX_WHITE);
                }

                int bUp = watchPos < chainLength - 1 ? chainLength - 2 : chainLength - 1;

                for (int b = 1; b < bUp; ++b) {
                    if (b < watchPos) {
                        watchWhiteMaxWhiteMap->put(BLACK, b, chainLength - b - 1);
                    }
                    else {
                        watchWhiteMaxWhiteMap->put(BLACK, b, chainLength - b - 2);
                    }
                }
            }

            // root имеет белый цвет.
            if (watchPos == 0) {
                if (chainLength > 1) {
                    watchWhiteMaxWhiteMap->put(WHITE, chainLength - 1, INVALID_MAX_WHITE);
                }

                watchWhiteMaxWhiteMap->put(WHITE, 0, chainLength);
                watchWhiteMaxWhiteMap->put(WHITE, chainLength, INVALID_MAX_WHITE);

                for (int b = 1; b < chainLength - 1; ++b) {
                    watchWhiteMaxWhiteMap->put(WHITE, b, chainLength - b - 1);
                }
            }
            else {
                int bUp = std::max(0, watchPos - 3) + std::max(0, chainLength - watchPos - 2) + 1;
                watchWhiteMaxWhiteMap->put(WHITE, 0, chainLength);

                for (int b = 1; b < bUp; ++b) {
                    if (b < (chainLength - (watchPos + 1))) {
                        watchWhiteMaxWhiteMap->put(WHITE, b, chainLength - b - 1);
                    }
                    else if (b < (watchPos - 2)) {
                        watchWhiteMaxWhiteMap->put(WHITE, b, chainLength - b - 2);
                    }
                    else {
                        watchWhiteMaxWhiteMap->put(WHITE, b, chainLength - b - 3);
                    }
                }

                for (int b = bUp; b <= chainLength; ++b) {
                    watchWhiteMaxWhiteMap->put(WHITE, b, INVALID_MAX_WHITE);
                }
            }

            // root имеет серый цвет.
            if (watchPos == 0) {
                for (int b = 0; b <= chainLength; ++b) {
                    watchWhiteMaxWhiteMap->put(GRAY, b, INVALID_MAX_WHITE);
                }
            }
            else {
                int bUp;

                if (watchPos == 1 || watchPos == (chainLength - 1)) {
                    if (chainLength > 2) {
                        watchWhiteMaxWhiteMap->put(GRAY, chainLength - 2, INVALID_MAX_WHITE);
                        bUp = chainLength - 2;
                    }
                    else {
                        bUp = chainLength - 1;
                    }
                }
                else {
                    if (chainLength > 3) {
                        watchWhiteMaxWhiteMap->put(GRAY, chainLength - 3, INVALID_MAX_WHITE);
                        bUp = chainLength - 3;
                    }
                    else {
                        bUp = chainLength - 2;
                    }
                }

                watchWhiteMaxWhiteMap->put(GRAY, chainLength - 1, INVALID_MAX_WHITE);
                watchWhiteMaxWhiteMap->put(GRAY, chainLength, INVALID_MAX_WHITE);

                int topLength = std::max(0, watchPos - 2);
                int bottomLength = std::max(0, chainLength - watchPos - 2);

                for (int b = 0; b < bUp; ++b) {
                    if (b <= topLength) {
                        watchWhiteMaxWhiteMap->put(GRAY, b, chainLength - b - 2);
                    }
                    else if (b <= bottomLength) {
                        watchWhiteMaxWhiteMap->put(GRAY, b, chainLength - b - 2);
                    }
                    else {
                        watchWhiteMaxWhiteMap->put(GRAY, b, chainLength - b - 3);
                    }
                }
            }

            /* ***** watch имеет серый цвет. ***** */
            std::shared_ptr<ColorMaxWhiteMap>& watchGrayMaxWhiteMap = maxWhiteMaps[GRAY];
            watchGrayMaxWhiteMap.reset(new ColorMaxWhiteMap(chainLength));

            // root имеет чёрный цвет.
            if (watchPos == 0) {
                for (int b = 0; b <= chainLength; ++b) {
                    watchGrayMaxWhiteMap->put(BLACK, b, INVALID_MAX_WHITE);
                }
            }
            else {
                watchGrayMaxWhiteMap->put(BLACK, chainLength, INVALID_MAX_WHITE);

                for (int b = 0; b < chainLength; ++b) {
                    if (b <= watchPos) {
                        watchGrayMaxWhiteMap->put(BLACK, b, std::max(0, watchPos - b - 1) + (chainLength - watchPos - 1));
                    }
                    else {
                        watchGrayMaxWhiteMap->put(BLACK, b, std::max(0, chainLength - b - 2));
                    }
                }
            }

            // root имеет белый цвет.
            if (watchPos == 0) {
                for (int b = 0; b <= chainLength; ++b) {
                    watchGrayMaxWhiteMap->put(WHITE, b, INVALID_MAX_WHITE);
                }
            }
            else {
                int bottomLength = chainLength - watchPos - 1;
                int bUp = bottomLength + std::max(0, watchPos - 2) + 1;

                for (int b = 0; b < bUp; ++b) {
                    if (b == bottomLength) {
                        watchGrayMaxWhiteMap->put(WHITE, b, watchPos);
                    }
                    else {
                        watchGrayMaxWhiteMap->put(WHITE, b, chainLength - b - 2);
                    }
                }

                for (int b = bUp; b <= chainLength; ++b) {
                    watchGrayMaxWhiteMap->put(WHITE, b, INVALID_MAX_WHITE);
                }
            }

            // root имеет серый цвет.
            if (watchPos == 0) {
                watchGrayMaxWhiteMap->put(GRAY, 0, chainLength - 1);
                watchGrayMaxWhiteMap->put(GRAY, chainLength, INVALID_MAX_WHITE);

                for (int b = 1; b < chainLength; ++b) {
                    watchGrayMaxWhiteMap->put(GRAY, b, std::max(0, chainLength - b - 2));
                }
            }
            else {
                watchGrayMaxWhiteMap->put(GRAY, chainLength - 1, INVALID_MAX_WHITE);
                watchGrayMaxWhiteMap->put(GRAY, chainLength, INVALID_MAX_WHITE);

                int topLength = watchPos - 1;
                int bottomLength = chainLength - watchPos - 1;

                for (int b = 0; b < chainLength - 1; ++b) {
                    if (b == topLength) {
                        watchGrayMaxWhiteMap->put(GRAY, b, bottomLength);
                    }
                    else if (b == bottomLength) {
                        watchGrayMaxWhiteMap->put(GRAY, b, topLength);
                    }
                    else if (b < topLength) {
                        watchGrayMaxWhiteMap->put(GRAY, b, bottomLength + std::max(0, topLength - b - 1));
                    }
                    else if (b < bottomLength) {
                        watchGrayMaxWhiteMap->put(GRAY, b, topLength + std::max(0, bottomLength - b - 1));
                    }
                    else {
                        watchGrayMaxWhiteMap->put(GRAY, b, std::max(0, chainLength - b - 3));
                    }
                }
            }
        }

    private:

        NodeType* root;
        NodeType* watch;
        int watchPos;
        bool rootToWatchForward;
        bool rootToWatchForwardLeft;
        bool watchToLeafForward;
        bool watchToLeafForwardLeft;
        int chainLength;
        std::shared_ptr<ColorMaxWhiteMap> maxWhiteMaps[COLOR_COUNT];
    };

    /**
     * Задача типа "Head".
     */
    class HeadSolution : public Solution {

    public:

        HeadSolution(const std::shared_ptr<ChainSolution>& newChainSolution) : chainSolution(newChainSolution) {}

        HeadSolution(
            const std::shared_ptr<ChainSolution>& newChainSolution,
            const std::shared_ptr<Solution>& newBottomSolution,
            NodeType* newConnectiveNode,
            NodeType* newConnectiveNodePrev,
            const bool newIsConnectiveNodeWatch) :

            chainSolution(newChainSolution),
            bottomSolution(newBottomSolution),
            connectiveNode(newConnectiveNode),
            connectiveNodePrev(newConnectiveNodePrev),
            isConnectiveNodeWatch(newIsConnectiveNodeWatch) {}

        const std::shared_ptr<ColorMaxWhiteMap>& getMaxWhiteMap() {
            if (!maxWhiteMap) {
                buildMaxWhiteMap();
            }

            return maxWhiteMap;
        }

        ColoringType* getColoring(ColoringType*& coloring, int blackCount, int whiteCount) {
            if (!maxWhiteMap) {
                buildMaxWhiteMap();
            }

            ColorType watchColor = BLACK;
            int maxWhite = maxWhiteMap->get(BLACK, blackCount);

            if (maxWhiteMap->get(WHITE, blackCount) > maxWhite) {
                watchColor = WHITE;
                maxWhite = maxWhiteMap->get(WHITE, blackCount);
            }

            if (maxWhiteMap->get(GRAY, blackCount) > maxWhite) {
                watchColor = GRAY;
                maxWhite = maxWhiteMap->get(GRAY, blackCount);
            }

            return getColoring(coloring, blackCount, whiteCount, watchColor);
        }

        ColoringType* getColoring(ColoringType*& coloring, int blackCount, int whiteCount, const ColorType watchColor) {
            if (!maxWhiteMap) {
                buildMaxWhiteMap();
            }

            if (!bottomSolution) {
                return chainSolution->getColoring(coloring, blackCount, whiteCount, watchColor);
            }

            if (watchColor == BLACK && isConnectiveNodeWatch) {
                ++blackCount;
            }

            const std::shared_ptr<ColorMaxWhiteMap>& watchMaxWhiteMap = watchBottomMaxWhiteMaps[watchColor];
            ColorType rootColor = BLACK;
            int rootMaxWhite = watchMaxWhiteMap->get(BLACK, blackCount);

            if (watchMaxWhiteMap->get(WHITE, blackCount) > rootMaxWhite) {
                rootColor = WHITE;
                rootMaxWhite = watchMaxWhiteMap->get(WHITE, blackCount);
            }

            if (watchMaxWhiteMap->get(GRAY, blackCount) > rootMaxWhite) {
                rootColor = GRAY;
                rootMaxWhite = watchMaxWhiteMap->get(GRAY, blackCount);
            }

            // Вероятно, здесь ошибка: ++blackCount; должно быть для root.

            for (int i = 0; i <= blackCount; ++i) {
                int chainMaxWhite = watchMaxWhiteMap->get(rootColor, i);
                int bottomMaxWhite = bottomSolution->getMaxWhiteMap()->get(rootColor, blackCount - i);

                if (chainMaxWhite != INVALID_MAX_WHITE && bottomMaxWhite != INVALID_MAX_WHITE && (chainMaxWhite + bottomMaxWhite >= whiteCount)) {
                    // Chain.
                    chainSolution->getColoring(coloring, i, chainMaxWhite, rootColor, watchColor);

                    // Bottom.
                    bool forward;
                    int childPos;

                    if (connectiveNodePrev == connectiveNode->getParent()) {
                        forward = true;
                        childPos = connectiveNodePrev->getChildPosition(connectiveNode);
                        connectiveNodePrev->removeChild(connectiveNode);
                    }
                    else {
                        forward = false;
                        childPos = connectiveNode->getChildPosition(connectiveNodePrev);
                        connectiveNode->removeChild(connectiveNodePrev);
                    }

                    bottomSolution->getColoring(coloring, blackCount - i, bottomMaxWhite, rootColor);

                    if (forward) {
                        connectiveNodePrev->addChild(connectiveNode, childPos);
                    }
                    else {
                        connectiveNode->addChild(connectiveNodePrev, childPos);
                    }

                    return coloring;
                }

                chainMaxWhite = watchMaxWhiteMap->get(rootColor, blackCount - i);
                bottomMaxWhite = bottomSolution->getMaxWhiteMap()->get(rootColor, i);

                if (chainMaxWhite != INVALID_MAX_WHITE && bottomMaxWhite != INVALID_MAX_WHITE && (chainMaxWhite + bottomMaxWhite >= whiteCount)) {
                    // Chain.
                    chainSolution->getColoring(coloring, blackCount - i, chainMaxWhite, rootColor, watchColor);

                    // Bottom.
                    bool forward;
                    int childPos;

                    if (connectiveNodePrev == connectiveNode->getParent()) {
                        forward = true;
                        childPos = connectiveNodePrev->getChildPosition(connectiveNode);
                        connectiveNodePrev->removeChild(connectiveNode);
                    }
                    else {
                        forward = false;
                        childPos = connectiveNode->getChildPosition(connectiveNodePrev);
                        connectiveNode->removeChild(connectiveNodePrev);
                    }

                    bottomSolution->getColoring(coloring, i, bottomMaxWhite, rootColor);

                    if (forward) {
                        connectiveNodePrev->addChild(connectiveNode, childPos);
                    }
                    else {
                        connectiveNode->addChild(connectiveNodePrev, childPos);
                    }

                    return coloring;
                }
            }
        }

    private:

        void buildMaxWhiteMap() {
            if (bottomSolution) {
                watchBottomMaxWhiteMaps[BLACK].reset(chainSolution->getMaxWhiteMap(BLACK)->makeUnion(bottomSolution->getMaxWhiteMap().get()));
                qDebug() << "watchBottomMaxWhiteMaps[BLACK]";
                //watchBottomMaxWhiteMaps[BLACK]->print();
                watchBottomMaxWhiteMaps[BLACK]->print();

                watchBottomMaxWhiteMaps[WHITE].reset(chainSolution->getMaxWhiteMap(WHITE)->makeUnion(bottomSolution->getMaxWhiteMap().get()));
                qDebug() << "watchBottomMaxWhiteMaps[WHITE]";
                watchBottomMaxWhiteMaps[WHITE]->print();

                watchBottomMaxWhiteMaps[GRAY].reset(chainSolution->getMaxWhiteMap(GRAY)->makeUnion(bottomSolution->getMaxWhiteMap().get()));
                qDebug() << "watchBottomMaxWhiteMaps[GRAY]";
                watchBottomMaxWhiteMaps[GRAY]->print();

                int nodeCount = watchBottomMaxWhiteMaps[BLACK]->getNodeCount();
                maxWhiteMap.reset(new ColorMaxWhiteMap(nodeCount));

                for (int b = 0; b <= nodeCount; ++b) {
                    maxWhiteMap->put(BLACK, b, watchBottomMaxWhiteMaps[BLACK]->get(b));
                    maxWhiteMap->put(WHITE, b, watchBottomMaxWhiteMaps[WHITE]->get(b));
                    maxWhiteMap->put(GRAY, b, watchBottomMaxWhiteMaps[GRAY]->get(b));
                }
            }
            else {
                maxWhiteMap.reset(new ColorMaxWhiteMap(chainSolution->getChainLength()));

                for (int b = 0; b <= chainSolution->getChainLength(); ++b) {
                    maxWhiteMap->put(BLACK, b, chainSolution->getMaxWhiteMap(BLACK)->get(b));
                    maxWhiteMap->put(WHITE, b, chainSolution->getMaxWhiteMap(WHITE)->get(b));
                    maxWhiteMap->put(GRAY, b, chainSolution->getMaxWhiteMap(GRAY)->get(b));
                }
            }
        }

    private:

        std::shared_ptr<ChainSolution> chainSolution;
        std::shared_ptr<Solution> bottomSolution;
        NodeType* connectiveNode;
        NodeType* connectiveNodePrev;
        bool isConnectiveNodeWatch;
        std::shared_ptr<ColorMaxWhiteMap> watchBottomMaxWhiteMaps[COLOR_COUNT];
        std::shared_ptr<ColorMaxWhiteMap> maxWhiteMap;
    };

    class HeadProblem : public Problem {

    public:

        HeadProblem(
            NodeType* newRoot,
            NodeType* newWatch,
            bool newRootToWatchForward,
            bool newRootToWatchForwardLeft,
            const TreeInfo& newTreeInfo) :

            Problem(newRoot, newWatch, newTreeInfo),
            rootToWatchForward(newRootToWatchForward),
            rootToWatchForwardLeft(newRootToWatchForwardLeft) {}

        const std::shared_ptr<Solution>& solve() {
            if (solution) {
                return solution;
            }

            TrunkIterator trunkIter(this->root, this->watch, rootToWatchForward, rootToWatchForwardLeft, true, true);
            int watchPos = 0;
            NodeType* prev = trunkIter.getNext();
            int length = 1;

            while (trunkIter.hasNext()) {
                NodeType* current = trunkIter.getNext();
                ++length;

                if (current == this->watch) {
                    watchPos = length - 1;
                }

                if ((!rootToWatchForward && current == this->watch && current->getChildCount() > 2) ||
                    (!current->isLeaf() && !current->hasSingleChild())) {

                    std::shared_ptr<ChainSolution> chainSolution =
                        std::make_shared<ChainSolution>(
                            current,
                            this->watch,
                            length - watchPos - 1,
                            false,
                            false,
                            !rootToWatchForward,
                            false,
                            length);

                    // --
                    qDebug() << "HEAD CHAIN";
                    chainSolution->getMaxWhiteMap(BLACK)->print();
                    // --

                    bool isPrevParentForCurrent = false;
                    int childPos;

                    if (current->getParent() == prev) {
                        isPrevParentForCurrent = true;
                        childPos = prev->getChildPosition(current);
                        prev->removeChild(current);
                    }
                    else {
                        childPos = current->getChildPosition(prev);
                        current->removeChild(prev);
                    }

                    std::unique_ptr<Problem> bottomProblem;

                    if (current->getChildCount() > (!rootToWatchForward && current == this->watch ? 3 : 2)) {
                        bottomProblem.reset(new BranchProblem(this->treeInfo.tree->getRightmostAncestor(current), current, false, false, this->treeInfo));
                    }
                    else {
                        bottomProblem.reset(new BetweenProblem(this->treeInfo.tree->getRightmostAncestor(current), current, false, false, this->treeInfo));
                    }

                    solution.reset(new HeadSolution(chainSolution, bottomProblem->solve(), current, prev, current == this->watch));

                    if (isPrevParentForCurrent) {
                        prev->addChild(current, childPos);
                    }
                    else {
                        current->addChild(prev, childPos);
                    }

                    break;
                }

                prev = current;
            }

            if (!solution) {
                std::shared_ptr<ChainSolution> chainSolution =
                    std::make_shared<ChainSolution>(
                        this->root,
                        this->watch,
                        watchPos,
                        rootToWatchForward,
                        rootToWatchForwardLeft,
                        true,
                        true,
                        length);
                solution.reset(new HeadSolution(chainSolution));
            }

            // --
            qDebug() << "HEAD";
            solution->getMaxWhiteMap()->print();
            // --

            return solution;
        }

    private:

        bool rootToWatchForward;
        bool rootToWatchForwardLeft;
        std::shared_ptr<Solution> solution;
    };

    /**
     * Задача типа "Branch".
     */
    class BranchSolution : public Solution {

    public:

        BranchSolution(
            const std::shared_ptr<Solution>& newTopSolution,
            const std::shared_ptr<Solution>& newBottomSolution,
            NodeType* newWatch,
            const bool newRootToWatchForward,
            const bool newRootToWatchForwardLeft) :

            topSolution(newTopSolution),
            bottomSolution(newBottomSolution),
            watch(newWatch),
            rootToWatchForward(newRootToWatchForward),
            rootToWatchForwardLeft(newRootToWatchForwardLeft) {}

        const std::shared_ptr<ColorMaxWhiteMap>& getMaxWhiteMap() {
            if (!maxWhiteMap) {
                buildMaxWhiteMap();
            }

            return maxWhiteMap;
        }

        ColoringType* getColoring(ColoringType*& coloring, int blackCount, int whiteCount) {
            if (!maxWhiteMap) {
                buildMaxWhiteMap();
            }

            ColorType watchColor = BLACK;
            int maxWhite = maxWhiteMap->get(BLACK, blackCount);

            if (maxWhiteMap->get(WHITE, blackCount) > maxWhite) {
                watchColor = WHITE;
                maxWhite = maxWhiteMap->get(WHITE, blackCount);
            }

            if (maxWhiteMap->get(GRAY, blackCount) > maxWhite) {
                watchColor = GRAY;
                maxWhite = maxWhiteMap->get(GRAY, blackCount);
            }

            return getColoring(coloring, blackCount, whiteCount, watchColor);
        }

        ColoringType* getColoring(ColoringType*& coloring, int blackCount, int whiteCount, const ColorType watchColor) {
            if (!maxWhiteMap) {
                buildMaxWhiteMap();
            }

            if (watchColor == BLACK) {
                ++blackCount;
            }

            for (int i = 0; i <= blackCount; ++i) {
                int topMaxWhite = topSolution->getMaxWhiteMap()->get(watchColor, i);
                int bottomMaxWhite = bottomSolution->getMaxWhiteMap()->get(watchColor, blackCount - i);

                if (topMaxWhite != INVALID_MAX_WHITE && bottomMaxWhite != INVALID_MAX_WHITE && (topMaxWhite + bottomMaxWhite >= whiteCount)) {
                    // Top.
                    this->watch->printChildren();

                    std::vector<NodeType*> watchChildren;
                    int removingWatchChildIndexUp = rootToWatchForward ? watch->getChildCount() : watch->getChildCount() - 1;

                    for (int i = 0; i < removingWatchChildIndexUp; ++i) {
                        watchChildren.push_back(watch->getChild(i));
                    }

                    foreach (auto watchChild, watchChildren) {
                        watch->removeChild(watchChild);
                    }

                    topSolution->getColoring(coloring, i, topMaxWhite, watchColor);
                    NodeType* watchRightmostChild = !this->watch->isLeaf() ? this->watch->getRightmostChild() : 0;

                    if (watchRightmostChild != 0) {
                        this->watch->removeChild(watchRightmostChild);
                    }

                    for (int i = 0; i < watchChildren.size(); ++i) {
                        watch->addChild(watchChildren[i]);
                    }

                    if (watchRightmostChild != 0) {
                        this->watch->addChild(watchRightmostChild);
                    }

                    this->watch->printChildren();

                    // Bottom.
                    NodeType* watchParent;
                    int watchChildPos;

                    if (rootToWatchForward) {
                        watchParent = watch->getParent();
                        watchChildPos = watchParent->getChildPosition(watch);
                        watchParent->removeChild(watch);
                    }
                    else {
                        watchRightmostChild = watch->getRightmostChild();
                        watch->removeChild(watchRightmostChild);
                    }

                    this->watch->printChildren();

                    bottomSolution->getColoring(coloring, blackCount - i, bottomMaxWhite, watchColor);

                    if (rootToWatchForward) {
                        watchParent->addChild(watch, watchChildPos);
                    }
                    else {
                        watch->addChild(watchRightmostChild);
                    }

                    this->watch->printChildren();

                    return coloring;
                }

                topMaxWhite = topSolution->getMaxWhiteMap()->get(watchColor, blackCount - i);
                bottomMaxWhite = bottomSolution->getMaxWhiteMap()->get(watchColor, i);

                if (topMaxWhite != INVALID_MAX_WHITE && bottomMaxWhite != INVALID_MAX_WHITE && (topMaxWhite + bottomMaxWhite >= whiteCount)) {
                    // Top.
                    std::vector<NodeType*> watchChildren;
                    int removingWatchChildIndexUp = rootToWatchForward ? watch->getChildCount() : watch->getChildCount() - 1;

                    for (int i = 0; i < removingWatchChildIndexUp; ++i) {
                        watchChildren.push_back(watch->getChild(i));
                    }

                    foreach (auto watchChild, watchChildren) {
                        watch->removeChild(watchChild);
                    }

                    topSolution->getColoring(coloring, blackCount - i, topMaxWhite, watchColor);

                    NodeType* watchRightmostChild = !this->watch->isLeaf() ? this->watch->getRightmostChild() : 0;

                    if (watchRightmostChild != 0) {
                        this->watch->removeChild(watchRightmostChild);
                    }

                    for (int i = 0; i < watchChildren.size(); ++i) {
                        watch->addChild(watchChildren[i]);
                    }

                    if (watchRightmostChild != 0) {
                        this->watch->addChild(watchRightmostChild);
                    }

                    // Bottom.
                    NodeType* watchParent;
                    int watchChildPos;

                    if (rootToWatchForward) {
                        watchParent = watch->getParent();
                        watchChildPos = watchParent->getChildPosition(watch);
                        watchParent->removeChild(watch);
                    }
                    else {
                        watchRightmostChild = watch->getRightmostChild();
                        watch->removeChild(watchRightmostChild);
                    }

                    bottomSolution->getColoring(coloring, i, bottomMaxWhite, watchColor);

                    if (rootToWatchForward) {
                        watchParent->addChild(watch, watchChildPos);
                    }
                    else {
                        watch->addChild(watchRightmostChild);
                    }

                    return coloring;
                }
            }
        }

    private:

        void buildMaxWhiteMap() {
            maxWhiteMap.reset(topSolution->getMaxWhiteMap()->makeUnion(bottomSolution->getMaxWhiteMap().get()));
        }

    private:

        std::shared_ptr<Solution> topSolution;
        std::shared_ptr<Solution> bottomSolution;
        NodeType* watch;
        bool rootToWatchForward;
        bool rootToWatchForwardLeft;
        std::shared_ptr<ColorMaxWhiteMap> maxWhiteMap;
    };

    class BranchProblem : public Problem {

    public:

        BranchProblem(
            NodeType* newRoot,
            NodeType* newWatch,
            const bool newRootToWatchForward,
            const bool newRootToWatchForwardLeft,
            const TreeInfo& newTreeInfo) :

            Problem(newRoot, newWatch, newTreeInfo),
            rootToWatchForward(newRootToWatchForward),
            rootToWatchForwardLeft(newRootToWatchForwardLeft) {}

        const std::shared_ptr<Solution>& solve() {
            if (solution) {
                return solution;
            }

            // Top.
            std::vector<NodeType*> watchChildren;
            int removingWatchChildIndexUp = rootToWatchForward ? this->watch->getChildCount() : this->watch->getChildCount() - 1;

            for (int i = 0; i < removingWatchChildIndexUp; ++i) {
                watchChildren.push_back(this->watch->getChild(i));
            }

            foreach (auto watchChild, watchChildren) {
                this->watch->removeChild(watchChild);
            }

            std::shared_ptr<Solution> topSolution(TailProblem(this->root, this->watch, rootToWatchForward, rootToWatchForwardLeft, this->treeInfo).solve());
            NodeType* watchRightmostChild = !this->watch->isLeaf() ? this->watch->getRightmostChild() : 0;

            if (watchRightmostChild != 0) {
                this->watch->removeChild(watchRightmostChild);
            }

            for (int i = 0; i < watchChildren.size(); ++i) {
                this->watch->addChild(watchChildren[i]);
            }

            if (watchRightmostChild != 0) {
                this->watch->addChild(watchRightmostChild);
            }

            // Bottom.
            NodeType* watchParent;
            int watchChildPos;

            if (rootToWatchForward) {
                watchParent = this->watch->getParent();
                watchChildPos = watchParent->getChildPosition(this->watch);
                watchParent->removeChild(this->watch);
            }
            else {
                watchRightmostChild = this->watch->getRightmostChild();
                this->watch->removeChild(watchRightmostChild);
            }

            std::shared_ptr<Solution> bottomSolution;

            if (this->watch->getChildCount() > (rootToWatchForward ? 2 : 3)) {
                bottomSolution = BranchProblem(this->treeInfo.tree->getRightmostAncestor(this->watch), this->watch, false, false, this->treeInfo).solve();
            }
            else {
                bottomSolution = BetweenProblem(this->treeInfo.tree->getRightmostAncestor(this->watch), this->watch, false, false, this->treeInfo).solve();
            }

            if (rootToWatchForward) {
                watchParent->addChild(this->watch, watchChildPos);
            }
            else {
                this->watch->addChild(watchRightmostChild);
            }

            // Solution.
            solution.reset(new BranchSolution(topSolution, bottomSolution, this->watch, rootToWatchForward, rootToWatchForwardLeft));
            return solution;
        }

    private:

        bool rootToWatchForward;
        bool rootToWatchForwardLeft;
        std::shared_ptr<Solution> solution;
    };

    /**
     * Задача типа "Between".
     */
    class BetweenSolution : public Solution {

    public:

        BetweenSolution(
            const std::shared_ptr<Solution>& newTopSolution,
            const std::shared_ptr<Solution>& newBottomSolution,
            NodeType* newWatch,
            const bool newRootToWatchForward,
            const bool newRootToWatchForwardLeft) :

            topSolution(newTopSolution),
            bottomSolution(newBottomSolution),
            watch(newWatch),
            rootToWatchForward(newRootToWatchForward),
            rootToWatchForwardLeft(newRootToWatchForwardLeft) {}

        const std::shared_ptr<ColorMaxWhiteMap>& getMaxWhiteMap() {
            if (!maxWhiteMap) {
                buildMaxWhiteMap();
            }

            return maxWhiteMap;
        }

        ColoringType* getColoring(ColoringType*& coloring, int blackCount, int whiteCount) {
            if (!maxWhiteMap) {
                buildMaxWhiteMap();
            }

            ColorType watchColor = BLACK;
            int maxWhite = maxWhiteMap->get(BLACK, blackCount);

            if (maxWhiteMap->get(WHITE, blackCount) > maxWhite) {
                watchColor = WHITE;
                maxWhite = maxWhiteMap->get(WHITE, blackCount);
            }

            if (maxWhiteMap->get(GRAY, blackCount) > maxWhite) {
                watchColor = GRAY;
                maxWhite = maxWhiteMap->get(GRAY, blackCount);
            }

            return getColoring(coloring, blackCount, whiteCount, watchColor);
        }

        ColoringType* getColoring(ColoringType*& coloring, int blackCount, int whiteCount, const ColorType watchColor) {
            if (!maxWhiteMap) {
                buildMaxWhiteMap();
            }

            if (watchColor == BLACK) {
                ++blackCount;
            }

            for (int i = 0; i <= blackCount; ++i) {
                int topMaxWhite = topSolution->getMaxWhiteMap()->get(watchColor, i);
                int bottomMaxWhite = bottomSolution->getMaxWhiteMap()->get(watchColor, blackCount - i);

                if (topMaxWhite != INVALID_MAX_WHITE && bottomMaxWhite != INVALID_MAX_WHITE && (topMaxWhite + bottomMaxWhite >= whiteCount)) {
                    // Top.
                    this->watch->printChildren();

                    NodeType* watchChild = watch->getLeftmostChild();
                    watch->removeChild(watchChild);

                    this->watch->printChildren();

                    topSolution->getColoring(coloring, i, topMaxWhite, watchColor);
                    watch->addChild(watchChild, 0);

                    this->watch->printChildren();

                    // Bottom.
                    int watchChildPos;
                    NodeType* watchParent;
                    NodeType* watchRightmostChild;

                    if (rootToWatchForward) {
                        watchParent = watch->getParent();
                        watchChildPos = watchParent->getChildPosition(watch);
                        watchParent->removeChild(watch);
                    }
                    else {
                        watchRightmostChild = watch->getRightmostChild();
                        watch->removeChild(watchRightmostChild);
                    }

                    this->watch->printChildren();

                    bottomSolution->getColoring(coloring, blackCount - i, bottomMaxWhite, watchColor);

                    if (rootToWatchForward) {
                        watchParent->addChild(watch, watchChildPos);
                    }
                    else {
                        watch->addChild(watchRightmostChild);
                    }

                    return coloring;
                }

                topMaxWhite = topSolution->getMaxWhiteMap()->get(watchColor, blackCount - i);
                bottomMaxWhite = bottomSolution->getMaxWhiteMap()->get(watchColor, i);

                if (topMaxWhite != INVALID_MAX_WHITE && bottomMaxWhite != INVALID_MAX_WHITE && (topMaxWhite + bottomMaxWhite >= whiteCount)) {
                    // Top.
                    NodeType* watchChild = watch->getLeftmostChild();
                    watch->removeChild(watchChild);
                    topSolution->getColoring(coloring, blackCount - i, topMaxWhite, watchColor);
                    watch->addChild(watchChild, 0);

                    // Bottom.
                    int watchChildPos;
                    NodeType* watchParent;
                    NodeType* watchRightmostChild;

                    if (rootToWatchForward) {
                        watchParent = watch->getParent();
                        watchChildPos = watchParent->getChildPosition(watch);
                        watchParent->removeChild(watch);
                    }
                    else {
                        watchRightmostChild = watch->getRightmostChild();
                        watch->removeChild(watchRightmostChild);
                    }

                    bottomSolution->getColoring(coloring, i, bottomMaxWhite, watchColor);

                    if (rootToWatchForward) {
                        watchParent->addChild(watch, watchChildPos);
                    }
                    else {
                        watch->addChild(watchRightmostChild);
                    }

                    return coloring;
                }
            }
        }

    private:

        void buildMaxWhiteMap() {
            maxWhiteMap.reset(topSolution->getMaxWhiteMap()->makeUnion(bottomSolution->getMaxWhiteMap().get()));
        }

    private:

        std::shared_ptr<Solution> topSolution;
        std::shared_ptr<Solution> bottomSolution;
        NodeType* watch;
        bool rootToWatchForward;
        bool rootToWatchForwardLeft;
        std::shared_ptr<ColorMaxWhiteMap> maxWhiteMap;
    };

    class BetweenProblem : public Problem {

    public:

        BetweenProblem(
            NodeType* newRoot,
            NodeType* newWatch,
            bool newRootToWatchForward,
            bool newRootToWatchForwardLeft,
            const TreeInfo& newTreeInfo) :

            Problem(newRoot, newWatch, newTreeInfo),
            rootToWatchForward(newRootToWatchForward),
            rootToWatchForwardLeft(newRootToWatchForwardLeft) {}

        const std::shared_ptr<Solution>& solve() {
            if (solution) {
                return solution;
            }

            // Top.
            NodeType* watchChild = this->watch->getLeftmostChild();
            this->watch->removeChild(watchChild);
            std::shared_ptr<Solution> topSolution(TailProblem(this->root, this->watch, rootToWatchForward, rootToWatchForwardLeft, this->treeInfo).solve());
            this->watch->addChild(watchChild, 0);

            // Bottom.
            int watchChildPos;
            NodeType* watchParent;
            NodeType* watchRightmostChild;

            if (rootToWatchForward) {
                watchParent = this->watch->getParent();
                watchChildPos = watchParent->getChildPosition(this->watch);
                watchParent->removeChild(this->watch);
            }
            else {
                watchRightmostChild = this->watch->getRightmostChild();
                this->watch->removeChild(watchRightmostChild);
            }

            std::shared_ptr<Solution> bottomSolution(HeadProblem(this->watch, this->watch, true, true, this->treeInfo).solve());

            if (rootToWatchForward) {
                watchParent->addChild(this->watch, watchChildPos);
            }
            else {
                this->watch->addChild(watchRightmostChild);
            }

            // Solution.
            solution.reset(new BetweenSolution(topSolution, bottomSolution, this->watch, rootToWatchForward, rootToWatchForwardLeft));

            qDebug() << "BETWEEN";
            solution->getMaxWhiteMap()->print();
            return solution;
        }

    private:

        bool rootToWatchForward;
        bool rootToWatchForwardLeft;
        std::shared_ptr<Solution> solution;
    };

    /**
     * Задача типа "Tail".
     */
    class TailSolution : public HeadSolution {

    public:

        TailSolution(const std::shared_ptr<ChainSolution>& newChainSolution) : HeadSolution(newChainSolution) {}

        TailSolution(
            const std::shared_ptr<ChainSolution>& newChainSolution,
            const std::shared_ptr<Solution>& newTopSolution,
            NodeType* newConnectiveNode,
            NodeType* newConnectiveNodePrev,
            bool isConnWatch) :

            HeadSolution(
                newChainSolution,
                newTopSolution,
                newConnectiveNode,
                newConnectiveNodePrev,
                isConnWatch) {}
    };

    class TailProblem : public Problem {

    public:

        TailProblem(
            NodeType* newRoot,
            NodeType* newWatch,
            const bool newRootToWatchForward,
            const bool newRootToWatchForwardLeft,
            const TreeInfo& newTreeInfo) :

            Problem(newRoot, newWatch, newTreeInfo),
            rootToWatchForward(newRootToWatchForward),
            rootToWatchForwardLeft(newRootToWatchForwardLeft) {}

        const std::shared_ptr<Solution>& solve() {
            if (solution) {
                return solution;
            }

            TrunkIterator trunkIter(this->watch, this->watch, false, false, !rootToWatchForward, true);
            int watchPos = 0;
            NodeType* prev = trunkIter.getNext();
            int length = 1;

            while (trunkIter.hasNext()) {
                NodeType* current = trunkIter.getNext();
                ++length;

                if (current->hasParent() && !current->isLeaf() && !current->hasSingleChild()) {
                    watchPos = length - 1;
                    trunkIter = TrunkIterator(this->watch, this->watch);

                    if (trunkIter.hasNext()) {
                        trunkIter.getNext();
                    }

                    while (trunkIter.hasNext()) {
                        ++length;
                        trunkIter.getNext();
                    }

                    std::shared_ptr<ChainSolution> chainSolution =
                        std::make_shared<ChainSolution>(
                            current,
                            this->watch,
                            watchPos,
                            rootToWatchForward,
                            rootToWatchForwardLeft,
                            true,
                            true,
                            length);

                    int childPos;

                    if (rootToWatchForward) {
                        childPos = current->getChildPosition(prev);
                        current->removeChild(prev);
                    }
                    else {
                        childPos = prev->getChildPosition(current);
                        prev->removeChild(current);
                    }

                    std::unique_ptr<Problem> topProblem;

                    if (current->getChildCount() > 2) {
                        topProblem.reset(new BranchProblem(this->root, current, rootToWatchForward, rootToWatchForwardLeft, this->treeInfo));
                    }
                    else {
                        topProblem.reset(new BetweenProblem(this->root, current, rootToWatchForward, rootToWatchForwardLeft, this->treeInfo));
                    }

                    solution.reset(new TailSolution(chainSolution, topProblem->solve(), current, prev, this->watch == current));

                    if (rootToWatchForward) {
                        current->addChild(prev, childPos);
                    }
                    else {
                        prev->addChild(current, childPos);
                    }

                    break;
                }

                prev = current;
            }

            if (!solution) {
                watchPos = length - 1;

                if (rootToWatchForward || this->watch->getChildCount() > 1) {
                    TrunkIterator trunkIter(this->watch, this->watch);

                    if (trunkIter.hasNext()) {
                        trunkIter.getNext();
                    }

                    while (trunkIter.hasNext()) {
                        ++length;
                        trunkIter.getNext();
                    }

                }

                std::shared_ptr<ChainSolution> chainSolution =
                    std::make_shared<ChainSolution>(
                        this->root,
                        this->watch,
                        watchPos,
                        rootToWatchForward,
                        rootToWatchForwardLeft,
                        true,
                        true,
                        length);

                solution.reset(new TailSolution(chainSolution));
            }

            qDebug() << "TAIL";
            solution->getMaxWhiteMap()->print();
            return solution;
        }

    private:

        bool rootToWatchForward;
        bool rootToWatchForwardLeft;
        std::shared_ptr<Solution> solution;
    };

private:

    void buildBody() {
        // Определяем тип корневой задачи.
        // Формируем экземпляр задачи этого типа.
        // Решаем сформированный экземпляр задачи.
        treeInfo.setTree(tree);
        std::unique_ptr<Problem> problem;
        NodeType* root = tree->getRoot();

        // HEAD.
        if (root->isLeaf() || root->hasSingleChild()) {
            problem.reset(new HeadProblem(root, root, true, true, treeInfo));
        }
        // BETWEEN.
        else if (root->getChildCount() == 2) {
            problem.reset(new BetweenProblem(tree->getRightmostAncestor(root), root, false, false, treeInfo));
        }
        // BRANCH.
        else {
            problem.reset(new BranchProblem(tree->getRightmostAncestor(root), root, false, false, treeInfo));
        }

        solution = problem->solve();

        // Формируем отображение Max_White.
        const std::shared_ptr<ColorMaxWhiteMap>& rootMaxWhiteMap = solution->getMaxWhiteMap();

        qDebug() << "***** Root Max_White *****";
        rootMaxWhiteMap->print();

        for (int b = 0; b <= nodeCount; ++b) {
            maxWhiteMap->set(b, rootMaxWhiteMap->get(b));
        }
    }

    void buildColoring() {
        if (!solution) {
            buildBody();
        }

        coloring = solution->getColoring(coloring, blackCount, whiteCount);

        if (coloring->getWhiteCount() > whiteCount) {
            int excess = coloring->getWhiteCount() - whiteCount;
            TreeIterator treeIter = treeIterator();

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

    TreeInfo treeInfo;
    std::shared_ptr<Solution> solution;
};

template<typename NodeType> const typename AverageBWTreeColorer<NodeType>::ColorType AverageBWTreeColorer<NodeType>::BLACK = AverageBWTreeColorer<NodeType>::ColoringType::BLACK;
template<typename NodeType> const typename AverageBWTreeColorer<NodeType>::ColorType AverageBWTreeColorer<NodeType>::WHITE = AverageBWTreeColorer<NodeType>::ColoringType::WHITE;
template<typename NodeType> const typename AverageBWTreeColorer<NodeType>::ColorType AverageBWTreeColorer<NodeType>::GRAY = AverageBWTreeColorer<NodeType>::ColoringType::GRAY;
template<typename NodeType> const typename AverageBWTreeColorer<NodeType>::ColorType AverageBWTreeColorer<NodeType>::COLOR_COUNT = AverageBWTreeColorer<NodeType>::ColoringType::COLOR_COUNT;
template<typename NodeType> const typename AverageBWTreeColorer<NodeType>::ColorType AverageBWTreeColorer<NodeType>::INVALID_COLOR = AverageBWTreeColorer<NodeType>::ColoringType::INVALID_COLOR;

} // namespace bw.

#endif

