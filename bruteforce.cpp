#include "bruteforce.h"
#include "winalleg.h"
void bruteforce(Maze* m,Graphic g)
{

    Maze tmp= *m;
    std::vector<unsigned short> deplacement;


    bool win=false;
    deplacement.push_back(TOP);
    while(win==false)
    {
        tmp=*m;

        for(int i=0;i<deplacement.size();i++)
        {

            win=tmp.updatePlayer(deplacement[i]);
//
//            Sleep(100);
//         g.clear();
//        tmp.draw(g);
//        g.display(Coord::m_nb_col);


        }

//        for(int j=0;j<deplacement.size();j++)
//            std::cout << deplacement[j];

//        std::cout <<    std::endl;
        int sizetmp=deplacement.size();
        deplacement[0]++;
        for(int i=0;i<sizetmp;i++)
        {


            if(deplacement[i]>=4)
              {
                if(deplacement.size()-1==i)
                    {
                        deplacement.push_back(TOP);
                    }
                else deplacement[i+1]++;
                deplacement[i]=0;

              }


        }




    }

}
