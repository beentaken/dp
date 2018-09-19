#include "Stdafx.h"
#include "Astar.h"
#include <utility>
#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>

using namespace AStar;

int g_bounds = 0;

AStar::AStarNode::AStarNode(Point p, AStarNode* p_ptr, float f, float g, OnList list):
  fcost(f), gcost(g), parent(p_ptr), pos(p), whichlist(list)
{
}

AStar::OpenList::OpenList() : last(0)
{
  //initialize all to nullptr
  for (size_t i = 0; i < openList.size(); ++i)
  {
    openList[i] = nullptr;
  }
}

AStar::AStarNode* AStar::OpenList::PopCheapest()
{
  //find lowest cost
  unsigned min = 0;
  for (int i = 0; i < last; ++i)
  {
    if (openList[i]->fcost < openList[min]->fcost)
    {
      min = i;
    }
  }

  //swap the cheapest with current last element
  std::swap(openList[min], openList[--last]);

  //remove from openlist
  openList[last]->whichlist = Closed;
  g_terrain.SetColor(openList[last]->pos.r, openList[last]->pos.c, DEBUG_COLOR_YELLOW);

  //return pointer to cheapest node from nodes
  int actualIndex = openList[last]->pos.singleIndex();
  return &nodes[actualIndex];
}

AStar::AStarNode* AStar::OpenList::find(const Point & pos)
{
  int actualIndex = pos.singleIndex();
  return &nodes[actualIndex];
}

void AStar::OpenList::UpdateCost(AStarNode* node, AStarNode* p_ptr, float newf, float newg)
{
  //update cost and parent ptr if its cheaper
  if (newf < node->fcost)
  {
    node->fcost = newf;
    node->gcost = newg;
    node->parent = p_ptr;
  }
}

void AStar::OpenList::push(const AStarNode & node)
{
  //add to openlist
  g_terrain.SetColor(node.pos.r, node.pos.c, DEBUG_COLOR_BLUE);

  //add to list of nodes
  int actualIndex = node.pos.singleIndex();
  nodes[actualIndex] = node;

  //add pointer to stored node to openlist
  openList[last] = &nodes[actualIndex];

  //increment end
  ++last;
}

void AStar::OpenList::clear()
{
  last = 0;
  //std::fill(openList.begin(), openList.end(), nullptr);
  std::fill(nodes.begin(), nodes.end(), AStarNode());
}

bool AStar::OpenList::empty() const
{
  return !last;
}

std::ostream & AStar::operator<<(std::ostream & out, const Point & p)
{
  out << "(" << static_cast<int>(p.r) << ", " << static_cast<int>(p.c) << ")" << std::endl;
  return out;
}

AStar::Point::Point(int row, int col) :
  r(row), 
  c(col)
{
}

Point & AStar::Point::operator+=(const Point & p)
{
  r += p.r;
  c += p.c;
  return *this;
}

Point & AStar::Point::operator-=(const Point & p)
{
  r -= p.r;
  c -= p.c;
  return *this;
}

Point AStar::Point::operator+(const Point & p) const
{
  Point result(r, c);
  result += p;
  return result;
}

Point AStar::Point::operator-(const Point & p) const
{
  Point result(r, c);
  result -= p;
  return result;
}

bool AStar::Point::operator==(const Point & p) const
{
  return (r == p.r) && (c == p.c);
}

bool AStar::Point::operator!=(const Point & p) const
{
  return (r != p.r) || (c != p.c);
}

bool AStar::Point::inBounds() const
{
  return r >= 0 && r < g_bounds && c >= 0 && c < g_bounds;
}

/*
inline int AStar::Point::row() const
{
  return r;
}

inline int AStar::Point::col() const
{
  return c;
}
*/

inline int AStar::Point::singleIndex() const
{
  return (r * g_bounds) + c;
}

float AStar::MovementAlgo::GetHCost(const Point& currPos, const Point& endPos)
{
  float rdiff = std::abs(static_cast<float>(endPos.r - currPos.r));
  float cdiff = std::abs(static_cast<float>(endPos.c - currPos.c));
  switch (heur)
  {
  case Manhattan:
  {
    return rdiff + cdiff;
  }
  case Octile:
  {
    float min = (std::min)(rdiff, cdiff);
    return (min * sqrtf(2.f)) + (std::max)(rdiff, cdiff) - min;
  }
  case Chebyshev:
  {
    return(std::max)(rdiff, cdiff);
  }
  case Euclidean:
  {
    return sqrtf((rdiff*rdiff) + (cdiff*cdiff));
  }
  default: //just octile
  {
    float min = (std::min)(rdiff, cdiff);
    return (min * sqrtf(2.f)) + (std::max)(rdiff, cdiff) - min;
  }
  }
}

AStar::MovementAlgo::MovementAlgo():
  heur(Octile),
  h_weight(0.f),
  isSingleStep(false)
{
  float diagCost = sqrtf(2.f);                                       //coordinate system is row, col
  directions[0] = std::make_pair(Point(1, 0), 1.f);                  //up
  directions[1] = std::make_pair(Point(-1, 0), 1.f);                 //down
  directions[2] = std::make_pair(Point(0, -1), 1.f);                 //left
  directions[3] = std::make_pair(Point(0, 1), 1.f);                  //right
  directions[4] = std::make_pair(Point(1, 1), diagCost);             //up_right
  directions[5] = std::make_pair(Point(-1, 1), diagCost);            //down_right
  directions[6] = std::make_pair(Point(1, -1), diagCost);            //up_left
  directions[7] = std::make_pair(Point(-1, -1), diagCost);           //down_left
}

bool AStar::MovementAlgo::PathFind(WaypointList & path)
{
  Terrain& terrain = g_terrain;
  MovementAlgo& g_algo = g_movement_algo;

  while (!openlist.empty())
  {
    AStarNode* current = openlist.PopCheapest();
    
    if (current->pos == goal) //if startnode is the goal
    {
      //follow parent pointer back to start from goal and add
      int curR, curC;
      D3DXVECTOR3 spot;
      AStarNode* next = current;
      while (next != nullptr)
      {
        curR = next->pos.r;
        curC = next->pos.c;
        spot = terrain.GetCoordinates(curR, curC);
        path.push_front(spot);
        next = next->parent;
      }
      return true;
    }
    else
    {
      Point neighbour;
      //add neighbours if they are valid
      for (unsigned i = 0; i < 8; ++i)
      {
        //diagonal correctness check
        if (i >= 4 && isDiagonalsWalls(current->pos, directions[i].first))
          continue;

        //who are my neighbours?
        neighbour = current->pos + directions[i].first;

        //if neighbour is out of map or is a wall, do not add
        if (!neighbour.inBounds()) continue;
        if (terrain.IsWall(neighbour.r, neighbour.c)) continue;

        //if node is on the closed list skip
        AStarNode* nptr = openlist.find(neighbour);
        if (nptr->whichlist == Closed) continue;        //comment if heuristic is not stable

        //compute total cost of node
        float hcost = h_weight * GetHCost(neighbour, goal);
        float gcost = current->gcost + directions[i].second;
        float fcost = hcost + gcost;

        //if this node is not in open or closed list, add as new node
        if (nptr->whichlist == None)
        {
          openlist.push(AStarNode(neighbour, current, fcost, gcost, Open));
        }
        else
        {
          openlist.UpdateCost(nptr, current, fcost, gcost);
        }
      }
    }
    //if singlestep break out of the while loop
    if (isSingleStep) return false;
  }

  //if unable to find path just push start node
  if (openlist.empty())
  {
    D3DXVECTOR3 spot = terrain.GetCoordinates(start.r, start.c);
    path.push_front(spot);
  }
  return true;
}

bool AStar::MovementAlgo::StraightLineCheck(const Point& start, const Point & end)
{
  Terrain& terrain = g_terrain;

  //return true if no walls
  Point delta = end - start;
  char r_sign = (delta.r > 0) ? 1 : -1;
  char c_sign = (delta.c > 0) ? 1 : -1;
  char r_end = delta.r + r_sign;
  char c_end = delta.c + c_sign;

  for (char i = 0; i != r_end; i += r_sign)
  {
    for (char j = 0; j != c_end; j += c_sign)
    {
      if (terrain.IsWall(start.r + i, start.c + j))
        return false;
    }
  }
  return true;
}

bool AStar::MovementAlgo::isDiagonalsWalls(const Point & pt, const Point & dir)
{
  Point pt1 = pt + Point(dir.r, 0);
  Point pt2 = pt + Point(0, dir.c);

  bool diag1 = pt1.inBounds() && g_terrain.IsWall(pt1.r, pt1.c);
  bool diag2 = pt2.inBounds() && g_terrain.IsWall(pt2.r, pt2.c);

  return diag1 || diag2;
}


void AStar::MovementAlgo::RubberbandPath(WaypointList& list)
{
  using LIter = WaypointList::iterator;
  Terrain& terrain = g_terrain;

  //if less than 3 points no need to rubberband
  if (list.size() < 3) return;

  //starting from the end of the list, check the middle node
  LIter front = list.begin();  
  LIter middle = list.begin(); ++middle;
  LIter back = list.begin(); std::advance(back, 2);

  while (back != list.end())
  {
    int r1, c1, r2, c2;
    terrain.GetRowColumn(&(*front), &r1, &c1);
    terrain.GetRowColumn(&(*back), &r2, &c2);
    Point first(r1, c1); Point last(r2, c2);

    //if no walls, remove the middle node
    if (StraightLineCheck(first, last))
    {
      list.erase(middle);
      middle = back;
      ++back;
    }
    //move all 3 iterators forward
    else
    {
      ++front; ++middle; ++back;
    }
  }
}

void AStar::MovementAlgo::SmoothPath(WaypointList& list)
{
  using LIter = WaypointList::iterator;
  //cant smooth if less than 2 points
  if (list.size() < 2) return;

  LIter first = list.begin();
  LIter last = list.end(); --last;
  LIter p1, p2, p3, p4;
  p1 = p2 = p3 = p4 = first;

  //set p1 p2 p3 p4 based on list size
  if (list.size() == 2)
  {
    p3 = std::next(p3, 1);
    p4 = std::next(p4, 1);
  }
  else
  {
    p3 = std::next(p3, 1);
    p4 = std::next(p4, 2);
  }

  //loop until p3 == p4
  while (1)
  {
    D3DXVECTOR3 out;
    for (int i = 1; i < 4; ++i)
    {
      //use catmullrom spline
      D3DXVec3CatmullRom(&out, &(*p1), &(*p2), &(*p3), &(*p4), i * 0.25f);
      list.insert(p3, out);
    }

    if (p3 == p4)
      break;

    //if at the beginning dont change p1
    if (p1 != p2) p1 = p2;
    p2 = p3;
    p3 = p4;
    //if p4 at last element dont advance p4
    if (p4 != last) ++p4;
  }
}

void AStar::MovementAlgo::SetBounds(int bounds)
{
  g_bounds = bounds;
}

void AStar::MovementAlgo::AddPointsForSmoothing(WaypointList & list)
{
  using LIter = WaypointList::iterator;
  float maxdist = 1.5f * (1.0f / static_cast<float>(g_bounds));

  if (list.size() < 2) return;
  LIter p1, p2;
  p1 = p2 = list.begin();
  ++p2;

  while (p2 != list.end())
  {
    //check distance between 2 points
    D3DXVECTOR3 dist = *p2 - *p1;
    float length = D3DXVec3Length(&dist);
    if (length > maxdist)
    { 
      //if dist is greater, add points in the middle until its less
      LIter temp = p2;
      int added = 0;
      while (length > maxdist)
      {
        //new pt is halfway between
        D3DXVECTOR3 newpt = *p1 + (0.5f * dist);
        list.insert(p2, newpt);
        ++added;
        p2 = std::next(p1, 1);
        //check length again
        dist = *p2 - *p1;
        length = D3DXVec3Length(&dist); 
      }

      //move p1 to the next point to check
      p1 = std::next(temp, -added);
      p2 = std::next(p1, 1);
    }
    else
    {
      //if not greater just increment point
      ++p1;
      ++p2; 
    }
  }
}
