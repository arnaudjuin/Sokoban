

#include "maze.h"
#include "graphic.h"
#include "utils/console.h"
#include "utils/coord.h"
#include "bruteforce.h"
#include <iostream>

// Init allegro with 800x600 (resolution)
Graphic g(800,600,32);

int main()
{
    // Do not touch !
    Console::getInstance()->setColor(_COLOR_DEFAULT);

    // Load level from a file
    Maze m("./levels/hard/hard_2.dat");
    if (!m.init()) return -1;
    std::cout << m << std::endl;

    if (g.init())
    {
       //  While playing...
        while (!g.keyGet(KEY_ESC) && !g.keyGet(KEY_A))
        {
            // Check if user has pressed a key
            if (g.keyPressed())
            {
                bool win = false;
                switch (g.keyRead())
                {
                    case ARROW_UP:
                        win = m.updatePlayer(TOP);
                        break;
                    case ARROW_BOTTOM:
                        win = m.updatePlayer(BOTTOM);
                        break;
                    case ARROW_RIGHT:
                        win = m.updatePlayer(RIGHT);
                        break;
                    case ARROW_LEFT:
                        win = m.updatePlayer(LEFT);
                        break;
                }

//                if(!m.isMazeSolvable()) std::cout << "perdu" << std::endl;
                if (win==true) std::cout << "Win ! " << std::endl;
            }
              // bruteforce(&m,g);
            // Display on screen
            g.clear();
            m.draw(g);
            g.display(Coord::m_nb_col);
      }
    }

    // Free memory
    g.deinit();
    Console::deleteInstance();

    return 0;
}
END_OF_MAIN()
