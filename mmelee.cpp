/* omega copyright (c) 1987,1988,1989 by Laurence Raphael Brothers */
/* mmelee */
/* various functions to do with monster melee */

#include "glob.h"

#include <algorithm>

void m_hit(monster *m, int dtype)
{
  std::string monster_name;
  if(m->uniqueness == COMMON)
  {
    monster_name = "a ";
  }
  monster_name += m->monstring;
  if((Player.status[DISPLACED] > 0) && (random_range(2) == 1))
  {
    mprint("The attack was displaced!");
  }
  else
  {
    p_damage(random_range(m->dmg), dtype, monster_name);
  }
}

/* execute monster attacks versus player */
void tacmonster(monster *m)
{
  drawvision(Player.x, Player.y);
  transcribe_monster_actions(m);
  for(size_t i = 0; i < strlen(m->meleestr) && m->hp > 0; i += 2)
  {
    std::string monster_name;
    if(m->uniqueness == COMMON)
    {
      monster_name = "The ";
    }
    monster_name += m->monstring;
    if(m->meleestr[i] == 'A')
    {
      if(Verbosity == VERBOSE)
      {
        mprint(monster_name + " attacks " + actionlocstr(m->meleestr[i + 1]));
      }
      monster_melee(m, m->meleestr[i + 1], 0);
    }
    else if(m->meleestr[i] == 'L')
    {
      if(Verbosity == VERBOSE)
      {
        mprint(monster_name + " lunges " + actionlocstr(m->meleestr[i + 1]));
      }
      monster_melee(m, m->meleestr[i + 1], m->level);
    }
    i += 2;
  }
}

void monster_melee(struct monster *m, char hitloc, int bonus)
{
  if(player_on_sanctuary())
  {
    print1("The aegis of your deity protects you!");
  }
  else
  {
    /* It's lawful to wait to be attacked */
    if(m->attacked == 0)
    {
      ++Player.alignment;
    }
    ++m->attacked;
    std::string monster_name;
    if(m->uniqueness == COMMON)
    {
      monster_name = "The ";
    }
    monster_name += m->monstring;
    if(monster_hit(m, hitloc, bonus))
    {
      switch(m->meleef)
      {
        case M_NO_OP:
          mprint(monster_name + " touches you.");
          break;
        case M_MELEE_NORMAL:
          mprint(monster_name + " hits you.");
          m_hit(m, NORMAL_DAMAGE);
          break;
        case M_MELEE_NG:
          mprint(monster_name + " hits you.");
          m_hit(m, NORMAL_DAMAGE);
          if(random_range(5) == 3)
          {
            m_sp_ng(m);
          }
          break;
        case M_MELEE_FIRE:
          mprint(monster_name + " blasts you with fire.");
          m_hit(m, FLAME);
          break;
        case M_MELEE_DRAGON:
          mprint(monster_name + " hits you and blasts you with fire.");
          m_hit(m, NORMAL_DAMAGE);
          m_hit(m, FLAME);
          break;
        case M_MELEE_ELEC:
          mprint(monster_name + " lashes you with electricity.");
          m_hit(m, ELECTRICITY);
          break;
        case M_MELEE_COLD:
          mprint(monster_name + " freezes you with cold.");
          m_hit(m, ELECTRICITY);
          break;
        case M_MELEE_POISON:
          mprint(monster_name + " hits you.");
          m_hit(m, NORMAL_DAMAGE);
          if(random_range(10) < m->level)
          {
            mprint("You've been poisoned!");
            p_poison(m->dmg);
          }
          break;
        case M_MELEE_GRAPPLE:
          mprint(monster_name + " grabs you.");
          m_hit(m, NORMAL_DAMAGE);
          ++Player.status[IMMOBILE];
          break;
        case M_MELEE_SPIRIT:
          mprint(monster_name + " touches you.");
          m_hit(m, NORMAL_DAMAGE);
          drain_life(m->level);
          break;
        case M_MELEE_DISEASE:
          mprint(monster_name + " hits you.");
          m_hit(m, NORMAL_DAMAGE);
          if(random_range(10) < m->level)
          {
            mprint("You've been infected!");
            disease(m->level);
          }
          break;
        case M_MELEE_SLEEP:
          mprint(monster_name + " hit you.");
          m_hit(m, NORMAL_DAMAGE);
          if(random_range(10) < m->level)
          {
            mprint("You feel drowsy");
            sleep_player(m->level);
          }
          break;
      }
    }
    else
    {
      if(random_range(10))
      {
        mprint(monster_name + " missed you.");
      }
      else
      {
        if(Verbosity == TERSE)
        {
          switch(random_range(10))
          {
            case 0:
              mprint(monster_name + " blundered severely.");
              m_damage(m, m->dmg, UNSTOPPABLE);
              break;
            case 1:
              mprint(monster_name + " tripped while attacking.");
              m_dropstuff(m);
              break;
            case 2:
              mprint(monster_name + " seems seriously confused.");
              m->speed = std::min(30, m->speed * 2);
              break;
            default:
              mprint(monster_name + " missed you.");
          }
        }
        else
        {
          switch(random_range(10))
          {
            case 0:
              mprint(monster_name + " flailed stupidly at you.");
              break;
            case 1:
              mprint(monster_name + " made you laugh.");
              break;
            case 2:
              mprint(monster_name + " blundered severely.");
              m_damage(m, m->dmg, UNSTOPPABLE);
              break;
            case 3:
              mprint(monster_name + " tripped while attacking.");
              m_dropstuff(m);
              break;
            case 4:
              mprint(monster_name + " seems seriously confused.");
              m->speed = std::min(30, m->speed * 2);
              break;
            case 5:
              mprint(monster_name + " is seriously ashamed.");
              break;
            case 6:
              mprint(monster_name + " made a boo-boo.");
              break;
            case 7:
              mprint(monster_name + " blundered.");
              break;
            case 8:
              mprint(monster_name + " cries out in anger and frustration.");
              break;
            case 9:
              mprint(monster_name + " curses your ancestry.");
              break;
          }
        }
      }
    }
  }
}

/* checks to see if player hits with hitmod vs. monster m at location hitloc */
bool monster_hit(monster *m, char hitloc, int bonus)
{
  int    goodblocks = 0;
  bool   blocks = false, riposte = false;
  size_t meleestr_length = std::min(strlen(Player.meleestr), maneuvers() * 2);
  for(size_t i = 0; i < meleestr_length; i += 2)
  {
    if(Player.meleestr[i] == 'B' || (Player.meleestr[i] == 'R'))
    {
      blocks = true;
      if(hitloc == Player.meleestr[i + 1])
      {
        ++goodblocks;
        if(Player.meleestr[i] == 'R' && Player.possessions[O_WEAPON_HAND] &&
           Player.possessions[O_WEAPON_HAND]->type == THRUSTING)
        {
          riposte = true;
        }
      }
    }
  }
  if(!blocks)
  {
    goodblocks = -1;
  }
  bool hit = hitp(m->hit + bonus, Player.defense + goodblocks * 10);
  if(!hit && goodblocks > 0)
  {
    if(Verbosity == VERBOSE)
    {
      mprint("You blocked it!");
    }
    if(riposte)
    {
      if(Verbosity != TERSE)
      {
        mprint("You got a riposte!");
      }
      if(hitp(Player.hit, m->ac))
      {
        mprint("You hit!");
        weapon_use(0, Player.possessions[O_WEAPON_HAND], m);
      }
      else
      {
        mprint("You missed.");
      }
    }
  }
  return hit;
}

/* decide monster actions in tactical combat mode */
/* if monster is skilled, it can try see the player's attacks coming and
   try to block appropriately. */

void transcribe_monster_actions(monster *m)
{
  char        attack_loc, block_loc;
  static char mmstr[80];

  int p_blocks[3];
  int p_attacks[3];

  for(uint8_t i = 0; i < 3; ++i)
  {
    p_blocks[i] = p_attacks[i] = 0;
  }

  /* Find which area player blocks and attacks least in */
  size_t meleestr_length = std::min(strlen(Player.meleestr), maneuvers() * 2);
  for(size_t i = 0; i < meleestr_length; i += 2)
  {
    if((Player.meleestr[i] == 'B') || (Player.meleestr[i] == 'R'))
    {
      if(Player.meleestr[i + 1] == 'H')
      {
        ++p_blocks[0];
      }
      if(Player.meleestr[i + 1] == 'C')
      {
        ++p_blocks[1];
      }
      if(Player.meleestr[i + 1] == 'L')
      {
        ++p_blocks[2];
      }
    }
    else if((Player.meleestr[i] == 'A') || (Player.meleestr[i] == 'L'))
    {
      if(Player.meleestr[i + 1] == 'H')
      {
        ++p_attacks[0];
      }
      if(Player.meleestr[i + 1] == 'C')
      {
        ++p_attacks[1];
      }
      if(Player.meleestr[i + 1] == 'L')
      {
        ++p_attacks[2];
      }
    }
  }

  if(p_blocks[2] <= p_blocks[1] && p_blocks[2] <= p_blocks[0])
  {
    attack_loc = 'L';
  }
  else if(p_blocks[1] <= p_blocks[2] && p_blocks[1] <= p_blocks[0])
  {
    attack_loc = 'C';
  }
  else
  {
    attack_loc = 'H';
  }

  if(p_attacks[2] >= p_attacks[1] && p_attacks[2] >= p_attacks[0])
  {
    block_loc = 'L';
  }
  else if(p_attacks[1] >= p_attacks[2] && p_attacks[1] >= p_attacks[0])
  {
    block_loc = 'C';
  }
  else
  {
    block_loc = 'H';
  }

  if(m->id != NPC)
  {
    strcpy(mmstr, Monsters[m->id].meleestr);
  }
  else
  {
    strcpy(mmstr, "");
    for(int i = 0; i < m->level; i += 2)
    {
      strcat(mmstr, "L?R?");
    }
  }

  for(size_t i = 0; i < strlen(m->meleestr); i += 2)
  {
    if(m->meleestr[i] == 'A' || m->meleestr[i] == 'L')
    {
      if(m->meleestr[i + 1] == '?')
      {
        if(m->level + random_range(30) > Player.level + random_range(20))
        {
          mmstr[i + 1] = attack_loc;
        }
        else
        {
          mmstr[i + 1] = random_loc();
        }
      }
      else if(m->meleestr[i + 1] == 'X')
      {
        mmstr[i + 1] = random_loc();
      }
    }
    else if(m->meleestr[i] == 'B' || m->meleestr[i] == 'R')
    {
      if(mmstr[i + 1] == '?')
      {
        if(m->level + random_range(30) > Player.level + random_range(20))
        {
          mmstr[i + 1] = block_loc;
        }
        else
        {
          mmstr[i + 1] = random_loc();
        }
      }
      else if(mmstr[i + 1] == 'X')
      {
        mmstr[i + 1] = random_loc();
      }
    }
  }
  m->meleestr = mmstr;
}

char random_loc()
{
  switch(random_range(3))
  {
    case 0:
      return ('H');
    case 1:
      return ('C');
    default:
      return ('L');
  }
}
