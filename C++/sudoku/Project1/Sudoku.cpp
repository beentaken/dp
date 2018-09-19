/******************************************************************************/
/*!
\file   Sudoku.cpp
\author Chan Yu Hong
\par    email: yuhong.chan\@digipen.edu
\par    DigiPen login: yuhong.chan
\par    Course: CS280
\par    Assignment: 3
\date   01/03/2017
\brief  
  This file contains the implementation of the Sudoku class.
    
  Hours spent on this assignment:  12

  Specific portions that gave you the most trouble: backtracking and recursion

*/
/******************************************************************************/
#include "Sudoku.h"
#include <cstring> //std::strcpy
/******************************************************************************/
/*!
   Global constants
*/
/******************************************************************************/
static const char OPEN_CELL = '.';
static unsigned INITIAL_POS = 0;
/******************************************************************************/
/*!
  \brief
    Constructor of the Sudoku class
    Inits default values based on input arguments.
  
*/
/******************************************************************************/
Sudoku::Sudoku(int basesize, SymbolType stype, CALLBACK callback)
  :
  type_(stype),
  sendMsgFn_(callback),
  boardSize_(0),
  dimension_(basesize * basesize) 
{
  stats_.basesize = basesize;
  
  //assign starting and ending character based on type of sudoku.
  if (stype == SYM_NUMBER)
  {
    first_ = '1';
  }
  if (stype == SYM_LETTER)
  {
    first_ = 'A';
  }

  last_ = static_cast<char>(first_ + dimension_);
}
/******************************************************************************/
/*!
  \brief
    Destructor. Deallocates memory for board.
  
*/
/******************************************************************************/
Sudoku::~Sudoku()
{
  delete[] board_;
}
/******************************************************************************/
/*!
  \brief
    Gets the size and board from the client and initializes the board.
*/
/******************************************************************************/
void Sudoku::SetupBoard(const char *values, size_t size)
{
  //copy the board
  board_ = new char[size];
  std::strcpy(board_, values);

  //cast to unsigned for comparison
  boardSize_ = static_cast<unsigned>(size);

  //temporary solution to change all '.' to ' '
  for (unsigned i = 0; i < boardSize_; ++i)
  {
    if (board_[i] == OPEN_CELL)
    {
      board_[i] = EMPTY_CHAR;
    }
  }

}
/******************************************************************************/
/*!
  \brief
    Recursively calls place_value to try and find a solution. 
    Returns true if a solution is found
*/
/******************************************************************************/
bool Sudoku::Solve()
{
  sendMsg(MSG_STARTING, first_, INITIAL_POS);

  if (place_value(0, first_))
  {
    //algorithm has finished
    sendMsg(MSG_FINISHED_OK, first_, INITIAL_POS);
    return true;
  }
  else
  {
    sendMsg(MSG_FINISHED_FAIL, first_, INITIAL_POS);
    return false;
  }
}
/******************************************************************************/
/*!
  \brief
    Returns a pointer to the board
  
*/
/******************************************************************************/
const char* Sudoku::GetBoard() const
{
  return board_;
}
/******************************************************************************/
/*!
  \brief
    Returns the current stats of the board
*/
/******************************************************************************/
Sudoku::SudokuStats Sudoku::GetStats() const
{
  return stats_;
}
/******************************************************************************/
/*!
  \brief
    Tries to place a value on the next empty position in the board
    If values has a conflict, tries the next value
    Else try the next position.  
    When there are no positions left, a solution is found
    If all the recursive functions return false, there is no solution.
*/
/******************************************************************************/
bool Sudoku::place_value(unsigned index, char init)
{
  unsigned currPos = index;

  //if this is the last position left on the board
  if (!CheckForEmptyCells())
  {
    //solution found
    return true;
  }

  //place the value
  while (board_[currPos] != EMPTY_CHAR && currPos <= boardSize_ - 1)
    ++currPos;

  for (char value = init; value != last_; ++value)
  {
    //----------ATTEMPT TO PLACE VALUE INTO CELL----------//
    //increment stats
    ++stats_.moves;

    //check if ok to place
    if (!sendMsg(MSG_ABORT_CHECK, value, currPos))
    {
      ++stats_.placed;
      board_[currPos] = value;
    }
    else
    {
      return false;
    }
    //send a message after placing
    sendMsg(MSG_PLACING, value, currPos);
    //----------------------------------------------------//
    
    //if no conflicts
    if (CheckForConflicts(currPos, value) == false)
    {
      if (place_value(currPos + 1, first_))
      {
        return true;
      }
    }
    else
    {
      //check if ok to remove
      if (!sendMsg(MSG_ABORT_CHECK, value, currPos))
      {
        //remove the value
        --stats_.placed;
        board_[currPos] = EMPTY_CHAR;
      }
      else
      {
        return false;
      }
      //before removing send a msg
      sendMsg(MSG_REMOVING, value, currPos);
    }
  }

  //remove the value whenever backtracking
  --stats_.placed;
  board_[currPos] = EMPTY_CHAR;

  //increment backtracks
  ++stats_.backtracks;
  //return from function, 
  return false;
}
/******************************************************************************/
/*!
  \brief
    Checks if there is any equivalent values in the same row, same column
    and the same subsquare as the position that the value will be placed.
    If there is return true.
*/
/******************************************************************************/
bool Sudoku::CheckForConflicts(unsigned index, char value) const
{
  unsigned rowIndex = getRowFromIndex(index);
  unsigned colIndex = getColumnFromIndex(index);
  //check current row for conflicts
  int curr = colIndex - 1;
  //check all values to the left
  while (curr >= 0)
  {
    //return true if there is a conflict
    if (board_[ getActualIndex(rowIndex, curr) ] == value)
      return true;
    --curr;
  }
  //check all values to the right
  curr = colIndex + 1;
  while (curr <= dimension_ - 1)
  {
    //return true if there is a conflict
    if (board_[getActualIndex(rowIndex, curr)] == value)
      return true;
    ++curr;
  }

  //check current column for conflicts
  curr = rowIndex - 1;
  //check all values above this index
  while (curr >= 0)
  {
    //return true if there is a conflict
    if (board_[getActualIndex(curr, colIndex)] == value)
      return true;
    --curr;
  }
  //check all values below this index
  curr = rowIndex + 1;
  while (curr <= dimension_ - 1)
  {
    //return true if there is a conflict
    if (board_[getActualIndex(curr, colIndex)] == value)
      return true;
    ++curr;
  }
 
  //check subsquare for conflicts

  //get to the top left position in this subsquare
  unsigned startCol = colIndex - (colIndex % stats_.basesize);
  unsigned startRow = rowIndex - (rowIndex % stats_.basesize);

  //loop through rows and columns and check for conflicts
  for (unsigned i = startRow; i < startRow + stats_.basesize; ++i)
  {
    for (unsigned j = startCol; j < startCol + stats_.basesize; ++j)
    {
      //skip the value we just inserted
      if (i == rowIndex && j == colIndex)
      {
        continue;
      }
      //if found a value that is the same that was not inserted, return true
      if (board_[getActualIndex(i, j)] == value)
      {
        return true;
      }
    }
  }

  //otherwise no conflicts
  return false;
}
/******************************************************************************/
/*!
  \brief
    Checks if they are any empty cells left on the board.
    If there is return true.
*/
/******************************************************************************/
bool Sudoku::CheckForEmptyCells() const
{
  for (unsigned i = 0; i < boardSize_; ++i)
  {
    if (board_[i] == EMPTY_CHAR)
    {
      return true;
    }
  }
  return false;
}
/******************************************************************************/
/*!
  \brief
    Converts row and col into a single dimensional index.
*/
/******************************************************************************/
unsigned Sudoku::getActualIndex(unsigned row, unsigned col) const
{
  return row * dimension_ + col;
}
/******************************************************************************/
/*!
  \brief
    Gets the row from a single index  
*/
/******************************************************************************/
unsigned Sudoku::getRowFromIndex(unsigned index) const
{
  return index / dimension_;
}
/******************************************************************************/
/*!
  \brief
    Gets column from a single index
*/
/******************************************************************************/
unsigned Sudoku::getColumnFromIndex(unsigned index) const
{
  return index % dimension_;
}
/******************************************************************************/
/*!
  \brief
    Wrapper around the callback function
    Sends the type of message given in the input arguments.  
*/
/******************************************************************************/
bool Sudoku::sendMsg(MessageType msg, char value, unsigned pos)
{
  //just calls callback fn with msg
  return sendMsgFn_(*this, board_, msg, stats_.moves, 
                    stats_.basesize, pos, value);
}


