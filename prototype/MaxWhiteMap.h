#ifndef BW_MAX_WHITE_MAP_H
#define BW_MAX_WHITE_MAP_H

namespace bw {

class MaxWhiteMap {

public:

    static const int INVALID_MAX_WHITE = -1;

public:

    MaxWhiteMap(const int newNodeCount) : map(new int[newNodeCount + 1]), nodeCount(newNodeCount) {}

    void set(const int blackCount, const int maxWhite) {
        map[blackCount] = maxWhite;
    }

    int get(const int blackCount) const {
        return blackCount >= 0 && blackCount <= nodeCount ? map[blackCount] : INVALID_MAX_WHITE;
    }

    int getNodeCount() const {
        return nodeCount;
    }

    ~MaxWhiteMap() {
        delete[] map;
    }

private:

    int* map;
    int nodeCount;
};

} // namespace bw.

#endif
