Set vs_version=2017    :: 2013 or 2015 or 2017

goto case_%vs_version%      
:case_2013
  cd Pathfinding\                        
  "%VS120COMNTOOLS%..\IDE\devenv" PathfindingProject.sln /build Release
  goto end_case
:case_2015
  cd Pathfinding\                
  "%VS140COMNTOOLS%..\IDE\devenv" PathfindingProject.sln /build Release
  goto end_case
:case_2017
  cd Pathfinding\                
  "%VS150COMNTOOLS%..\IDE\devenv" PathfindingProject.sln /build Release
  goto end_case
:end_case
  Copy Release\PathfindingProject.exe .\
  cd ..
  pause