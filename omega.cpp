/* omega copyright (c) 1987,1988,1989 by Laurence Raphael Brothers */

/* this file includes main() and some top-level functions */
/* omega.c */

#include <csignal>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <random>
#include <unistd.h>
/* Note: in order to avoid a memory bug I've been told about, I'm
   explicitly initializing every global to something. */

#include "glob.h"

extern void expand_message_window();
extern void shrink_message_window();

#ifdef SAVE_LEVELS
extern void msdos_init();
#endif

extern std::mt19937 generator;

/* most globals originate in omega.c */

const char *Omegalib; /* contains the path to the library files */

/* Objects and Monsters are allocated and initialized in init.c */

/* one of each spell */
struct spell Spells[NUMSPELLS + 1];

/* locations of city sites [0] - found, [1] - x, [2] - y */
int CitySiteList[NUMCITYSITES][3];

/* Currently defined in caps since it is now a variable, was a constant */
int  LENGTH = MAXLENGTH;
int  WIDTH  = MAXWIDTH;
bool terminal_size_too_small;

bool           received_directions = false;
bool           IsMenu              = false;
long           GameStatus          = 0L; /* Game Status bit vector */
int            ScreenLength        = 0;  /* How large is level window */
int            ScreenWidth         = 0;
struct player  Player;                       /* the player */
struct terrain Country[MAXWIDTH][MAXLENGTH]; /* The countryside */
#ifdef SAVE_LEVELS
struct level TheLevel;
#endif
struct level   *City                = NULL; /* The city of Rampart */
struct level   *TempLevel           = NULL; /* Place holder */
struct level   *Level               = NULL; /* Pointer to current Level */
struct level   *Dungeon             = NULL; /* Pointer to current Dungeon */
int             Villagenum          = 0;    /* Current Village number */
int             ScreenOffset        = 0;    /* Offset of displayed screen to level */
int             HorizontalOffset    = 0;
int             MaxDungeonLevels    = 0;             /* Deepest level allowed in dungeon */
int             Current_Dungeon     = -1;            /* What is Dungeon now */
int             Current_Environment = E_CITY;        /* Which environment are we in */
int             Last_Environment    = E_COUNTRYSIDE; /* Which environment were we in */
int             Dirs[2][9];                          /* 9 xy directions */
int             Cmd                  = 's';          /* last player command */
int             Command_Duration     = 0;            /* how long does current command take */
struct monster *Arena_Monster        = NULL;         /* Opponent in arena */
int             Arena_Opponent       = 0;            /* case label of opponent in l_arena()*/
int             Arena_Victory        = 0;            /* did player win in arena? */
int             Imprisonment         = 0;            /* amount of time spent in jail */
int             Precipitation        = 0;            /* Hours of rain, snow, etc */
int             Lunarity             = 0;            /* Effect of the moon on character */
int             Phase                = 0;            /* Phase of the moon */
int             Date                 = 0;            /* Starting date */
int             Pawndate             = -1;           /* Pawn Shop item generation date */
pob             Pawnitems[PAWNITEMS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
/* items in pawn shop */
int  SymbolUseHour = -1;                   /* holy symbol use marker */
int  ViewHour      = -1;                   /* crystal ball use marker */
int  ZapHour       = -1;                   /* staff of enchantment use marker */
int  HelmHour      = -1;                   /* helm of teleportation use marker*/
int  Constriction  = 0;                    /* Dragonlord Attack State */
int  Blessing      = false;                /* Altar Blessing State */
int  LastDay       = -1;                   /* DPW date of dole */
int  RitualHour    = -1;                   /* last use of ritual magic */
int  RitualRoom    = -1;                   /* last room of ritual magic */
int  Lawstone      = 0;                    /* magic stone counter */
int  Chaostone     = 0;                    /* magic stone counter */
int  Mindstone     = 0;                    /* magic stone counter */
int  Searchnum     = 1;                    /* number of times to search on 's' */
int  Behavior;                             /* Player NPC behavior */
int  Verbosity = VERBOSE;                  /* verbosity level */
long Time      = 0;                        /* turn number */
int  Tick      = 0;                        /* 10 a turn; action coordinator */
char Stringbuffer[STRING_BUFFER_SIZE][80]; /* last strings printed */
long Gymcredit       = 0;                  /* credit at rampart gym */
int  Spellsleft      = 0;                  /* research allowance at college */
int  StarGemUse      = 0;                  /* last date of star gem use */
int  HiMagicUse      = 0;                  /* last date of high magic use */
int  HiMagic         = 0;                  /* current level for l_throne */
long Balance         = 0;                  /* bank account */
long FixedPoints     = 0;                  /* points are frozen after adepthood*/
int  LastTownLocX    = 0;                  /* previous position in village or city */
int  LastTownLocY    = 0;                  /* previous position in village or city */
int  LastCountryLocX = 0;                  /* previous position in countryside */
int  LastCountryLocY = 0;                  /* previous position in countryside */
char Password[64];                         /* autoteller password */
char Str1[STRING_LEN], Str2[STRING_LEN], Str3[STRING_LEN], Str4[STRING_LEN];
/* Some string space, random uses */

pol Condoitems = NULL; /* Items in condo */

/* high score names, levels, behavior */
int  Shadowlordbehavior, Archmagebehavior, Primebehavior, Commandantbehavior;
int  Championbehavior, Priestbehavior[7], Hibehavior, Dukebehavior;
int  Chaoslordbehavior, Lawlordbehavior, Justiciarbehavior;
char Shadowlord[80], Archmage[80], Prime[80], Commandant[80], Duke[80];
char Champion[80], Priest[7][80], Hiscorer[80], Hidescrip[80];
char Chaoslord[80], Lawlord[80], Justiciar[80];
int  Shadowlordlevel, Archmagelevel, Primelevel, Commandantlevel, Dukelevel;
int  Championlevel, Priestlevel[7], Hilevel, Justiciarlevel;
long Hiscore        = 0L;
int  Chaoslordlevel = 0, Lawlordlevel = 0, Chaos = 0, Law = 0;

/* New globals which used to be statics */
int twiddle       = false;
int saved         = false;
int onewithchaos  = false;
int club_hinthour = 0;
int winnings      = 0;
int tavern_hinthour;
int scroll_ids[30];
int potion_ids[30];
int stick_ids[30];
int ring_ids[30];
int cloak_ids[30];
int boot_ids[30];

int deepest[E_MAX + 1];
int level_seed[E_MAX + 1]; /* random number seed that generated level */

/* environment is the environment about to be generated, or -1 for the first */
/* time, or -2 if we want to restore the random number point */
void initrand(int environment, int level)
{
  static int store;
  int        seed;

  if(environment >= 0)
  {
    store = random_range(RAND_MAX);
  }
  /* Pseudo Random Seed */
  if(environment == E_RANDOM)
  {
    seed = static_cast<int>(time(nullptr));
  }
  else if(environment == E_RESTORE)
  {
    seed = store;
  }
  else
  {
    seed = level_seed[environment] + 1000 * level;
  }
  generator.seed(seed);
}

int game_restore(int argc, char *argv[])
{
  char savestr[80];
  int  ok;
  if(argc == 2)
  {
    strcpy(savestr, argv[1]);
    ok = restore_game(savestr);
    if(!ok)
    {
      endgraf();
      printf("Try again with the right save file, luser!\n");
      exit(0);
    }
    change_to_user_perms();
    unlink(savestr);
    change_to_game_perms();
    return (true);
  }
  else
  {
    return (false);
  }
}

void omega_title();

int main(int argc, char *argv[])
{
  int continuing;
  int count;

  /* always catch ^c and hang-up signals */

#ifdef SIGINT
  signal(SIGINT, quit);
#endif
#ifdef SIGHUP
  signal(SIGHUP, signalsave);
#endif

  if(CATCH_SIGNALS)
  {
#ifndef PLATFORM_WINDOWS
    signal(SIGQUIT, signalexit);
#endif
    signal(SIGILL, signalexit);
#ifndef PLATFORM_WINDOWS
    signal(SIGTRAP, signalexit);
#endif
    signal(SIGFPE, signalexit);
    signal(SIGSEGV, signalexit);
#ifdef SIGIOT
    signal(SIGIOT, signalexit);
#endif
#ifdef SIGABRT
    signal(SIGABRT, signalexit);
#endif
#ifdef SIGEMT
    signal(SIGEMT, signalexit);
#endif
#ifdef SIGBUS
    signal(SIGBUS, signalexit);
#endif
#ifdef SIGSYS
    signal(SIGSYS, signalexit);
#endif
  }

#ifndef FIXED_OMEGALIB
  if(!(Omegalib = getenv("OMEGALIB")))
  {
#endif
    Omegalib = OMEGALIB;
  }

  /* if filecheck is 0, some necessary data files are missing */
  if(filecheck() == 0)
  {
    exit(0);
  }

  /* all kinds of initialization */
  init_perms();
  initgraf();
  initdirs();
  initrand(E_RANDOM, 0);
  initspells();

  for(count = 0; count < STRING_BUFFER_SIZE; count++)
  {
    strcpy(Stringbuffer[count], "<nothing>");
  }

#ifdef SAVE_LEVELS
  msdos_init();
#endif

  omega_title();
  showscores();

  /* game restore attempts to restore game if there is an argument */
  continuing = game_restore(argc, argv);

  /* monsters initialized in game_restore if game is being restored */
  /* items initialized in game_restore if game is being restored */
  if(!continuing)
  {
    inititem(true);
    Date  = random_range(360);
    Phase = random_range(24);
    strcpy(Password, "");
    expand_message_window();
    initplayer();
    init_world();
    shrink_message_window();
    xredraw();
    mprint("'?' for help or commandlist, 'Q' to quit.");
  }
  else
  {
    mprint("Your adventure continues....");
  }

  timeprint();
  calc_melee();
  if(Current_Environment != E_COUNTRYSIDE)
  {
    showroom(Level->site[Player.x][Player.y].roomnumber);
  }
  else
  {
    terrain_check(false);
  }

  if(optionp(SHOW_COLOUR, Player))
  {
    colour_on();
  }
  else
  {
    colour_off();
  }

  screencheck(Player.x, Player.y);

  /* game cycle */
  if(!continuing)
  {
    time_clock(true);
  }
  while(true)
  {
    if(Current_Environment == E_COUNTRYSIDE)
    {
      p_country_process();
    }
    else
    {
      time_clock(false);
    }
  }
}

void signalexit(int)
{
  int reply;
  mprint("Yikes!");
  mprint("Sorry, caught a core-dump signal.");
  mprint("Want to try and save the game?");
  reply = ynq();
  if(reply == 'y')
  {
    save(true); // force save
  }
  else if(reply == EOF)
  {
    signalsave();
  }
  mprint("Bye!");
  endgraf();
  exit(0);
}

/* Start up game with new dungeons; start with player in city */
void init_world()
{
  int env, i;

  City = Level = TempLevel = Dungeon = NULL;
  for(env = 0; env <= E_MAX; env++)
  {
    level_seed[env] = random_range(RAND_MAX);
  }
  load_country();
  for(i = 0; i < NUMCITYSITES; i++)
  {
    CitySiteList[i][0] = false;
  }
  load_city(true);
  WIDTH               = 64;
  LENGTH              = 64;
  Player.x            = 62;
  Player.y            = 21;
  Level               = City;
  Current_Environment = E_CITY;
  print1("You pass through the massive gates of Rampart, the city.");
}

/* set variable item names */
void inititem(int reset)
{
  int i;

  if(reset)
  {
    shuffle(scroll_ids, 30);
    shuffle(potion_ids, 20);
    shuffle(stick_ids, 20);
    shuffle(boot_ids, 20);
    shuffle(cloak_ids, 20);
    shuffle(ring_ids, 20);
  }
  for(i = 0; i < NUMSCROLLS; i++)
  {
    Objects[SCROLLID + i].objstr = scrollname(i);
  }
  for(i = 0; i < NUMPOTIONS; i++)
  {
    Objects[POTIONID + i].objstr = potionname(i);
  }
  Objects[ARTIFACTID + 10].objstr = potionname(18);
  Objects[ARTIFACTID + 13].objstr = potionname(19);
  for(i = 0; i < NUMSTICKS; i++)
  {
    Objects[STICKID + i].objstr = stickname(i);
  }
  for(i = 0; i < NUMBOOTS; i++)
  {
    Objects[BOOTID + i].objstr = bootname(i);
  }
  for(i = 0; i < NUMCLOAKS; i++)
  {
    Objects[CLOAKID + i].objstr = cloakname(i);
  }
  for(i = 0; i < NUMRINGS; i++)
  {
    Objects[RINGID + i].objstr = ringname(i);
  }
}
