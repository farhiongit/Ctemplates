#define _GNU_SOURCE
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>
#include <ctype.h>

#include "list_impl.h"
#include "set_impl.h"
#include "map_impl.h"

#if DEBUG >= 1
#  define PRINT(...) printf(__VA_ARGS__)
#else
#  define PRINT(...) sizeof ((__VA_ARGS__))
#endif

#define WHERESTR  "[file %s, function %s, line %d]: "
#define WHEREARG  __FILE__, __func__, __LINE__
#define FATALPRINT2(...)      fprintf(stderr, __VA_ARGS__)
#define FATALPRINT(_fmt, ...)  FATALPRINT2(WHERESTR _fmt, WHEREARG, __VA_ARGS__)
#define FATAL(cond,msg) \
do { \
  if (!(cond)) \
  { \
    const char *__str__ = (msg); \
    if (!__str__ || !*__str__) \
      __str__ = "Fatal error"; \
    FATALPRINT ("%s (condition '%s' is false.)\n", __str__, #cond); \
    exit(0) ; \
  } \
} while(0)

#if DEBUG >= 2
#  define PRINT2(...)      fprintf(stderr, __VA_ARGS__)
#  define ASSERT(cond,msg) FATAL(cond,msg)
#else
#  define PRINT2(...)      sizeof ((__VA_ARGS__))
#  define ASSERT(cond,msg) do { if ((cond)){} } while(0)
#endif

/// Macro de contrôle d'allocation mémoire à l'exécution.
#define CHECK_ALLOC(ptr) FATAL(ptr, "Memory allocation error.")

#ifdef XXL
// Using intmax_t for a space as large as possible.
#  define XYPOS_TYPE intmax_t
#  define XYPOS_FORMAT PRIiMAX
#  define XYPOS_MIN INTMAX_MIN
#  define XYPOS_MAX INTMAX_MAX
#else
#  define XYPOS_TYPE int
#  define XYPOS_FORMAT "i"
#  define XYPOS_MIN INT_MIN
#  define XYPOS_MAX INT_MAX
#endif

typedef struct
{
  XYPOS_TYPE x, y;
} XYPos;

typedef struct
{
  int alive, nb_neighbors;
} Status;

/* *INDENT-OFF* */
DECLARE_LIST (XYPos);
DECLARE_MAP (XYPos, Status);
typedef BNODE (XYPos, Status) *Node;
DECLARE_LIST (Node);
DECLARE_SET (Node);

DEFINE_OPERATORS (XYPos);
DEFINE_OPERATORS (Status);
DEFINE_OPERATORS (Node);

DEFINE_LIST (XYPos);
DEFINE_MAP (XYPos, Status);
DEFINE_LIST (Node);
DEFINE_SET (Node);
/* *INDENT-ON* */

__attribute__ ((__unused__))
     static int less_than (XYPos a, XYPos b)
{
  return a.x < b.x || (a.x == b.x && a.y < b.y);
}

typedef struct
{
  struct
  {
    XYPos xymin, xymax;
  } extension;
  int B, S;
  size_t generation, nb_cells, max_cells;
    MAP (XYPos, Status) * neighbourhood_positions;
    LIST (XYPos) * born;
    LIST (Node) * dead;
    SET (Node) * modified_neighborhood;
} GameOfLife;

static int
increment_nine_fields (LNODE (XYPos) * birth, void *g)
{
  XYPos *p = LNODE_VALUE (birth);       // Position of the cell.
  FATAL (p->x != XYPOS_MIN && p->y != XYPOS_MIN && p->x != XYPOS_MAX
         && p->y != XYPOS_MAX, "Space boundaries were reached !!!\n");

  GameOfLife *gol = g;
  PRINT2 ("+ (%+" XYPOS_FORMAT ", %+" XYPOS_FORMAT ")\n", p->x, p->y);
  FATAL (gol->nb_cells++ < SIZE_MAX, "Overflow");       // Check that nb_cells does not exceed size_t maximum value before increment.

  if (p->x > gol->extension.xymax.x)
    gol->extension.xymax.x = p->x;
  else if (p->x < gol->extension.xymin.x)
    gol->extension.xymin.x = p->x;

  if (p->y > gol->extension.xymax.y)
    gol->extension.xymax.y = p->y;
  else if (p->y < gol->extension.xymin.y)
    gol->extension.xymin.y = p->y;

  // Cell will count for 1 in each of the nine positions around cell.
  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      XYPos neighbour;
      neighbour.x = p->x + dx;
      neighbour.y = p->y + dy;

      Node neighbour_pos;
      if ((neighbour_pos = MAP_KEY (gol->neighbourhood_positions, neighbour)) != MAP_END (gol->neighbourhood_positions))
      {
        if (dx == 0 && dy == 0)
          BNODE_VALUE (neighbour_pos)->alive = 1;
        else
          BNODE_VALUE (neighbour_pos)->nb_neighbors++;
      }
      else
      {
        Status s;
        /* *INDENT-OFF* */
        if (dx == 0 && dy == 0)
          s = (Status){.alive = 1,.nb_neighbors = 0};
        else
          s = (Status){.alive = 0,.nb_neighbors = 1};
        /* *INDENT-ON* */
        neighbour_pos = MAP_SET_VALUE (gol->neighbourhood_positions, neighbour, s);     // CPU consuming, due to XYPos comparisons
      }

      SET_INSERT (gol->modified_neighborhood, neighbour_pos);
    }

  return EXIT_SUCCESS;
}

static int
decrement_nine_fields (LNODE (Node) * death, void *g)
{
  XYPos *p = BNODE_KEY (*LNODE_VALUE (death));  // Position of the cell.
  FATAL (p->x != XYPOS_MIN && p->y != XYPOS_MIN && p->x != XYPOS_MAX
         && p->y != XYPOS_MAX, "Space boundaries were reached !!!\n");

  GameOfLife *gol = g;
  PRINT2 ("- (%+" XYPOS_FORMAT ", %+" XYPOS_FORMAT ")\n", p->x, p->y);
  ASSERT (gol->nb_cells--, 0);  // Check that nb_cells is > 0 before decrement.

  // Cell will count for 1 in each of the nine positions around cell.
  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      XYPos neighbour;
      neighbour.x = p->x + dx;
      neighbour.y = p->y + dy;

      Node neighbour_pos;
      ASSERT ((neighbour_pos =
               MAP_KEY (gol->neighbourhood_positions, neighbour)) != MAP_END (gol->neighbourhood_positions), 0);
      if (dx == 0 && dy == 0)
        BNODE_VALUE (neighbour_pos)->alive = 0;
      else
      {
        ASSERT (BNODE_VALUE (neighbour_pos)->nb_neighbors, 0);
        BNODE_VALUE (neighbour_pos)->nb_neighbors--;
      }

      SET_INSERT (gol->modified_neighborhood, neighbour_pos);
    }

  return EXIT_SUCCESS;
}

static int
find_changes (SNODE (Node) * pos, void *g)
{
  GameOfLife *gol = g;
  XYPos *p = BNODE_KEY (*SNODE_KEY (pos));
  Status *s = BNODE_VALUE (*SNODE_KEY (pos));

  if (s->alive)
  {
    if ((1 << s->nb_neighbors) & gol->S)        // Survive
    {
      // Any live cell with two or three live neighbors lives on to the next generation.
    }
    else
    {
      // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
      // Any live cell with more than three live neighbors dies, as if by overpopulation.
      LIST_APPEND (gol->dead, *SNODE_KEY (pos));
      PRINT2 ("! (%+" XYPOS_FORMAT ", %+" XYPOS_FORMAT ") has %i neighbors > -\n", p->x, p->y, s->nb_neighbors);
    }
  }
  else
  {
    if (!s->nb_neighbors)
    {
      // Garbage cllecting useless dead and isolated position
      MAP_REMOVE (gol->neighbourhood_positions, *SNODE_KEY (pos));
    }
    else if ((1 << s->nb_neighbors) & gol->B)   // Gives birth
    {
      // Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
      LIST_APPEND (gol->born, *p);
      PRINT2 ("! (%+" XYPOS_FORMAT ", %+" XYPOS_FORMAT ") has %i neighbors > +\n", p->x, p->y, s->nb_neighbors);
    }
    else
    {
      // Dead cells with other than three live neighbors remain dead.
    }
  }

  return EXIT_SUCCESS;
}

static size_t
GOL_next_generation (GameOfLife * gol)
{
  // Each generation is a pure function of the preceding one.
  SET_CLEAR (gol->modified_neighborhood);
  LIST_TRAVERSE (gol->born /* XYPos */ , increment_nine_fields, gol);
  LIST_TRAVERSE (gol->dead /* Node  */ , decrement_nine_fields, gol);

  if (gol->nb_cells > gol->max_cells)
    gol->max_cells = gol->nb_cells;

  PRINT ("Generation %zu has %zu cells (maximum %zu cells over [%+" XYPOS_FORMAT ";%+" XYPOS_FORMAT "] x [%+"
         XYPOS_FORMAT ";%+" XYPOS_FORMAT "]).\n", gol->generation, gol->nb_cells, gol->max_cells,
         gol->extension.xymin.x, gol->extension.xymax.x, gol->extension.xymin.y, gol->extension.xymax.y);

  FATAL (gol->generation++ < SIZE_MAX, "Overflow");

  PRINT ("%zu / %zu = %.1f %% of the universe is modified.\n",
         SET_SIZE (gol->modified_neighborhood), MAP_SIZE (gol->neighbourhood_positions),
         100. * SET_SIZE (gol->modified_neighborhood) / MAP_SIZE (gol->neighbourhood_positions));

  LIST_CLEAR (gol->born);
  LIST_CLEAR (gol->dead);
  // Each generation is created by applying the game rules simultaneously to every cell in the seed; births and deaths occur simultaneously.
  SET_TRAVERSE (gol->modified_neighborhood /* Node */ , find_changes, gol);

  return gol->nb_cells;
}

static GameOfLife *
GOL_init (void)
{
  //SET_LESS_THAN_OPERATOR (XYPos, less_than);

  GameOfLife *gol;
  CHECK_ALLOC (gol = malloc (sizeof (*gol)));

  gol->generation = 0;
  gol->nb_cells = gol->max_cells = 0;
  gol->extension.xymin.x = gol->extension.xymin.y = gol->extension.xymax.x = gol->extension.xymax.y = 0;
  //B3/S23
  gol->B = 1 << 3;
  gol->S = (1 << 2) | (1 << 3);

  CHECK_ALLOC (gol->neighbourhood_positions = MAP_CREATE (XYPos, Status));
  CHECK_ALLOC (gol->born = LIST_CREATE (XYPos));
  CHECK_ALLOC (gol->dead = LIST_CREATE (Node));
  CHECK_ALLOC (gol->modified_neighborhood = SET_CREATE (Node));

  return gol;
}

static void
GOL_destroy (GameOfLife * gol)
{
  LIST_DESTROY (gol->born);
  LIST_DESTROY (gol->dead);
  SET_DESTROY (gol->modified_neighborhood);
  MAP_DESTROY (gol->neighbourhood_positions);
  free (gol);
}

static size_t
RLE_read (LIST (XYPos) * l, const char *RLE)
{
  XYPOS_TYPE x = 0;
  XYPOS_TYPE y = 0;
  size_t length = 0;
  long int counter = 1;

  LIST_CLEAR (l);

  const char *c = RLE;
  while (*c && *c != '!')
  {
    switch (*c)
    {
      case 'O':                // alive cell
      case 'X':                // alive cell
      case 'o':                // alive cell
        for (; counter >= 1; counter--)
        {
          // *INDENT-OFF*
          LIST_APPEND (l, ((XYPos){.x = x,.y = y}));
          // *INDENT-ON*
          FATAL (x++ < XYPOS_MAX, "Overflow");
        }
        c++;
        counter = 1;
        break;
      case '.':                // dead cell
      case 'b':                // dead cell
        for (; counter >= 1; counter--)
          FATAL (x++ < XYPOS_MAX, "Overflow");
        c++;
        counter = 1;
        break;
      case '$':                // end of line
        for (; counter >= 1; counter--)
          FATAL (y-- > XYPOS_MIN, "Overflow");
        x = 0;
        c++;
        counter = 1;
        break;
      default:
        if (isdigit (*c))
        {
          char *endptr = 0;
          errno = 0;
          counter = strtol (c, &endptr, 10);
          ASSERT (endptr > c && errno == 0 && counter >= 1, "Invalid number");
          c = endptr;
        }
        else
        {
          // ignore
        }
        break;
    }
  }

  return length;
}

int
main (void)
{
  PRINT ("Space size is [%+" XYPOS_FORMAT ";%+" XYPOS_FORMAT "] x [%+" XYPOS_FORMAT ";%+" XYPOS_FORMAT "].\n",
         XYPOS_MIN, XYPOS_MAX, XYPOS_MIN, XYPOS_MAX);

  GameOfLife *gol = GOL_init ();

  const char *pattern =
    // Queen bee shuttle, period 30:
    //"9bo12b$7bobo12b$6bobo13b$2o3bo2bo11b2o$2o4bobo11b2o$7bobo12b$9bo!";
    // Gosper glider gun:
    //"24bo11b$22bobo11b$12b2o6b2o12b2o$11bo3bo4b2o12b2o$2o8bo5bo3b2o14b$2o8bo3bob2o4bobo11b$10bo5bo7bo11b$11bo3bo20b$12b2o!";
    // R-pentomino, stabilizes at generation 1103 with 116 cells, including one escaped glider at generation 69:
    //".XX$XX$.X";
    // Pentadecathlon (period 15):
    //"10X";
    // Blinker:
    //"3X";
    // Glider:
    //".X.$..X$XXX";
    // Acorn, takes 5206 generations to stabilize to 633 cells, including 13 escaped gliders:
    //"bo5b$3bo3b$2o2b3o!";
  // Tee or Tetromino, stabilizes to 12 cells in a 9x9 square at 10th generation.
  //"XXX$.X.";
  // Die-hard, eventually disappears after 130 generations
  //"......X$XX$.X...XXX";
  // Infinite growth, block-laying switch engine that leaves behind two-by-two still life blocks as its translates itself across the game's universe.
  //"......X$....X.XX$....X.X$....X$..X$X.X";
  // 18-cell 40514-generation methuselah. The stable pattern that results from 40514M (excluding 70 escaping gliders) has 3381 cells and consists of 248 blinkers (including 21 traffic lights), 218 blocks, 163 beehives (including nine honey farms), 56 loaves, 39 boats, 10 ships, nine tubs, five ponds, four beacons, two toads, one barge, one eater 1 and one long boat.
  "77bo$77bo$77bo21$3o20$3bo$3bo$3bo5$20b3o$9b3o10bo$22bo$21bo!";

  RLE_read (gol->born, pattern);

  const size_t MAX_GENERATIONS = 10000;
  while ((!MAX_GENERATIONS || gol->generation <= MAX_GENERATIONS) && GOL_next_generation (gol))
  {
    /* nothing */
  }

  GOL_destroy (gol);
}

/*
TODO:
  - curses display
  - informations: number of cells, generation rank
  - center of the window
  - active arrows or sliders forward outside cells on left, right,top, bottom.
  - grid, cells
  - play, pause button
  - add and remove cells with mouse click.
  - programmable drift of the center of the window (dx moves horizontally and dy moves vertically every n generation).
*/
