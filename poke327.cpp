#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <climits>
#include <sys/time.h>
#include <cassert>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <cctype>

#include "heap.h"
#include "poke327.h"
#include "character.h"
#include "io.h"
#include "db_parse.h"

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

/* Even unallocated, a WORLD_SIZE x WORLD_SIZE array of pointers is a very *
 * large thing to put on the stack.  To avoid that, world is a global.     */
class world world;
class pokemon_move struggle;

pair_t all_dirs[8] = {
  { -1, -1 },
  { -1,  0 },
  { -1,  1 },
  {  0, -1 },
  {  0,  1 },
  {  1, -1 },
  {  1,  0 },
  {  1,  1 },
};

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t edge_penalty(int8_t x, int8_t y)
{
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static void dijkstra_path(map *m, pair_t from, pair_t to)
{
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  int32_t x, y;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = (path_t *) heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        /* Don't overwrite the gate */
        if (x != to[dim_x] || y != to[dim_y]) {
          mapxy(x, y) = ter_path;
          heightxy(x, y) = 0;
        }
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

static int build_paths(map *m)
{
  pair_t from, to;

  /*  printf("%d %d %d %d\n", m->n, m->s, m->e, m->w);*/

  if (m->e != -1 && m->w != -1) {
    from[dim_x] = 1;
    to[dim_x] = MAP_X - 2;
    from[dim_y] = m->w;
    to[dim_y] = m->e;

    dijkstra_path(m, from, to);
  }

  if (m->n != -1 && m->s != -1) {
    from[dim_y] = 1;
    to[dim_y] = MAP_Y - 2;
    from[dim_x] = m->n;
    to[dim_x] = m->s;

    dijkstra_path(m, from, to);
  }

  if (m->e == -1) {
    if (m->s == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->w == -1) {
    if (m->s == -1) {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->n == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->s == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }

    dijkstra_path(m, from, to);
  }

  return 0;
}

static int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};

static int smooth_height(map *m)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  /*  FILE *out;*/
  uint8_t height[MAP_Y][MAP_X];

  memset(&height, 0, sizeof (height));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1) {
      head = tail = (queue_node_t *) malloc(sizeof (*tail));
    } else {
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);
  */
  
  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
      height[y - 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1]) {
      height[y][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
      height[y + 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x]) {
      height[y - 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x]) {
      height[y + 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
      height[y - 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1]) {
      height[y][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
      height[y + 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->height, sizeof (m->height), 1, out);
  fclose(out);
  */

  return 0;
}

static void find_building_location(map *m, pair_t p)
{
  do {
    p[dim_x] = rand() % (MAP_X - 3) + 1;
    p[dim_y] = rand() % (MAP_Y - 3) + 1;

    if ((((mapxy(p[dim_x] - 1, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x] + 2, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] - 1) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] + 2) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path)))   &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path)))) {
          break;
    }
  } while (1);
}

static int place_pokemart(map *m)
{
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(map *m)
{  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_center;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

/* Chooses tree or boulder for border cell.  Choice is biased by dominance *
 * of neighboring cells.                                                   */
static terrain_type_t border_type(map *m, int32_t x, int32_t y)
{
  int32_t p, q;
  int32_t r, t;
  int32_t miny, minx, maxy, maxx;
  
  r = t = 0;
  
  miny = y - 1 >= 0 ? y - 1 : 0;
  maxy = y + 1 <= MAP_Y ? y + 1: MAP_Y;
  minx = x - 1 >= 0 ? x - 1 : 0;
  maxx = x + 1 <= MAP_X ? x + 1: MAP_X;

  for (q = miny; q < maxy; q++) {
    for (p = minx; p < maxx; p++) {
      if (q != y || p != x) {
        if (m->map[q][p] == ter_mountain ||
            m->map[q][p] == ter_boulder) {
          r++;
        } else if (m->map[q][p] == ter_forest ||
                   m->map[q][p] == ter_tree) {
          t++;
        }
      }
    }
  }
  
  if (t == r) {
    return rand() & 1 ? ter_boulder : ter_tree;
  } else if (t > r) {
    if (rand() % 10) {
      return ter_tree;
    } else {
      return ter_boulder;
    }
  } else {
    if (rand() % 10) {
      return ter_boulder;
    } else {
      return ter_tree;
    }
  }
}

static int map_terrain(map *m, int8_t n, int8_t s, int8_t e, int8_t w)
{
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  //  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest, num_water, num_total;
  terrain_type_t type;
  int added_current = 0;
  
  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
  num_water = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest + num_water;

  memset(&m->map, 0, sizeof (m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0) {
      type = ter_grass;
    } else if (i == num_grass) {
      type = ter_clearing;
    } else if (i == num_grass + num_clearing) {
      type = ter_mountain;
    } else if (i == num_grass + num_clearing + num_mountain) {
      type = ter_forest;
    } else if (i == num_grass + num_clearing + num_mountain + num_forest) {
      type = ter_water;
    }
    m->map[y][x] = type;
    if (i == 0) {
      head = tail = (queue_node_t *) malloc(sizeof (*tail));
    } else {
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    type = m->map[y][x];
    
    if (x - 1 >= 0 && !m->map[y][x - 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x - 1] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y - 1][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y + 1][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x + 1] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = type;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (y == 0 || y == MAP_Y - 1 ||
          x == 0 || x == MAP_X - 1) {
        mapxy(x, y) = border_type(m, x, y);
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  if (n != -1) {
    mapxy(n,         0        ) = ter_gate;
    mapxy(n,         1        ) = ter_bailey;
  }
  if (s != -1) {
    mapxy(s,         MAP_Y - 1) = ter_gate;
    mapxy(s,         MAP_Y - 2) = ter_bailey;
  }
  if (w != -1) {
    mapxy(0,         w        ) = ter_gate;
    mapxy(1,         w        ) = ter_bailey;
  }
  if (e != -1) {
    mapxy(MAP_X - 1, e        ) = ter_gate;
    mapxy(MAP_X - 2, e        ) = ter_bailey;
  }

  return 0;
}

static int place_boulders(map *m)
{
  int i;
  int x, y;

  for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_forest &&
        m->map[y][x] != ter_path   &&
        m->map[y][x] != ter_gate  &&
        m->map[y][x] != ter_bailey) {
      m->map[y][x] = ter_boulder;
    }
  }

  return 0;
}

static int place_trees(map *m)
{
  int i;
  int x, y;
  
  for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_mountain &&
        m->map[y][x] != ter_path     &&
        m->map[y][x] != ter_water    &&
        m->map[y][x] != ter_gate  &&
        m->map[y][x] != ter_bailey) {
      m->map[y][x] = ter_tree;
    }
  }

  return 0;
}

void rand_pos(pair_t pos)
{
  pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
  pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;
}

void calculate_weaknesses(game_pokemon *p){
  int i;

  for(i = 0; i < num_pokemon_types; i++){
    p->type_effectiveness[i] = 1.0;
  }

  switch(p->types[0]){
    case type_normal:
    p->type_effectiveness[type_fighting] *= 2;
    p->type_effectiveness[type_ghost] = 0;
    break;
    case type_fighting:
    p->type_effectiveness[type_flying] *= 2;
    p->type_effectiveness[type_psychic] *= 2;
    p->type_effectiveness[type_fairy] *= 2;
    p->type_effectiveness[type_rock] /= 2;
    p->type_effectiveness[type_bug] /= 2;
    p->type_effectiveness[type_dark] /= 2;
    break;
    case type_flying:
    p->type_effectiveness[type_rock] *= 2;
    p->type_effectiveness[type_electric] *= 2;
    p->type_effectiveness[type_ice] *= 2;
    p->type_effectiveness[type_fighting] /= 2;
    p->type_effectiveness[type_bug] /= 2;
    p->type_effectiveness[type_grass] /= 2;
    p->type_effectiveness[type_ground] = 0;
    break;
    case type_poison:
    p->type_effectiveness[type_ground] *= 2;
    p->type_effectiveness[type_psychic] *= 2;
    p->type_effectiveness[type_fighting] /= 2;
    p->type_effectiveness[type_poison] /= 2;
    p->type_effectiveness[type_bug] /= 2;
    p->type_effectiveness[type_grass] /= 2;
    p->type_effectiveness[type_fairy] /= 2;
    break;
    case type_ground:
    p->type_effectiveness[type_water] *= 2;
    p->type_effectiveness[type_grass] *= 2;
    p->type_effectiveness[type_ice] *= 2;
    p->type_effectiveness[type_poison] /= 2;
    p->type_effectiveness[type_rock] /= 2;
    p->type_effectiveness[type_electric] = 0;
    break;
    case type_rock:
    p->type_effectiveness[type_fighting] *= 2;
    p->type_effectiveness[type_ground] *= 2;
    p->type_effectiveness[type_steel] *= 2;
    p->type_effectiveness[type_water] *= 2;
    p->type_effectiveness[type_grass] *= 2;
    p->type_effectiveness[type_normal] /= 2;
    p->type_effectiveness[type_flying] /= 2;
    p->type_effectiveness[type_poison] /= 2;
    p->type_effectiveness[type_fire] /= 2;
    break;
    case type_bug:
    p->type_effectiveness[type_flying] *= 2;
    p->type_effectiveness[type_rock] *= 2;
    p->type_effectiveness[type_fire] *= 2;
    p->type_effectiveness[type_fighting] /= 2;
    p->type_effectiveness[type_ground] /= 2;
    p->type_effectiveness[type_grass] /= 2;
    break;
    case type_ghost:
    p->type_effectiveness[type_ghost] *= 2;
    p->type_effectiveness[type_dark] *= 2;
    p->type_effectiveness[type_poison] /= 2;
    p->type_effectiveness[type_bug] /= 2;
    p->type_effectiveness[type_normal] = 0;
    p->type_effectiveness[type_fighting] = 0;
    break;
    case type_steel:
    p->type_effectiveness[type_fighting] *= 2;
    p->type_effectiveness[type_ground] *= 2;
    p->type_effectiveness[type_fire] *= 2;
    p->type_effectiveness[type_normal] /= 2;
    p->type_effectiveness[type_flying] /= 2;
    p->type_effectiveness[type_rock] /= 2;
    p->type_effectiveness[type_bug] /= 2;
    p->type_effectiveness[type_steel] /= 2;
    p->type_effectiveness[type_grass] /= 2;
    p->type_effectiveness[type_psychic] /= 2;
    p->type_effectiveness[type_ice] /= 2;
    p->type_effectiveness[type_dragon] /= 2;
    p->type_effectiveness[type_fairy] /= 2;
    p->type_effectiveness[type_poison] = 0;
    break;
    case type_fire:
    p->type_effectiveness[type_ground] *= 2;
    p->type_effectiveness[type_rock] *= 2;
    p->type_effectiveness[type_water] *= 2;
    p->type_effectiveness[type_bug] /= 2;
    p->type_effectiveness[type_steel] /= 2;
    p->type_effectiveness[type_fire] /= 2;
    p->type_effectiveness[type_grass] /= 2;
    p->type_effectiveness[type_ice] /= 2;
    p->type_effectiveness[type_fairy] /= 2;
    break;
    case type_water:
    p->type_effectiveness[type_grass] *= 2;
    p->type_effectiveness[type_electric] *= 2;
    p->type_effectiveness[type_steel] /= 2;
    p->type_effectiveness[type_fire] /= 2;
    p->type_effectiveness[type_water] /= 2;
    p->type_effectiveness[type_ice] /= 2;
    break;
    case type_grass:
    p->type_effectiveness[type_flying] *= 2;
    p->type_effectiveness[type_poison] *= 2;
    p->type_effectiveness[type_bug] *= 2;
    p->type_effectiveness[type_fire] *= 2;
    p->type_effectiveness[type_ice] *= 2;
    p->type_effectiveness[type_ground] /= 2;
    p->type_effectiveness[type_water] /= 2;
    p->type_effectiveness[type_grass] /= 2;
    p->type_effectiveness[type_electric] /= 2;
    break;
    case type_electric:
    p->type_effectiveness[type_ground] *= 2;
    p->type_effectiveness[type_flying] /= 2;
    p->type_effectiveness[type_steel] /= 2;
    p->type_effectiveness[type_electric] /= 2;
    break;
    case type_psychic:
    p->type_effectiveness[type_bug] *= 2;
    p->type_effectiveness[type_ghost] *= 2;
    p->type_effectiveness[type_dark] *= 2;
    p->type_effectiveness[type_fighting] /= 2;
    p->type_effectiveness[type_psychic] /= 2;
    break;
    case type_ice:
    p->type_effectiveness[type_fighting] *= 2;
    p->type_effectiveness[type_rock] *= 2;
    p->type_effectiveness[type_steel] *= 2;
    p->type_effectiveness[type_fire] *= 2;
    p->type_effectiveness[type_ice] /= 2;
    break;
    case type_dragon:
    p->type_effectiveness[type_ice] *= 2;
    p->type_effectiveness[type_dragon] *= 2;
    p->type_effectiveness[type_fairy] *= 2;
    p->type_effectiveness[type_fire] /= 2;
    p->type_effectiveness[type_water] /= 2;
    p->type_effectiveness[type_grass] /= 2;
    p->type_effectiveness[type_electric] /= 2;
    break;
    case type_dark:
    p->type_effectiveness[type_fighting] *= 2;
    p->type_effectiveness[type_bug] *= 2;
    p->type_effectiveness[type_fairy] *= 2;
    p->type_effectiveness[type_ghost] /= 2;
    p->type_effectiveness[type_dark] /= 2;
    p->type_effectiveness[type_psychic] = 0;
    break;
    case type_fairy:
    p->type_effectiveness[type_poison] *= 2;
    p->type_effectiveness[type_steel] *= 2;
    p->type_effectiveness[type_fighting] /= 2;
    p->type_effectiveness[type_bug] /= 2;
    p->type_effectiveness[type_dark] /= 2;
    p->type_effectiveness[type_dragon] = 0;
    break;
    default:
    break;
  }

  if(p->types[1] != type_null){
    switch(p->types[1]){
      case type_normal:
      p->type_effectiveness[type_fighting] *= 2;
      p->type_effectiveness[type_ghost] = 0;
      break;
      case type_fighting:
      p->type_effectiveness[type_flying] *= 2;
      p->type_effectiveness[type_psychic] *= 2;
      p->type_effectiveness[type_fairy] *= 2;
      p->type_effectiveness[type_rock] /= 2;
      p->type_effectiveness[type_bug] /= 2;
      p->type_effectiveness[type_dark] /= 2;
      break;
      case type_flying:
      p->type_effectiveness[type_rock] *= 2;
      p->type_effectiveness[type_electric] *= 2;
      p->type_effectiveness[type_ice] *= 2;
      p->type_effectiveness[type_fighting] /= 2;
      p->type_effectiveness[type_bug] /= 2;
      p->type_effectiveness[type_grass] /= 2;
      p->type_effectiveness[type_ground] = 0;
      break;
      case type_poison:
      p->type_effectiveness[type_ground] *= 2;
      p->type_effectiveness[type_psychic] *= 2;
      p->type_effectiveness[type_fighting] /= 2;
      p->type_effectiveness[type_poison] /= 2;
      p->type_effectiveness[type_bug] /= 2;
      p->type_effectiveness[type_grass] /= 2;
      p->type_effectiveness[type_fairy] /= 2;
      break;
      case type_ground:
      p->type_effectiveness[type_water] *= 2;
      p->type_effectiveness[type_grass] *= 2;
      p->type_effectiveness[type_ice] *= 2;
      p->type_effectiveness[type_poison] /= 2;
      p->type_effectiveness[type_rock] /= 2;
      p->type_effectiveness[type_electric] = 0;
      break;
      case type_rock:
      p->type_effectiveness[type_fighting] *= 2;
      p->type_effectiveness[type_ground] *= 2;
      p->type_effectiveness[type_steel] *= 2;
      p->type_effectiveness[type_water] *= 2;
      p->type_effectiveness[type_grass] *= 2;
      p->type_effectiveness[type_normal] /= 2;
      p->type_effectiveness[type_flying] /= 2;
      p->type_effectiveness[type_poison] /= 2;
      p->type_effectiveness[type_fire] /= 2;
      break;
      case type_bug:
      p->type_effectiveness[type_flying] *= 2;
      p->type_effectiveness[type_rock] *= 2;
      p->type_effectiveness[type_fire] *= 2;
      p->type_effectiveness[type_fighting] /= 2;
      p->type_effectiveness[type_ground] /= 2;
      p->type_effectiveness[type_grass] /= 2;
      break;
      case type_ghost:
      p->type_effectiveness[type_ghost] *= 2;
      p->type_effectiveness[type_dark] *= 2;
      p->type_effectiveness[type_poison] /= 2;
      p->type_effectiveness[type_bug] /= 2;
      p->type_effectiveness[type_normal] = 0;
      p->type_effectiveness[type_fighting] = 0;
      break;
      case type_steel:
      p->type_effectiveness[type_fighting] *= 2;
      p->type_effectiveness[type_ground] *= 2;
      p->type_effectiveness[type_fire] *= 2;
      p->type_effectiveness[type_normal] /= 2;
      p->type_effectiveness[type_flying] /= 2;
      p->type_effectiveness[type_rock] /= 2;
      p->type_effectiveness[type_bug] /= 2;
      p->type_effectiveness[type_steel] /= 2;
      p->type_effectiveness[type_grass] /= 2;
      p->type_effectiveness[type_psychic] /= 2;
      p->type_effectiveness[type_ice] /= 2;
      p->type_effectiveness[type_dragon] /= 2;
      p->type_effectiveness[type_fairy] /= 2;
      p->type_effectiveness[type_poison] = 0;
      break;
      case type_fire:
      p->type_effectiveness[type_ground] *= 2;
      p->type_effectiveness[type_rock] *= 2;
      p->type_effectiveness[type_water] *= 2;
      p->type_effectiveness[type_bug] /= 2;
      p->type_effectiveness[type_steel] /= 2;
      p->type_effectiveness[type_fire] /= 2;
      p->type_effectiveness[type_grass] /= 2;
      p->type_effectiveness[type_ice] /= 2;
      p->type_effectiveness[type_fairy] /= 2;
      break;
      case type_water:
      p->type_effectiveness[type_grass] *= 2;
      p->type_effectiveness[type_electric] *= 2;
      p->type_effectiveness[type_steel] /= 2;
      p->type_effectiveness[type_fire] /= 2;
      p->type_effectiveness[type_water] /= 2;
      p->type_effectiveness[type_ice] /= 2;
      break;
      case type_grass:
      p->type_effectiveness[type_flying] *= 2;
      p->type_effectiveness[type_poison] *= 2;
      p->type_effectiveness[type_bug] *= 2;
      p->type_effectiveness[type_fire] *= 2;
      p->type_effectiveness[type_ice] *= 2;
      p->type_effectiveness[type_ground] /= 2;
      p->type_effectiveness[type_water] /= 2;
      p->type_effectiveness[type_grass] /= 2;
      p->type_effectiveness[type_electric] /= 2;
      break;
      case type_electric:
      p->type_effectiveness[type_ground] *= 2;
      p->type_effectiveness[type_flying] /= 2;
      p->type_effectiveness[type_steel] /= 2;
      p->type_effectiveness[type_electric] /= 2;
      break;
      case type_psychic:
      p->type_effectiveness[type_bug] *= 2;
      p->type_effectiveness[type_ghost] *= 2;
      p->type_effectiveness[type_dark] *= 2;
      p->type_effectiveness[type_fighting] /= 2;
      p->type_effectiveness[type_psychic] /= 2;
      break;
      case type_ice:
      p->type_effectiveness[type_fighting] *= 2;
      p->type_effectiveness[type_rock] *= 2;
      p->type_effectiveness[type_steel] *= 2;
      p->type_effectiveness[type_fire] *= 2;
      p->type_effectiveness[type_ice] /= 2;
      break;
      case type_dragon:
      p->type_effectiveness[type_ice] *= 2;
      p->type_effectiveness[type_dragon] *= 2;
      p->type_effectiveness[type_fairy] *= 2;
      p->type_effectiveness[type_fire] /= 2;
      p->type_effectiveness[type_water] /= 2;
      p->type_effectiveness[type_grass] /= 2;
      p->type_effectiveness[type_electric] /= 2;
      break;
      case type_dark:
      p->type_effectiveness[type_fighting] *= 2;
      p->type_effectiveness[type_bug] *= 2;
      p->type_effectiveness[type_fairy] *= 2;
      p->type_effectiveness[type_ghost] /= 2;
      p->type_effectiveness[type_dark] /= 2;
      p->type_effectiveness[type_psychic] = 0;
      break;
      case type_fairy:
      p->type_effectiveness[type_poison] *= 2;
      p->type_effectiveness[type_steel] *= 2;
      p->type_effectiveness[type_fighting] /= 2;
      p->type_effectiveness[type_bug] /= 2;
      p->type_effectiveness[type_dark] /= 2;
      p->type_effectiveness[type_dragon] = 0;
      break;
      default:
      break;
    }
  }
}

bool four_unique_values(int val1, int val2, int val3, int val4){
  if(val1 == val2 || val1 == val3 || val1 == val4){
    return false;
  }
  if(val2 == val3 || val2 == val4){
    return false;
  }
  if(val3 == val4){
    return false;
  }
  return true;
}

void find_evolves(game_pokemon *p){
  int i;
  int gender;

  gender = (p->gender == 'f') ? 1 : 2;

  p->evolves_into_id = -1;
  p->evolves_at_level = 101;
  for(i = 1; i < 460; i++){
    if(species[evolutions[i].evolved_species].evolves_from_species_id == p->species_id && evolutions[i].evolution_trigger == 1){
      if(evolutions[i].gender_id == gender || evolutions[i].gender_id == INT_MAX){
        p->evolves_into_id = evolutions[i].evolved_species;
        p->evolves_at_level = evolutions[i].minimum_level;
      }
    }
  }
}

game_pokemon generate_pokemon(){
  int i, j, move_index, distance;
  game_pokemon encounter;
  pokemon_species_db wild;
  std::vector<int> move_ids;
  std::vector<pokemon_move_db> learnset;
  pokemon_move_db *index, move1, move2, move3, move4;
  move_db tmp_move;
  pokemon_types_db *type_index;
  pokemon_move first_move, second_move, third_move, fourth_move;

  pokemon_db val = pokemon[(rand() % 1092) + 1];

  encounter.leveled_last_battle = 0;

  encounter.species_id = val.species_id;

  wild = species[val.species_id];

  strcpy(encounter.name, wild.identifier);

  encounter.name[0] = std::toupper(encounter.name[0]);

  for(i = 1; i < 6553; i += 6){
    if(pokemon_stats[i].pokemon_id == val.id){
      for(j = 0; j < 6; j++){
        encounter.baseStat[j] = pokemon_stats[i + j].base_stat;
      }
      break;
    }
  }

  for(i = 0; i < 6; i++){
    encounter.iv[i] = rand() % 16;
  }

  distance = (abs(world.cur_idx[dim_y] - 200) + abs(world.cur_idx[dim_x] - 200) <= 0) ? 0 : abs(world.cur_idx[dim_y] - 200) + abs(world.cur_idx[dim_x] - 200);

  if(distance < 2){
    encounter.level = 1;
  }else if(distance <= 200){
    encounter.level = (rand() % (distance / 2) + 1);
  }else if(distance < 400){
    encounter.level = (rand() % (100 - ((distance - 200) / 2)) + ((distance - 200) / 2) + 1);
  }else{
    encounter.level = 100;
  }

  encounter.currentStat[0] = encounter.battleStat[0] = ((((encounter.baseStat[0] + encounter.iv[0]) * 2) * encounter.level) / 100) + encounter.level + 10;

  for(i = 1; i < 6; i++){
    encounter.currentStat[i] = encounter.battleStat[i] = ((((encounter.baseStat[i] + encounter.iv[i]) * 2) * encounter.level) / 100) + 5;
  }

  encounter.isShiny = ((rand() % 8192) == 0);

  if(rand() % 2){
    encounter.gender = 'M';
  }else{
    encounter.gender = 'F';
  }

  
  //Won't ever set to after the index of the moves, but reduces amount of time spent searching through the db
  index = &pokemon_moves[wild.id];
  while(index && index->pokemon_id <= wild.id){
    if(index->pokemon_id == wild.id && index->pokemon_move_method_id == 1 && std::find(move_ids.begin(), move_ids.end(), index->move_id) == move_ids.end()){
      encounter.level_up_moves.push_back(*index);
      move_ids.push_back(index->move_id);
      if(index->level <= encounter.level){
        learnset.push_back(*index);
      }
    }
    index++;
  }

  encounter.baseExp = val.base_experience;

  encounter.currentExp = experience[(100 * wild.growth_rate_id) + 1 + encounter.level].experience;

  encounter.xp_growth_group = wild.growth_rate_id;

  if(learnset.size() == 1){
    move1 = learnset.at(0);
    if(move1.move_id < 10000){
      move_index = move1.move_id;
    }else{
      move_index = 826 + (move1.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(first_move.name, tmp_move.identifier);
    first_move.name[0] = std::toupper(first_move.name[0]);
    first_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    first_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    first_move.max_pp = tmp_move.pp;
    first_move.current_pp = tmp_move.pp;
    first_move.priority = tmp_move.priority;
    first_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(first_move);
  }else if(learnset.size() == 2){
    move1 = learnset.at(0);
    move2 = learnset.at(1);

    if(move1.move_id < 10000){
      move_index = move1.move_id;
    }else{
      move_index = 826 + (move1.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(first_move.name, tmp_move.identifier);
    first_move.name[0] = std::toupper(first_move.name[0]);
    first_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    first_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    first_move.max_pp = tmp_move.pp;
    first_move.current_pp = tmp_move.pp;
    first_move.priority = tmp_move.priority;
    first_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(first_move);

    if(move2.move_id < 10000){
      move_index = move2.move_id;
    }else{
      move_index = 826 + (move2.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(second_move.name, tmp_move.identifier);
    second_move.name[0] = std::toupper(second_move.name[0]);
    second_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    second_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    second_move.max_pp = tmp_move.pp;
    second_move.current_pp = tmp_move.pp;
    second_move.priority = tmp_move.priority;
    second_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(second_move);
  }else if(learnset.size() == 3){
    move1 = learnset.at(0);
    move2 = learnset.at(1);
    move3 = learnset.at(2);

    if(move1.move_id < 10000){
      move_index = move1.move_id;
    }else{
      move_index = 826 + (move1.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(first_move.name, tmp_move.identifier);
    first_move.name[0] = std::toupper(first_move.name[0]);
    first_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    first_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    first_move.max_pp = tmp_move.pp;
    first_move.current_pp = tmp_move.pp;
    first_move.priority = tmp_move.priority;
    first_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(first_move);

    if(move2.move_id < 10000){
      move_index = move2.move_id;
    }else{
      move_index = 826 + (move2.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(second_move.name, tmp_move.identifier);
    second_move.name[0] = std::toupper(second_move.name[0]);
    second_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    second_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    second_move.max_pp = tmp_move.pp;
    second_move.current_pp = tmp_move.pp;
    second_move.priority = tmp_move.priority;
    second_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(second_move);

    if(move3.move_id < 10000){
      move_index = move3.move_id;
    }else{
      move_index = 826 + (move3.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(third_move.name, tmp_move.identifier);
    third_move.name[0] = std::toupper(third_move.name[0]);
    third_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    third_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    third_move.max_pp = tmp_move.pp;
    third_move.current_pp = tmp_move.pp;
    third_move.priority = tmp_move.priority;
    third_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(third_move);
  }else if(!learnset.size()){
    first_move = struggle;
    encounter.moves.push_back(first_move);
  }else{
    move1 = learnset.at(rand() % learnset.size());
    move2 = learnset.at(rand() % learnset.size());
    move3 = learnset.at(rand() % learnset.size());
    move4 = learnset.at(rand() % learnset.size());
    while(!four_unique_values(move1.move_id, move2.move_id, move3.move_id, move4.move_id)){
      move1 = learnset.at(rand() % learnset.size());
      move2 = learnset.at(rand() % learnset.size());
      move3 = learnset.at(rand() % learnset.size());
      move4 = learnset.at(rand() % learnset.size());
    }

    if(move1.move_id < 10000){
      move_index = move1.move_id;
    }else{
      move_index = 826 + (move1.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(first_move.name, tmp_move.identifier);
    first_move.name[0] = std::toupper(first_move.name[0]);
    first_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    first_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    first_move.max_pp = tmp_move.pp;
    first_move.current_pp = tmp_move.pp;
    first_move.priority = tmp_move.priority;
    first_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(first_move);

    if(move2.move_id < 10000){
      move_index = move2.move_id;
    }else{
      move_index = 826 + (move2.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(second_move.name, tmp_move.identifier);
    second_move.name[0] = std::toupper(second_move.name[0]);
    second_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    second_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    second_move.max_pp = tmp_move.pp;
    second_move.current_pp = tmp_move.pp;
    second_move.priority = tmp_move.priority;
    second_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(second_move);

    if(move3.move_id < 10000){
      move_index = move3.move_id;
    }else{
      move_index = 826 + (move3.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(third_move.name, tmp_move.identifier);
    third_move.name[0] = std::toupper(third_move.name[0]);
    third_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    third_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    third_move.max_pp = tmp_move.pp;
    third_move.current_pp = tmp_move.pp;
    third_move.priority = tmp_move.priority;
    third_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(third_move);

    if(move4.move_id < 10000){
      move_index = move4.move_id;
    }else{
      move_index = 826 + (move4.move_id - 10000);
    }
    tmp_move = moves[move_index];
    strcpy(fourth_move.name, tmp_move.identifier);
    fourth_move.name[0] = std::toupper(fourth_move.name[0]);
    fourth_move.move_type = (tmp_move.type_id <= 18) ? static_cast<pokemon_type_t>(tmp_move.type_id - 1) : type_shadow;
    fourth_move.power = (tmp_move.power == INT_MAX) ? 0 : tmp_move.power;
    fourth_move.max_pp = tmp_move.pp;
    fourth_move.current_pp = tmp_move.pp;
    fourth_move.priority = tmp_move.priority;
    fourth_move.accuracy = tmp_move.accuracy;
    encounter.moves.push_back(fourth_move);
  }

  //Won't ever set to after the index of the types, but reduces amount of time spent searching through the db
  type_index = &pokemon_types[wild.id];
  while(type_index && type_index->pokemon_id <= wild.id){
    if(type_index->pokemon_id == wild.id){
      if((type_index + 1)->pokemon_id == wild.id){
        encounter.types[0] = static_cast<pokemon_type_t>(type_index->type_id - 1);
        encounter.types[1] = static_cast<pokemon_type_t>((type_index + 1)->type_id - 1);
      }else{
        encounter.types[0] = static_cast<pokemon_type_t>(type_index->type_id - 1);
        encounter.types[1] = type_null;
      }
      break;
    }
    type_index++;
  }

  find_evolves(&encounter);

  calculate_weaknesses(&encounter);

  return encounter;
}

void new_hiker()
{
  pair_t pos;
  npc *c;
  int add_more;

  do {
    rand_pos(pos);
  } while (world.hiker_dist[pos[dim_y]][pos[dim_x]] == DIJKSTRA_PATH_MAX ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]                   ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4                      ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_hiker;
  c->mtype = move_hiker;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = HIKER_SYMBOL;
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;

  c->team.push_back(generate_pokemon());
  add_more = (rand() % 100 < 60);
  while(c->team.size() < 6 && add_more){
    c->team.push_back(generate_pokemon());
    add_more = (rand() % 100 < 60);
  }
  
  heap_insert(&world.cur_map->turn, c);
}

void new_rival()
{
  pair_t pos;
  npc *c;
  int add_more;

  do {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == DIJKSTRA_PATH_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0                  ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]                   ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4                      ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_rival;
  c->mtype = move_rival;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = RIVAL_SYMBOL;
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;

  c->team.push_back(generate_pokemon());
  add_more = (rand() % 100 < 60);
  while(c->team.size() < 6 && add_more){
    c->team.push_back(generate_pokemon());
    add_more = (rand() % 100 < 60);
  }

  heap_insert(&world.cur_map->turn, c);
}

void new_swimmer()
{
  pair_t pos;
  npc *c;
  int add_more;

  do {
    rand_pos(pos);
  } while (world.cur_map->map[pos[dim_y]][pos[dim_x]] != ter_water ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_swimmer;
  c->mtype = move_swim;
  rand_dir(c->dir);
  c->defeated = 0;
  c->symbol = SWIMMER_SYMBOL;
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;

  c->team.push_back(generate_pokemon());
  add_more = (rand() % 100 < 60);
  while(c->team.size() < 6 && add_more){
    c->team.push_back(generate_pokemon());
    add_more = (rand() % 100 < 60);
  }

  heap_insert(&world.cur_map->turn, c);
}

void new_char_other()
{
  pair_t pos;
  npc *c;
  int add_more;

  do {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == DIJKSTRA_PATH_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0                  ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]                   ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4                      ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_other;
  switch (rand() % 4) {
  case 0:
    c->mtype = move_pace;
    c->symbol = PACER_SYMBOL;
    break;
  case 1:
    c->mtype = move_wander;
    c->symbol = WANDERER_SYMBOL;
    break;
  case 2:
    c->mtype = move_sentry;
    c->symbol = SENTRY_SYMBOL;
    break;
  case 3:
    c->mtype = move_explore;
    c->symbol = EXPLORER_SYMBOL;
    break;
  }
  rand_dir(c->dir);
  c->defeated = 0;
  c->next_turn = 0;
  c->seq_num = world.char_seq_num++;

  c->team.push_back(generate_pokemon());
  add_more = (rand() % 100 < 60);
  while(c->team.size() < 6 && add_more){
    c->team.push_back(generate_pokemon());
    add_more = (rand() % 100 < 60);
  }

  heap_insert(&world.cur_map->turn, c);
}

void place_characters()
{
  world.cur_map->num_trainers = 3;

  //Always place a hiker and a rival, then place a random number of others
  new_hiker();
  new_rival();
  new_swimmer();
  do {
    //higher probability of non- hikers and rivals
    switch(rand() % 10) {
    case 0:
      new_hiker();
      break;
    case 1:
      new_rival();
      break;
    case 2:
      new_swimmer();
      break;
    default:
      new_char_other();
      break;
    }
    /* Game attempts to continue to place trainers until the probability *
     * roll fails, but if the map is full (or almost full), it's         *
     * impossible (or very difficult) to continue to add, so we abort if *
     * we've tried MAX_TRAINER_TRIES times.                              */
  } while (++world.cur_map->num_trainers < MIN_TRAINERS ||
           ((rand() % 100) < ADD_TRAINER_PROB));
}

void init_pc()
{
  int x, y;

  do {
    x = rand() % (MAP_X - 2) + 1;
    y = rand() % (MAP_Y - 2) + 1;
  } while (world.cur_map->map[y][x] != ter_path);

  world.pc.pos[dim_x] = x;
  world.pc.pos[dim_y] = y;
  world.pc.symbol = PC_SYMBOL;

  world.cur_map->cmap[y][x] = &world.pc;
  world.pc.next_turn = 0;

  world.pc.seq_num = world.char_seq_num++;

  world.pc.num_revives = world.pc.num_potions = world.pc.num_p_balls = 10;

  heap_insert(&world.cur_map->turn, &world.pc);
}

void place_pc()
{
  character *c;

  if (world.pc.pos[dim_x] == 1) {
    world.pc.pos[dim_x] = MAP_X - 2;
  } else if (world.pc.pos[dim_x] == MAP_X - 2) {
    world.pc.pos[dim_x] = 1;
  } else if (world.pc.pos[dim_y] == 1) {
    world.pc.pos[dim_y] = MAP_Y - 2;
  } else if (world.pc.pos[dim_y] == MAP_Y - 2) {
    world.pc.pos[dim_y] = 1;
  }

  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;

  if ((c = (character *) heap_peek_min(&world.cur_map->turn))) {
    world.pc.next_turn = c->next_turn;
  } else {
    world.pc.next_turn = 0;
  }
}

// New map expects cur_idx to refer to the index to be generated.  If that
// map has already been generated then the only thing this does is set
// cur_map.
int new_map(int teleport)
{
  int d, p;
  int e, w, n, s;
  int x, y;
  
  if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]]) {
    world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]];
    place_pc();

    return 0;
  }

  world.cur_map = new map;
  world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]] = world.cur_map;
    

  smooth_height(world.cur_map);
  
  if (!world.cur_idx[dim_y]) {
    n = -1;
  } else if (world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]) {
    n = world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]->s;
  } else {
    n = 3 + rand() % (MAP_X - 6);
  }
  if (world.cur_idx[dim_y] == WORLD_SIZE - 1) {
    s = -1;
  } else if (world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]) {
    s = world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]->n;
  } else  {
    s = 3 + rand() % (MAP_X - 6);
  }
  if (!world.cur_idx[dim_x]) {
    w = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]) {
    w = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]->e;
  } else {
    w = 3 + rand() % (MAP_Y - 6);
  }
  if (world.cur_idx[dim_x] == WORLD_SIZE - 1) {
    e = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]) {
    e = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]->w;
  } else {
    e = 3 + rand() % (MAP_Y - 6);
  }
  
  map_terrain(world.cur_map, n, s, e, w);
     
  place_boulders(world.cur_map);
  place_trees(world.cur_map);
  build_paths(world.cur_map);
  d = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
       abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  p = d > 200 ? 5 : (50 - ((45 * d) / 200));
  //  printf("d=%d, p=%d\n", d, p);
  if ((rand() % 100) < p || !d) {
    place_pokemart(world.cur_map);
  }
  if ((rand() % 100) < p || !d) {
    place_center(world.cur_map);
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      world.cur_map->cmap[y][x] = NULL;
    }
  }

  heap_init(&world.cur_map->turn, cmp_char_turns, delete_character);

  if ((world.cur_idx[dim_x] == WORLD_SIZE / 2) &&
      (world.cur_idx[dim_y] == WORLD_SIZE / 2)) {
    init_pc();
  } else {
    place_pc();
  }

  pathfind(world.cur_map);
  if (teleport) {
    do {
      world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;
      world.pc.pos[dim_x] = rand_range(1, MAP_X - 2);
      world.pc.pos[dim_y] = rand_range(1, MAP_Y - 2);
    } while (world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ||
             (move_cost[char_pc][world.cur_map->map[world.pc.pos[dim_y]]
                                                   [world.pc.pos[dim_x]]] ==
              DIJKSTRA_PATH_MAX)                                           ||
             world.rival_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] < 0);
    world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;
    pathfind(world.cur_map);
  }
  
  place_characters();

  return 0;
}

// The world is global because of its size, so init_world is parameterless
void init_world()
{
  world.quit = 0;
  world.cur_idx[dim_x] = world.cur_idx[dim_y] = WORLD_SIZE / 2;
  world.char_seq_num = 0;
  new_map(0);

  //Create global struggle so we don't have to constantly create new struggle instances whenever Pokemon call Struggle
  move_db struggle_data = moves[165];
  struggle_data.identifier[0] = 'S';
  strcpy(struggle.name, struggle_data.identifier);
  struggle.accuracy = struggle_data.accuracy;
  struggle.current_pp = struggle.max_pp = struggle_data.pp;
  struggle.move_type = static_cast<pokemon_type_t>(struggle_data.type_id - 1);
  struggle.power = struggle_data.power;
  struggle.priority = struggle_data.priority;
}

void delete_world()
{
  int x, y;

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      if (world.world[y][x]) {
        heap_delete(&world.world[y][x]->turn);
        delete world.world[y][x];
        world.world[y][x] = NULL;
      }
    }
  }
}

void print_hiker_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.hiker_dist[y][x] == DIJKSTRA_PATH_MAX) {
        printf("   ");
      } else {
        printf(" %02d", world.hiker_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void print_rival_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.rival_dist[y][x] == DIJKSTRA_PATH_MAX ||
          world.rival_dist[y][x] < 0) {
        printf("   ");
      } else {
        printf(" %02d", world.rival_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void leave_map(pair_t d)
{
  if (d[dim_x] == 0) {
    world.cur_idx[dim_x]--;
  } else if (d[dim_y] == 0) {
    world.cur_idx[dim_y]--;
  } else if (d[dim_x] == MAP_X - 1) {
    world.cur_idx[dim_x]++;
  } else {
    world.cur_idx[dim_y]++;
  }
  new_map(0);
}

void game_loop()
{
  character *c;
  npc *n;
  pc *p;
  pair_t d;
  
  while (!world.quit) {
    c = (character *) heap_remove_min(&world.cur_map->turn);
    n = dynamic_cast<npc *> (c);
    p = dynamic_cast<pc *> (c);

    if(p && world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] == ter_grass){
      if(rand() % 100 < 10){
        game_pokemon encounter = generate_pokemon();
        io_wild_battle(encounter);
      }
    }

    move_func[n ? n->mtype : move_pc](c, d);

    world.cur_map->cmap[c->pos[dim_y]][c->pos[dim_x]] = NULL;
    if (p && (d[dim_x] == 0 || d[dim_x] == MAP_X - 1 ||
              d[dim_y] == 0 || d[dim_y] == MAP_Y - 1)) {
      leave_map(d);
      d[dim_x] = c->pos[dim_x];
      d[dim_y] = c->pos[dim_y];
    }
    world.cur_map->cmap[d[dim_y]][d[dim_x]] = c;

    if (p) {
      pathfind(world.cur_map);
    }

    c->next_turn += move_cost[n ? n->ctype : char_pc]
                             [world.cur_map->map[d[dim_y]][d[dim_x]]];

    c->pos[dim_y] = d[dim_y];
    c->pos[dim_x] = d[dim_x];

    heap_insert(&world.cur_map->turn, c);
  }
}

void usage(char *s)
{
  fprintf(stderr, "Usage: %s [-s|--seed <seed>]\n", s);

  exit(1);
}

int main(int argc, char *argv[])
{
  struct timeval tv;
  uint32_t seed;
  int long_arg;
  int do_seed;
  //  char c;
  //  int x, y;
  int i;

  db_parse(false);

  do_seed = 1;
  
  if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-seed")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%u", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to start the game more than once *
     * per second, as opposed to time().          */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Using seed: %u\n", seed);
  srand(seed);

  io_init_terminal();
  
  init_world();

  io_choose_starter();

  /* print_hiker_dist(); */
  
  /*
  do {
    print_map();  
    printf("Current position is %d%cx%d%c (%d,%d).  "
           "Enter command: ",
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S',
           world.cur_idx[dim_x] - (WORLD_SIZE / 2),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2));
    scanf(" %c", &c);
    switch (c) {
    case 'n':
      if (world.cur_idx[dim_y]) {
        world.cur_idx[dim_y]--;
        new_map();
      }
      break;
    case 's':
      if (world.cur_idx[dim_y] < WORLD_SIZE - 1) {
        world.cur_idx[dim_y]++;
        new_map();
      }
      break;
    case 'e':
      if (world.cur_idx[dim_x] < WORLD_SIZE - 1) {
        world.cur_idx[dim_x]++;
        new_map();
      }
      break;
    case 'w':
      if (world.cur_idx[dim_x]) {
        world.cur_idx[dim_x]--;
        new_map();
      }
      break;
     case 'q':
      break;
    case 'f':
      scanf(" %d %d", &x, &y);
      if (x >= -(WORLD_SIZE / 2) && x <= WORLD_SIZE / 2 &&
          y >= -(WORLD_SIZE / 2) && y <= WORLD_SIZE / 2) {
        world.cur_idx[dim_x] = x + (WORLD_SIZE / 2);
        world.cur_idx[dim_y] = y + (WORLD_SIZE / 2);
        new_map();
      }
      break;
    case '?':
    case 'h':
      printf("Move with 'e'ast, 'w'est, 'n'orth, 's'outh or 'f'ly x y.\n"
             "Quit with 'q'.  '?' and 'h' print this help message.\n");
      break;
    default:
      fprintf(stderr, "%c: Invalid input.  Enter '?' for help.\n", c);
      break;
    }
  } while (c != 'q');

  */

  game_loop();
  
  delete_world();

  io_reset_terminal();
  
  return 0;
}
