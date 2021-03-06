
#include "maze.h"
#include "graphic.h"
#include "utils/console.h"
#include "utils/coord.h"
#include <fstream>
#include <iomanip>

Maze::Maze(const std::string& path)
    : m_lig(0), m_col(0), m_pos_player(0), m_dir_player(TOP), m_level_path(path)
{
}

Maze::~Maze()
{
}

bool Maze::init()
{
    bool res = this->_load(this->m_level_path);
    if (!res)
    {
        std::cerr << "Cannot load maze... Check file : " << this->m_level_path << std::endl;
        return false;
    }

    return res;
}

// Check if all boxes are on a goal
bool Maze::_isCompleted() const
{
    for (unsigned int i=0; i<this->m_pos_boxes.size(); ++i)
    {
        if (!this->isSquareBoxPlaced(this->m_pos_boxes[i]))
            return false;
    }
    return true;
}

// Check if we can push a box in a direction
// INPUT: position of the box to check, direction,
// OUTPUT : the position of the box after pushing
//      TRUE if all goes right otherwise FALSE
bool Maze::_canPushBox(unsigned short posBox, char dir, unsigned short& newPosBox) const
{
    // Check if this position is a box !
    if (!this->isSquareBox(posBox))
        return false;

    // Compute new position according to push direction
    newPosBox = Coord::getDirPos(posBox, dir);

    // Can we push the box ?
    return this->isSquareWalkable(newPosBox);
}

// Load a maze from a file (DO NOT TOUCH)
bool Maze::_load(const std::string& path)
{
    std::vector<unsigned short> tmpPosBoxes;
    std::ifstream ifs(path.c_str());
    if (ifs)
    {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(ifs, line))
        {
            lines.push_back(line);
            this->m_lig++;
            this->m_col = (this->m_col < line.size() ? line.size() : this->m_col);
        }
        ifs.close();

        if (this->m_col > NB_MAX_WIDTH || this->m_lig > NB_MAX_HEIGHT)
        {
            std::cerr << "Maze::load => Bad formatting in level data..." << std::endl;
            return false;
        }

        Coord::m_nb_col = this->m_col;
        for (unsigned int i=0; i<lines.size(); i++)
        {
            //LDebug << "Maze::load => Reading : " << lines[i];
            for (unsigned int j=0; j<this->m_col; j++)
            {
                if (j < lines[i].size())
                {
                    bool both = false;
                    unsigned short pos = Coord::coord1D(i, j);
                    unsigned char s = (unsigned char)(lines[i][j] - '0');

                    // Need to add a goal and a box ;)
                    if (s == SPRITE_BOX_PLACED)
                    {
                        both = true;
                    }

                    if (s == SPRITE_GOAL || both)
                    {
                        this->m_pos_goals.push_back(pos);
                    }
                    if (s == SPRITE_BOX || both)
                    {
                        tmpPosBoxes.push_back(pos);
                    }

                    // Assign player position
                    if (s == SPRITE_MARIO)
                    {
                        this->m_pos_player = pos;
                        //LDebug << "\tAdding player pos (" << pos << ")";
                        s = SPRITE_GROUND;
                    }

                    // Add this value in the field
                    this->m_field.push_back(s);
                }
                else
                {
                    // Here - Out of bound
                    this->m_field.push_back(SPRITE_GROUND);
                }
            }
        }

        // Copy box position
        this->m_pos_boxes.resize(tmpPosBoxes.size());
        for (unsigned int i=0; i<tmpPosBoxes.size(); ++i)
        {
            this->m_pos_boxes[i] = tmpPosBoxes[i];
        }

        return (this->m_pos_boxes.size() == this->m_pos_goals.size());
    }
    else
    {
        std::cerr << "Maze::load => File does not exist..." << std::endl;
    }

    return false;
}

bool Maze::updatePlayer(char dir)
{
    if (dir < 0 || dir > MAX_DIR)
    {
        std::cerr << "Maze::updatePlayer => Direction not correct... " << +dir << std::endl;
        return false;
    }

    // Implement player moving and pushing here
    unsigned short tmp;

    tmp=Coord::getDirPos(this->getPosPlayer(),dir);

    unsigned short newPosBox;
    if(this->isSquareWalkable(tmp)) this->setPlayerPos(tmp);
    else if(this->isSquareBox(tmp)&& this->_canPushBox(tmp,dir,newPosBox))
    {

        for(unsigned int i=0; i<this->m_pos_boxes.size(); i++)
        {
            if(this->m_pos_boxes[i]==tmp)
            {
                this->m_pos_boxes[i]=Coord::getDirPos(tmp,dir);
                this->m_field[tmp]=SPRITE_GROUND;
                this->m_field[Coord::getDirPos(tmp,dir)]=SPRITE_BOX;
                this->setPlayerPos(tmp);
            }

        }

    }
    this->m_dir_player=dir;
    bool tmpb=false;
    deadlocks();
    for(int i=0; i<this->m_pos_boxes.size(); ++i)
    {
        tmpb=false;
        for(int j=0; j<this->m_pos_goals.size(); ++j)
        {
            if(this->m_pos_boxes[i]==this->m_pos_goals[j])
                tmpb=true;
        }
        if(!tmpb) return false;
    }
    return true;
}

// Display maze on screen with Allegro
void Maze::draw(const Graphic& g) const
{
    for(unsigned int i=0; i<this->getSize(); i++)
    {
        unsigned int l = 0, c = 0;
        Coord::coord2D(i, l, c);

        if (i == this->m_pos_player)
        {
            g.drawT(g.getSpritePlayer(this->m_dir_player), c, l);
        }
        else
        {
            g.drawT(g.getSprite(this->m_field[i]), c, l);
        }
    }
}

// DO NOT TOUCH !
// Overload function for displaying debug information
// about Maze class
std::ostream& operator << (std::ostream& O, const Maze& m)
{
    unsigned int l, c;
    int i = 0;
    Coord::coord2D(m.m_pos_player, l, c);
    O << "Player position " << m.m_pos_player << " (" << l << "," << c << ")" << std::endl;
    O << "Field Size " << +m.m_lig << " x " << +m.m_col << " = " << m.getSize() << std::endl;
    O << "Field Vector capacity : " << m.m_field.capacity() << std::endl;
    O << "Field array : " << std::endl << std::endl;
    for(unsigned int l=0; l<m.getSize(); l++)
    {
        if (l == m.m_pos_player) Console::getInstance()->setColor(_COLOR_YELLOW);
        else if (m.isSquareWall(l)) Console::getInstance()->setColor(_COLOR_PURPLE);
        else if (m.isSquareBoxPlaced(l) || m.isSquareGoal(l)) Console::getInstance()->setColor(_COLOR_GREEN);
        else if (m.isSquareBox(l)) Console::getInstance()->setColor(_COLOR_BLUE);
        else if (m.m_field[l] == SPRITE_DEADSQUARE) Console::getInstance()->setColor(_COLOR_RED);
        else Console::getInstance()->setColor(_COLOR_WHITE);

        O << std::setw(2) << +m.m_field[l] << " "; // + => print as "int"

        if ((l+1) % m.m_col == 0)
        {
            O << std::endl;
        }
    }
    Console::getInstance()->setColor(_COLOR_DEFAULT);

    O << std::endl;
    O << "Box position : " << std::endl;
    for (unsigned int i=0; i<m.m_pos_boxes.size(); i++)
    {
        Coord::coord2D(m.m_pos_boxes[i], l, c);
        O << "\t" << "Box #" << i << " => " << std::setw(3) << m.m_pos_boxes[i] << std::setw(2) << " (" << l << "," << c << ")" << std::endl;
    }

    O << std::endl;
    O << "Goal position : " << std::endl;
    for (const auto& goal : m.m_pos_goals)
    {
        unsigned int l, c;
        Coord::coord2D(goal, l, c);
        if (m.isSquareBoxPlaced(goal)) Console::getInstance()->setColor(_COLOR_GREEN);
        O << "\t" << "Goal #" << i << " => " << std::setw(3) << goal << std::setw(2) << " (" << l << "," << c << ")" << std::endl;
        if (m.isSquareBoxPlaced(goal)) Console::getInstance()->setColor(_COLOR_DEFAULT);
        i++;
    }

    return O;
}

//Check if the maze is still solvable
bool Maze::isMazeSolvable()
{
    //Check all the corners

    for(int i=0; i < this->m_pos_boxes.size(); ++i)
    {
        if(this->isCorner(this->m_pos_boxes[i]) && !(this->isSquareGoal(this->m_pos_boxes[i])))
            return false;
    }
    return true;
}

bool Maze::isCorner(unsigned short pos)
{
    if(this->m_field[Coord::getDirPos(pos,TOP)]==SPRITE_WALL && this->m_field[Coord::getDirPos(pos,LEFT)]==SPRITE_WALL) return true;
    if(this->m_field[Coord::getDirPos(pos,TOP)]==SPRITE_WALL && this->m_field[Coord::getDirPos(pos,RIGHT)]==SPRITE_WALL)return true;
    if(this->m_field[Coord::getDirPos(pos,BOTTOM)]==SPRITE_WALL && this->m_field[Coord::getDirPos(pos,LEFT)]==SPRITE_WALL)return true;
    if(this->m_field[Coord::getDirPos(pos,BOTTOM)]==SPRITE_WALL && this->m_field[Coord::getDirPos(pos,RIGHT)]==SPRITE_WALL)return true;



    return false;
}

bool Maze::isCompleted()
{

    return this->_isCompleted();
}
void Maze::deadlocks()
{
    unsigned short tmp;
    bool tmpdeadlocks [4];
    bool tmpp [4];
    for (unsigned short i=0; i<m_pos_boxes.size(); i++)
    {
        for (int j=0; j<4; j++)
            {
                  tmpdeadlocks[j]=true;        //on remet a zero a chaque tour de boucle
                  tmpp[j]=true;
            }


        tmp=m_pos_boxes[i];
        if ((isSquareWall(Coord::getDirPos(tmp,0)) || isSquareBoxLock(Coord::getDirPos(tmp,0))))
            tmpdeadlocks[0]=false;
        if ((isSquareWall(Coord::getDirPos(tmp,1)) || isSquareBoxLock(Coord::getDirPos(tmp,1))))
            tmpdeadlocks[1]=false;
        if ((isSquareWall(Coord::getDirPos(tmp,2)) || isSquareBoxLock(Coord::getDirPos(tmp,2))))
            tmpdeadlocks[2]=false;
        if ((isSquareWall(Coord::getDirPos(tmp,3)) || isSquareBoxLock(Coord::getDirPos(tmp,3))))
            tmpdeadlocks[3]=false;

if (!isSquareBoxPlaced((tmp)))
{
        if (tmpdeadlocks[0]==false && tmpdeadlocks[2]==false )
           {
               setSquare(tmp, SPRITE_DEADSQUARE);
               tmpp[0]=false;
           }

        if (tmpdeadlocks[0]==true && tmpdeadlocks[2]==true )
            setSquare(tmp, SPRITE_BOX);

        if (tmpdeadlocks[0]==false && tmpdeadlocks[3]==false )
              {
               setSquare(tmp, SPRITE_DEADSQUARE);
               tmpp[1]=false;
           }
        if (tmpdeadlocks[0]==true && tmpdeadlocks[3]==true && tmpp[1]==true && tmpp[0]==false )
            setSquare(tmp, SPRITE_BOX);

        if (tmpdeadlocks[1]==false && tmpdeadlocks[3]==false )
               {
               setSquare(tmp, SPRITE_DEADSQUARE);
               tmpp[2]=false;
           }
        if (tmpdeadlocks[1]==true && tmpdeadlocks[3]==true  && tmpp[0]==true && tmpp[1]==true && tmpp[2]==true )
            setSquare(tmp, SPRITE_BOX);

        if (tmpdeadlocks[1]==false && tmpdeadlocks[2]==false )
             {
               setSquare(tmp, SPRITE_DEADSQUARE);
               tmpp[3]=false;
           }
        if (tmpdeadlocks[1]==true && tmpdeadlocks[2]==true && tmpp[0]==true  && tmpp[1]==true && tmpp[2]==true && tmpp[3]==true )
            setSquare(tmp, SPRITE_BOX);

    }
    }

}
