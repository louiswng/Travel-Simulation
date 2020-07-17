#include "fibonacciheap.h"

FibonacciHeap::FibonacciHeap():minNode(-1), rootNum(0)
{
    pa.resize(cityAmount);
    child.resize(cityAmount);
    left.resize(cityAmount);
    right.resize(cityAmount);
    deg.resize(cityAmount);
    danger.resize(cityAmount);
    vis.resize(cityAmount);
}

void FibonacciHeap::initNode(int u, double _danger)
{
    pa[u] = child[u] = -1;
    left[u] = right[u] = u;
    danger[u] = _danger;
    deg[u] = 0;
    vis[u] = false;
}
void FibonacciHeap::merge(int u)
{
    pa[u] = -1;
    left[u] = right[u] = u;
    if (minNode == -1) minNode = u;
    right[left[minNode]] = u;
    left[u] = left[minNode];
    left[minNode] = u;
    right[u] = minNode;
    if (danger[minNode] > danger[u]) minNode = u;
    ++rootNum;
}

void FibonacciHeap::link(int u, int v)
{
    right[left[v]] = right[v];
    left[right[v]] = left[v];
    left[v] = right[v] = v;
    assert(danger[v] >= danger[u]);
    assert(pa[v] == -1 && pa[u] == -1);
    pa[v] = u;

    if (child[u] == -1) child[u] = v;
    right[left[child[u]]] = v;
    left[v] = left[child[u]];
    left[child[u]] = v;
    right[v] = child[u];
    child[u] = v;
    deg[u]++;
    --rootNum;
}
void FibonacciHeap::consolidate()
{
    int arr[40];
    memset(arr, -1, sizeof(arr));
    int u = minNode;
    for (int _ = rootNum; _; _--)
    {
        int nxt = right[u];
        while (~arr[deg[u]])
        {
            int v = arr[deg[u]];
            arr[deg[u]] = -1;
            if (danger[u] > danger[v])
                std::swap(u, v);
            link(u, v);
        }
        arr[deg[u]] = u;
        u = nxt;
    }

    minNode = -1;
    rootNum = 0;
    for (int i = 0; i < 40; i++)
        if (~arr[i]) merge(arr[i]);
}
int FibonacciHeap::extractMin()
{
    if (minNode == -1) return -1;
//		assert(~minNode);
    assert(pa[minNode] == -1);
    int lastMinNode = minNode;
    if (~child[lastMinNode])
    {
        int u = child[lastMinNode];
        do {
            int nxt = right[u];
            merge(u);
            u = nxt;
        } while (u != child[lastMinNode]);
    }
    right[left[lastMinNode]] = right[lastMinNode];
    left[right[lastMinNode]] = left[lastMinNode];
    minNode = right[lastMinNode];
    rootNum--;
    if (lastMinNode == right[lastMinNode] && child[lastMinNode] == -1)
        minNode = -1;
    else
        consolidate();
    return lastMinNode;
}
void FibonacciHeap::cut(int u)
{
    assert(~pa[u]);
    if (u == right[u])
        child[pa[u]] = -1;
    right[left[u]] = right[u];
    left[right[u]] = left[u];
    if (u == child[pa[u]] && u != right[u])
        child[pa[u]] = right[u];
    deg[pa[u]]--;
    merge(u);
}
void FibonacciHeap::cutRec(int u)
{
    if (pa[u] == -1) return;
    if (vis[u]) {
        int _ = pa[u];
        cut(u);
        cutRec(_);
    }
    else
        vis[u] = true;
}
void FibonacciHeap::decreaseDanger(int u, int val)
{
    danger[u] = val;
    if (~pa[u] && danger[u] < danger[pa[u]])
    {
        int _ = pa[u];
        cut(u);
        cutRec(_);
    }
    if (danger[u] < danger[minNode])
        minNode = u;
}
