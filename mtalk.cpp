/* omega copyright (c) 1987,1988,1989 by Laurence Raphael Brothers */
/* mtalk.c */
/* monster talk functions */

#include "glob.h"

#include <algorithm>

extern bool received_directions;

/* The druid's altar is in the northern forest */
void m_talk_druid(struct monster *m)
{
  int i;
  pml curr;

  if(!m_statusp(*m, HOSTILE))
  {
    print1("The Archdruid raises a hand in greeting.");
    if(!gamestatusp(SPOKE_TO_DRUID, GameStatus))
    {
      setgamestatus(SPOKE_TO_DRUID, GameStatus);
      print1("The Archdruid congratulates you on reaching his sanctum.");
      print2("You feel competent.");
      gain_experience(300);
      if(Player.patron == DRUID)
      {
        print1("The Archdruid conveys to you the wisdom of nature....");
        print2("You feel like a sage.");
        for(i = 0; i < NUMRANKS; i++)
        {
          if(Player.guildxp[i] > 0)
          {
            Player.guildxp[i] += 300;
          }
        }
      }
    }
    mprint("Do you request a ritual of neutralization? [yn] ");
    if(ynq() == 'y')
    {
      if(Phase / 2 == 6 || Phase / 2 == 0)
      { /* full or new moon */
        mprint("\"Unfortunately, I cannot perform a ritual of balance on");
        if(Phase / 2 == 6)
        {
          mprint("this lawful day.\"");
        }
        else
        {
          mprint("this chaotic day.\"");
        }
      }
      else if(Phase / 2 == 3 || Phase / 2 == 9)
      { /* half moon */
        mprint("You take part in today's holy celebration of balance...");
        Player.alignment = 0;
        Player.mana      = calcmana();
        if(Player.patron == DRUID)
        {
          gain_experience(200); /* if a druid wants to spend 2 days */
        }
        Time += 60; /* celebrating for 1600 xp, why not? */
        hourly_check();
        Time += 60;
        hourly_check();
        Time += 60;
        hourly_check();
        Time += 60;
        hourly_check();
        Time += 60;
        hourly_check();
        Time += 60;
        hourly_check();
      }
      else
      {
        mprint("The ArchDruid conducts a sacred rite of balance...");
        if(Player.patron == DRUID)
        {
          Player.alignment = 0;
          Player.mana      = calcmana();
        }
        else
        {
          Player.alignment -= Player.alignment * std::max(0, 10 - Player.level) / 10;
        }
        /* the higher level the character is, the more set in his/her ways */
        Time += 60;
        hourly_check();
      }
      dataprint();
    }
  }
  else
  {
    mprint("The ArchDruid looks at you and cries: 'Unclean! Unclean!'");
    disrupt(Player.x, Player.y, 100);
    mprint("This seems to have satiated his desire for vengeance.");
    mprint("'Have you learned your lesson?' The ArchDruid asks. [yn] ");
    if(ynq())
    {
      mprint("'I certainly hope so!' says the ArchDruid.");
      for(curr = Level->mlist; curr; curr = curr->next)
      {
        m_status_reset(*curr->m, HOSTILE);
      }
      m_vanish(m);
    }
    else
    {
      mprint("'Idiot.' mutters the ArchDruid.");
      p_damage(500, UNSTOPPABLE, "the ArchDruid's Vengeance");
    }
  }
}

void m_talk_silent(struct monster *m)
{
  int reply = random_range(4);

  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  switch(reply)
  {
    case 0:
      strcat(Str2, " does not reply. ");
      break;
    case 1:
      strcat(Str2, " shrugs silently. ");
      break;
    case 2:
      strcat(Str2, " hold a finger to his mouth. ");
      break;
    case 3:
      strcat(Str2, " glares at you but says nothing. ");
      break;
  }
  mprint(Str2);
}

void m_talk_stupid(struct monster *m)
{
  int reply = random_range(4);
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  switch(reply)
  {
    case 0:
      strcat(Str2, " looks at you with mute incomprehension.");
      break;
    case 1:
      strcat(Str2, " growls menacingly and ignores you.");
      break;
    case 2:
      strcat(Str2, " does not seem to have heard you.");
      break;
    case 3:
      strcat(Str2, " tries to pretend it didn't hear you.");
      break;
  }
  mprint(Str2);
}

void m_talk_greedy(struct monster *m)
{
  int reply = random_range(4);
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  switch(reply)
  {
    case 0:
      strcat(Str2, " says: Give me a treasure.... ");
      break;
    case 1:
      strcat(Str2, " says: Stand and deliver, knave! ");
      break;
    case 2:
      strcat(Str2, " says: Your money or your life! ");
      break;
    case 3:
      strcat(Str2, " says: Yield or Die! ");
      break;
  }
  mprint(Str2);
}

void m_talk_hungry(struct monster *m)
{
  int reply = random_range(4);
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  switch(reply)
  {
    case 0:
      strcat(Str2, " says: I hunger, foolish adventurer! ");
      break;
    case 1:
      strcat(Str2, " drools menacingly at you. ");
      break;
    case 2:
      strcat(Str2, " says: You're invited to be lunch! ");
      break;
    case 3:
      strcat(Str2, " says: Feeed Meee! ");
      break;
  }
  mprint(Str2);
}

void m_talk_guard(struct monster *m)
{
  if(m_statusp(*m, HOSTILE))
  {
    print1("'Surrender in the name of the Law!'");
    print2("Do it? [yn] ");
    if(ynq2() == 'y')
    {
      Player.alignment++;
      if(Current_Environment == E_CITY)
      {
        print1("Go directly to jail. Do not pass go, do not collect 200Au.");
        print2("You are taken to the city gaol.");
        send_to_jail();
        drawvision(Player.x, Player.y);
      }
      else
      {
        print1("Mollified, the guard disarms you and sends you away.");
        dispose_lost_objects(1, Player.possessions[O_WEAPON_HAND]);
        pacify_guards();
      }
    }
    else
    {
      print1("All right, you criminal scum, you asked for it!");
    }
  }
  else if(!received_directions && Current_Environment == E_CITY)
  {
    print1("The guard offers to show you a map of Rampart.");
    print2("Look at the map? [yn]  ");
    if(ynq2() == 'y')
    {
      for(int i = 0; i < NUMCITYSITES; ++i)
      {
        int site = CITYSITEBASE + i;
        if(site != L_THIEVES_GUILD && site != L_ORACLE && site != L_BROTHEL)
        {
          CitySiteList[i][0] = true;
        }
      }
      print1("You feel more knowledgeable!");
    }
    else
    {
      print1("Next time you need help, you can ask someone else!");
    }
    received_directions = true;
  }
  else if(Player.rank[ORDER] > 0)
  {
    print1("'Greetings comrade! May you always tread the paths of Law.'");
  }
  else
  {
    print1("Move it right along, stranger!");
  }
}

void m_talk_mp(struct monster *)
{
  mprint("The mendicant priest asks you to spare some treasure for the needy");
}

void m_talk_titter(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  strcat(Str2, " titters obscenely at you.");
  mprint(Str2);
}

void m_talk_ninja(struct monster *)
{
  mprint("The black-garbed figure says apologetically:");
  mprint("'Situree simasita, wakarimasen.'");
}

void m_talk_thief(struct monster *m)
{
  if(Player.rank[THIEVES])
  {
    if(m->level == 2)
    {
      m->monstring = "sneak thief";
    }
    else
    {
      m->monstring = "master thief";
    }
    print1("The cloaked figure makes a gesture which you recognize...");
    print2("...the thieves' guild recognition signal!");
    print3("'Sorry, mate, thought you were a mark....'");
    m_vanish(m);
  }
  else
  {
    m_talk_man(m);
  }
}

void m_talk_assassin(struct monster *m)
{
  m->monstring = "master assassin";
  print1("The ominous figure does not reply, but hands you an embossed card:");
  print2("'Guild of Assassins Ops are forbidden to converse with targets.'");
}

void m_talk_im(struct monster *m)
{
  if(strcmp(m->monstring, "itinerant merchant") != 0)
  {
    m->monstring = "itinerant merchant";
  }
  if(m->possessions == NULL)
  {
    mprint("The merchant says: Alas! I have nothing to sell!");
  }
  else
  {
    m->possessions->thing->known = 2;
    long price                   = std::max(10l, 4 * true_item_value(m->possessions->thing));
    mprint("I have a fine " + std::string(itemid(m->possessions->thing)) + " for only " +
           std::to_string(price) + " Au. Want it? [yn] ");
    if(ynq() == 'y')
    {
      if(Player.cash < price)
      {
        if(Player.alignment > 10)
        {
          mprint("Well, I'll let you have it for what you've got.");
          Player.cash = 0;
          gain_item(m->possessions->thing);
          m->possessions = NULL;
        }
        else
        {
          mprint("Beat it, you deadbeat!");
        }
      }
      else
      {
        mprint("Here you are. Have a good day.");
        Player.cash -= price;
        gain_item(m->possessions->thing);
        m->possessions = NULL;
      }
    }
    else
    {
      mprint("Well then, I must be off. Good day.");
    }
    m_vanish(m);
  }
}

void m_talk_man(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  switch(random_range(5))
  {
    case 0:
      strcat(Str2, " asks you for the way home.");
      break;
    case 1:
      strcat(Str2, " wishes you a pleasant day.");
      break;
    case 2:
      strcat(Str2, " sneers at you contemptuously.");
      break;
    case 3:
      strcat(Str2, " smiles and nods.");
      break;
    case 4:
      strcat(Str2, " tells you a joke.");
      break;
  }
  mprint(Str2);
}

void m_talk_evil(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  switch(random_range(14))
  {
    case 0:
      strcat(Str2, " says: 'THERE CAN BE ONLY ONE!'");
      break;
    case 1:
      strcat(Str2, " says: 'Prepare to die, Buckwheat!'");
      break;
    case 2:
      strcat(Str2, " says: 'Time to die!'");
      break;
    case 3:
      strcat(Str2, " says: 'There will be no mercy.'");
      break;
    case 4:
      strcat(Str2, " insults your mother-in-law.");
      break;
    case 5:
      strcat(Str2, " says: 'Kurav tu ando mul!'");
      break;
    case 6:
      strcat(Str2, " says: '!va al infierno!'");
      break;
    case 7:
      strcat(Str2, " says: 'dame desu, nee.'");
      break;
    case 8:
      strcat(Str2, " spits on your rug and calls your cat a bastard.");
      break;
    case 9:
      strcat(Str2, " snickers malevolently and draws a weapon.");
      break;
    case 10:
      strcat(Str2, " sends 'rm -r *' to your shell!");
      break;
    case 11:
      strcat(Str2, " tweaks your nose and cackles evilly.");
      break;
    case 12:
      strcat(Str2, " thumbs you in the eyes.");
      break;
    case 13:
      strcat(Str2, " kicks you in the groin.");
      break;
  }
  mprint(Str2);
}

void m_talk_robot(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  switch(random_range(4))
  {
    case 0:
      strcat(Str2, " says: 'exterminate...Exterminate...EXTERMINATE!!!'");
      break;
    case 1:
      strcat(Str2, " says: 'Kill ... Crush ... Destroy'");
      break;
    case 2:
      strcat(Str2, " says: 'Danger -- Danger'");
      break;
    case 3:
      strcat(Str2, " says: 'Yo Mama -- core dumped.'");
      break;
  }
  mprint(Str2);
}

void m_talk_slithy(struct monster *)
{
  mprint("It can't talk -- it's too slithy!");
}

void m_talk_mimsy(struct monster *)
{
  mprint("It can't talk -- it's too mimsy!");
}

void m_talk_burble(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  strcat(Str2, " burbles hatefully at you.");
  mprint(Str2);
}

void m_talk_beg(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  strcat(Str2, " asks you for alms.");
  mprint(Str2);
}

void m_talk_hint(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  if(m_statusp(*m, HOSTILE))
  {
    strcat(Str2, " only sneers at you. ");
    mprint(Str2);
  }
  else
  {
    strcat(Str2, " whispers in your ear: ");
    mprint(Str2);
    hint();
    m->talkf = M_TALK_SILENT;
  }
}

void m_talk_gf(struct monster *m)
{
  mprint("The good fairy glints: Would you like a wish?");
  if(ynq() == 'y')
  {
    mprint("The good fairy glows: Are you sure?");
    if(ynq() == 'y')
    {
      mprint("The good fairy radiates: Really really sure?");
      if(ynq() == 'y')
      {
        mprint("The good fairy beams: I mean, like, sure as sure can be?");
        if(ynq() == 'y')
        {
          mprint("The good fairy dazzles: You don't want a wish, right?");
          if(ynq() == 'y')
          {
            mprint("The good fairy laughs: I thought not.");
          }
          else
          {
            wish(0);
          }
        }
      }
    }
  }
  mprint("In a flash of sweet-smelling light, the fairy vanishes....");
  Player.hp   = std::max(Player.hp, Player.maxhp);
  Player.mana = std::max(Player.mana, calcmana());
  mprint("You feel mellow.");
  m_vanish(m);
}

void m_talk_ef(struct monster *m)
{
  mprint("The evil fairy roils: Eat my pixie dust!");
  mprint("She waves her black-glowing wand, which screams thinly....");
  m->movef    = M_MOVE_SMART;
  m->meleef   = M_MELEE_POISON;
  m->specialf = M_SP_THIEF;
  acquire(-1);
  bless(-1);
  sleep_player(m->level / 2);
  summon(-1, -1);
  summon(-1, -1);
  summon(-1, -1);
  summon(-1, -1);
}

void m_talk_seductor(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  if(Player.preference == 'n')
  {
    strcat(Str2, " notices your disinterest and leaves with a pout.");
    mprint(Str2);
  }
  else
  {
    strcat(Str2, " beckons seductively...");
    mprint(Str2);
    mprint("Flee? [yn] ");
    if(ynq() == 'y')
    {
      mprint("You feel stupid.");
    }
    else
    {
      strcpy(Str2, "The ");
      strcat(Str2, m->monstring);
      strcat(Str2, " shows you a good time....");
      mprint(Str2);
      gain_experience(500);
      Player.con++;
    }
  }
  m_vanish(m);
}

void m_talk_demonlover(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  if(Player.preference == 'n')
  {
    strcat(Str2, " notices your disinterest and changes with a snarl...");
    mprint(Str2);
  }
  else
  {
    strcat(Str2, " beckons seductively...");
    mprint(Str2);
    mprint("Flee? [yn] ");
    if(ynq() == 'y')
    {
      mprint("You feel fortunate....");
    }
    else
    {
      if(m->uniqueness == COMMON)
      {
        strcpy(Str2, "The ");
        strcat(Str2, m->monstring);
      }
      else
      {
        strcpy(Str2, m->monstring);
      }
      strcat(Str2, " shows you a good time....");
      mprint(Str2);
      mprint("You feel your life energies draining...");
      level_drain(random_range(3) + 1, "a demon's kiss");
    }
  }
  m->talkf    = M_TALK_EVIL;
  m->meleef   = M_MELEE_SPIRIT;
  m->specialf = M_SP_DEMON;

  if((m->monchar & 0xff) == 's')
  {
    m->monchar   = 'I' | CLR(RED);
    m->monstring = "incubus";
  }
  else
  {
    m->monchar   = 'S' | CLR(RED);
    m->monstring = "succubus";
  }
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  strcat(Str2, " laughs insanely.");
  mprint(Str2);
  mprint("You now notice the fangs, claws, batwings...");
}

void m_talk_horse(struct monster *m)
{
  if(m_statusp(*m, HOSTILE))
  {
    mprint("The horse neighs angrily at you.");
  }
  else if(m_statusp(*m, HUNGRY))
  {
    mprint("The horse noses curiously at your pack.");
  }
  else if(gamestatusp(MOUNTED, GameStatus))
  {
    mprint("The horse and your steed don't seem to get along.");
  }
  else if(Current_Environment == Current_Dungeon)
  {
    mprint("The horse shies; maybe he doesn't like the dungeon air....");
  }
  else
  {
    mprint("The horse lets you pat his nose. Want to ride him? [yn] ");
    if(ynq() == 'y')
    {
      m->hp                            = -1;
      Level->site[m->x][m->y].creature = NULL;
      putspot(m->x, m->y, getspot(m->x, m->y, false));
      setgamestatus(MOUNTED, GameStatus);
      calc_melee();
      mprint("You are now equitating!");
    }
  }
}

void m_talk_hyena(struct monster *)
{
  mprint("The hyena only laughs at you...");
}

void m_talk_parrot(struct monster *)
{
  mprint("Polly wanna cracker?");
}

void m_talk_servant(struct monster *m)
{
  int target, x = Player.x, y = Player.y;
  if(m->id == SERV_LAW)
  {
    target = SERV_CHAOS;
    mprint("The Servant of Law pauses in thought for a moment.");
    mprint("You are asked: Are there any Servants of Chaos hereabouts? [yn] ");
  }
  else
  {
    target = SERV_LAW;
    mprint("The Servant of Chaos grins mischievously at you.");
    mprint("You are asked: Are there any Servants of Law hereabouts? [yn] ");
  }
  if(ynq() == 'y')
  {
    print1("Show me.");
    show_screen();
    drawmonsters(true);
    setspot(&x, &y);
    if(Level->site[x][y].creature != NULL)
    {
      if(Level->site[x][y].creature->id == target)
      {
        mprint("The Servant launches itself towards its enemy.");
        mprint("In a blaze of combat, the Servants annihilate each other!");
        gain_experience(m->xpv);
        m_death(Level->site[x][y].creature);
        Level->site[m->x][m->y].creature = NULL;
        m->x                             = x;
        m->y                             = y;
        Level->site[x][y].creature       = m;
        m_death(Level->site[x][y].creature);
      }
      else
      {
        mprint("Right. Tell me about it. Idiot!");
      }
    }
    else
    {
      mprint("Right. Tell me about it. Idiot!");
    }
  }
  else
  {
    mprint("The servant shrugs and turns away.");
  }
}

void m_talk_animal(struct monster *m)
{
  if(m->uniqueness == COMMON)
  {
    strcpy(Str2, "The ");
    strcat(Str2, m->monstring);
  }
  else
  {
    strcpy(Str2, m->monstring);
  }
  mprint(Str2);
  mprint("shows you a scholarly paper by Dolittle, D. Vet.");
  mprint("which demonstrates that animals don't have speech centers");
  mprint("complex enough to communicate in higher languages.");
  mprint("It giggles softly to itself and takes back the paper.");
}

void m_talk_scream(struct monster *m)
{
  mprint("A thinly echoing scream reaches your ears....");
  mprint("You feel doomed....");
  mprint("A bird appears and flies three times widdershins around your head.");
  summon(-1, QUAIL);
  m->talkf = M_TALK_EVIL;
}

void m_talk_archmage(struct monster *m)
{
  if(m_statusp(*m, HOSTILE))
  {
    mprint("The Archmage ignores your attempt at conversation");
    mprint("and concentrates on his spellcasting....");
  }
  else if(Current_Environment == E_COURT)
  {
    mprint("The Archmage congratulates you on getting this far.");
    mprint("He invites you to attempt the Throne of High Magic");
    mprint("but warns you that it is important to wield the Sceptre");
    mprint("before sitting on the throne.");
    if(Level->site[m->x][m->y].p_locf == L_THRONE)
    {
      mprint("The Archmage smiles and makes an arcane gesture....");
      m_vanish(m);
    }
  }
  else
  {
    mprint("The Archmage tells you to find him again in his");
    mprint("Magical Court at the base of his castle in the mountains");
    mprint("of the far North-East; if you do he will give you some");
    mprint("important information.");
  }
}

void m_talk_merchant(struct monster *m)
{
  if(!m_statusp(*m, HOSTILE))
  {
    if(Current_Environment == E_VILLAGE)
    {
      mprint("The merchant asks you if you want to buy a horse for 250GP.");
      mprint("Pay the merchant? [yn] ");
      if(ynq() == 'y')
      {
        if(Player.cash < 250)
        {
          mprint("The merchant says: 'Come back when you've got the cash!'");
        }
        else
        {
          Player.cash -= 250;
          mprint("The merchant takes your money and tells you to select");
          mprint("any horse you want in the stables.");
          mprint("He says: 'You'll want to get to know him before trying to");
          mprint("ride him. By the way, food for the horse is not included.'");
          mprint("The merchant runs off toward the bank, cackling gleefully.");
          m_vanish(m);
        }
      }
      else
      {
        mprint("The merchant tells you to stop wasting his time.");
      }
    }
    else
    {
      mprint("The merchant tells you to visit his stables at his village");
      mprint("for a great deal on a horse.");
    }
  }
  else
  {
    mprint("The merchant ignores you and screams:");
    mprint("'Help! Help! I'm being oppressed!'");
  }
}

void m_talk_prime(struct monster *m)
{
  if(!m_statusp(*m, HOSTILE))
  {
    if(Current_Environment == E_CIRCLE)
    {
      print1("The Prime nods brusquely at you, removes a gem from his");
      print2("sleeve, places it on the floor, and vanishes wordlessly.");
      m_dropstuff(m);
      m_vanish(m);
    }
    else
    {
      print1("The Prime makes an intricate gesture, which leaves behind");
      print2("glowing blue sparks... He winks mischievously at you....");
      if(Player.rank[CIRCLE] > 0)
      {
        print1("The blue sparks strike you! You feel enhanced!");
        print2("You feel more experienced....");
        Player.pow += Player.rank[CIRCLE];
        Player.mana += calcmana();
        gain_experience(1000);
        m_vanish(m);
      }
    }
  }
  else
  {
    m_talk_evil(m);
  }
}
