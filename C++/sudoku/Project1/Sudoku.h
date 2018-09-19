/******************************************************************************/
/*!
\file   Sudoku.h
\author Chan Yu Hong
\par    email: yuhong.chan\@digipen.edu
\par    DigiPen login: yuhong.chan
\par    Course: CS280
\par    Assignment: 3
\date   01/03/2017
\brief  
  
  This file contains the interface of the Sudoku class. 
  
  Hours spent on this assignment: 12

  Specific portions that gave you the most trouble: backtracking and recursion

*/
/******************************************************************************/
//---------------------------------------------------------------------------
#ifndef SUDOKUH
#define SUDOKUH
//---------------------------------------------------------------------------
#include <stddef.h> // size_t
/******************************************************************************/
/*!
  \class Sudoku
  \brief  
    The class that models the Sudoku game.
    Contains the board and functions to solve the sudoku
    and keep track of the stats.
*/
/******************************************************************************/
class Sudoku
{
  public:
      // Used by the callback function 
    enum MessageType
    {
      MSG_STARTING,      // the board is setup, ready to go
      MSG_FINISHED_OK,   // finished and found a solution
      MSG_FINISHED_FAIL, // finished but no solution found
      MSG_ABORT_CHECK,   // checking to see if algorithm should continue
      MSG_PLACING,       // placing a symbol on the board
      MSG_REMOVING       // removing a symbol (back-tracking)
    };

      // 1-9 for 9x9, A-P for 16x16, A-Y for 25x25
    enum SymbolType {SYM_NUMBER, SYM_LETTER};

    const static char EMPTY_CHAR = ' ';

      // Implemented in the client and called during the search for a solution
    typedef bool (*CALLBACK)
      (const Sudoku& sudoku, // the gameboard object itself
       const char *board,    // one-dimensional array of symbols
       MessageType message,  // type of message
       size_t move,          // the move number
       unsigned basesize,    // 3, 4, 5, etc. (for 9x9, 16x16, 25x25, etc.)
       unsigned index,       // index of current cell
       char value            // symbol (value) in current cell
      );

    struct SudokuStats
    {
      unsigned basesize; // 3, 4, 5, etc.
      unsigned placed;   // the number of values the algorithm has placed 
      size_t moves;      // total number of values that have been tried
      size_t backtracks; // total number of times the algorithm backtracked
      SudokuStats() : basesize(0), placed(0), moves(0), backtracks(0) {}
    };

      // Constructor
    Sudoku(int basesize, SymbolType stype = SYM_NUMBER, CALLBACK callback = 0);

      // Destructor
    ~Sudoku();

      // The client (driver) passed the board in the values parameter
    void SetupBoard(const char *values, size_t size);

      // Once the board is setup, this will start the search for the solution
    bool Solve();

      // For debugging with the driver
    const char *GetBoard() const;
    SudokuStats GetStats() const;

  private:
    //void place_value(unsigned row, unsigned col, unsigned value);
    bool place_value(unsigned index, char init);
    bool CheckForConflicts(unsigned index, char value) const;
    bool CheckForEmptyCells() const;

    //helpers
    unsigned getActualIndex(unsigned row, unsigned col) const;
    unsigned getRowFromIndex(unsigned index) const;
    unsigned getColumnFromIndex(unsigned index) const;
    bool sendMsg(MessageType msg, char value, unsigned pos);

    
    //variables
    SudokuStats stats_;
    char* board_;
    SymbolType type_;
    CALLBACK sendMsgFn_;
    unsigned boardSize_;
    int dimension_;
    char first_;
    char last_;
};

#endif  // SUDOKUH
