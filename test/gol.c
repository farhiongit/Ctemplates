#define _GNU_SOURCE
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "set_impl.h"
#include "map_impl.h"

#define DEBUG 1
#if DEBUG >= 1
#  define PRINT(...) printf(__VA_ARGS__)
#else
#  define PRINT(...) sizeof (__VA_ARGS__)
#endif

#if DEBUG >= 2
#  define PRINT2(...) printf(__VA_ARGS__)
#else
#  define PRINT2(...) sizeof (__VA_ARGS__)
#endif

typedef struct
{
  intmax_t x, y;
} XYPos;

DECLARE_SET (XYPos);
DECLARE_MAP (XYPos, int);

DEFINE_OPERATORS (int);
DEFINE_OPERATORS (XYPos);

DEFINE_SET (XYPos);
DEFINE_MAP (XYPos, int);

static int
less_than (XYPos a, XYPos b)
{
  return a.x < b.x || (a.x == b.x && a.y < b.y);
}

static int
compute_nine_fields (SNODE (XYPos) * cell, void *n)
{
  XYPos *p = SNODE_KEY (cell);
  if (p->x == INTMAX_MIN || p->y == INTMAX_MIN || p->x == INTMAX_MAX || p->y == INTMAX_MAX)
    return EXIT_FAILURE;

  MAP (XYPos, int) *neighbourhood_cells = n;

  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      XYPos neighbour;
      neighbour.x = p->x + dx;
      neighbour.y = p->y + dy;

      BNODE (XYPos, int) * neighbour_cell;
      if ((neighbour_cell = MAP_KEY (neighbourhood_cells, neighbour)) != MAP_END (neighbourhood_cells))
        MAP_SET_VALUE (neighbourhood_cells, neighbour, *BNODE_VALUE (neighbour_cell) + 1);
      else
        MAP_SET_VALUE (neighbourhood_cells, neighbour, 1);
    }

  return EXIT_SUCCESS;
}

static int
next_generation (BNODE (XYPos, int) * cell, void *l)
{
  SET (XYPos) * living_cells = l;
  SNODE (XYPos) * n;

  // If the sum of all nine fields in a given neighborhood is three, the inner field state for the next generation will be life;
  // if the all-field sum is four, the inner field retains its current state;
  // and every other sum sets the inner field to death.
  switch (*BNODE_VALUE (cell))
  {
    case 3:
      SET_INSERT (living_cells, *BNODE_KEY (cell));
      break;
    case 4:
      break;
    default:
      n = SET_FIND (living_cells, *BNODE_KEY (cell));
      if (n != SET_END (living_cells))
        SET_REMOVE (living_cells, n);
      break;
  }

  return EXIT_SUCCESS;
}

__attribute__ ((unused))
     static int print_neighbours (BNODE (XYPos, int) * cell, void *null)
{
  (void) null;

  PRINT2 ("Cell (%" PRIiMAX ", %" PRIiMAX ") has %i neighbours.\n", BNODE_KEY (cell)->x, BNODE_KEY (cell)->y,
          *BNODE_VALUE (cell));

  return EXIT_SUCCESS;
}

__attribute__ ((unused))
     static int print_cells (SNODE (XYPos) * cell, void *null)
{
  (void) null;

  PRINT2 ("Cell (%" PRIiMAX ", %" PRIiMAX ") is alive.\n", SNODE_KEY (cell)->x, SNODE_KEY (cell)->y);

  return EXIT_SUCCESS;
}

int
main (void)
{
  SET_LESS_THAN_OPERATOR (XYPos, less_than);

  SET (XYPos) * living_cells = SET_CREATE (XYPos);

  //XYPos cells[] = { {0, 0}, {0, 1}, {0, -1} };  // blinker
  //XYPos cells[] = { {-1, 0}, {0, 0}, {1, 0}, {1, 1}, {0, 2} };  // glider
  XYPos cells[] = { {0, 0}, {0, 1}, {0, 4}, {0, 5}, {0, 6}, {1, 3}, {2, 1} };   // Acorn, takes 5206 generations to stabilize to 633 cells, including 13 escaped gliders
  //XYPos cells[] = { {0, 0}, {0, 1}, {0, -1}, {1, 0} };  // Tee
  for (size_t i = 0; i < sizeof (cells) / sizeof (*cells); i++)
    SET_INSERT (living_cells, cells[i]);

  PRINT ("Generation %zu has %zu cells:\n", (size_t) 0, SET_SIZE (living_cells));
#if DEBUG >= 2
  SET_TRAVERSE (living_cells, print_cells, 0);
#endif

  MAP (XYPos, int) * neighbourhood_cells = MAP_CREATE (XYPos, int);

  const size_t MAX_GENERATIONS = 1000000;
  for (size_t generation = 0; (!MAX_GENERATIONS || generation < MAX_GENERATIONS) && SET_SIZE (living_cells);
       generation++)
  {
    PRINT2 ("Generation %zu:\n", generation + 1);

    // Generate a new generation.
    // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    // Any live cell with two or three live neighbors lives on to the next generation.
    // Any live cell with more than three live neighbors dies, as if by overpopulation.
    // Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    // Each generation is created by applying the above rules simultaneously to every cell in the seed; births and deaths occur simultaneously.
    SET_TRAVERSE (living_cells, compute_nine_fields, neighbourhood_cells);

#if DEBUG >= 2
    SET_TRAVERSE (neighbourhood_cells, print_neighbours, 0);
#endif
    PRINT2 ("----\n");

    SET_TRAVERSE (neighbourhood_cells, next_generation, living_cells);

    PRINT ("Generation %zu has %zu cells:\n", (size_t) generation + 1, SET_SIZE (living_cells));
#if DEBUG >= 2
    SET_TRAVERSE (living_cells, print_cells, 0);
#endif

    MAP_CLEAR (neighbourhood_cells);
  }

  MAP_DESTROY (neighbourhood_cells);
  SET_DESTROY (living_cells);
}
