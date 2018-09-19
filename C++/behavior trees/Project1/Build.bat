Set vs_version=2017    :: 2013 or 2015 or 2017

goto case_%vs_version%           
:case_2013    
  cd BehaviorTrees\                        
  "%VS120COMNTOOLS%..\IDE\devenv" BehaviorTree.sln /build Release
  goto end_case
:case_2015            
  cd BehaviorTrees\                
  "%VS140COMNTOOLS%..\IDE\devenv" BehaviorTree.sln /build Release
  goto end_case
:case_2017
  cd BehaviorTrees\                
  "%VS150COMNTOOLS%..\IDE\devenv" BehaviorTree.sln /build Release
  goto end_case
:end_case
  Copy Release\BehaviorTrees.exe .\
  cd ..
  pause