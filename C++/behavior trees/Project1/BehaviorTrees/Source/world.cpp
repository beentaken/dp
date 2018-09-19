/* Copyright Steve Rabin, 2013. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2013"
 */

#include <Stdafx.h>
#include "Custom.h"
#include "singleton.h"

//#define UNIT_TESTING

World::World(void)
: m_initialized(false)
{

}

World::~World(void)
{
}

void World::InitializeSingletons( void )
{
	//Create Singletons
	m_clock = &g_clock;
	m_database = &g_database;
	m_msgroute = &g_msgroute;
	m_debuglog = &g_debuglog;
	m_random = &g_random;
}

void World::Initialize( CMultiAnim *pMA, std::vector< CTiny* > *pv_pChars, CSoundManager *pSM, double dTimeCurrent )
{
	if(!m_initialized)
	{
		m_initialized = true;
	

#ifdef UNIT_TESTING

		//Create unit test game objects
		GameObject* unittest1 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest1" );
		GameObject* unittest2 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest2" );
		GameObject* unittest3a = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest3a" );
		GameObject* unittest3b = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest3b" );
		GameObject* unittest4 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest4" );
		GameObject* unittest5 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest5" );
		GameObject* unittest6 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest6" );
		
		D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
		unittest1->CreateBody( 100, pos );
		unittest2->CreateBody( 100, pos );
		unittest3a->CreateBody( 100, pos );
		unittest3b->CreateBody( 100, pos );
		unittest4->CreateBody( 100, pos );
		unittest5->CreateBody( 100, pos );
		unittest6->CreateBody( 100, pos );

		unittest1->CreateStateMachineManager();
		unittest2->CreateStateMachineManager();
		unittest3a->CreateStateMachineManager();
		unittest3b->CreateStateMachineManager();
		unittest4->CreateStateMachineManager();
		unittest5->CreateStateMachineManager();
		unittest6->CreateStateMachineManager();
		
		g_database.Store( *unittest1 );
		g_database.Store( *unittest2 );
		g_database.Store( *unittest3a );
		g_database.Store( *unittest3b );
		g_database.Store( *unittest4 );
		g_database.Store( *unittest5 );
		g_database.Store( *unittest6 );

		//Give the unit test game objects a state machine
		unittest1->GetStateMachineManager()->PushStateMachine( *new UnitTest1( *unittest1 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest2->GetStateMachineManager()->PushStateMachine( *new UnitTest2a( *unittest2 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest3a->GetStateMachineManager()->PushStateMachine( *new UnitTest3a( *unittest3a ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest3b->GetStateMachineManager()->PushStateMachine( *new UnitTest3b( *unittest3b ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest4->GetStateMachineManager()->PushStateMachine( *new UnitTest4( *unittest4 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest5->GetStateMachineManager()->PushStateMachine( *new UnitTest5( *unittest5 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest6->GetStateMachineManager()->PushStateMachine( *new UnitTest6( *unittest6 ), STATE_MACHINE_QUEUE_0, TRUE );

#else
		g_trees.Initialize();		// initialize behavior tree

#if 0
    //normal tinys
    for (int i = 0; i<9; i++)
    {
      //Create game objects
      char name[10] = "Spawner";
      sprintf(name, "%s%d", name, i);
      GameObject* npc = new GameObject(g_database.GetNewObjectID(), OBJECT_NPC, name);
      D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
      pos.x = g_random.RangeFloat();
      pos.z = g_random.RangeFloat();
      npc->CreateBody(100, pos);
      npc->CreateMovement();
      npc->CreateTiny(pMA, pv_pChars, pSM, dTimeCurrent, 1.0f, 1.0f, 1.0f);	//Color if needed
      g_database.Store(*npc);

      /*
      g_trees.Register(name, "Example");							// register agent to behavior tree
      g_trees.GetAgentData(name).InitialTinyBlackBoard(npc);		// initialize local blackboard for each tiny
      */
      g_trees.Register(name, "SpawnerTree");							// register agent to behavior tree
      g_trees.GetAgentData(name).InitialTinyBlackBoard(npc);		// initialize local blackboard for each tiny
    }
#endif

    //player
    char playername[10] = "Player";
    GameObject* player = new GameObject(g_database.GetNewObjectID(), OBJECT_Player, playername);
    D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
    pos.x = g_random.RangeFloat();
    pos.z = g_random.RangeFloat();
    player->CreateBody(100, pos);
    player->CreateMovement();
    player->CreateTiny(pMA, pv_pChars, pSM, dTimeCurrent, 0.0f, 1.0f, 0.0f);	//Color if needed
    g_database.Store(*player);
    g_trees.Register(playername, "PlayerTree");							// register agent to behavior tree
    g_trees.GetAgentData(playername).InitialTinyBlackBoard(player);		// initialize local blackboard for each tiny

    //change player scale
    player->GetBody().SetScale(0.5f);

    //pass data needed to create tiny to singleton
    using namespace Custom;
    TinyCreationData& data = Singleton<TinyCreationData>::GetInstance();
    data.p_multianim = pMA;
    data.p_tinys = pv_pChars;
    data.p_SM = pSM;

    //initialize spawn data
    g_spawnmgr.spawnercount = 0;
    g_spawnmgr.max_spawner_count = 9;
    g_spawnmgr.attackercount = 0;
    g_spawnmgr.enemycount = 0;
    g_spawnmgr.maxcount = 75;      //max number of dynamic tinys
    g_spawnmgr.playerID = player->GetID();
    g_spawnmgr.killcount = 0;
    g_spawnmgr.alivecount = 0;
    g_spawnmgr.losecount = 75;

    //create spawners 
    D3DXVECTOR3 color(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 7; ++i)
    {
      char agent[10] = "Spawner";
      D3DXVECTOR3 pos(0.0f, 0.0f, 1.0f);
      pos.x = g_random.RangeFloat();
      pos.z = g_random.RangeFloat();
      g_spawnmgr.SpawnTiny(agent, "SpawnerTree", pos, color, 100, true);
    }
#endif

	}
}


void World::PostInitialize()
{
	g_database.Initialize();
}


void World::Update(float dt)
{
	g_clock.MarkTimeThisTick();
	g_database.Update();
	g_trees.Update(dt);
}

void World::Animate( double dTimeDelta )
{
	g_database.Animate( dTimeDelta );
}

void World::AdvanceTimeAndDraw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj, double dTimeDelta, D3DXVECTOR3 *pvEye )
{
	g_database.AdvanceTimeAndDraw( pd3dDevice, pViewProj, dTimeDelta, pvEye );
}

void World::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return( g_database.RestoreDeviceObjects( pd3dDevice ) );
}

void World::InvalidateDeviceObjects( void )
{
	g_database.InvalidateDeviceObjects();
}
