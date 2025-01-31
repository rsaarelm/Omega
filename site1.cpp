/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* site1.c */
/* 1st half of site functions and aux functions to them */

#include "glob.h"
#include "interactive_menu.hpp"
#include "scrolling_buffer.hpp"

#include <algorithm>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

extern void item_unequip(object *);

extern void           print_messages();
extern void           queue_message(const std::string &message);
extern void           append_message(const std::string &message, bool force_break = false);
extern scrolling_buffer message_buffer;
extern interactive_menu *menu;

/* the bank; can be broken into (!) */
void l_bank()
{
  int  done = false, valid = false;
  long amount;
  char response;
  std::string passwd;
  queue_message("First Bank of Omega: Autoteller Carrel.");

  if(gamestatusp(BANK_BROKEN, GameStatus))
  {
    append_message("You see a damaged autoteller.", true);
  }
  else
  {
    append_message("The proximity sensor activates the autoteller as you approach.", true);
    while(!done)
    {
      append_message("Current Balance: " + std::to_string(Balance) + "Au.", true);
      append_message("Enter command (? for help) > ", true);
      response = mgetc();
      if(response == '?')
      {
        std::vector<std::string> lines;
        lines.emplace_back("?: This List");
        if(strcmp(Password, "") == 0)
        {
          lines.emplace_back("O: Open an account");
        }
        else
        {
          lines.emplace_back("P: Enter password");
          lines.emplace_back("D: Deposit");
          lines.emplace_back("W: Withdraw");
        }
        lines.emplace_back("X: eXit");
        menu->load(lines);
        menu->print();
        continue;
      }
      else if((response == 'P') && (strcmp(Password, "") != 0))
      {
        append_message("Password: ", true);
        passwd = msgscanstring();
        valid = (passwd == Password);
        if(!valid)
        {
          done = true;
          std::vector<std::string> lines =
          {
            {"Alert! Alert! Invalid Password!"},
            {"The police are being summoned!"},
            {"Please wait for the police to arrive...."},
            {""},
            {"----Hit space bar to continue----"}
          };
          menu->load(lines);
          response = menu->get_player_input();
          if(response == ' ')
          {
            Player.alignment += 5;
            xredraw();
            append_message("Ah ha! Trying to rob the bank, eh?", true);
            append_message("Take him away, boys!");
            send_to_jail();
          }
          else
          {
            Player.alignment -= 5;
            lines.clear();
            menu->load(lines);
            menu->print();
            std::this_thread::sleep_for(std::chrono::seconds(4));
            lines.emplace_back("^@^@^@^@^@00AD1203BC0F0000FFFFFFFFFFFF");
            lines.emplace_back("Interrupt in _get_space. Illegal Character.");
            menu->load(lines);
            menu->print();
            std::this_thread::sleep_for(std::chrono::seconds(4));
            lines.emplace_back("Aborting _police_alert.....");
            lines.emplace_back("Attempting reboot.....");
            menu->load(lines);
            menu->print();
            std::this_thread::sleep_for(std::chrono::seconds(4));
            lines.emplace_back("Warning: Illegal shmop at _count_cash.");
            lines.emplace_back("Warning: Command Buffer NOT CLEARED");
            menu->load(lines);
            menu->print();
            std::this_thread::sleep_for(std::chrono::seconds(4));
            lines.emplace_back("Reboot Complete. Execution Continuing.");
            lines.emplace_back("Withdrawing: 4294967297 Au.");
            lines.emplace_back("Warning: Arithmetic Overflow in _withdraw");
            menu->load(lines);
            menu->print();
            std::this_thread::sleep_for(std::chrono::seconds(4));
            lines.emplace_back("Yo mama. Core dumped.");
            menu->load(lines);
            menu->print();
            std::this_thread::sleep_for(std::chrono::seconds(4));
            xredraw();
            print1("The cash machine begins to spew gold pieces!");
            print2("You pick up your entire balance and then some!");
            Player.cash += Balance + 1000 + random_range(3000);
            Balance = 0;
            setgamestatus(BANK_BROKEN, GameStatus);
          }
        }
        else
        {
          append_message("Password accepted. Working.", true);
        }
      }
      else if((response == 'D') && valid)
      {
        append_message("Amount: ", true);
        amount = get_money(Player.cash);
        if(amount < 1)
        {
          append_message("Transaction aborted.", true);
        }
        else if(amount > Player.cash)
        {
          append_message("Deposit too large -- transaction aborted.", true);
        }
        else
        {
          append_message("Transaction accomplished.", true);
          Balance += amount;
          Player.cash -= amount;
        }
      }
      else if((response == 'W') && valid)
      {
        append_message("Amount: ", true);
        amount = get_money(Balance);
        if(amount < 1)
        {
          append_message("Transaction aborted.", true);
        }
        else if(amount > Balance)
        {
          append_message("Withdrawal too large -- transaction aborted.", true);
        }
        else
        {
          append_message("Transaction accomplished.", true);
          Balance -= amount;
          Player.cash += amount;
        }
      }
      else if(response == 'X')
      {
        append_message("Bye!", true);
        done = true;
      }
      else if((response == 'O') && (strcmp(Password, "") == 0))
      {
        append_message("Opening new account.", true);
        append_message(" Please enter new password: ");
        strcpy(Password, msgscanstring().c_str());
        if(strcmp(Password, "") == 0)
        {
          append_message("Illegal to use null password -- aborted.", true);
          done = true;
        }
        else
        {
          append_message("Password validated; account saved.", true);
          valid = true;
        }
      }
      else
      {
        append_message("Illegal command.", true);
      }
      dataprint();
    }
  }
  xredraw();
}

void l_armorer()
{
  int done = false;
  if(hour() == 12)
  {
    queue_message("Unfortunately, this is Julie's lunch hour -- try again later.");
  }
  else if(nighttime())
  {
    queue_message("It seems that Julie keeps regular business hours.");
  }
  else
  {
    while(!done)
    {
      queue_message("Julie's: Buy Armor, Weapons, or Leave [a,w,ESCAPE] ");
      int player_input = mgetc();
      if(player_input == ESCAPE)
      {
        done = true;
      }
      else if(player_input == 'a')
      {
        buyfromstock(ARMORID, 10);
      }
      else if(player_input == 'w')
      {
        buyfromstock(WEAPONID, 23);
      }
    }
  }
  xredraw();
}

void buyfromstock(int base, int numitems)
{
  pob newitem;

  append_message("Purchase which item? [ESCAPE to quit] ", true);
  std::vector<std::string> lines;
  for(int i = 0; i < numitems; ++i)
  {
    lines.emplace_back(std::string(1, i + 'a') + ":" + Objects[base + i].objstr);
  }
  menu->load(lines);
  int player_input = ' ';
  while(player_input != ESCAPE && (player_input < 'a' || player_input >= 'a' + numitems))
  {
    player_input = menu->get_player_input();
  }
  if(player_input != ESCAPE)
  {
    int i          = player_input - 'a';
    newitem        = ((pob)checkmalloc(sizeof(objtype)));
    *newitem       = Objects[base + i];
    newitem->known = 2;
    long price     = 2 * true_item_value(newitem);
    append_message("I can let you have it for " + std::to_string(price) + " Au. Buy it? [yn] ", true);
    if(ynq1() == 'y')
    {
      if(Player.cash < price)
      {
        append_message("Why not try again some time you have the cash?", true);
        free((char *)newitem);
      }
      else
      {
        Player.cash -= price;
        dataprint();
        gain_item(newitem);
      }
    }
    else
    {
      free((char *)newitem);
    }
  }
}

void l_club()
{
#define hinthour club_hinthour
  char response;

  queue_message("Rampart Explorers' Club.");
  if(!gamestatusp(CLUB_MEMBER, GameStatus))
  {
    if(Player.level < 2)
    {
      append_message("Only reknowned adventurers need apply.", true);
    }
    else
    {
      append_message("Dues are 100Au. Pay it? [yn] ", true);
      if(ynq2() == 'y')
      {
        if(Player.cash < 100)
        {
          append_message("Beat it, or we'll blackball you!", true);
        }
        else
        {
          append_message("Welcome to the club! You are taught the spell of Return.", true);
          append_message("When cast on the first level of a dungeon it", true);
          append_message("will transport you down to the lowest level");
          append_message("you have explored, and vice versa.");
          Spells[S_RETURN].known = true;
          Player.cash -= 100;
          setgamestatus(CLUB_MEMBER, GameStatus);
        }
      }
      else
      {
        append_message("OK, but you're missing out on our benefits....", true);
      }
    }
  }
  else
  {
    append_message("Shop at the club store or listen for rumors [sl] ", true);
    do
    {
      response = (char)mcigetc();
    } while((response != 's') && (response != 'l') && (response != ESCAPE));
    if(response == 'l')
    {
      if(hinthour == hour())
      {
        append_message("You don't hear anything useful.", true);
      }
      else
      {
        append_message("You overhear a conversation....", true);
        hint();
        hinthour = hour();
      }
    }
    else if(response == 's')
    {
      buyfromstock(THINGID + 7, 2);
      xredraw();
    }
    else if(response == ESCAPE)
    {
      append_message("Be seeing you, old chap!", true);
    }
  }
}
#undef hinthour

void l_gym()
{
  int done    = true;
  int trained = 0;
  print_messages();
  std::vector<std::string> lines =
  {
    {"The Rampart Gymnasium"},
    {""},
    {""},
    {"Train for 2000 Au. Choose:"},
    {""},
    {"a: work out in the weight room"},
    {"b: use our gymnastics equipment"},
    {"c: take our new anaerobics course"},
    {"d: enroll in dance lessons"},
    {"ESCAPE: Leave this place"}
  };
  menu->load(lines);
  menu->print();
  append_message("The Rampart Gymnasium", true);
  do
  {
    if((Gymcredit > 0) || (Player.rank[ARENA]))
    {
      append_message("-- Credit: " + std::to_string(Gymcredit) + " Au.");
    }
    done = false;
    switch(mgetc())
    {
      case 'a':
        gymtrain(&(Player.maxstr), &(Player.str));
        break;
      case 'b':
        gymtrain(&(Player.maxdex), &(Player.dex));
        break;
      case 'c':
        gymtrain(&(Player.maxcon), &(Player.con));
        break;
      case 'd':
        gymtrain(&(Player.maxagi), &(Player.agi));
        break;
      case ESCAPE:
        if(trained == 0)
        {
          append_message("Well, it's your body you're depriving!", true);
        }
        else if(trained < 3)
        {
          append_message("You towel yourself off, and find the exit.", true);
        }
        else
        {
          append_message("A refreshing bath, and you're on your way.", true);
        }
        done = true;
        break;
      default:
        trained--;
        break;
    }
    trained++;
  } while(!done);
  xredraw();
  calc_melee();
}

void l_healer()
{
  std::vector<std::string> lines;
  lines.emplace_back("     Rampart Healers. Member RMA.");
  lines.emplace_back("a: Heal injuries (50 crowns).");
  lines.emplace_back("b: Cure disease (250 crowns).");
  lines.emplace_back("ESCAPE: Leave these antiseptic alcoves.");
  menu->load(lines);
  switch(menu->get_player_input())
  {
    case 'a':
      healforpay();
      break;
    case 'b':
      cureforpay();
      break;
    default:
      append_message("OK, but suppose you have Acute Satyriasis?", true);
      break;
  }
}

void statue_random(int x, int y)
{
  pob item;
  int i, j;
  switch(random_range(difficulty() + 3) - 1)
  {
    default:
      l_statue_wake();
      break;
    case 0:
      print1("The statue crumbles with a clatter of gravel.");
      Level->site[x][y].locchar = RUBBLE;
      Level->site[x][y].p_locf  = L_RUBBLE;
      plotspot(x, y, true);
      lset(x, y, CHANGED, *Level);
      break;
    case 1:
      print1("The statue stoutly resists your attack.");
      break;
    case 2:
      print1("The statue crumbles with a clatter of gravel.");
      Level->site[x][y].locchar = RUBBLE;
      Level->site[x][y].p_locf  = L_RUBBLE;
      plotspot(x, y, true);
      lset(x, y, CHANGED, *Level);
      make_site_treasure(x, y, difficulty());
      break;
    case 3:
      print1("The statue hits you back!");
      p_damage(random_range(difficulty() * 5), UNSTOPPABLE, "a statue");
      break;
    case 4:
      print1("The statue looks slightly pained. It speaks:");
      hint();
      break;
    case 5:
      if((Current_Environment == Current_Dungeon) || (Current_Environment == E_CITY))
      {
        print1("You hear the whirr of some mechanism.");
        print2("The statue glides smoothly into the floor!");
        /* WDT HACK: I shouldn't be making this choice on a level
       * where no stairs can be (or perhaps I should, and I should
       * implement a bonus level!). */
        Level->site[x][y].locchar = STAIRS_DOWN;
        Level->site[x][y].p_locf  = L_NO_OP;
        lset(x, y, CHANGED | STOPS, *Level);
      }
      break;
    case 6:
      print1("The statue was covered with contact cement!");
      print2("You can't move....");
      Player.status[IMMOBILE] += random_range(6) + 2;
      break;
    case 7:
      print1("A strange radiation emanates from the statue!");
      dispel(-1);
      break;
    case 8: /* I think this is particularly evil. Heh heh. */
      if(Player.possessions[O_WEAPON_HAND] != NULL)
      {
        print1("Your weapon sinks deeply into the statue and is sucked away!");
        item = Player.possessions[O_WEAPON_HAND];
        conform_lost_object(Player.possessions[O_WEAPON_HAND]);
        item->blessing = -1 - abs(item->blessing);
        drop_at(x, y, item);
      }
      break;
    case 9:
      print1("The statue extends an arm. Beams of light illuminate the level!");
      for(i = 0; i < WIDTH; i++)
      {
        for(j = 0; j < LENGTH; j++)
        {
          lset(i, j, SEEN, *Level);
          if(loc_statusp(i, j, SECRET, *Level))
          {
            lreset(i, j, SECRET, *Level);
            lset(i, j, CHANGED, *Level);
          }
        }
      }
      show_screen();
      break;
  }
}

void l_statue_wake()
{
  int i;
  int x = Player.x, y = Player.y;
  for(i = 0; i < 9; i++)
  {
    wake_statue(x + Dirs[0][i], y + Dirs[1][i], true);
  }
}

void wake_statue(int x, int y, int first)
{
  int i;
  pml tml;
  if(Level->site[x][y].locchar == STATUE)
  {
    if(!first)
    {
      mprint("Another statue awakens!");
    }
    else
    {
      mprint("A statue springs to life!");
    }
    Level->site[x][y].locchar = FLOOR;
    lset(x, y, CHANGED, *Level);
    tml    = ((pml)checkmalloc(sizeof(mltype)));
    tml->m = (Level->site[x][y].creature = m_create(x, y, 0, difficulty() + 1));
    m_status_set(*Level->site[x][y].creature, HOSTILE);
    tml->next    = Level->mlist;
    Level->mlist = tml;
    for(i = 0; i < 8; i++)
    {
      wake_statue(x + Dirs[0][i], y + Dirs[1][i], false);
    }
  }
}

void l_casino()
{
  queue_message("Rampart Mithril Nugget Casino.");
  if(random_range(10) == 1)
  {
    append_message("Casino closed due to Grand Jury investigation.", true);
  }
  else
  {
    std::vector<std::string> lines;
    lines.emplace_back("     Rampart Mithril Nugget Casino.");
    lines.emplace_back("a: Drop 100Au in the slots.");
    lines.emplace_back("b: Risk 1000Au at roulette.");
    lines.emplace_back("ESCAPE: Leave this green baize hall.");
    menu->load(lines);
    for(bool done = false; !done;)
    {
      switch(menu->get_player_input())
      {
        case 'a':
          if(Player.cash < 100)
          {
            append_message("No credit, jerk.", true);
          }
          else
          {
            Player.cash -= 100;
            dataprint();
            print_messages();
            const std::string REEL_VALUES[] = {
              "<Slime Mold>", "<Lemon>", "<Copper>", "<Nymph>", "<Sword>",
              "<Shield>",     "<Chest>", "<Bar>",    "<Orb>",   "<Mithril Nugget>",
            };
            append_message("", true);
            int a, b, c;
            for(int i = 0; i < 20; ++i)
            {
              if(i == 19)
              {
                std::this_thread::sleep_for(std::chrono::seconds(1));
              }
              else
              {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
              }
              a = random_range(10);
              b = random_range(10);
              c = random_range(10);
              message_buffer.replace_last(REEL_VALUES[a] + " " + REEL_VALUES[b] + " " + REEL_VALUES[c]);
              print_messages();
            }
            if(winnings > 0)
            {
              do
              {
                a = random_range(10);
                b = random_range(10);
                c = random_range(10);
              } while((a == b) || (a == c) || (b == c));
            }
            else
            {
              a = random_range(10);
              b = random_range(10);
              c = random_range(10);
            }
            message_buffer.replace_last(REEL_VALUES[a] + " " + REEL_VALUES[b] + " " + REEL_VALUES[c]);
            print_messages();
            if((a == b) && (a == c))
            {
              append_message("Jackpot Winner!", true);
              long reward = (a + 2) * (b + 2) * (c + 2) * 5;
              winnings += reward;
              Player.cash += reward;
              dataprint();
            }
            else if(a == b)
            {
              append_message("Winner!", true);
              long reward = (a + 2) * (b + 2) * 5;
              winnings += reward;
              Player.cash += reward;
              dataprint();
            }
            else if(a == c)
            {
              append_message("Winner!", true);
              long reward = (a + 2) * (c + 2) * 5;
              winnings += reward;
              Player.cash += reward;
              dataprint();
            }
            else if(c == b)
            {
              append_message("Winner!", true);
              long reward = (c + 2) * (b + 2) * 5;
              winnings += reward;
              Player.cash += reward;
              dataprint();
            }
            else
            {
              append_message("Loser!", true);
              winnings -= 100;
            }
          }
          break;
        case 'b':
          if(Player.cash < 1000)
          {
            append_message("No credit, jerk.", true);
          }
          else
          {
            Player.cash -= 1000;
            dataprint();
            append_message("Red or Black? [rb] ", true);
            int response;
            do
            {
              response = static_cast<char>(mcigetc());
            } while((response != 'r') && (response != 'b'));
            int match = (response == 'r' ? 0 : 1);
            append_message("", true);
            int a, b;
            for(int i = 0; i < 20; ++i)
            {
              if(i == 19)
              {
                std::this_thread::sleep_for(std::chrono::seconds(1));
              }
              else
              {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
              }
              a = random_range(37);
              b = a % 2;
              if(a == 0)
              {
                message_buffer.replace_last("0 ");
              }
              else if(a == 1)
              {
                message_buffer.replace_last("0 - 0 ");
              }
              else
              {
                message_buffer.replace_last((b == 0 ? "Red " : "Black ") + std::to_string(a - 1));
              }
              print_messages();
            }
            if(winnings > 0)
            {
              do
              {
                a = random_range(37);
                b = a % 2;
              } while(b == match);
            }
            else
            {
              a = random_range(37);
              b = a % 2;
            }
            if(a == 0)
            {
              message_buffer.replace_last("0 ");
            }
            else if(a == 1)
            {
              message_buffer.replace_last("0 - 0 ");
            }
            else
            {
              message_buffer.replace_last((b == 0 ? "Red " : "Black ") + std::to_string(a - 1));
            }
            print_messages();
            if((a > 1) && (b == match))
            {
              append_message("Winner!", true);
              winnings += 1000;
              Player.cash += 2000;
              dataprint();
            }
            else
            {
              append_message("Loser!", true);
              winnings -= 1000;
              dataprint();
            }
          }
          break;
        case ESCAPE:
          done = true;
          break;
      }
    }
  }
}

void l_commandant()
{
  int num;
  pob food;
  queue_message("Commandant Sonder's Rampart-fried Lyzzard partes. Open 24 hrs.");
  append_message("Buy a bucket! Only 5 Au. Make a purchase? [yn] ", true);
  if(ynq2() == 'y')
  {
    append_message("How many? ", true);
    num = (int)parsenum();
    if(num < 1)
    {
      print3("Cute. Real cute.");
    }
    else if(num * 5 > Player.cash)
    {
      print3("No handouts here, mac!");
    }
    else
    {
      Player.cash -= num * 5;
      food         = ((pob)checkmalloc(sizeof(objtype)));
      *food        = Objects[FOODID + 0]; /* food ration */
      food->number = num;
      if(num == 1)
      {
        append_message("There you go, mac! One Lyzzard Bucket, coming up.", true);
      }
      else
      {
        append_message("A passel of Lyzzard Buckets, for your pleasure.", true);
      }
      gain_item(food);
    }
  }
  else
  {
    append_message("Don't blame the Commandant if you starve!", true);
  }
}

void l_diner()
{
  print1("The Rampart Diner. All you can eat, 25Au.");
  print2("Place an order? [yn] ");
  if(ynq2() == 'y')
  {
    if(Player.cash < 25)
    {
      mprint("TANSTAAFL! Now git!");
    }
    else
    {
      Player.cash -= 25;
      dataprint();
      Player.food = 44;
      foodcheck();
    }
  }
}

void l_crap()
{
  print1("Les Crapeuleaux. (****) ");
  if((hour() < 17) || (hour() > 23))
  {
    print2("So sorry, we are closed 'til the morrow...");
  }
  else
  {
    print2("May I take your order? [yn] ");
    if(ynq2() == 'y')
    {
      if(Player.cash < 1000)
      {
        print2("So sorry, you have not the funds for dinner.");
      }
      else
      {
        print2("Hope you enjoyed your tres expensive meal, m'sieur...");
        Player.cash -= 1000;
        dataprint();
        Player.food += 8;
        foodcheck();
      }
    }
  }
}

void l_tavern()
{
#define hinthour tavern_hinthour
  char response;
  print1("The Centaur and Nymph -- J. Riley, prop.");
  if(nighttime())
  {
    std::vector<std::string> lines =
    {
      {"Riley says: Whataya have?"},
      {""},
      {"a: Pint of Riley's ultra-dark 1Au"},
      {"b: Shot of Tullimore Dew 10Au"},
      {"c: Round for the House. 100Au"},
      {"d: Bed and Breakfast. 25Au"},
      {"ESCAPE: Leave this comfortable haven."}
    };
    menu->load(lines);
    menu->print();
    do
    {
      response = (char)mcigetc();
    } while((response != 'a') && (response != 'b') && (response != 'c') && (response != 'd') &&
            (response != ESCAPE));
    switch(response)
    {
      case 'a':
        if(Player.cash < 1)
        {
          print2("Aw hell, have one on me.");
        }
        else
        {
          Player.cash -= 1;
          dataprint();
          if(hinthour != hour())
          {
            if(random_range(3))
            {
              print1("You overhear a rumor...");
              hint();
            }
            else
            {
              print1("You don't hear much of interest.");
            }
            hinthour = hour();
          }
          else
          {
            print1("You just hear the same conversations again.");
          }
        }
        break;
      case 'b':
        if(Player.cash < 10)
        {
          print2("I don't serve the Dew on no tab, buddy!");
        }
        else
        {
          Player.cash -= 10;
          print1("Ahhhhh....");
          if(Player.status[POISONED] || Player.status[DISEASED])
          {
            print2("Phew! That's, er, smooth stuff!");
          }
          Player.status[POISONED] = 0;
          Player.status[DISEASED] = 0;
          showflags();
        }
        break;
      case 'c':
        if(Player.cash < 100)
        {
          print1("Whatta feeb!");
          print2("Outta my establishment.... Now!");
          p_damage(random_range(20), UNSTOPPABLE, "Riley's right cross");
        }
        else
        {
          Player.cash -= 100;
          dataprint();
          print1("'What a guy!'");
          print2("'Hey, thanks, fella.'");
          print3("'Make mine a double...'");
          switch(random_range(4))
          {
            case 0:
              print1("'You're a real pal. Say, have you heard.... ");
              hint();
              break;
            case 1:
              print1("A wandering priest of Dionysus blesses you...");
              if((Player.patron == ODIN) || (Player.patron == ATHENA))
              {
                Player.alignment++;
              }
              else if((Player.patron == HECATE) || (Player.patron == SET))
              {
                Player.alignment--;
              }
              else if(Player.alignment > 0)
              {
                Player.alignment--;
              }
              else
              {
                Player.alignment++;
              }
              break;
            case 2:
              print1("A thirsty bard promises to put your name in a song!");
              gain_experience(20);
              break;
            case 3:
              print1("Riley draws you a shot of his 'special reserve'");
              print2("Drink it [yn]?");
              if(ynq2() == 'y')
              {
                if(Player.con < random_range(20))
                {
                  print1("<cough> Quite a kick!");
                  print2("You feel a fiery warmth in your tummy....");
                  Player.con++;
                  Player.maxcon++;
                }
                else
                {
                  print2("You toss it back nonchalantly.");
                }
              }
          }
        }
        break;
      case 'd':
        if(Player.cash < 25)
        {
          print2("Pay in advance, mac!");
        }
        else
        {
          Player.cash -= 25;
          print2("How about a shot o' the dew for a nightcap?");
          Time += (6 + random_range(4)) * 60;
          Player.status[POISONED] = 0;
          Player.status[DISEASED] = 0;
          Player.food             = 40;
          /* reduce temporary stat gains to max stat levels */
          toggle_item_use(true);
          Player.str = std::min(Player.str, Player.maxstr);
          Player.con = std::min(Player.con, Player.maxcon);
          Player.agi = std::min(Player.agi, Player.maxagi);
          Player.dex = std::min(Player.dex, Player.maxdex);
          Player.iq  = std::min(Player.iq, Player.maxiq);
          Player.pow = std::min(Player.pow, Player.maxpow);
          toggle_item_use(false);
          timeprint();
          dataprint();
          showflags();
          print1("The next day.....");
          if(hour() > 10)
          {
            print2("Oh my! You overslept!");
          }
        }
        break;
      default:
        print2("So? Just looking? Go on!");
        break;
    }
  }
  else
  {
    print2("The pub don't open til dark, fella.");
  }
  xredraw();
}
#undef hinthour

void l_alchemist()
{
  queue_message("Ambrosias' Potions et cie.");
  if(nighttime())
  {
    queue_message("Ambrosias doesn't seem to be in right now.");
  }
  else
  {
    std::vector<std::string> lines;
    lines.emplace_back("     Ambrosias' Potions et cie.");
    lines.emplace_back("a: Sell monster components.");
    lines.emplace_back("b: Pay for transformation.");
    lines.emplace_back("ESCAPE: Leave this place.");
    menu->load(lines);
    for(bool done = false; !done;)
    {
      int i;
      switch(menu->get_player_input())
      {
        case 'a':
          done = true;
          i    = getitem(CORPSE);
          if(i != ABORT && Player.possessions[i])
          {
            pob obj = Player.possessions[i];
            if(Monsters[obj->charge].transformid == -1)
            {
              queue_message("I don't want such a thing.");
              if(obj->basevalue > 0)
              {
                queue_message("You might be able to sell it to someone else, though.");
              }
            }
            else
            {
              long sell_price = obj->basevalue / 3;
              queue_message("I'll give you " + std::to_string(sell_price) + "Au each. Take it? [yn] ");
              if(ynq1() == 'y')
              {
                int n = getnumber(obj->number);
                Player.cash += sell_price * n;
                conform_lost_objects(n, obj);
              }
              else
              {
                queue_message("Well, keep the smelly old thing, then!");
              }
            }
          }
          else
          {
            queue_message("So nu?");
          }
          break;
        case 'b':
          done = true;
          i    = getitem(CORPSE);
          if(i != ABORT && Player.possessions[i])
          {
            pob obj = Player.possessions[i];
            if(Monsters[obj->charge].transformid == -1)
            {
              queue_message("Oy vey! You want me to transform such a thing?");
            }
            else if(obj->number > 1 && Objects[Monsters[obj->charge].transformid].objchar == STICK)
            {
              queue_message("I can only work with one of these at a time.");
            }
            else
            {
              int mlevel = Monsters[obj->charge].level;
              long transform_price = std::max(10l, obj->basevalue * 2 * obj->number);
              queue_message("It'll cost you " + std::to_string(transform_price) +
                               " Au for the transformation. Pay it? [yn] ");
              if(ynq1() == 'y')
              {
                if(Player.cash < transform_price)
                {
                  queue_message("You can't afford it!");
                }
                else
                {
                  queue_message("Voila! A tap of the Philosopher's Stone...");
                  Player.cash -= transform_price;
                  int n       = obj->number;
                  *obj        = Objects[Monsters[obj->charge].transformid];
                  obj->number = n;
                  if((obj->id >= STICKID) && (obj->id < STICKID + NUMSTICKS))
                  {
                    obj->charge = 20;
                  }
                  if(obj->plus == 0)
                  {
                    obj->plus = mlevel;
                  }
                  if(obj->blessing == 0)
                  {
                    obj->blessing = 1;
                  }
                }
              }
              else
              {
                queue_message("I don't need your business, anyhow.");
              }
            }
          }
          else
          {
            queue_message("So nu?");
          }
          break;
        case ESCAPE:
          done = true;
          break;
        default:
          break;
      }
    }
  }
}

void l_dpw()
{
  queue_message("Rampart Department of Public Works.");
  if(Date - LastDay < 7)
  {
    append_message("G'wan! Get a job!", true);
  }
  else if(Player.cash < 100)
  {
    append_message("Do you want to go on the dole? [yn] ", true);
    if(ynq2() == 'y')
    {
      append_message("Well, ok, but spend it wisely.", true);
      append_message("Please enter your name for our records:");
      std::string name = msgscanstring();
      if(name[0] >= 'a' && name[0] <= 'z')
      {
        name[0] += 'A' - 'a';
      }
      if(name[0] == '\0')
      {
        append_message("Maybe you should come back when you've learned to write.", true);
      }
      else if(name != Player.name)
      {
        append_message("Aha! Welfare Fraud! It's off to gaol for you, lout!", true);
        send_to_jail();
      }
      else
      {
        append_message("Here's your handout, layabout!", true);
        LastDay     = Date;
        Player.cash = 99;
        dataprint();
      }
    }
  }
  else
  {
    print2("You're too well off for us to help you!");
  }
}

void l_library()
{
  char response;
  int  studied = false;
  int  done = false, fee = 1000;
  queue_message("Rampart Public Library.");
  if(nighttime())
  {
    print2("CLOSED");
  }
  else
  {
    append_message("Library Research Fee: 1000Au.", true);
    if(Player.maxiq < 18)
    {
      append_message("The Rampart student aid system has arranged a grant!", true);
      fee = std::max(50, 1000 - (18 - Player.maxiq) * 125);
      append_message("Your revised fee is: " + std::to_string(fee) + "Au.", true);
    }
    while(!done)
    {
      append_message("Pay the fee? [yn] ", true);
      if(ynq1() == 'y')
      {
        if(Player.cash < fee)
        {
          append_message("No payee, No studee.", true);
          done = true;
        }
        else
        {
          Player.cash -= fee;
          do
          {
            studied = true;
            dataprint();
            std::vector<std::string> lines =
            {
              {"Peruse a scroll:"},
              {"a: Omegan Theology"},
              {"b: Guide to Rampart"},
              {"c: High Magick"},
              {"d: Odd Uncatalogued Document"},
              {"e: Attempt Advanced Research"},
              {"ESCAPE: Leave this font of learning."}
            };
            menu->load(lines);
            menu->print();
            response = (char)mcigetc();
            if(response == 'a')
            {
              append_message("You unfurl an ancient, yellowing scroll...", true);
              theologyfile();
            }
            else if(response == 'b')
            {
              append_message("You unroll a slick four-color document...", true);
              cityguidefile();
            }
            else if(response == 'c')
            {
              append_message("This scroll is written in a strange magical script...", true);
              wishfile();
            }
            else if(response == 'd')
            {
              append_message("You find a strange document, obviously misfiled", true);
              append_message("under the heading 'acrylic fungus painting technique'");
              adeptfile();
            }
            else if(response == 'e')
            {
              if(random_range(30) > Player.iq)
              {
                append_message("You feel more knowledgeable!", true);
                Player.iq++;
                Player.maxiq++;
                dataprint();
                if(Player.maxiq < 19 && fee != std::max(50, 1000 - (18 - Player.maxiq) * 125))
                {
                  fee = std::max(50, 1000 - (18 - Player.maxiq) * 125);
                  append_message("Your revised fee is: " + std::to_string(fee) + "Au.", true);
                }
              }
              else
              {
                append_message("You find advice in an ancient tome: ", true);
                hint();
              }
            }
            else if(response == ESCAPE)
            {
              done = true;
              append_message("That was an expensive browse...", true);
            }
            else
            {
              studied = false;
            }
          } while(!studied);
        }
        xredraw();
      }
      else
      {
        done = true;
        if(studied)
        {
          append_message("Come back anytime we're open, 7am to 8pm.", true);
        }
        else
        {
          append_message("You philistine!", true);
        }
      }
    }
  }
}

void l_pawn_shop()
{
  int i, j, k, limit, number, done = false;

  if(nighttime())
  {
    queue_message("Shop Closed: Have a Nice (K)Night");
  }
  else
  {
    limit    = std::min(5, Date - Pawndate);
    Pawndate = Date;
    for(k = 0; k < limit; k++)
    {
      if(Pawnitems[0] != NULL)
      {
        if(Objects[Pawnitems[0]->id].uniqueness > UNIQUE_UNMADE)
        {
          Objects[Pawnitems[0]->id].uniqueness = UNIQUE_UNMADE;
        }
        /* could turn up anywhere, really :) */
        free((char *)Pawnitems[0]);
        Pawnitems[0] = NULL;
      }
      for(i = 0; i < PAWNITEMS - 1; i++)
      {
        Pawnitems[i] = Pawnitems[i + 1];
      }
      Pawnitems[PAWNITEMS - 1] = NULL;
      for(i = 0; i < PAWNITEMS; i++)
      {
        if(Pawnitems[i] == NULL)
        {
          do
          {
            if(Pawnitems[i] != NULL)
            {
              free(Pawnitems[i]);
            }
            Pawnitems[i]        = create_object(5);
            Pawnitems[i]->known = 2;
          } while((Pawnitems[i]->objchar == CASH) || (Pawnitems[i]->objchar == ARTIFACT) ||
                  (true_item_value(Pawnitems[i]) <= 0));
        }
      }
    }
    queue_message("Knight's Pawn Shop:");
    while(!done)
    {
      append_message("Buy item, Sell item, sell Pack contents, Leave [b,s,p,ESCAPE] ", true);
      std::vector<std::string> lines;
      for(i = 0; i < PAWNITEMS; ++i)
      {
        if(Pawnitems[i])
        {
          lines.emplace_back(std::string(1, i + 'a') + ":" + itemid(Pawnitems[i]));
        }
      }
      menu->load(lines);
      int player_input = menu->get_player_input();
      if(player_input == ESCAPE)
      {
        done = true;
      }
      else if(player_input == 'b')
      {
        append_message("Purchase which item? [ESCAPE to quit] ", true);
        player_input = ' ';
        while(player_input != ESCAPE && (player_input < 'a' || player_input >= 'a' + PAWNITEMS))
        {
          player_input = menu->get_player_input();
        }
        if(player_input != ESCAPE)
        {
          i = player_input - 'a';
          if(Pawnitems[i] == NULL)
          {
            append_message("No such item!", true);
          }
          else if(true_item_value(Pawnitems[i]) <= 0)
          {
            append_message("Hmm, how did that junk get on my shelves?", true);
            append_message("I'll just remove it.");
            free((char *)Pawnitems[i]);
            Pawnitems[i] = NULL;
          }
          else
          {
            long price = Pawnitems[i]->number * true_item_value(Pawnitems[i]);
            append_message("The low, low, cost is: " + std::to_string(price) + " Buy it? [ynq] ", true);
            if(ynq1() == 'y')
            {
              if(Player.cash < price)
              {
                append_message("No credit! Gwan, Beat it!", true);
              }
              else
              {
                Player.cash -= price;
                Objects[Pawnitems[i]->id].known = 1;
                gain_item(Pawnitems[i]);
                Pawnitems[i] = NULL;
              }
            }
          }
        }
      }
      else if(player_input == 's')
      {
        menuclear();
        print2("Sell which item: ");
        i = getitem(NULL_ITEM);
        if((i != ABORT) && (Player.possessions[i] != NULL))
        {
          if(cursed(Player.possessions[i]))
          {
            print1("No loans on cursed items! I been burned before....");
          }
          else if(true_item_value(Player.possessions[i]) <= 0)
          {
            print1("That looks like a worthless piece of junk to me.");
          }
          else
          {
            long price = item_value(Player.possessions[i]) / 2;
            print1("You can get " + std::to_string(price) + " Au each. Sell [yn]? ");
            if(ynq1() == 'y')
            {
              number = getnumber(Player.possessions[i]->number);
              if((number >= Player.possessions[i]->number) && Player.possessions[i]->used)
              {
                item_unequip(Player.possessions[i]);
              }
              Player.cash += number * price;
              free((char *)Pawnitems[0]);
              for(j = 0; j < PAWNITEMS - 1; j++)
              {
                Pawnitems[j] = Pawnitems[j + 1];
              }
              Pawnitems[PAWNITEMS - 1]         = ((pob)checkmalloc(sizeof(objtype)));
              *(Pawnitems[PAWNITEMS - 1])      = *(Player.possessions[i]);
              Pawnitems[PAWNITEMS - 1]->number = number;
              Pawnitems[PAWNITEMS - 1]->known  = 2;
              dispose_lost_objects(number, Player.possessions[i]);
              dataprint();
            }
          }
        }
      }
      else if(player_input == 'p')
      {
        for(i = 0; i < Player.packptr; i++)
        {
          if(Player.pack[i]->blessing > -1 && true_item_value(Player.pack[i]) > 0)
          {
            long price = item_value(Player.pack[i]) / 2;
            print1("Sell " + std::string(itemid(Player.pack[i])) + " for " + std::to_string(price) +
                   " Au each? [ynq] ");
            player_input = ynq1();
            if(player_input == 'y')
            {
              number = getnumber(Player.pack[i]->number);
              if(number > 0)
              {
                Player.cash += number * price;
                free((char *)Pawnitems[0]);
                for(j = 0; j < PAWNITEMS - 1; j++)
                {
                  Pawnitems[j] = Pawnitems[j + 1];
                }
                Pawnitems[PAWNITEMS - 1]         = ((pob)checkmalloc(sizeof(objtype)));
                *(Pawnitems[PAWNITEMS - 1])      = *(Player.pack[i]);
                Pawnitems[PAWNITEMS - 1]->number = number;
                Pawnitems[PAWNITEMS - 1]->known  = 2;
                Player.pack[i]->number -= number;
                if(Player.pack[i]->number < 1)
                {
                  free((char *)Player.pack[i]);
                  Player.pack[i] = NULL;
                }
                dataprint();
              }
            }
            else if(player_input == 'q')
            {
              break;
            }
          }
        }
        fixpack();
      }
    }
  }
  calc_melee();
  xredraw();
}
