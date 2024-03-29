/*!******************************************************************
 * \file      Sudoku.cpp
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course: 
 * \par       Section: A
 * \par      
 * \date      29-03-2024
 * 
 * \brief     This file contains the definitions for the Sudoku.
 *********************************************************************/

#include "Sudoku.h"

/**
 * @brief Constructs a Sudoku object.
 *
 * This constructor initializes a Sudoku object with the specified parameters.
 *
 * @param basesize The size of the base grid. This determines the size of each sub-grid in the Sudoku puzzle.
 * @param stype The symbol type used in the Sudoku puzzle.
 * @param callback The callback function to be called when a solution is found.
 */
Sudoku::Sudoku(int basesize, SymbolType stype, SUDOKU_CALLBACK callback)
    : symbol_t{stype}, cb{callback}
{
  length = basesize * basesize;
  stats.basesize = basesize;
  board = new char[length * length];
}


/**
 * @brief Destructor for the Sudoku class.
 * 
 * This destructor is responsible for cleaning up any resources
 * allocated by the Sudoku class.
 */
Sudoku::~Sudoku()
{
  delete[] board;
}


/**
 * @brief Sets up the Sudoku board with the given values.
 * 
 * This function takes an array of characters representing the initial values of the Sudoku board,
 * and sets up the board accordingly. The size parameter specifies the number of elements in the array.
 * 
 * @param values An array of characters representing the initial values of the Sudoku board.
 * @param size The number of elements in the values array.
 */
void Sudoku::SetupBoard(const char *values, size_t size)
{
  for (size_t i = 0; i < size; ++i)
    board[i] = values[i] == '.' ? EMPTY_CHAR : values[i];
}


/**
 * Solves the Sudoku puzzle.
 * This function uses a backtracking algorithm to find the solution for the Sudoku puzzle.
 * It starts by finding an empty cell in the puzzle and tries different numbers (1-9) in that cell.
 * If a number is valid, it moves to the next empty cell and repeats the process.
 * If a number is not valid, it backtracks to the previous cell and tries a different number.
 * This process continues until a valid solution is found or all possibilities are exhausted.
 */
void Sudoku::Solve()
{
  unsigned x = 0;
  unsigned y = 0;

  cb(*this, board, MessageType::MSG_STARTING, stats.moves, stats.basesize, -1, 0);

  if (place_value(x, y))
    cb(*this, board, MessageType::MSG_FINISHED_OK, stats.moves, stats.basesize, -1, 0);
  else
    cb(*this, board, MessageType::MSG_FINISHED_FAIL, stats.moves, stats.basesize, -1, 0);
}


/**
 * @brief Get the current state of the Sudoku board.
 * 
 * @return const char* A pointer to the Sudoku board.
 */
const char *Sudoku::GetBoard() const
{
  return board;
}


/**
 * @class SudokuStats
 * @brief Represents the statistics of a Sudoku puzzle.
 *
 * This class stores the statistics related to a Sudoku puzzle, such as the number of solved cells,
 * the number of backtracks performed, and the time taken to solve the puzzle.
 */
Sudoku::SudokuStats Sudoku::GetStats() const
{
  return stats;
}


/**
 * Places a value in the Sudoku grid at the specified coordinates.
 *
 * @param x The x-coordinate of the cell in the grid.
 * @param y The y-coordinate of the cell in the grid.
 * @return True if the value was successfully placed, false otherwise.
 */
bool Sudoku::place_value(unsigned x, unsigned y)
{
  if (y == length)
    return true;
  unsigned index = static_cast<unsigned>(x + length * y);

  if (board[index] != EMPTY_CHAR)
  {
    if (x == length - 1)
    {
      if (place_value(0, y + 1))
        return true;
    }
    else
    {
      if (place_value(x + 1, y))
        return true;
    }
    return false;
  }

  char val = symbol_t == SymbolType::SYM_NUMBER ? '1' : 'A';

  for (size_t i = 0; i < length; ++i)
  {
    if (cb(*this, board, MessageType::MSG_ABORT_CHECK, stats.moves, stats.basesize, index, val))
      return false;

    board[index] = val;
    ++stats.moves;
    ++stats.placed;
    cb(*this, board, MessageType::MSG_PLACING, stats.moves, stats.basesize, index, val);
    if (CheckValidMove(x, y, val))
    {
      if (x == length - 1)
      {
        if (place_value(0, y + 1))
          return true;
      }
      else
      {
        if (place_value(x + 1, y))
          return true;
      }

      board[index] = EMPTY_CHAR;
      ++stats.backtracks;
      cb(*this, board, MessageType::MSG_REMOVING, stats.moves, stats.basesize, index, val);
    }

    board[index] = EMPTY_CHAR;
    --stats.placed;
    cb(*this, board, MessageType::MSG_REMOVING, stats.moves, stats.basesize, index, val);
    ++val;
  }

  return false;
}


/**
 * Checks if a move is valid in the Sudoku grid.
 *
 * @param x The x-coordinate of the cell.
 * @param y The y-coordinate of the cell.
 * @param val The value to be placed in the cell.
 * @return True if the move is valid, false otherwise.
 */
bool Sudoku::CheckValidMove(unsigned x, unsigned y, char val)
{
  auto index = x + length * y;

  for (size_t i = 0; i < length; ++i)
  {

    auto row_index = i + length * y;
    auto col_index = x + length * i;

    if ((index != row_index) && (val == board[row_index]))
      return false;

    if ((index != col_index) && (val == board[col_index]))
      return false;
  }

  auto max_width = static_cast<unsigned>(stats.basesize - 1);
  while (x > max_width)
    max_width += stats.basesize;

  auto max_height = static_cast<unsigned>(stats.basesize - 1);
  while (y > max_height)
    max_height += stats.basesize;

  auto min_width = max_width - stats.basesize + 1;
  auto min_height = max_height - stats.basesize + 1;

  for (auto i = min_height; i <= max_height; ++i)
  {
    for (auto j = min_width; j <= max_width; ++j)
    {
      auto cur_index = j + length * i;
      if (i == y || j == x)
        continue;

      if (board[cur_index] == val)
        return false;
    }
  }
  return true;
}