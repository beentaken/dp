/* Start Header
*****************************************************************/
/*!
\file uShell.h
\author Chan Yu Hong, yuhong.chan, 390002115
\par yuhong.chan@digipen.edu
\date 28 Oct 2016
\brief
  This file contains the interface of the class uShell

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/* End Header
*******************************************************************/ 
#ifndef USHELL_H
#define USHELL_H

#include <vector>
#include <map>

//forward declaration
class CommandObject;

/******************************************************************************/
/*!
  \class uShell
  \brief  
    Contains all the functions that run uShell.
    
*/
/******************************************************************************/
class uShell
{
public:
  uShell();
  void Init();
  bool Run();
  void AddArgs(char* args);
  void SetPrompt(const std::string& prompt);
  void PrintCommandNotFound(const std::string& value);
  void PrintVariableNotDefined(const std::string& var);
  void PrintExecutableNotValid(const std::string& var);
  void AddVariableToMap(const std::string& key, const std::string& value);
  void AddProcessID(int pid);
  void MarkProcessAsDone(size_t index);
  bool FindProcessIDWithIndex(size_t index, int& pid) const;
  size_t getProcessIndex() const;
  bool ExecuteProcess(const std::vector<std::string>& process);
  //void PrintVariableNotSet(const std::string& value);
  void Quit(int code);
  int  getExitCode() const;
  
  using CommandObjectMap = std::map<std::string, CommandObject*>;
  using ShellVariableMap = std::map<std::string, std::string>;
  using ProcessIDMap = std::map<size_t, int>;
private:
//functions
  void Tokenize(const std::string& str, char delim);
  bool ProcessCommand();
  bool CheckIfExecutable(const std::string& token) const;
  bool CheckIfPipes();
  void DoPipes();
  
//internal functions for string manipulation
  std::string ProcessToken(std::string token);
  std::string RemoveLeadingSpaces(const std::string&  str) const;
  std::string ReplaceTabs(const std::string& string) const;
  void TokenizePATH(const std::string& str);
//variables
  bool m_verbose;
  bool m_canPrint;
  bool m_running;
  size_t m_ProcessIndex;
  int m_exitCode;
  std::string m_prompt;
  std::string m_input;
//containers
  std::vector<std::string> m_argsVec;
  std::vector<std::string> m_tokens;
  std::vector<std::string> m_env;
  std::vector<std::vector<std::string> > m_processes;
  ProcessIDMap m_pids;
  ShellVariableMap m_variables;
  CommandObjectMap m_commands;
};


#endif
