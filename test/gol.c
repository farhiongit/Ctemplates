#define _GNU_SOURCE
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <regex.h>
#include <locale.h>

#include "list_impl.h"
#include "set_impl.h"
#include "map_impl.h"

__attribute__ ((__unused__))
     static void nop (const char *format, ...)
{
  (void) format;
}

#define NOP(...) do { nop (__VA_ARGS__); } while (0)
#if DEBUG >= 1
#  define PRINT(...) do { fprintf(stderr, __VA_ARGS__) ; fflush (stderr) ; } while (0)
#else
#  define PRINT(...) NOP(__VA_ARGS__)
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
    exit(EXIT_FAILURE) ; \
  } \
} while(0)

#define IFNOTEXIT(cond, ...) \
do { \
  if (!(cond)) \
  { \
    fprintf (stderr, "" __VA_ARGS__); \
    fprintf (stderr, "\n"); \
    exit (EXIT_FAILURE); \
  } \
} while(0)

#if DEBUG >= 2
#  define PRINT2(...)      PRINT (__VA_ARGS__)
#  define ASSERT(cond,msg) FATAL(cond,msg)
#else
#  define PRINT2(...)      NOP(__VA_ARGS__)
#  define ASSERT(cond,msg) do { if ((cond)){} } while(0)
#endif

#define CHECK_ALLOC(ptr) FATAL(ptr, "Memory allocation error.")

#ifdef XXL
// Using intmax_t for a space as large as possible.
#  define XYPOS_TYPE intmax_t
#  define XYPOS_FORMAT "ji"
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
typedef void *Node;   // Iterator of a MAP (XYPos, Status)
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

static int
less_than_xypos (XYPos a, XYPos b)
{
  return a.x < b.x || (a.x == b.x && a.y < b.y);
}

typedef struct
{
  unsigned int B, S;
  size_t generation, nb_cells, max_cells;
/* *INDENT-OFF* */
  MAP (XYPos, Status) * neighbourhood_positions;   // Dictionary of populated positions and their status
  LIST (XYPos) * born;                             // Temporary list of born cells for the next generation
  LIST (Node) * dead;                              // Temporary list of dead cells for the next generation
  SET (Node) * modified_neighborhood;              // Temporary set of cells which neighborhood is modified by the current generation
/* *INDENT-ON* */
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

  // Cell will count for 1 in each of the nine positions around cell.
  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      XYPos neighbour;
      neighbour.x = p->x + dx;
      neighbour.y = p->y + dy;

      BNODE (XYPos, Status) * neighbour_pos;
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
  XYPos *p = BNODE_KEY ((BNODE (XYPos, Status) *) * LNODE_VALUE (death));       // Position of the cell.
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

      BNODE (XYPos, Status) * neighbour_pos;
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
  XYPos *p = BNODE_KEY ((BNODE (XYPos, Status) *) * SNODE_KEY (pos));
  Status *s = BNODE_VALUE ((BNODE (XYPos, Status) *) * SNODE_KEY (pos));

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
  else                          // if (!s->alive)
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
  PRINT ("_");
  LIST_TRAVERSE (gol->born /* XYPos */ , increment_nine_fields, gol);
  PRINT ("\bo_");
  LIST_TRAVERSE (gol->dead /* Node */ , decrement_nine_fields, gol);
  PRINT ("\bl");

  if (gol->nb_cells > gol->max_cells)
    gol->max_cells = gol->nb_cells;

  PRINT (">Generation #%'zu has %'zu cells [+%zu-%zu] (maximum being %'zu cells).\n", gol->generation, gol->nb_cells, LIST_SIZE (gol->born), LIST_SIZE (gol->dead), gol->max_cells);

  FATAL (gol->generation++ < SIZE_MAX, "Overflow");

  PRINT ("    (%'zu / %'zu = %.1f %% of the active cells and their neighborhoods are modified.)\n",
         SET_SIZE (gol->modified_neighborhood), MAP_SIZE (gol->neighbourhood_positions),
         100. * SET_SIZE (gol->modified_neighborhood) / MAP_SIZE (gol->neighbourhood_positions));

  LIST_CLEAR (gol->born);
  LIST_CLEAR (gol->dead);
  // Each generation is created by applying the game rules simultaneously to every cell in the seed; births and deaths occur simultaneously.
  PRINT ("_");
  SET_TRAVERSE (gol->modified_neighborhood /* Node */ , find_changes, gol);
  PRINT ("\bg");

  return gol->nb_cells;
}

static GameOfLife *
GOL_init (void)
{
  SET_LESS_THAN_OPERATOR (XYPos, less_than_xypos);

  GameOfLife *gol;
  CHECK_ALLOC (gol = malloc (sizeof (*gol)));

  gol->generation = 0;
  gol->nb_cells = gol->max_cells = 0;
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
RLE_readfile (GameOfLife * gol, FILE * f, XYPOS_TYPE x, XYPOS_TYPE y, int header)
{
  if (header)
  {
    char *line = 0;
    size_t length;
    do
    {
      if (getline (&line, &length, f) < 0)
      {
        free (line);
        line = 0;
      }
    }
    while (line && *line == '#');

    IFNOTEXIT (line, "Missing header line");

    regex_t regex;
    ASSERT (regcomp (&regex, " *([[:alnum:]]+) *= *([^ ,]+) *,?", REG_EXTENDED | REG_ICASE) == 0,
            "Invalid ERE. Comma separated parameters of the form 'var=value' expected.");
    regmatch_t match[3];
    for (size_t offset = 0;
         regexec (&regex, line + offset, sizeof (match) / sizeof (*match), match, REG_NOTBOL | REG_NOTEOL) == 0;
         offset += match[0].rm_eo)
    {
      if (!strncmp ("rule", line + offset + match[1].rm_so, match[1].rm_eo - match[1].rm_so))
      {
        ASSERT (regcomp (&regex, "B([[:digit:]]+)/S([[:digit:]]+)", REG_EXTENDED | REG_ICASE) == 0, "Invalid ERE");
        regmatch_t matchBS[3];
        IFNOTEXIT (regexec
                   (&regex, line + offset + match[2].rm_so, sizeof (matchBS) / sizeof (*matchBS), matchBS,
                    REG_NOTBOL | REG_NOTEOL) == 0, "Invalid format for 'rule'. Format 'rule=Bnnn/Snnn' expected.");

        gol->B = gol->S = 0;
        for (const char *c = line + offset + match[2].rm_so + matchBS[1].rm_so;
             c < line + offset + match[2].rm_so + matchBS[1].rm_eo; c++)
        {
          IFNOTEXIT (isdigit (*c), "Invalid number '%c' for rule B", *c);
          gol->B |= 1 << (*c - '0');
        }
        for (const char *c = line + offset + match[2].rm_so + matchBS[2].rm_so;
             c < line + offset + match[2].rm_so + matchBS[2].rm_eo; c++)
        {
          IFNOTEXIT (isdigit (*c), "Invalid number '%c' for rule S", *c);
          gol->S |= 1 << (*c - '0');
        }
      }
    }
    free (line);
  }

  long unsigned int counter = 1;

  LIST_CLEAR (gol->born);

  for (int c = 0; (c = fgetc (f)) && c != '!' && c != EOF;)
  {
    PRINT2 ("%c", c);
    switch (c)
    {
      case 'O':                // alive cell
      case 'X':                // alive cell
      case 'o':                // alive cell
      case 'x':                // alive cell
        for (; counter >= 1; counter--)
        {
          // *INDENT-OFF*
          LIST_APPEND (gol->born, ((XYPos){.x = x,.y = y}));
          // *INDENT-ON*
          FATAL (x++ < XYPOS_MAX, "Overflow");
        }
        counter = 1;
        break;
      case '.':                // dead cell
      case 'b':                // dead cell
      case 'B':                // dead cell
        for (; counter >= 1; counter--)
          FATAL (x++ < XYPOS_MAX, "Overflow");
        counter = 1;
        break;
      case '$':                // end of line
        for (; counter >= 1; counter--)
          FATAL (y-- > XYPOS_MIN, "Overflow");
        x = 0;
        counter = 1;
        break;
      default:
        if (isdigit (c))
        {
          ungetc (c, f);
          errno = 0;
          IFNOTEXIT (fscanf (f, "%lu", &counter) == 1, "Invalid character '%c'", c);
        }
        else
          IFNOTEXIT (isblank (c) || iscntrl (c), "Invalid character '%c'", c);
        break;
    }
  }

  return LIST_SIZE (gol->born);
}

int
main (int argc, char *const argv[])
{
  setlocale (LC_ALL, "");

  int opt;
  XYPOS_TYPE x = 0;
  XYPOS_TYPE y = 0;
  size_t MAX_GENERATIONS = 0;
  const char *optstring = ":Hg:x:y:B:S:";
  char *endptr = 0;
  long int li = 0;
  int header = 0, B = 0, S = 0;
  while ((opt = getopt (argc, argv, optstring)) != -1)
  {
    switch (opt)
    {
      case 'H':
        header = 1;
        break;
      case 'x':
        errno = 0;
        x = strtol (optarg, &endptr, 10);
        IFNOTEXIT (optarg && *optarg && *endptr == 0, "Invalid number %s for option '-%c'", optarg, opt);
        break;
      case 'y':
        errno = 0;
        y = strtol (optarg, &endptr, 10);
        IFNOTEXIT (optarg && *optarg && *endptr == 0, "Invalid number %s for option '-%c'", optarg, opt);
        break;
      case 'B':
        B = 0;
        for (const char *c = optarg; optarg && *c; c++)
        {
          IFNOTEXIT (isdigit (*c), "Invalid number %c for option '-%c'", *c, opt);
          B |= 1 << (*c - '0');
        }
        break;
      case 'S':
        S = 0;
        for (const char *c = optarg; *c; c++)
        {
          IFNOTEXIT (isdigit (*c), "Invalid number %c for option '-%c'", *c, opt);
          S |= 1 << (*c - '0');
        }
        break;
      case 'g':
        errno = 0;
        li = strtol (optarg, &endptr, 10);
        IFNOTEXIT (optarg && *optarg && *endptr == 0 && li >= 0, "Invalid number %s for option '-%c'", optarg, opt);
        break;
      case '?':
        fprintf (stderr, "Known options are:");
        for (const char *c = optstring; *c; c++)
          if (isalnum (*c))
          {
            fprintf (stderr, " -%c", *c);
            if (*(c + 1) == ':')
              fprintf (stderr, " arg");
          }

        fprintf (stderr, "\n");
        IFNOTEXIT (0, "Unknown option '-%c'", optopt);
        break;
      case ':':
        IFNOTEXIT (0, "Missing argument for option '-%c'", optopt);
        break;
    }
  }

  FILE *f = 0;
  if (optind < argc)
  {
    IFNOTEXIT (f = fopen (argv[optind], "r"), "Can not read file '%s'", argv[optind]);
    PRINT ("%s: ", argv[optind]);
  }
  else
  {
    f = stdin;

    int c;
    if ((c = fgetc (f)) != EOF)
      ungetc (c, f);
    else
    {
      //const char *pattern = "9bo12b$7bobo12b$6bobo13b$2o3bo2bo11b2o$2o4bobo11b2o$7bobo12b$9bo!";  // Queen bee shuttle, period 30:
      //const char *pattern = "24bo11b$22bobo11b$12b2o6b2o12b2o$11bo3bo4b2o12b2o$2o8bo5bo3b2o14b$2o8bo3bob2o4bobo11b$10bo5bo7bo11b$11bo3bo20b$12b2o!";      // Gosper glider gun:
      //const char *pattern = ".XX$XX$.X";  // R-pentomino, stabilizes at generation 1103 with 116 cells, including one escaped glider at generation 69:
      //const char *pattern = "10X";        // Pentadecathlon (period 15):
      //const char *pattern = "3X"; // Blinker:
      //const char *pattern = ".X.$..X$XXX";        // Glider:
      //const char *pattern = "bo5b$3bo3b$2o2b3o!"; // Acorn, takes 5206 generations to stabilize to 633 cells, including 13 escaped gliders:
      //const char *pattern = "XXX$.X.";    // Tee or Tetromino, stabilizes to 12 cells in a 9x9 square at 10th generation.
      //const char *pattern = "......X$XX$.X...XXX";        // Die-hard, eventually disappears after 130 generations
      //const char *pattern = "......X$....X.XX$....X.X$....X$..X$X.X";     // Infinite growth, block-laying switch engine that leaves behind two-by-two still life blocks as its translates itself across the game's universe.
      //const char *pattern = "77bo$77bo$77bo21$3o20$3bo$3bo$3bo5$20b3o$9b3o10bo$22bo$21bo!";       // 18-cell 40514-generation methuselah. The stable pattern that results from 40514M (excluding 70 escaping gliders) has 3731 cells and consists of 248 blinkers (including 21 traffic lights), 218 blocks, 163 beehives (including nine honey farms), 56 loaves, 39 boats, 10 ships, nine tubs, five ponds, four beacons, two toads, one barge, one eater 1 and one long boat.
      const char *pattern = "10001o!";
      //const char *pattern = "15366bo$15366bo$15364boo$15363bo$15363bo$15363bo$15363bo6$15393bo$" "15392boo$15390bobbo$$15390bobo$15391bo133$15568boo$15569boo$15569bo29$" "15554bo$15553bobo$15555bo$15556bo507$59722boo$59721boo$59722bo29$" "59737bo$59736bobo$59736bo$59735bo13907$bo3bo$bbobo$obbo$o$o21$33bo$32b" "o$31bo$32bo$33bo$29b3o!";       // Metacatacryst, exhibits quadratic growth.

      PRINT ("Reading unit test pattern...\n");
      IFNOTEXIT (f = fmemopen ((void *) pattern, strlen (pattern), "r"), "Can not read pattern");
    }
  }

  GameOfLife *gol = GOL_init ();
  size_t nb_cells = 0;
  PRINT ("The colony has %'zu cells.\n", nb_cells = RLE_readfile (gol, f, x, y, header));
  fclose (f);
  IFNOTEXIT (nb_cells, "The space is empty.");
  PRINT ("Space size is [%'+" XYPOS_FORMAT ";%'+" XYPOS_FORMAT "] x [%'+" XYPOS_FORMAT ";%'+" XYPOS_FORMAT "].\n",
         XYPOS_MIN, XYPOS_MAX, XYPOS_MIN, XYPOS_MAX);

  if (B)
    gol->B = B;
  if (S)
    gol->S = S;
  MAX_GENERATIONS = li;

  PRINT2 ("B%i/S%i\n", gol->B, gol->S);

  while ((!MAX_GENERATIONS || gol->generation <= MAX_GENERATIONS) && GOL_next_generation (gol))
  {
    /* nothing */
  }

  printf (">Generation #%'zu has %'zu cells.\n", gol->generation - 1, gol->nb_cells);

  GOL_destroy (gol);
  PRINT ("Done.\n");
}
