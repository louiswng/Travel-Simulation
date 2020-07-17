#ifndef FIBONACCIHEAP_H
#define FIBONACCIHEAP_H

#include "schedule.h"

class FibonacciHeap {
public:
    FibonacciHeap();			/* 初始化变量 */
    void initNode(int, double);	/* 初始化结点 */
    void merge(int);			/* 插入结点u */
    void link(int, int);		/* 连接两结点 */
    void consolidate();			/* 确保没有根相同的度 */
    int extractMin();			/* 提取最小值 */
    void cut(int);				/* 切断关系 */
    void cutRec(int);			/* 确定关系是否应该被切断 */
    void decreaseDanger(int, int);	/* decrease key[u]到val */
private:
    std::vector<int> pa, child, left, right, deg;/* 分别为父亲、儿子、左右子树、度 */
    std::vector<double> danger;		/* 结点的风险值 */
    std::vector<bool> vis;			/* 结点是否已被访问 */
    int minNode;					/* 最小danger结点的索引 */
    int rootNum;					/* 根结点数量 */
};

#endif // FIBONACCIHEAP_H
