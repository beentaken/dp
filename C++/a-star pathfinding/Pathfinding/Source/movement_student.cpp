/* Copyright Steve Rabin, 2012. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2012"
 */
#include <Stdafx.h>
#include "Astar.h"

bool Movement::ComputePath( int r, int c, bool newRequest )
{
	m_goal = g_terrain.GetCoordinates( r, c );
	m_movementMode = MOVEMENT_WAYPOINT_LIST;

	// project 2: change this flag to true
	bool useAStar = true;

	if( useAStar )
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////
		//INSERT YOUR A* CODE HERE
		//1. You should probably make your own A* class.
		//2. You will need to make this function remember the current progress if it preemptively exits.
		//3. Return "true" if the path is complete, otherwise "false".
		///////////////////////////////////////////////////////////////////////////////////////////////////
    using namespace AStar;
    Terrain& terrain = g_terrain;
    MovementAlgo& g_algo = g_movement_algo;
    
    if (newRequest)
    {
      //clear previous lists
      m_waypointList.clear();
      g_algo.openlist.clear();

      //store the goal
      int goal_r, goal_c;
      terrain.GetRowColumn(&m_goal, &goal_r, &goal_c);
      Point goal(goal_r, goal_c);
      g_algo.goal = goal;

      //get start node
      D3DXVECTOR3 pos = m_owner->GetBody().GetPos();
      int curR, curC;
      terrain.GetRowColumn(&pos, &curR, &curC);
      Point start(curR, curC);
      g_algo.start = start;

      //init variables
      g_algo.heur = static_cast<Heuristic>(m_heuristicCalc);
      g_algo.isSingleStep = m_singleStep;
      g_algo.h_weight = m_heuristicWeight;
      g_movement_algo.SetBounds(g_terrain.GetWidth());

      //push start node onto openlist, gcost is zero, parent is nullptr
      float s_cost = g_algo.GetHCost(start, goal) * m_heuristicWeight;
      g_algo.openlist.push(AStarNode(start, nullptr, s_cost, 0.f, Open));
    }

    if (m_straightline)
    {
      //if its a straight line just push goal position
      if (g_algo.StraightLineCheck(g_algo.start, g_algo.goal))
      {
        D3DXVECTOR3 pos = terrain.GetCoordinates(g_algo.goal.r, g_algo.goal.c);
        m_waypointList.push_back(pos);
        return true;
      }
    }

    //run pathfinding
    bool isComplete;
    isComplete =  g_algo.PathFind(m_waypointList);

    //if path is complete 
    if (isComplete)
    {
      //rubberbanding
      if (m_rubberband)
        g_algo.RubberbandPath(m_waypointList);

      //add points back for smoothing
      if (m_rubberband && m_smooth)
        g_algo.AddPointsForSmoothing(m_waypointList);

      //smoothing
      if (m_smooth)
        g_algo.SmoothPath(m_waypointList);
    }

    return isComplete;
	}
	else
	{	
		//Randomly meander toward goal (might get stuck at wall)
		int curR, curC;
		D3DXVECTOR3 cur = m_owner->GetBody().GetPos();
		g_terrain.GetRowColumn( &cur, &curR, &curC );

		m_waypointList.clear();
		m_waypointList.push_back( cur );

		int countdown = 100;
		while( curR != r || curC != c )
		{
			if( countdown-- < 0 ) { break; }

			if( curC == c || (curR != r && rand()%2 == 0) )
			{	//Go in row direction
				int last = curR;
				if( curR < r ) { curR++; }
				else { curR--; }

				if( g_terrain.IsWall( curR, curC ) )
				{
					curR = last;
					continue;
				}
			}
			else
			{	//Go in column direction
				int last = curC;
				if( curC < c ) { curC++; }
				else { curC--; }

				if( g_terrain.IsWall( curR, curC ) )
				{
					curC = last;
					continue;
				}
			}

			D3DXVECTOR3 spot = g_terrain.GetCoordinates( curR, curC );
			m_waypointList.push_back( spot );
			g_terrain.SetColor( curR, curC, DEBUG_COLOR_YELLOW );
		}
		return true;
	}
}
