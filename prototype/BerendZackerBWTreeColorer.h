#ifndef BW_BEREND_ZACKER_BW_TREE_COLORER_H
#define BW_BEREND_ZACKER_BW_TREE_COLORER_H

#include <utility>
#include <math.h>
#include <vector>
#include <stack>
#include <queue>
#include "TreeNode.h"
#include "TreeData.h"
#include "Tree.h"
#include "StdBWTreeColorer.h"
#include "Summator.h"

namespace bw {

/**
 * Алгоритм Беренда-Заккера чёрно-белой раскраски дерева.
 *
 * Временная сложность этапа построения: O(p^2*[lg(p)]^3), где p - количество узлов в дереве.
 * Пространственная сложность: O(p^2).
 * Временная сложность этапа раскраски: O(p^2).
 */
template<typename NodeType>
class BerendZackerBWTreeColorer : public StdBWTreeColorer<NodeType> {

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

private:

    static const ColorType BLACK;
    static const ColorType WHITE;
    static const ColorType GRAY;
    static const ColorType COLOR_COUNT;
    static const ColorType INVALID_COLOR;

    static const int INVALID_MAX_WHITE;
    static const int INVALID_MIN_GRAY;

private:

    /**
     * Сопоставляет каждому узлу размер поддерева с корнем в этом узле.
     * В процессе работы алгоритма это значение меняется.
     * Например, это происходит после выполнения операции редукции, когда поддерево заменяется своим корнем.
     */
    typedef TreeData<NodeType, int> SubtreesNodeCountData;

    /**
     * Список, сопоставляемый некоторому числу серых узлов grayCount,
     * и содержащий всевозможные значения blackCount такие,
     * что существует оптимальная чёрно-белая раскраска дерева c grayCount серыми узлами и blackCount чёрными.
     */
    typedef std::vector<int> BlackList;

private:

    /**
     * Узел дерева Беренда-Заккера.
     */
    class BerendZackerNode;

protected:

    TreeIterator treeIterator() const {
        return tree->template iterator<IsLeafPredicate>();
    }

private:

    friend class BerendZackerTree;

public:

    BerendZackerBWTreeColorer() : berendZackerTree(this) {}

private:

    void buildBody() {
        berendZackerTree.from(tree);
        berendZackerTree.color();

        SubtreeMaxWhiteMap* rootMaxWhiteMap = ((BerendZackerNode*)berendZackerTree.getRoot())->getSubtreeMaxWhiteMap();

        for (int b = 0; b <= nodeCount; ++b) {
            int maxWhite = (rootMaxWhiteMap->get(BLACK, b) != INVALID_MAX_WHITE) ? rootMaxWhiteMap->get(BLACK, b) : INVALID_MAX_WHITE;

            if ((rootMaxWhiteMap->get(WHITE, b) != INVALID_MAX_WHITE) && (rootMaxWhiteMap->get(WHITE, b) > maxWhite)) {
                maxWhite = rootMaxWhiteMap->get(WHITE, b);
            }

            if ((rootMaxWhiteMap->get(GRAY, b) != INVALID_MAX_WHITE) && (rootMaxWhiteMap->get(GRAY, b) > maxWhite)) {
                maxWhite = rootMaxWhiteMap->get(GRAY, b);
            }

            maxWhiteMap->set(b, maxWhite);
        }
    }

    void buildColoring() {
        berendZackerTree.findColoring(blackCount, whiteCount);
    }

private:

    /**
     * Представление отображения SubtreeMaxWhiteMap в виде (color, grayCount) -> BlackList(color, grayCount).
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

        // Представление отображения SubtreeMaxWhiteMap в виде (color, grayCount) -> BlackList(color, grayCount).
        std::vector<BlackList*>* map[COLOR_COUNT];

        // Максимальное число серых узлов по всем чёрно-белым раскраскам поддерева.
        int maxGrayCount;

        // Максимальное число чёрных узлов по всем чёрно-белым раскраскам поддерева.
        int maxBlackCount;
    };

    /**
     * Каждому цвету color из множества {BLACK, WHITE, GRAY},
     * и каждому неотрицательному целому blackCount, не превышающему числа узлов в данном дереве,
     * сопоставляет неотрицательное целое, равное максимальному числу белых узлов по всем чёрно-белым раскраскам данного дерева,
     * в которых корень этого дерева окрашен в цвет color, и ровно blackCount его узлов окрашено в чёрный цвет.
     */
    class SubtreeMaxWhiteMap {

    public:

        SubtreeMaxWhiteMap() : subtreeNodeCount(-1) {}

        SubtreeMaxWhiteMap(const int newSubtreeNodeCount) : subtreeNodeCount(newSubtreeNodeCount) {
            initMap();
        }

        SubtreeMaxWhiteMap(const Gray2BlackListMap& source) : subtreeNodeCount(source.getMaxBlackCount()) {
            from(source);
        }

        SubtreeMaxWhiteMap(Gray2BlackListMap* source) : subtreeNodeCount(source->getMaxBlackCount()) {
            from(*source);
        }

        void put(const ColorType color, const int blackCount, const int maxWhite) {
            map[color][blackCount] = maxWhite;
        }

        int get(const ColorType color, const int blackCount) const {
            return map[color][blackCount];
        }

        int getSubtreeNodeCount() const {
            return subtreeNodeCount;
        }

        Gray2BlackListMap* toGray2BlackListMap() const {
            Gray2BlackListMap* gray2BlackListMap = new Gray2BlackListMap((int)log2(subtreeNodeCount) + 3);
            int grayCount;

            for (int b = 0; b <= subtreeNodeCount; ++b) {
                // Чёрный цвет.
                if (get(BLACK, b) != INVALID_MAX_WHITE) {
                    grayCount = subtreeNodeCount - (b + get(BLACK, b));
                    gray2BlackListMap->put(BLACK, grayCount, b);
                }

                // Белый цвет.
                if (get(WHITE, b) != INVALID_MAX_WHITE) {
                    grayCount = subtreeNodeCount - (b + get(WHITE, b));
                    gray2BlackListMap->put(WHITE, grayCount, b);
                }

                // Серый цвет.
                if (get(GRAY, b) != INVALID_MAX_WHITE) {
                    grayCount = subtreeNodeCount - (b + get(GRAY, b));
                    gray2BlackListMap->put(GRAY, grayCount, b);
                }
            }

            return gray2BlackListMap;
        }

        void from(const Gray2BlackListMap& sourse) {
            initMap();

            for (int g = 0; g <= sourse.getMaxGrayCount(); ++g) {
                // Чёрный цвет.
                BlackList* blackList = sourse.get(BLACK, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(BLACK, *blackListIter, subtreeNodeCount - (*blackListIter + g));
                    }
                }

                // Белый цвет.
                blackList = sourse.get(WHITE, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(WHITE, *blackListIter, subtreeNodeCount - (*blackListIter + g));
                    }
                }

                // Серый цвет.
                blackList = sourse.get(GRAY, g);

                if (blackList != 0) {
                    for (BlackList::iterator blackListIter = blackList->begin(); blackListIter != blackList->end(); ++blackListIter) {
                        put(GRAY, *blackListIter, subtreeNodeCount - (*blackListIter + g));
                    }
                }
            }
        }

        void destroy() {
            delete[] map[BLACK];
            delete[] map[WHITE];
            delete[] map[GRAY];
        }

    private:

        void initMap() {
            map[BLACK] = new int[subtreeNodeCount + 1];
            map[WHITE] = new int[subtreeNodeCount + 1];
            map[GRAY] = new int[subtreeNodeCount + 1];

            for (int b = 0; b <= subtreeNodeCount; ++b) {
                map[BLACK][b] = INVALID_MAX_WHITE;
                map[WHITE][b] = INVALID_MAX_WHITE;
                map[GRAY][b] = INVALID_MAX_WHITE;
            }
        }

    private:

        int* map[COLOR_COUNT];
        int subtreeNodeCount;
    };

    /**
     * Слияние отображений Max_White поддеревьев c корнями в дочерних узлах некоторого узла дерева Беренда-Заккера.
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

        virtual int getMinGrayCount(const ColorType prepare, const int blackCount) const = 0;

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
     * Слияние отображений Max_White поддеревьев c корнями в дочерних узлах некоторого узла дерева Беренда-Заккера в случае,
     * когда имеется только один дочерний узел.
     */
    class SingleChildFusion : public Fusion {

    public:

        SingleChildFusion(BerendZackerNode* subtreeRoot) :
            Fusion(0, 0),
            subtreeNodeCount(subtreeRoot->getSubtreeNodeCount()),
            subtreeMaxWhiteMap(subtreeRoot->getSubtreeMaxWhiteMap()) {}

        int getMinGrayCount(const ColorType color, const int blackCount) const {
            return blackCount >= 0 && blackCount <= subtreeNodeCount && subtreeMaxWhiteMap->get(color, blackCount) != INVALID_MAX_WHITE ? subtreeNodeCount - (blackCount + subtreeMaxWhiteMap->get(color, blackCount)) : INVALID_MIN_GRAY;
        }

        SubtreeMaxWhiteMap* getMaxWhiteMap() const {
            return subtreeMaxWhiteMap;
        }

    private:

        int subtreeNodeCount;
        SubtreeMaxWhiteMap* subtreeMaxWhiteMap;
    };

    /**
     * Слияние отображений Max_White поддеревьев c корнями в дочерних узлах некоторого узла дерева Беренда-Заккера в случае,
     * когда имеется несколько дочерних узлов.
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

        ~MultiChildFusion() {
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

        int* minGrayCount[COLOR_COUNT];
        int maxBlackCount;
    };

private:

    typedef std::stack<Fusion*> FusionStack;

private:

    /**
     * Узел дерева Беренда-Заккера.
     */
    class BerendZackerNode : public TreeNode {

    public:

        BerendZackerNode(NodeType* newNode, const int newSubtreeNodeCount) :
            node(newNode),
            currentSubtreeNodeCount(newSubtreeNodeCount),
            subtreeMaxWhiteMap(new SubtreeMaxWhiteMap(newSubtreeNodeCount)),
            fusionStack(0),
            reduced(false),
            topFusion(0) {}

        NodeType* getNode() const {
            return node;
        }

        void setSubtreeMaxWhiteMap(SubtreeMaxWhiteMap* newSubtreeMaxWhiteMap) {
            subtreeMaxWhiteMap = newSubtreeMaxWhiteMap;
        }

        SubtreeMaxWhiteMap* getSubtreeMaxWhiteMap() const {
            return subtreeMaxWhiteMap;
        }

        int getSubtreeNodeCount() const {
            return subtreeMaxWhiteMap->getSubtreeNodeCount();
        }

        int getCurrentSubtreeNodeCount() const {
            return currentSubtreeNodeCount;
        }

        void pushFusion(Fusion* fusion) {
            if (fusionStack == 0) {
                fusionStack = new FusionStack();
            }

            fusionStack->push(fusion);
            topFusion = fusion;
        }

        Fusion* getTopFusion() {
            if ((fusionStack == 0 || fusionStack->empty()) && hasSingleChild()) {
                pushFusion(new SingleChildFusion(this));
            }

            return !fusionStack->empty() ? fusionStack->top() : 0;
        }

        void popFusion() {
            if (fusionStack != 0 && !fusionStack->empty()) {
                fusionStack->pop();
            }
        }

        FusionStack* getFusionStack() const {
            return fusionStack;
        }

        void setReduced(const bool newReduced) {
            if (newReduced) {
                reduce();
            }
            else {
                unreduce();
            }
        }

        void reduce() {
            if (reduced) {
                return;
            }

            reduced = true;
            currentSubtreeNodeCount = 1;

            BerendZackerNode* currentNode = (BerendZackerNode*)getParent();

            while (currentNode != 0) {
                currentNode->updateCurrentSubtreeNodeCount();
                currentNode = (BerendZackerNode*)currentNode->getParent();
            }
        }

        void unreduce() {
            if (!reduced) {
                return;
            }

            reduced = false;
            currentSubtreeNodeCount = getSubtreeNodeCount();
        }

        bool isReduced() const {
            return reduced;
        }

        bool isLogicalLeaf() const {
            return currentSubtreeNodeCount == 1;
        }

        void removeAllFusions() {
            topFusion = 0;
            delete fusionStack;
            fusionStack = 0;
        }

        virtual ~BerendZackerNode() {
            delete subtreeMaxWhiteMap;
            delete fusionStack;
            delete topFusion;
            topFusion = 0;
        }

    private:

        void updateCurrentSubtreeNodeCount() {
            currentSubtreeNodeCount = 1;

            for (int i = 0; i < getChildCount(); ++i) {
                currentSubtreeNodeCount += ((BerendZackerNode*)getChild(i))->getCurrentSubtreeNodeCount();
            }
        }

    private:

        // Узел раскрашиваемого дерева.
        NodeType* node;

        // Текущее количество узлов в поддереве с корнем в узле node.
        // Более точно: это количество узлов в поддереве с корнем в узле node
        // на текущем шаге выполнения алгоритма Беренда-Заккера.
        // Операция редукции, например, приводит к уменьшению этого значения,
        // поскольку поддерево при этом заменяется на его корень.
        int currentSubtreeNodeCount;

        // Каждому цвету color из множества {BLACK, WHITE, GRAY},
        // и каждому неотрицательному целому blackCount, не превышающему числа узлов в данном дереве,
        // сопоставляет неотрицательное целое, равное максимальному числу белых узлов по всем чёрно-белым раскраскам данного дерева,
        // в которых корень этого дерева окрашен в цвет color, и ровно blackCount его узлов окрашено в чёрный цвет.
        SubtreeMaxWhiteMap* subtreeMaxWhiteMap;

        // Стек объединений отображений Max_White.
        FusionStack* fusionStack;

        // Имеет истинное значение в том и только в том случае, когда поддерево с корнем в узле node заменено на node.
        bool reduced;

        // Объединение верхнего уровня.
        Fusion* topFusion;
    };

    /**
     * Дерево Беренда-Заккера.
     */
    class BerendZackerTree : public Tree<BerendZackerNode> {

    public:

        using Tree<BerendZackerNode>::getRoot;
        using Tree<BerendZackerNode>::clear;
        using Tree<BerendZackerNode>::iterator;

    public:

        /**
         * Предикат, отбирающий листья дерева.
         */
        class IsLeafPredicate {

        public:

            bool operator()(BerendZackerNode* node) const {
                return node->isLogicalLeaf();
            }
        };

        /**
         * Итератор по дереву.
         */
        typedef typename Tree<BerendZackerNode>::template Iterator<IsLeafPredicate> NodeIterator;

    public:

        BerendZackerTree(BerendZackerBWTreeColorer* newColorer) : colorer(newColorer) {}

        BerendZackerTree(BerendZackerBWTreeColorer* newColorer, TreeType* source) : colorer(newColorer) {
            from(source);
        }

        NodeIterator getNodeIterator(BerendZackerNode* subtreeRoot) const {
            return NodeIterator(subtreeRoot);
        }

        void prepare() {
            clear();
            subtreesNodeCountData.clear();
        }

        void from(TreeType* source) {
            clear();
            calcSubtreesNodeCount(source);

            std::stack<BerendZackerNode*> developableNodes;
            BerendZackerNode* root = new BerendZackerNode(source->getRoot(), source->getNodeCount());
            developableNodes.push(root);
            this->setRoot(root);

            while (!developableNodes.empty()) {
                BerendZackerNode* developableNode = developableNodes.top();
                developableNodes.pop();
                NodeType* sourceNode = developableNode->getNode();
                int sourceNodeChildCount = sourceNode->getChildCount();

                if (sourceNodeChildCount == 0) {
                    SubtreeMaxWhiteMap* subtreeMaxWhiteMap = new SubtreeMaxWhiteMap(1);

                    subtreeMaxWhiteMap->put(BLACK, 0, INVALID_MAX_WHITE);
                    subtreeMaxWhiteMap->put(BLACK, 1, 0);

                    subtreeMaxWhiteMap->put(WHITE, 0, 1);
                    subtreeMaxWhiteMap->put(WHITE, 1, INVALID_MAX_WHITE);

                    subtreeMaxWhiteMap->put(GRAY, 0, 0);
                    subtreeMaxWhiteMap->put(GRAY, 1, INVALID_MAX_WHITE);

                    developableNode->setSubtreeMaxWhiteMap(subtreeMaxWhiteMap);
                }
                else {
                    for (int i = 0; i < sourceNodeChildCount; ++i) {
                        BerendZackerNode* newDevelopableNodeChild = new BerendZackerNode(sourceNode->getChild(i), subtreesNodeCountData.get(sourceNode->getChild(i)));
                        this->add(newDevelopableNodeChild, developableNode);
                        developableNodes.push(newDevelopableNodeChild);
                    }
                }
            }
        }

        void unreduce() {
            std::queue<BerendZackerNode*> bfsQueue;
            bfsQueue.push(getRoot());
            BerendZackerNode* node;

            while (!bfsQueue.empty()) {
                node = bfsQueue.front();
                bfsQueue.pop();
                node->unreduce();

                for (int i = 0; i < node->getChildCount(); ++i) {
                    bfsQueue.push((BerendZackerNode*)node->getChild(i));
                }
            }
        }

        void color() {
            color(getRoot());
        }

        void findColoring(const int blackCount, const int whiteCount) {
            unreduce();
            coloring = colorer->coloring;
            int grayCount = getRoot()->getSubtreeNodeCount() - (blackCount + whiteCount);
            findColoring(getRoot(), blackCount, grayCount);
        }

        ~BerendZackerTree() {
            typename std::vector<BerendZackerNode*>::iterator attachedChildSubtreeRootIter = attachedChildSubtreeRoots.begin();

            while (attachedChildSubtreeRootIter != attachedChildSubtreeRoots.end()) {
                BerendZackerNode* node = *attachedChildSubtreeRootIter;
                node->removeAllChildren();
                node->removeAllFusions();
                delete node;

                ++attachedChildSubtreeRootIter;
            }
        }

    private:

        void calcSubtreesNodeCount(TreeType* tree) {
            subtreesNodeCountData.clear();

            std::queue<NodeType*> unhandled;
            NodeType* root = tree->getRoot();
            unhandled.push(root);

            NodeType** topology = new NodeType*[tree->getNodeCount()];
            topology[0] = root;
            int order = 1;

            while (!unhandled.empty()) {
                NodeType* node = unhandled.front();
                unhandled.pop();
                int childCount = node->getChildCount();

                for (int i = 0; i < childCount; ++i) {
                    NodeType* child = node->getChild(i);
                    unhandled.push(child);
                    topology[order] = child;
                    ++order;
                }
            }

            for (order = tree->getNodeCount() - 1; order > -1; --order) {
                NodeType* node = topology[order];
                int childCount = node->getChildCount();

                if (childCount == 0) {
                    subtreesNodeCountData.set(node, 1);
                }
                else {
                    int totalChildSubtreesNodeCount = 0;

                    for (int i = 0; i < childCount; ++i) {
                        totalChildSubtreesNodeCount += subtreesNodeCountData.get(node->getChild(i));
                    }

                    subtreesNodeCountData.set(node, totalChildSubtreesNodeCount + 1);
                }
            }
        }

        void color(BerendZackerNode* subtreeRoot) {
            int subtreeNodeCount = subtreeRoot->getCurrentSubtreeNodeCount();

            if (subtreeNodeCount == 1) {
                return;
            }

            if (subtreeNodeCount == 2) {
                createEdgeMaxWhiteMap(subtreeRoot);
                return;
            }

            BerendZackerNode* separator = findSeparator(subtreeRoot);

            if (separator == subtreeRoot) {
                int childCount = separator->getChildCount();
                std::pair<Gray2BlackListMap*, Fusion*>* currentMerging = new std::pair<Gray2BlackListMap*, Fusion*>[childCount];

                for (int i = 0; i < childCount; ++i) {
                    BerendZackerNode* child = (BerendZackerNode*)separator->getChild(i);
                    BerendZackerNode* attachedChildSubtreeRoot = new BerendZackerNode(separator->getNode(), child->getCurrentSubtreeNodeCount() + 1);
                    attachedChildSubtreeRoot->addChild(child);

                    color(attachedChildSubtreeRoot);
                    currentMerging[i] = std::pair<Gray2BlackListMap*, Fusion*>(attachedChildSubtreeRoot->getSubtreeMaxWhiteMap()->toGray2BlackListMap(), attachedChildSubtreeRoot->getTopFusion());

                    attachedChildSubtreeRoots.push_back(attachedChildSubtreeRoot);
                }

                Gray2BlackListMap* topGray2BlackListMap = merge(currentMerging, 0, childCount - 1, separator).first;
                subtreeRoot->setSubtreeMaxWhiteMap(new SubtreeMaxWhiteMap(topGray2BlackListMap));
                delete topGray2BlackListMap;
                delete currentMerging;
            }

            else {
                color(separator);
                separator->reduce();
                color(subtreeRoot);
            }
        }

        void fill(BerendZackerNode* subtreeRoot, const ColorType color) {
            NodeIterator nodeIter = getNodeIterator(subtreeRoot);

            while (nodeIter.hasNext()) {
                coloring->color(nodeIter.getNext()->getNode(), color);
            }
        }

        void createEdgeMaxWhiteMap(BerendZackerNode* subtreeRoot) const {
            SubtreeMaxWhiteMap* childSubtreeMaxWhiteMap = ((BerendZackerNode*)subtreeRoot->getChild(0))->getSubtreeMaxWhiteMap();
            int childSubtreeNodeCount = childSubtreeMaxWhiteMap->getSubtreeNodeCount();
            int subtreeNodeCount = childSubtreeNodeCount + 1;

            SubtreeMaxWhiteMap* subtreeMaxWhiteMap = new SubtreeMaxWhiteMap(subtreeNodeCount);
            int maxWhite;

            // Чёрный цвет.
            subtreeMaxWhiteMap->put(BLACK, 0, INVALID_MAX_WHITE);

            for (int b = 1; b <= subtreeNodeCount; ++b) {
                maxWhite = childSubtreeMaxWhiteMap->get(BLACK, b - 1);

                if (childSubtreeMaxWhiteMap->get(GRAY, b - 1) > maxWhite) {
                    maxWhite = childSubtreeMaxWhiteMap->get(GRAY, b - 1);
                }

                subtreeMaxWhiteMap->put(BLACK, b, maxWhite);
            }

            // Белый цвет.
            subtreeMaxWhiteMap->put(WHITE, subtreeNodeCount, INVALID_MAX_WHITE);

            for (int b = 0; b < subtreeNodeCount; ++b) {
                maxWhite = childSubtreeMaxWhiteMap->get(WHITE, b);

                if (childSubtreeMaxWhiteMap->get(GRAY, b) > maxWhite) {
                    maxWhite = childSubtreeMaxWhiteMap->get(GRAY, b);
                }

                if (maxWhite != INVALID_MAX_WHITE) {
                    subtreeMaxWhiteMap->put(WHITE, b, maxWhite + 1);
                }
                else {
                    subtreeMaxWhiteMap->put(WHITE, b, INVALID_MAX_WHITE);
                }
            }

            // Серый цвет.
            subtreeMaxWhiteMap->put(GRAY, subtreeNodeCount, INVALID_MAX_WHITE);

            for (int b = 0; b < subtreeNodeCount; ++b) {
                maxWhite = childSubtreeMaxWhiteMap->get(BLACK, b);

                if (childSubtreeMaxWhiteMap->get(WHITE, b) > maxWhite) {
                    maxWhite = childSubtreeMaxWhiteMap->get(WHITE, b);
                }

                if (childSubtreeMaxWhiteMap->get(GRAY, b) > maxWhite) {
                    maxWhite = childSubtreeMaxWhiteMap->get(GRAY, b);
                }

                subtreeMaxWhiteMap->put(GRAY, b, maxWhite);
            }

            subtreeRoot->setSubtreeMaxWhiteMap(subtreeMaxWhiteMap);
        }

        BerendZackerNode* findSeparator(BerendZackerNode* subtreeRoot) const {
            int currentSubtreeNodeCount = subtreeRoot->getCurrentSubtreeNodeCount();
            NodeIterator subtreeIter = getNodeIterator(subtreeRoot);

            while (subtreeIter.hasNext()) {
                BerendZackerNode* subtreeNode = subtreeIter.getNext();
                bool isTopTreeNotTooLarge = currentSubtreeNodeCount - subtreeNode->getCurrentSubtreeNodeCount() <= currentSubtreeNodeCount / 2;

                if (!isTopTreeNotTooLarge) {
                    continue;
                }

                bool areChildTreesNotTooLarge = true;

                for (int i = 0; i < subtreeNode->getChildCount(); ++i) {
                    if (((BerendZackerNode*)subtreeNode->getChild(i))->getCurrentSubtreeNodeCount() > currentSubtreeNodeCount / 2) {
                        areChildTreesNotTooLarge = false;
                        break;
                    }
                }

                if (areChildTreesNotTooLarge) {
                    return subtreeNode;
                }
            }
        }

        Gray2BlackListMap* createUnion(Gray2BlackListMap* first, Gray2BlackListMap* second) const {
            int nodeCount = getRoot()->getSubtreeNodeCount();
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

        std::pair<Gray2BlackListMap*, Fusion*> merge(const std::pair<Gray2BlackListMap*, Fusion*>& left, const std::pair<Gray2BlackListMap*, Fusion*>& right) const {
            Gray2BlackListMap* unitedMap = createUnion(left.first, right.first);
            delete left.first;
            delete right.first;

            return std::pair<Gray2BlackListMap*, Fusion*>(unitedMap, new MultiChildFusion(unitedMap, left.second, right.second));
        }

        std::pair<Gray2BlackListMap*, Fusion*> merge(std::pair<Gray2BlackListMap*, Fusion*>* currentMerging, const int l, const int r, BerendZackerNode* separator) const {
            if (l == r) {
                return currentMerging[l];
            }

            if (r - l == 1) {
                std::pair<Gray2BlackListMap*, Fusion*> result = merge(currentMerging[l], currentMerging[r]);
                separator->pushFusion(result.second);

                return result;
            }
            else {
                std::pair<Gray2BlackListMap*, Fusion*> leftMerging = merge(currentMerging, l, (l + r) / 2, separator);
                std::pair<Gray2BlackListMap*, Fusion*> rightMerging = merge(currentMerging, (l + r) / 2 + 1, r, separator);

                std::pair<Gray2BlackListMap*, Fusion*> result = merge(leftMerging, rightMerging);
                separator->pushFusion(result.second);

                return result;
            }
        }

        void findLeafColoring(BerendZackerNode* leaf, const int blackCount, const int grayCount) {
            if (blackCount == 1) {
                coloring->color(leaf->getNode(), BLACK);
            }
            else if (grayCount == 1) {
                coloring->color(leaf->getNode(), GRAY);
            }
            else {
                coloring->color(leaf->getNode(), WHITE);
            }
        }

        void findSingleChildNodeColoring(BerendZackerNode* node, const int childIndex, int blackCount, int grayCount) {
            BerendZackerNode* child = (BerendZackerNode*)node->getChild(childIndex);

            if (coloring->isColored(node->getNode())) {
                if ((blackCount == 0) && (coloring->getWhiteCount() >= colorer->getWhiteCount())) {
                    fill(child, GRAY);
                }
                else {
                    ColorType color = coloring->getColor(node->getNode());

                    if (color == BLACK) {
                        --blackCount;
                    }
                    else if (color == GRAY) {
                        --grayCount;
                    }

                    findColoring(
                        child,
                        0,
                        child->getChildCount() - 1,
                        blackCount,
                        grayCount,
                        0);
                }
            }
            else {
                if ((blackCount == 0) && (coloring->getWhiteCount() >= colorer->getWhiteCount())) {
                    fill(node, GRAY);
                    return;
                }

                Fusion* fusion = node->getTopFusion();
                BerendZackerNode* parent = (BerendZackerNode*)node->getParent();
                ColorType parentColor = INVALID_COLOR;

                if ((parent != 0) && coloring->isColored(parent->getNode())) {
                    parentColor = coloring->getColor(parent->getNode());
                }

                if (parent == 0) {
                    if (fusion->getMinGrayCount(GRAY, blackCount) != INVALID_MIN_GRAY && fusion->getMinGrayCount(GRAY, blackCount) <= grayCount) {
                        coloring->color(node->getNode(), GRAY);
                        grayCount = fusion->getMinGrayCount(GRAY, blackCount) - 1;
                    }
                    else if (fusion->getMinGrayCount(BLACK, blackCount) != INVALID_MIN_GRAY && fusion->getMinGrayCount(BLACK, blackCount) <= grayCount) {
                        coloring->color(node->getNode(), BLACK);
                        grayCount = fusion->getMinGrayCount(BLACK, blackCount);
                        --blackCount;
                    }
                    else {
                        coloring->color(node->getNode(), WHITE);
                        grayCount = fusion->getMinGrayCount(WHITE, blackCount);
                    }
                }
                else {
                    if (parentColor != WHITE && fusion->getMinGrayCount(BLACK, blackCount) != INVALID_MIN_GRAY && fusion->getMinGrayCount(BLACK, blackCount) == grayCount) {
                        coloring->color(node->getNode(), BLACK);
                        --blackCount;
                    }
                    else if (parentColor != BLACK && fusion->getMinGrayCount(WHITE, blackCount) != INVALID_MIN_GRAY && fusion->getMinGrayCount(WHITE, blackCount) == grayCount) {
                        coloring->color(node->getNode(), WHITE);
                    }
                    else {
                        coloring->color(node->getNode(), GRAY);
                        --grayCount;
                    }
                }

                findColoring(
                    child,
                    0,
                    child->getChildCount() - 1,
                    blackCount,
                    grayCount,
                    0);
            }
        }

        void findColoring(BerendZackerNode* root, const int blackCount, const int grayCount) {
            findColoring(
                root,
                0,
                root->getChildCount() - 1,
                blackCount,
                grayCount,
                0);

            if (coloring->getWhiteCount() > colorer->whiteCount) {
                int excess = coloring->getWhiteCount() - colorer->getWhiteCount();
                TreeIterator treeIter = colorer->treeIterator();

                while (treeIter.hasNext() && excess > 0) {
                    NodeType* node = treeIter.getNext();

                    if (coloring->getColor(node) == WHITE) {
                        coloring->color(node, GRAY);
                        --excess;
                    }
                }
            }
        }

        void findColoring(BerendZackerNode* root, const int first, const int last, int blackCount, int grayCount, Fusion* fusion) {
            if ((blackCount == 0) && (coloring->getWhiteCount() >= colorer->getWhiteCount()))  {
                if (coloring->isColored(root->getNode())) {
                    for (int i = first; i <= last; ++i) {
                        fill((BerendZackerNode*)root->getChild(i), GRAY);
                    }
                }
                else {
                    fill(root, GRAY);
                }

                return;
            }

            if (root->isLeaf()) {
                findLeafColoring(root, blackCount, grayCount);
            }
            else if (root->hasSingleChild()) {
                findSingleChildNodeColoring(root, first, blackCount, grayCount);
            }
            else if (last - first == 0) {
                findSingleChildNodeColoring(root, first, blackCount, grayCount);
            }
            else {
                if (fusion == 0) {
                    fusion = root->getTopFusion();
                    root->popFusion();
                }

                ColorType rootColor;
                BerendZackerNode* parent = (BerendZackerNode*)root->getParent();
                ColorType parentColor = INVALID_COLOR;

                if ((parent != 0) && coloring->isColored(parent->getNode())) {
                    parentColor = coloring->getColor(parent->getNode());
                }

                if (
                   (parentColor != WHITE) &&
                   (fusion->getMinGrayCount(BLACK, blackCount) != INVALID_MIN_GRAY) &&
                   (fusion->getMinGrayCount(BLACK, blackCount) <= grayCount) &&
                   (!coloring->isColored(root->getNode()) || coloring->getColor(root->getNode()) == BLACK)) {

                    if (!coloring->isColored(root->getNode())) {
                        coloring->color(root->getNode(), BLACK);
                    }

                    rootColor = BLACK;
                    ++blackCount;
                }
                else if (
                   (parentColor != BLACK) &&
                   (fusion->getMinGrayCount(WHITE, blackCount) != INVALID_MIN_GRAY) &&
                   (fusion->getMinGrayCount(WHITE, blackCount) <= grayCount) &&
                   (!coloring->isColored(root->getNode()) || coloring->getColor(root->getNode()) == WHITE)) {

                    if (!coloring->isColored(root->getNode())) {
                        coloring->color(root->getNode(), WHITE);
                    }

                    rootColor = WHITE;
                }
                else if (
                    (fusion->getMinGrayCount(GRAY, blackCount) != INVALID_MIN_GRAY) &&
                    (fusion->getMinGrayCount(GRAY, blackCount) <= grayCount)) {

                    if (!coloring->isColored(root->getNode())) {
                        coloring->color(root->getNode(), GRAY);
                    }

                    rootColor = GRAY;
                    ++grayCount;
                }

                Fusion* leftFusion = fusion->getLeftFusion();
                Fusion* rightFusion = fusion->getRightFusion();

                for (int b = 0; b <= blackCount; ++b) {
                    if (
                       (leftFusion->getMinGrayCount(rootColor, b) != INVALID_MIN_GRAY) &&
                       (rightFusion->getMinGrayCount(rootColor, blackCount - b) != INVALID_MIN_GRAY) &&
                       (leftFusion->getMinGrayCount(rootColor, b) + rightFusion->getMinGrayCount(rootColor, blackCount - b) <= grayCount)) {

                        findColoring(
                            root,
                            first,
                            (first + last) / 2,
                            b,
                            leftFusion->getMinGrayCount(rootColor, b),
                            leftFusion);

                        findColoring(
                            root,
                            (first + last) / 2 + 1,
                            last,
                            blackCount - b,
                            rightFusion->getMinGrayCount(rootColor, blackCount - b),
                            rightFusion);

                        break;
                    }
                }
            }
        }

    private:

        // Раскрашиватель.
        BerendZackerBWTreeColorer* colorer;

        // Раскраска.
        ColoringType* coloring;

        // Размеры поддеревьев.
        SubtreesNodeCountData subtreesNodeCountData;

        // Временные корни.
        std::vector<BerendZackerNode*> attachedChildSubtreeRoots;
    };

private:

    // Дерево Беренда-Заккера.
    BerendZackerTree berendZackerTree;
};

template<typename NodeType> const typename BerendZackerBWTreeColorer<NodeType>::ColorType BerendZackerBWTreeColorer<NodeType>::BLACK = BerendZackerBWTreeColorer<NodeType>::ColoringType::BLACK;
template<typename NodeType> const typename BerendZackerBWTreeColorer<NodeType>::ColorType BerendZackerBWTreeColorer<NodeType>::WHITE = BerendZackerBWTreeColorer<NodeType>::ColoringType::WHITE;
template<typename NodeType> const typename BerendZackerBWTreeColorer<NodeType>::ColorType BerendZackerBWTreeColorer<NodeType>::GRAY = BerendZackerBWTreeColorer<NodeType>::ColoringType::GRAY;
template<typename NodeType> const typename BerendZackerBWTreeColorer<NodeType>::ColorType BerendZackerBWTreeColorer<NodeType>::COLOR_COUNT = BerendZackerBWTreeColorer<NodeType>::ColoringType::COLOR_COUNT;
template<typename NodeType> const typename BerendZackerBWTreeColorer<NodeType>::ColorType BerendZackerBWTreeColorer<NodeType>::INVALID_COLOR = BerendZackerBWTreeColorer<NodeType>::ColoringType::INVALID_COLOR;

template<typename NodeType> const int BerendZackerBWTreeColorer<NodeType>::INVALID_MAX_WHITE = -1;
template<typename NodeType> const int BerendZackerBWTreeColorer<NodeType>::INVALID_MIN_GRAY = -1;

} // namespace bw.

#endif
