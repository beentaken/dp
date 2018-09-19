#pragma once
#include <array>
#include <unordered_map>
#include <cfloat>
#include <iostream>
#include "movement.h"

#define MAX_NODES 1600 //biggest map is 40x40

namespace AStar
{
  struct Point
  {
    Point(int row = 0, int col = 0);
    Point& operator+=(const Point& p);
    Point& operator-=(const Point& p);
    Point operator+(const Point& p) const;
    Point operator-(const Point& p) const;
    bool operator==(const Point& p) const;
    bool operator!=(const Point& p) const;
    bool inBounds() const;
    int singleIndex() const;
    //int row() const;
    //int col() const;
    friend std::ostream& operator<<(std::ostream& out, const Point& p);

    int r;
    int c;
  };
};

namespace std
{
  template<>
  struct hash<AStar::Point>
  {
    inline std::size_t operator()(const AStar::Point& p) const
    {
      return ((std::hash<char>{}(p.r) ^ (std::hash<char>{}(p.c) << 1)) >> 1);
    }
  };
};

namespace AStar
{
  enum OnList 
  {
    None,
    Open,
    Closed
  };

  struct AStarNode
  {
    AStarNode(Point p = Point(), AStarNode* p_ptr = nullptr, float f = FLT_MAX, 
              float g = 0.f, OnList = None);
    Point pos;
    float fcost;
    float gcost;
    AStarNode* parent;
    OnList whichlist;
  };

  class OpenList
  {
  public:
    OpenList();
    AStarNode* PopCheapest();
    AStarNode* find(const Point& pos);
    void UpdateCost(AStarNode* node, AStarNode* p_ptr, float newf, float newg);
    void push(const AStarNode& node);
    void clear();
    bool empty() const;

  private:   
    //variables
    int last;
    std::array<AStarNode*, MAX_NODES> openList;
    std::array<AStarNode, MAX_NODES> nodes;
  };

  enum Heuristic
  {
    Euclidean = 0,
    Octile,
    Chebyshev,
    Manhattan
  };

  class MovementAlgo
  {
  public:
    using DirCostPair = std::pair<Point, float>;
    MovementAlgo();
    bool PathFind(WaypointList& path);
    float GetHCost(const Point& currPos, const Point& endPos);
    bool StraightLineCheck(const Point& start, const Point& end);
    bool isDiagonalsWalls(const Point& pt, const Point& dir);
    void RubberbandPath(WaypointList& path);
    void AddPointsForSmoothing(WaypointList& path);
    void SmoothPath(WaypointList& path);
    void SetBounds(int bounds);

    AStar::OpenList openlist;
    std::array<DirCostPair, 8> directions;
    AStar::Heuristic heur;
    float h_weight;
    Point goal;
    Point start;
    bool isSingleStep;
  };
};









