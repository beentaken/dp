/* Start Header
*****************************************************************/
/*!
\file uShell.cpp
\author Chan Yu Hong, yuhong.chan, 390002115
\par yuhong.chan@digipen.edu
\date 28 Oct 2016
\brief
  This file is the implementation of the uShell class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/* End Header
*******************************************************************/ 
#include <iostream>
#include <regex>
#include <string>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <iterator>
#include <algorithm>
#include <stdlib.h>   //exit, _exit
#include <unistd.h>   //standard POSIX header file
#include <sys/wait.h> //wait

#include "commands.h"
#include "uShell.h"

//macros for file descriptors
#define READ_END 0
#define WRITE_END 1
#define PIPE_SIZE 2
#define ERROR -1
#define SHELLVAR_ENCLOSURE_LENGTH 4

//static command objects
static Echo echo_command;
static Exit exit_command;
static ChangePrompt prompt_command;
static SetVar setvar_command;
static RunExecutable run_command;
static Finish finish_command;
/******************************************************************************/
/*!
  \brief
    Constructor of uShell. Inits the private variables used
    to default values, including the default prompt.
  
*/
/******************************************************************************/
uShell::uShell():
  m_verbose(false),
  m_canPrint(true),
  m_running(true),
  m_ProcessIndex(0),
  m_exitCode(0),
  m_prompt("uShell")      //default prompt
{
  //nothing in constructor
}
/******************************************************************************/
/*!
  \brief
    Inits uShell by setting the pointers in the map to the static
    CommandObjects.
    Calls getenv to get the PATH variable and store as a shell variable
    Checks if the mode is verbose.
*/
/******************************************************************************/
void uShell::Init()
{
  //add command objects
  m_commands["echo"] = &echo_command;
  m_commands["exit"] = &exit_command;
  m_commands["changeprompt"] = &prompt_command;
  m_commands["setvar"] = &setvar_command;
  m_commands["runExe"] = &run_command;
  m_commands["finish"] = &finish_command;
  
  //init the PATH variable by calling getenv
  std::string path = getenv("PATH");
  m_variables["PATH"] = path;
  //tokenize the path and store in m_env
  TokenizePATH(path);
  
  //check if verbose
  std::string verbose_check("-v");
  //if there are extra options
  if (!m_argsVec.empty())
  {
    //if they match
   if(verbose_check.compare(m_argsVec[0]) == 0)
   {
     //std::cout << "verbose" << std::endl;
     m_verbose = true;
   }     
  }
  //cout the ">"
  std::cout << m_prompt << ">";
}
/******************************************************************************/
/*!
  \brief
    Tokenizes the input string by whitespace. Also Removes leading spaces
    from the string and changes all shell variables to their actual values.
  
*/
/******************************************************************************/
void uShell::Tokenize(const std::string& str, char delim)
{
  //reserve an amount of spaces = size of str/2
  m_tokens.reserve(str.size() >> 1);
  //ignore leading whitespaces
  std::string newStr = RemoveLeadingSpaces(str);
  //create a new input stringstream 
  std::istringstream iss(newStr);
  std::string comment("#");
  std::string whitespace(" ");
  std::string token;
  
  //split entire input string into tokens at delimiting character
  while (std::getline(iss, token, delim) )
  {
    if (token == comment)
    {
      //do not add anything after a comment
      break;
    }
    //ignore whitespaces
    if (token == whitespace)
    {
      continue;
    }
    //ignore empty strings
    if(token.empty())
    {
      continue;
    }
    //replace shell variables with their values
    token = ProcessToken(token);
    //add to the vector
    m_tokens.push_back(token);
  }
#if 0
  int x = 0;
  for (auto& elem : m_tokens)
  {
    std::cout << "token " << x << "is: " << elem << std::endl;
    ++x;
  }
#endif
}
/******************************************************************************/
/*!
  \brief
    Main loop of the uShell program. Gets input from the user
    and chooses a command to run based on the input.
*/
/******************************************************************************/
bool uShell::Run()
{
  //main prompt loop
  while(std::getline(std::cin, m_input, '\n'))
  {
    //if verbose, print out the input again
    if(m_verbose)
    {
      std::cout << m_input << std::endl;
    }
    //replace tabs
    m_input = ReplaceTabs(m_input);
    if(!m_input.empty())
    {
      //tokenize the input using whitespace
      Tokenize(m_input, ' ');
     
      //process what kind of command it is
      bool result = ProcessCommand();
      
      //clear the vector after command is executed if not empty
      if(result)
      {
        m_tokens.clear();
        m_processes.clear();
      }
    }
    //check if quiting
    if(!m_running)
    {
      return false;
    }
    //print prompt again before asking for user input
    std::cout << m_prompt << ">";
    //allow printing of error message for next input
    m_canPrint = true;
    //clear the string for next input
    m_input.clear();
  }
  return true;
}
/******************************************************************************/
/*!
  \brief
    Searches the map of CommandObjects to decide which command to run.
*/
/******************************************************************************/
bool uShell::ProcessCommand()
{
  //check if there are any tokens at all
  if(m_tokens.empty()) return false;
  //check for piping
  
  //check if should pipe or execute a exe
  if(CheckIfPipes())
  {
    DoPipes();
    //do piping and return if it is a piping command
    return true;
  }
  
  //check if it is a absolute or relative path by doing a regex
  //check for ".exe" pattern
  if(CheckIfExecutable(m_tokens[0]) )
  {
    //if its a executable and not a pipe, run the executable
    m_commands["runExe"]->Execute(m_tokens);
    return true;
  }
  
  //if not running a executable, we check the map
  std::string key(m_tokens.at(0));
  CommandObjectMap::iterator it = m_commands.find(key);

  //if there was no errors printed, therefore no error happened
  if(m_canPrint)
  {
    //if it is a valid command that can be found in the map
    if(it != m_commands.end())
    {
      //execute the command based on the first token.
      it->second->Execute(m_tokens);
    }
    else
    {
      PrintCommandNotFound(key);
    }
  }
  return true;
}
/******************************************************************************/
/*!
  \brief
    Changes all shell variables found in each substring separated
    by whitespace into their actual values if the shell variable
    can be found in the map.
*/
/******************************************************************************/
std::string uShell::ProcessToken(std::string token)
{
  //ignore strings less than length 4
  if(token.size() < SHELLVAR_ENCLOSURE_LENGTH ) return token;
  
  //length of leftbracket
  int lb_len = 2;
  //length of entire enclosure
  int ec_len = 3;
  
  //look for the last occurence of the innermost "${" enclosure
  size_t startpos = token.rfind("${");
  size_t previouspos = token.size();
  //while we can still find a leftbracket, from the end of the string
  while (startpos != std::string::npos)
  {
    //std::cout << "leftbracket found at position " << startpos << std::endl;
    //look for the first occurence of the '}' character
    size_t endpos = token.find_first_of('}', startpos);
    //if cannot find the '}' enclosure or the rightbracket is on the right side
    //of the previous leftbracket
    if(endpos == std::string::npos || endpos > previouspos)
    {
      break;
    }
    //everything in between the enclosure is the variable,
    //which will be used as the key to the map
    std::string::const_iterator varfirst = token.begin() + startpos + lb_len;
    std::string::const_iterator varEnd = token.begin() + endpos;
    std::string key(varfirst, varEnd);
    
    //otherwise we check the map to see if the variable exist
    ShellVariableMap::const_iterator it = m_variables.find(key);
    if(it != m_variables.end())
    {
      //use the actual value of the shell variable and replace original string
      //3 is the length of the enclosure "${}"
      token.replace(startpos, key.length() + ec_len, it->second);
    }
    else //cannot find variable
    {
      PrintVariableNotDefined(key);
      break;
    }
    //store previous position
    previouspos = startpos;
    startpos = token.rfind("${", startpos);
  }
  return token;
}
/******************************************************************************/
/*!
  \brief
    Remove leading spaces from the original input string.
*/
/******************************************************************************/
std::string uShell::RemoveLeadingSpaces(const std::string& str) const
{
  //find the first non whitespace
  size_t offset = str.find_first_not_of(' ');
  //if there was only empty spaces, return empty string
  if(offset == std::string::npos)
  {
    return std::string();
  }
  else //create a new string and return
  {
    return std::string(str.begin() + offset, str.end()); 
  }
}
/******************************************************************************/
/*!
  \brief
    Replaces all tabs with whitespace as our checks later only
    uses whitespaces.
*/
/******************************************************************************/
std::string uShell::ReplaceTabs(const std::string& string) const
{
  std::string newstring;
  std::string::const_iterator it;
  for(it = string.begin(); it != string.end(); ++it)
  {
    //replace all tabs with whitespace
    if(*it == '\t')
    {
      newstring.push_back(' ');
    }
    else
    {
      //add to newstring if it is not a tab
      newstring.push_back(*it);
    }
  }
  return newstring;
}
/******************************************************************************/
/*!
  \brief
    Called by SetVar object to add a variable into the map.
  
*/
/******************************************************************************/
void uShell::AddVariableToMap(const std::string& key, const std::string& value)
{
  m_variables[key] = value;
}
/******************************************************************************/
/*!
  \brief
    Prints the error that the command is not valid
*/
/******************************************************************************/
void uShell::PrintCommandNotFound(const std::string& value)
{
  if(m_canPrint)
  {
    //output error message for command not found
    std::cout << value << ": command not found" << std::endl;
    m_canPrint = false;
  }
}
/******************************************************************************/
/*!
  \brief
    Prints the error that the variable does not exist.
  
*/
/******************************************************************************/
void uShell::PrintVariableNotDefined(const std::string& var)
{
  if(m_canPrint)
  {
    //output error message for variable not defined
    std::cout << "Error: " << var << " is not a defined variable." << std::endl;
    m_canPrint = false;
  }
}
/******************************************************************************/
/*!
  \brief
    Called by ChangePrompt object to set the prompt.
  
*/
/******************************************************************************/
void uShell::SetPrompt(const std::string& prompt)
{
  m_prompt = prompt;
}
/******************************************************************************/
/*!
  \brief
    Adds the args from the main function of uShell into a vector
    to use later.
*/
/******************************************************************************/
void uShell::AddArgs(char* args)
{
  m_argsVec.emplace_back(args);
}
/******************************************************************************/
/*!
  \brief
    Uses std::regex to check if the input is a executable.
    Returns true if the input is a executable.
*/
/******************************************************************************/
bool uShell::CheckIfExecutable(const std::string& token) const
{
  //use a regular expression to check for occurence of ".exe"
  std::regex exeCheck("\\w+(.exe)$");
  //if we found a match
  if(std::regex_search(token, exeCheck) )
  {
    return true;
  }
  //else
  return false;
}

/******************************************************************************/

/******************************************************************************/
/*!
  \brief
    Assignment 2 functions
*/
/******************************************************************************/

/******************************************************************************/

/******************************************************************************/
/*!
  \brief
    Adds a process ID to the map to check
    Increments the number of processes in m_ProcessIndex
*/
/******************************************************************************/
void uShell::AddProcessID(int pid)
{
  m_pids[m_ProcessIndex] = pid;
  ++m_ProcessIndex;
}
/******************************************************************************/
/*!
  \brief
    Returns the number of processes created
*/
/******************************************************************************/
size_t uShell::getProcessIndex() const
{
  return m_ProcessIndex;
}
/******************************************************************************/
/*!
  \brief
    Marks a process as completed by making its process ID negative
*/
/******************************************************************************/
void uShell::MarkProcessAsDone(size_t index)
{
  ProcessIDMap::iterator it = m_pids.find(index);
  if(it != m_pids.end())
  {
    //if a process is done, we make the process id negative
    int id = it->second;
    it->second = -id;
  }
}
/******************************************************************************/
/*!
  \brief
    Finds the process ID using the process index
*/
/******************************************************************************/
bool uShell::FindProcessIDWithIndex(size_t index, int& pid) const
{
  ProcessIDMap::const_iterator it = m_pids.find(index);
  //if process id index can be found
  if(it != m_pids.end())
  {
    pid = it->second;
    return true;
  }
  else
  {
    return false;
  }
}
/******************************************************************************/
/*!
  \brief
    Checks if we should pipe by checking for the occurence
    of '|'
*/
/******************************************************************************/
bool uShell::CheckIfPipes()
{
  unsigned found_pipe_count = 0;
  //count the number of pipes
  for (auto& elem : m_tokens)
  {
    if (elem == "|")
      ++found_pipe_count;
  }
  if(found_pipe_count)
  {
    std::vector<std::string>::const_iterator it;
    //used to store the vector 
    std::vector<std::string> temp;
    //go through all the tokens and put each executable
    //and its arguments into a vector of strings
    for(it = m_tokens.begin(); it != m_tokens.end(); ++it)
    {
      if(*it != "|")
      {
        //if its not a pipe, push back into the temp vector
        temp.push_back(*it);
      }
      if(*it == "|" || it == std::prev(m_tokens.end()) )
      {
        //if its a pipe, push back in as a process and clear the temp vector
        m_processes.push_back(temp);
        temp.clear();
      }
    }
    return true;
  }
  else
  {
    return false;
  }
}
/******************************************************************************/
/*!
  \brief
    Tokenizes the path variable, splitting at ':'
*/
/******************************************************************************/
void uShell::TokenizePATH(const std::string& str)
{
  //create a new stringstream 
  std::stringstream ss(str);
  //split entire input string into tokens at ':'
  while (!ss.eof())
  {
    std::string token;
    std::getline(ss, token, ':') ;
    //add to the vector
    m_env.emplace_back(std::move(token));
  }
}
/******************************************************************************/
/*!
  \brief
    Implementation of pipes
*/
/******************************************************************************/
void uShell::DoPipes()
{
  //init 2 pipes
  int prevPipe[PIPE_SIZE];
  int currPipe[PIPE_SIZE];
  
  //variable i keeps track of the number of child processes we have created
  for (size_t i = 0; i < m_processes.size(); ++i)
  {
    //create a new currPipe
    if(pipe(currPipe) == ERROR)
      return;
    
    if(fork())
    {
      //if not the first process    
      if(i != 0)
      {
        close(prevPipe[READ_END]);
        close(prevPipe[WRITE_END]);
      }
      
      //if not the last process
      if(i != m_processes.size()-1)
      {
        //set prevPipe to currPipe as everything is now in currPipe
        //to pass into next child process
        prevPipe[READ_END] = currPipe[READ_END];
        prevPipe[WRITE_END] = currPipe[WRITE_END];
      }
    }
    else
    {
      //child process
      
      //the first child process does not need to read from the previous pipe
      //if not the first child process
      if(i != 0)
      {
        //replace child's stdin with child's prevPipe's read end
        dup2(prevPipe[READ_END], 0);
        //close pipes before executing
        close(prevPipe[READ_END]);
        close(prevPipe[WRITE_END]);
        
      }
      //the last child process does not need to write into the pipe
      //if not the last child process
      if(i != m_processes.size()-1)
      {
        //replace child's stdout with currPipe's write end
        dup2(currPipe[WRITE_END], 1);
        //close pipes before executing
        close(currPipe[READ_END]);
        close(currPipe[WRITE_END]);
      }
      
      //after pipes have been setup, we call ExecuteProcess to call exec.
      ExecuteProcess(m_processes[i]);
      //call exit to make sure the child process ends
      exit(0);
    }
  }
  
  //wait for all child processes to finish
  for (size_t i = 0; i <  m_processes.size(); ++i)
  {
    wait(NULL);
  }
  
  //close resources created by the parent (uShell) before exit function
  close(currPipe[READ_END]);
  close(currPipe[WRITE_END]);
}
/******************************************************************************/
/*!
  \brief
    Executes a process using execv, with the vector containing the executable
    name and its arguments
*/
/******************************************************************************/
bool uShell::ExecuteProcess(const std::vector<std::string>& process)
{
  //create a c-style array to store the arguments
  char* args[process.size() + 1]{nullptr};
  //if there are extra arguments
  for(size_t i = 0; i < process.size(); ++i)
  {
    //do not add & into argv if its the last token in the command
    if(i == process.size()- 1 && process[i] == "&")
    {
      break;
    }
    //otherwise add
    args[i] = const_cast<char*>(process[i].c_str());
  }
  //null terminate the array
  args[process.size()] = nullptr;
  
  //save the executable name to append to later
  std::string exeName = process[0];
  
  //child process
  bool run = true;
  //if process dont have ".exe", dont execute
  if (!CheckIfExecutable(exeName))
  {
    run = false;
    PrintCommandNotFound(exeName);
    return run;
  }
  
  //try to run using the first executable name
  if(execvp(exeName.c_str(), args) < 0)
  { 
    //to store the new executable name
    std::string newExecutable;
    for(auto& elem : m_env)
    {
      //append to the end.
      newExecutable = elem + "/" + exeName;
      args[0] =  const_cast<char*>(exeName.c_str());
      if(execvp(newExecutable.c_str(), args) < 0)
      {
        run = false;
      }
      else 
      {
        //if we can run, break from the loop.
        run = true;
        break;
      }
    }
  }
  //if failed to execute
  if(!run)
  {
    //PrintCommandNotFound(exeName);
    PrintExecutableNotValid(exeName);
  }
  return run;
}

/******************************************************************************/
/*!
  \brief
    Print out for an invalid executable  
*/
/******************************************************************************/
void uShell::PrintExecutableNotValid(const std::string& value)
{
  if(m_canPrint)
  {
    //output error message for executable not valid
    std::cout << "Error: " << value << " cannot be found\n";
    m_canPrint = false;
  }
}
/******************************************************************************/
/*!
  \brief
    Quits uShell by setting m_running to false and the exit code
*/
/******************************************************************************/
void uShell::Quit(int code)
{
  m_running = false;
  m_exitCode = code;
}
/******************************************************************************/
/*!
  \brief
    Gets the exit code for the main function to return
*/
/******************************************************************************/
int uShell::getExitCode() const
{
  return m_exitCode;
}

