Student Name: Chan Yu Hong

Project Name: Attack of the Tinys

What I implemented:
Dynamic Spawning of Tinys and object pooling
Seek and Flee
4 behavior Trees satisfying requirements
10 nodes (3 decorators, 7 leafs)

Summary/Directions:
Basically a game. Click to spawn the player, which is green and slightly bigger in color
Left Mouse click to move the player. While moving, Player will flash red when it collides with enemies 
and destroy them. The player character will also get bigger as it destroys enemies

3 kinds of enemies, all white in color : Attackers, Enemies and Spawners

When the game starts, a predetermined number of spawners will be on the field.
Attackers will seek towards the players periodically, dropping the player's health.
Spawners will try to flee the players periodically, Periodically they will spawn more enemies as long as they are on the field, with chances to spawn Attackers, Enemies or Spawners. Before they spawn they will flash
in random colors.
All types of enemies have random movement.

Winning condition: Destroy all spawners
Losing condition: Run out of health: Player will turn black and be unable to move.
All initial stats of the game can be modified in world.cpp.


What I liked about the project and framework:
The behavior tree logic and nodes worked very well. The engine was easy to understand and use.

What I disliked about the project and framework:
Unable to delete Tinys at runtime or unregister and register trees without destabilizing
the engine and possibly causing a crash.

Any difficulties I experienced while doing the project:
Encountered vector iterator not dereferencable problem when trying to delete objects at runtime.


Hours spent: 36hrs

New selector node (name): None

New decorator nodes (names): 
D_AlwaysFail, 
D_DelayXSeconds, 
D_RepeatXTimes

10 total nodes (names):
L_DiscoFlasher,
L_FleePlayer,
L_SeekPlayer,
L_IncreaseSize,
L_MoveToNearestTarget,
L_PlayerCollision,
L_TinySpawner,
D_AlwaysFail, 
D_DelayXSeconds, 
D_RepeatXTimes

4 Behavior trees (names):
AttackerTree.bht
EnemyTree.bht
PlayerTree.bht
SpawnerTree.bht

Extra credit: None attempted.