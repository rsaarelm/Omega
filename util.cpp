/* copyright (c) 1987,1988,1989 by Laurence Raphael Brothers */
/* utils.c */

/* Random utility functions called from all over */

#include "glob.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <string>
#include <thread>
#include <unistd.h>

std::mt19937 generator;

/* x and y on level? */
int inbounds(int x, int y)
{
  return ((x >= 0) && (y >= 0) && (x < WIDTH) && (y < LENGTH));
}

int random_range(int k)
{
  int small, large;
  if(k > 0)
  {
    small = 0;
    large = k - 1;
  }
  else if(k < 0)
  {
    small = k + 1;
    large = 0;
  }
  else
  {
    return 0;
  }
  std::uniform_int_distribution<> distribution(small, large);
  return distribution(generator);
}

/* modify absolute y coord relative to which part of level we are on */
int screenmod(int y)
{
  return (y - ScreenOffset);
}
int screenmod_horizontal(int x)
{
  return x - HorizontalOffset;
}

int offscreen(int x, int y)
{
  return ((y < 0) || (y < ScreenOffset) || (y > ScreenOffset + ScreenLength - 1) || (y > LENGTH)) ||
         ((x < 0) || (x < HorizontalOffset) || (x > HorizontalOffset + ScreenWidth - 1) || (x > WIDTH));
}

/* always hit on a natural 0; never hit on a natural 19 */
bool hitp(int hit, int ac)
{
  int roll = random_range(20);
  if(roll == 0)
  {
    return true;
  }
  else if(roll == 19)
  {
    return false;
  }
  else
  {
    return roll < (hit - ac);
  }
}

/* number of moves from x1,y1 to x2,y2 */
int distance(int x1, int y1, int x2, int y2)
{
  return (std::max(abs(x2 - x1), abs(y2 - y1)));
}

/* can you shoot, or move monsters through a spot? */
int unblocked(int x, int y)
{
  if((!inbounds(x, y)) || (Level->site[x][y].creature != NULL) || (Level->site[x][y].locchar == WALL) ||
     (Level->site[x][y].locchar == PORTCULLIS) || (Level->site[x][y].locchar == STATUE) ||
     (Level->site[x][y].locchar == HEDGE) || (Level->site[x][y].locchar == CLOSED_DOOR) ||
     loc_statusp(x, y, SECRET, *Level) || ((x == Player.x) && (y == Player.y)))
  {
    return (false);
  }
  else
  {
    return (true);
  }
}

/* do monsters want to move through a spot */
int m_unblocked(struct monster *m, int x, int y)
{
  if((!inbounds(x, y)) || ((x == Player.x) && (y == Player.y)))
  {
    return (false);
  }
  else if((Level->site[x][y].creature != NULL) || (Level->site[x][y].locchar == SPACE))
  {
    return (false);
  }
  else if(m_statusp(*m, ONLYSWIM))
  {
    return (Level->site[x][y].locchar == WATER);
  }
  else if(loc_statusp(x, y, SECRET, *Level))
  {
    if(m->movef == M_MOVE_SMART)
    {
      if(los_p(x, y, Player.x, Player.y))
      {
        mprint("You see a secret door swing open!");
        lreset(x, y, SECRET, *Level);
        lset(x, y, CHANGED, *Level);
      }
      else
      {
        mprint("You hear a door creak open, and then close again.");
      }
      /* smart monsters would close secret doors behind them if the */
      /* player didn't see them using it */
      return (true);
    }
    else
    {
      return (m_statusp(*m, INTANGIBLE));
    }
  }
  else if((Level->site[x][y].locchar == FLOOR) || (Level->site[x][y].locchar == OPEN_DOOR))
  {
    return (true);
  }
  else if((Level->site[x][y].locchar == PORTCULLIS) || (Level->site[x][y].locchar == WALL) ||
          (Level->site[x][y].locchar == STATUE))
  {
    return (m_statusp(*m, INTANGIBLE));
  }
  else if(Level->site[x][y].locchar == WATER)
  {
    return (m_statusp(*m, SWIMMING) || m_statusp(*m, ONLYSWIM) || m_statusp(*m, INTANGIBLE) ||
            m_statusp(*m, FLYING));
  }
  else if(Level->site[x][y].locchar == CLOSED_DOOR)
  {
    if(m->movef == M_MOVE_SMART)
    {
      mprint("You hear a door creak open.");
      Level->site[x][y].locchar = OPEN_DOOR;
      lset(x, y, CHANGED, *Level);
      return (true);
    }
    else if(random_range(m->dmg) > random_range(100) && Level->site[x][y].p_locf != L_ORACLE)
    {
      mprint("You hear a door shattering.");
      Level->site[x][y].locchar = RUBBLE;
      lset(x, y, CHANGED, *Level);
      return (true);
    }
    else
    {
      return (m_statusp(*m, INTANGIBLE));
    }
  }
  else if(Level->site[x][y].locchar == LAVA)
  {
    return ((m_immunityp(*m, FLAME) && m_statusp(*m, SWIMMING)) || m_statusp(*m, INTANGIBLE) ||
            m_statusp(*m, FLYING));
  }
  else if(Level->site[x][y].locchar == FIRE)
  {
    return (m_statusp(*m, INTANGIBLE) || m_immunityp(*m, FLAME));
  }
  else if((Level->site[x][y].locchar == TRAP) || (Level->site[x][y].locchar == HEDGE) ||
          (Level->site[x][y].locchar == ABYSS))
  {
    return ((m->movef == M_MOVE_CONFUSED) || m_statusp(*m, INTANGIBLE) || m_statusp(*m, FLYING));
  }
  else
  {
    return (true);
  }
}

/* can you see through a spot? */
int view_unblocked(int x, int y)
{
  if(!inbounds(x, y))
  {
    return (false);
  }
  else if((Level->site[x][y].locchar == WALL) || (Level->site[x][y].locchar == STATUE) ||
          (Level->site[x][y].locchar == HEDGE) || (Level->site[x][y].locchar == FIRE) ||
          (Level->site[x][y].locchar == CLOSED_DOOR) || loc_statusp(x, y, SECRET, *Level))
  {
    return (false);
  }
  else
  {
    return (true);
  }
}

/* 8 moves in Dirs */
void initdirs()
{
  Dirs[0][0] = 1;
  Dirs[0][1] = 1;
  Dirs[0][2] = -1;
  Dirs[0][3] = -1;
  Dirs[0][4] = 1;
  Dirs[0][5] = -1;
  Dirs[0][6] = 0;
  Dirs[0][7] = 0;
  Dirs[0][8] = 0;
  Dirs[1][0] = 1;
  Dirs[1][1] = -1;
  Dirs[1][2] = 1;
  Dirs[1][3] = -1;
  Dirs[1][4] = 0;
  Dirs[1][5] = 0;
  Dirs[1][6] = 1;
  Dirs[1][7] = -1;
  Dirs[1][8] = 0;
}

/* do_los moves pyx along a lineofsight from x1 to x2 */
/* x1 and x2 are pointers because as a side effect they are changed */
/* to the final location of the pyx */
void do_los(Symbol pyx, int *x1, int *y1, int x2, int y2)
{
  int dx, dy, ox, oy;
  int major, minor;
  int error, delta, step;
  int blocked;

  if(x2 - *x1 < 0)
  {
    dx = 5;
  }
  else if(x2 - *x1 > 0)
  {
    dx = 4;
  }
  else
  {
    dx = -1;
  }
  if(y2 - *y1 < 0)
  {
    dy = 7;
  }
  else if(y2 - *y1 > 0)
  {
    dy = 6;
  }
  else
  {
    dy = -1;
  }
  if(abs(x2 - *x1) > abs(y2 - *y1))
  {
    major = dx;
    minor = dy;
    step  = abs(x2 - *x1);
    delta = 2 * abs(y2 - *y1);
  }
  else
  {
    major = dy;
    minor = dx;
    step  = abs(y2 - *y1);
    delta = 2 * abs(x2 - *x1);
  }
  if(major == -1)
  { /* x1,y2 already == x2,y2 */
    return;
  }
  error = 0;
  do
  {
    ox = *x1;
    oy = *y1;
    *x1 += Dirs[0][major];
    *y1 += Dirs[1][major];
    error += delta;
    if(error > step)
    { /* don't need to check that minor >= 0 */
      *x1 += Dirs[0][minor];
      *y1 += Dirs[1][minor];
      error -= 2 * step;
    }
    blocked = !unblocked(*x1, *y1);
    if(error < 0 && (*x1 != x2 || *y1 != y2) && blocked)
    {
      *x1 -= Dirs[0][minor];
      *y1 -= Dirs[1][minor];
      error += 2 * step;
      blocked = !unblocked(*x1, *y1);
    }
    Level->site[*x1][*y1].showchar = pyx;
    plotchar(pyx, *x1, *y1);
    plotspot(ox, oy, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  } while((*x1 != x2 || *y1 != y2) && !blocked);
  plotspot(*x1, *y1, true);
  levelrefresh();
}

/* This is the same as do_los, except we stop before hitting nonliving
obstructions */
void do_object_los(Symbol pyx, int *x1, int *y1, int x2, int y2)
{
  int dx, dy, ox, oy;
  int major, minor;
  int error, delta, step;
  int blocked;

  if(x2 - *x1 < 0)
  {
    dx = 5;
  }
  else if(x2 - *x1 > 0)
  {
    dx = 4;
  }
  else
  {
    dx = -1;
  }
  if(y2 - *y1 < 0)
  {
    dy = 7;
  }
  else if(y2 - *y1 > 0)
  {
    dy = 6;
  }
  else
  {
    dy = -1;
  }
  if(abs(x2 - *x1) > abs(y2 - *y1))
  {
    major = dx;
    minor = dy;
    step  = abs(x2 - *x1);
    delta = 2 * abs(y2 - *y1);
  }
  else
  {
    major = dy;
    minor = dx;
    step  = abs(y2 - *y1);
    delta = 2 * abs(x2 - *x1);
  }
  if(major == -1)
  { /* x1,y2 already == x2,y2 */
    return;
  }
  error = 0;
  do
  {
    ox = *x1;
    oy = *y1;
    *x1 += Dirs[0][major];
    *y1 += Dirs[1][major];
    error += delta;
    if(error > step)
    { /* don't need to check that minor >= 0 */
      *x1 += Dirs[0][minor];
      *y1 += Dirs[1][minor];
      error -= 2 * step;
    }
    blocked = !unblocked(*x1, *y1);
    if(error < 0 && (*x1 != x2 || *y1 != y2) && blocked)
    {
      *x1 -= Dirs[0][minor];
      *y1 -= Dirs[1][minor];
      error += 2 * step;
      blocked = !unblocked(*x1, *y1);
    }
    plotspot(ox, oy, true);
    if(unblocked(*x1, *y1))
    {
      plotchar(pyx, *x1, *y1);
      Level->site[*x1][*y1].showchar = pyx;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  } while((*x1 != x2 || *y1 != y2) && !blocked);
  if(Level->site[*x1][*y1].creature == NULL && blocked)
  {
    *x1 = ox;
    *y1 = oy;
  }
  plotspot(*x1, *y1, true);
  levelrefresh();
}

/* los_p checks to see whether there is an unblocked los from x1,y1 to x2,y2 */
int los_p(int x1, int y1, int x2, int y2)
{
  int dx, dy;
  int major, minor;
  int error, delta, step;
  int blocked;

  if(x2 - x1 < 0)
  {
    dx = 5;
  }
  else if(x2 - x1 > 0)
  {
    dx = 4;
  }
  else
  {
    dx = -1;
  }
  if(y2 - y1 < 0)
  {
    dy = 7;
  }
  else if(y2 - y1 > 0)
  {
    dy = 6;
  }
  else
  {
    dy = -1;
  }
  if(abs(x2 - x1) > abs(y2 - y1))
  {
    major = dx;
    minor = dy;
    step  = abs(x2 - x1);
    delta = 2 * abs(y2 - y1);
  }
  else
  {
    major = dy;
    minor = dx;
    step  = abs(y2 - y1);
    delta = 2 * abs(x2 - x1);
  }
  if(major == -1)
  { /* x1,y2 already == x2,y2 */
    return true;
  }
  error = 0;
  do
  {
    x1 += Dirs[0][major];
    y1 += Dirs[1][major];
    error += delta;
    if(error > step)
    { /* don't need to check that minor >= 0 */
      x1 += Dirs[0][minor];
      y1 += Dirs[1][minor];
      error -= 2 * step;
    }
    blocked = !unblocked(x1, y1);
    if(error < 0 && (x1 != x2 || y1 != y2) && blocked)
    {
      x1 -= Dirs[0][minor];
      y1 -= Dirs[1][minor];
      error += 2 * step;
      blocked = !unblocked(x1, y1);
    }
  } while((x1 != x2 || y1 != y2) && !blocked);
  return ((x1 == x2) && (y1 == y2));
}

/* view_los_p sees through monsters */
int view_los_p(int x1, int y1, int x2, int y2)
{
  int dx, dy;
  int major, minor;
  int error, delta, step;
  int blocked;

  if(x2 - x1 < 0)
  {
    dx = 5;
  }
  else if(x2 - x1 > 0)
  {
    dx = 4;
  }
  else
  {
    dx = -1;
  }
  if(y2 - y1 < 0)
  {
    dy = 7;
  }
  else if(y2 - y1 > 0)
  {
    dy = 6;
  }
  else
  {
    dy = -1;
  }
  if(abs(x2 - x1) > abs(y2 - y1))
  {
    major = dx;
    minor = dy;
    step  = abs(x2 - x1);
    delta = 2 * abs(y2 - y1);
  }
  else
  {
    major = dy;
    minor = dx;
    step  = abs(y2 - y1);
    delta = 2 * abs(x2 - x1);
  }
  if(major == -1)
  { /* x1,y2 already == x2,y2 */
    return true;
  }
  error = 0;
  do
  {
    x1 += Dirs[0][major];
    y1 += Dirs[1][major];
    error += delta;
    if(error > step)
    {
      x1 += Dirs[0][minor];
      y1 += Dirs[1][minor];
      error -= 2 * step;
    }
    blocked = !view_unblocked(x1, y1);
    if(error < 0 && (x1 != x2 || y1 != y2) && blocked)
    {
      x1 -= Dirs[0][minor];
      y1 -= Dirs[1][minor];
      error += 2 * step;
      blocked = !view_unblocked(x1, y1);
    }
  } while((x1 != x2 || y1 != y2) && !blocked);
  return ((x1 == x2) && (y1 == y2));
}

/* returns the command direction from the index into Dirs */
char inversedir(int dirindex)
{
  switch(dirindex)
  {
    case 0:
      return ('n');
    case 1:
      return ('u');
    case 2:
      return ('b');
    case 3:
      return ('y');
    case 4:
      return ('l');
    case 5:
      return ('h');
    case 6:
      return ('j');
    case 7:
      return ('k');
    default:
      return ('\0');
  }
}

long calc_points()
{
  int  i;
  long points = 0;

  if(gamestatusp(SPOKE_TO_DRUID, GameStatus))
  {
    points += 50;
  }
  if(gamestatusp(COMPLETED_CAVES, GameStatus))
  {
    points += 100;
  }
  if(gamestatusp(COMPLETED_SEWERS, GameStatus))
  {
    points += 200;
  }
  if(gamestatusp(COMPLETED_CASTLE, GameStatus))
  {
    points += 300;
  }
  if(gamestatusp(COMPLETED_ASTRAL, GameStatus))
  {
    points += 400;
  }
  if(gamestatusp(COMPLETED_VOLCANO, GameStatus))
  {
    points += 500;
  }
  if(gamestatusp(KILLED_DRAGONLORD, GameStatus))
  {
    points += 100;
  }
  if(gamestatusp(KILLED_EATER, GameStatus))
  {
    points += 100;
  }
  if(gamestatusp(KILLED_LAWBRINGER, GameStatus))
  {
    points += 100;
  }

  points += Player.xp / 50;

  points += Player.cash / 500;

  for(i = 0; i < MAXITEMS; i++)
  {
    if(Player.possessions[i] != NULL)
    {
      points += Player.possessions[i]->level * (Player.possessions[i]->known + 1);
    }
  }

  for(i = 0; i < MAXPACK; i++)
  {
    if(Player.pack[i] != NULL)
    {
      points += Player.pack[i]->level * (Player.pack[i]->known + 1);
    }
  }

  for(i = 0; i < NUMRANKS; i++)
  {
    if(Player.rank[i] == 5)
    {
      points += 500;
    }
    else
    {
      points += 20 * Player.rank[i];
    }
  }

  if(Player.hp < 1)
  {
    points = (points / 2);
  }
  else if(Player.rank[ADEPT])
  {
    points *= 10;
  }

  return (points);
}

/* returns the 24 hour clock hour */
int hour()
{
  return static_cast<int>(((Time + 720) / 60) % 24);
}

/* returns 0, 10, 20, 30, 40, or 50 */
int showminute()
{
  return static_cast<int>(((Time % 60) / 10) * 10);
}

/* returns the 12 hour clock hour */
int showhour()
{
  int showtime;
  if((hour() == 0) || (hour() == 12))
  {
    showtime = 12;
  }
  else
  {
    showtime = (hour() % 12);
  }
  return (showtime);
}

/* nighttime is defined from 9 PM to 6AM */
int nighttime()
{
  return ((hour() > 20) || (hour() < 7));
}

const char *getarticle(const std::string &str)
{
  if((str[0] == 'a') || (str[0] == 'A') || (str[0] == 'e') || (str[0] == 'E') || (str[0] == 'i') ||
     (str[0] == 'I') || (str[0] == 'o') || (str[0] == 'O') || (str[0] == 'u') || (str[0] == 'U') ||
     (((str[0] == 'h') || (str[0] == 'H')) && ((str[1] == 'i') || (str[1] == 'e'))))
  {
    return ("an ");
  }
  else
  {
    return ("a ");
  }
}

int day()
{
  return ((Date % 30) + 1);
}

const char *ordinal(int number)
{
  if((number == 11) || (number == 12) || (number == 13))
  {
    return ("th");
  }
  else
  {
    switch(number % 10)
    {
      case 1:
        return ("st");
      case 2:
        return ("nd");
      case 3:
        return ("rd");
      default:
        return ("th");
    }
  }
}

const char *month()
{
  switch((Date % 360) / 30)
  {
    case 0:
      return ("Freeze");
    case 1:
      return ("Ice");
    case 2:
      return ("Mud");
    case 3:
      return ("Storm");
    case 4:
      return ("Breeze");
    case 5:
      return ("Light");
    case 6:
      return ("Flame");
    case 7:
      return ("Broil");
    case 8:
      return ("Cool");
    case 9:
      return ("Haunt");
    case 10:
      return ("Chill");
    case 11:
      return ("Dark");
    case 12:
      return ("Twixt");
    default:
      return ("***Error***");
  }
}

/* WDT: code for the following two functions contributed by Sheldon
 * Simms. */
/* finds floor space on level with buildaux not equal to baux,
sets x,y there. There must *be* floor space somewhere on level.... */
int spaceok(int i, int j, int baux)
{
  return ((Level->site[i][j].locchar == FLOOR) && (Level->site[i][j].creature == NULL) &&
          (!loc_statusp(i, j, SECRET, *Level)) && (Level->site[i][j].buildaux != baux));
}

void findspace(int *x, int *y, int baux)
{
  int i, j, tog = true, done = false;

  do
  {
    i = random_range(WIDTH);
    j = random_range(LENGTH);
    if(spaceok(i, j, baux))
    {
      done = true;
    }
    else
    {
      if(tog)
      {
        tog = !tog;
        while(1)
        {
          i++;
          if(i >= WIDTH)
          {
            break;
          }
          else if(spaceok(i, j, baux))
          {
            done = true;
            break;
          }
        }
      }
      else
      {
        tog = !tog;
        while(1)
        {
          j++;
          if(j >= LENGTH)
          {
            break;
          }
          else if(spaceok(i, j, baux))
          {
            done = true;
            break;
          }
        }
      }
    }
  } while(!done);
  *x = i;
  *y = j;
}

/* is prefix a prefix of s? */
int strprefix(const std::string &prefix, const std::string &s)
{
  int    matched = true;
  size_t i       = 0;
  if(prefix.length() > s.length())
  {
    return (false);
  }
  else
  {
    while(matched && (i < prefix.length()))
    {
      matched = (prefix[i] == s[i]);
      i++;
    }
    return (matched);
  }
}

int confirmation()
{
  switch(random_range(4))
  {
    case 0:
      mprint("Are you sure?");
      break;
    case 1:
      mprint("Certain about that?");
      break;
    case 2:
      mprint("Do you really mean it?");
      break;
    case 3:
      mprint("Confirm that, would you?");
      break;
  }
  if(optionp(PARANOID_CONFIRM, Player))
  {
    mprint("[yes] [no]");
    return msgscanstring() == "yes";
  }
  else
  {
    mprint("[yn] (n)");
    return ynq() == 'y';
  }
}

/* is character c a member of string s */
int strmem(char c, const std::string &s)
{
  int    found = false;
  size_t i     = 0;
  for(i = 0; ((i < s.length()) && (!found)); i++)
  {
    found = (s[i] == c);
  }
  return (found);
}

void calc_weight()
{
  int i, weight = 0;

  for(i = 1; i < MAXITEMS; i++)
  {
    if(Player.possessions[i] != NULL)
    {
      weight += Player.possessions[i]->weight * Player.possessions[i]->number;
    }
  }
  if((Player.possessions[O_WEAPON_HAND] != NULL) &&
     (Player.possessions[O_READY_HAND] == Player.possessions[O_WEAPON_HAND]))
  {
    weight -= Player.possessions[O_READY_HAND]->weight * Player.possessions[O_READY_HAND]->number;
  }
  for(i = 0; i < MAXPACK; i++)
  {
    if(Player.pack[i] != NULL)
    {
      weight += Player.pack[i]->weight * Player.pack[i]->number;
    }
  }
  Player.itemweight = weight;
  dataprint();
}

/* returns true if its ok to get rid of a level */
int ok_to_free(plv level)
{
  if(level == NULL)
  {
    return (false);
  }
  else
  {
    return ((level->environment != E_CITY) && (level->environment != E_VILLAGE) &&
            (level->environment != Current_Dungeon));
  }
}

void free_objlist(pol pobjlist)
{
  pol tmp;

  while(pobjlist)
  {
    free((tmp = pobjlist)->thing);
    pobjlist = pobjlist->next;
    free(tmp);
  }
}

void free_mons_and_objs(pml mlist)
{
  pml tmp;

  while(mlist)
  {
    free_objlist((tmp = mlist)->m->possessions);
    free(tmp->m);
    mlist = mlist->next;
    free(tmp);
  }
}

/* Free up monsters and items on a level*/
void free_level(plv level)
{
  int i, j;

  free_mons_and_objs(level->mlist);
  for(i = 0; i < MAXWIDTH; i++)
  {
    for(j = 0; j < MAXLENGTH; j++)
    {
      if(level->site[i][j].things)
      {
        free_objlist(level->site[i][j].things);
        level->site[i][j].things = NULL;
      }
    }
  }
#ifndef SAVE_LEVELS
  free(level);
#endif
}

/* malloc function that checks its return value - if NULL, tries to free */
/* some memory... */
void *checkmalloc(unsigned int bytes)
{
  void         *ptr = malloc(bytes);
  struct level *curr, **prev, **oldest;

  if(ptr)
  {
    return ptr;
  }
  for(curr = Dungeon, oldest = prev = &Dungeon; curr; curr = curr->next)
  {
    if((*oldest)->last_visited > curr->last_visited)
    {
      oldest = prev;
    }
    prev = &(curr->next);
  }
  if(*oldest && *oldest != Level)
  {
    curr    = *oldest;
    *oldest = (*oldest)->next;
    free_level(curr);
    ptr = malloc(bytes);
  }
  if(ptr)
  {
    return ptr;
  }
  else
  {
    print1("Out of memory!  Saving and quitting.");
    save(true);
    endgraf();
    exit(0);
  }
}

/* alloc just enough string space for str, strcpy, and return pointer */
const char *salloc(const char *str)
{
  char *s = (char *)checkmalloc((unsigned)(strlen(str) + 1));
  strcpy(s, str);
  return (s);
}

char cryptkey(const std::string &fname)
{
  int key = 0;

  for(int pos = 0; fname[pos]; pos++)
  {
    key += 3 * (fname[pos] - ' ');
  }
  return (key & 0xff);
}

int game_uid;
int user_uid;

void init_perms()
{
#ifndef PLATFORM_WINDOWS
  user_uid = getuid();
  game_uid = geteuid();
#endif
}

void change_to_user_perms()
{
#ifndef PLATFORM_WINDOWS
  seteuid(user_uid);
#endif
}

void change_to_game_perms()
{
#ifndef PLATFORM_WINDOWS
  seteuid(game_uid);
#endif
}
