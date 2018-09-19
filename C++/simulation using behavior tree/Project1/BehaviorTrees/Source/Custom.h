#include "Source\world.h"
#include <vector>
#include <string>
#include <map>

//macros
#define g_spawnmgr Singleton<Custom::SpawnManager>::GetInstance()
#define MAX_SPAWNER_COUNT 9

//using directives
using TinyContainerPtr = std::vector<CTiny*>*;
using MultiAnimPtr = CMultiAnim*;

//forward declarations
class GameObject;

namespace Custom
{
	struct TinyCreationData //singleton struct
	{
		MultiAnimPtr p_multianim;
    TinyContainerPtr p_tinys;
    CSoundManager* p_SM;

    double getTimeCurrent() { return DXUTGetGlobalTimer()->GetTime();}
	};

  struct AData
  {
    bool isAlive;
    std::string type;

    //default constructor
    AData() : isAlive(false) {}
  };

  struct SpawnManager
  {
    //player data and functions
    objectID playerID;
    bool PlayerAlive() { return alivecount < losecount; }
    //if number of enemies reaches this amount player loses
    int losecount;

    //spawn data
    using AgentMap = std::unordered_map<objectID, AData>;
    AgentMap agentmap;
    int maxcount;
    int spawnercount;
    int enemycount;
    int attackercount;
    int max_spawner_count;
    int alivecount;
    int killcount;
    
    //for object pooling
    bool canSpawn() { return (enemycount + spawnercount + attackercount) < maxcount; }

    bool SpawnTiny(char* agentname, const std::string& treename,
                   D3DXVECTOR3 pos, D3DXVECTOR3 color, int health = 100, bool randompos = false);
   
    void RemoveAgent(GameObject* go);

    bool isAlive(objectID id);

    GameObject* Recycle(const std::string& type);

  };

  using AgentMapIter = SpawnManager::AgentMap::iterator;

  
}