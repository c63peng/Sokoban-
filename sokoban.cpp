#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

constexpr int NORTH = 0;
constexpr int EAST = 1; 
constexpr int SOUTH = 2; 
constexpr int WEST = 3; 

class Sokoban : public olc::PixelGameEngine
{
public:
	Sokoban()
	{
	sAppName = "Sokoban";
	}

    std::string sLevel = 
        "################"
        "#..............#"
        "#.....|....-...#"
        "#..p..|........#"  
        "#.....|........#"
        "#.......+....-.#"
        "#..5...........#"
        "#....-.........#"
        "#.........#....#"
        "#.+..@....#....#"
        "#....@.........#"
        "#....@....+....#"
        "#.....|........#"
        "#...+.....-....#"
        "#..............#"
        "################";
                        
    olc::vf2d vLevelSize = { 16, 15 };
    olc::vf2d vBlockSize = { 16, 16 };

    std::vector<olc::vi2d> vGoals;

    olc::Renderable gfxTiles;

    struct block
    {
        block()
        {
        //constructor
        }

        virtual void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable skin)
        {
        //passing throught the drawing tool so it can draw itself
        } 

        virtual bool Push (const int from )
        {
        return true;//can be pushed in all directions
        }

        virtual void Move(){}
    };


    struct block_solid : public block //inherit from base class block
        {
            void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable skin) override
            {
            pge -> FillRect(pos * size, size, olc::BLUE);
            } 

            virtual bool Push (const int from ) override
            {
            return false; //solid block can never move
            }
        };


    struct block_player : public block //inherit from base class block
        {
            void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable skin) override
            {
            pge -> FillRect(pos * size, size, olc::WHITE);
            } 

            virtual bool Push (const int from ) override
            {
            return true; //can be pushed in all directions
            }

        };


    struct block_simple : public block //inherit from base class block
        {
            void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable skin) override
            {
            pge -> FillRect(pos * size, size, olc::RED);
            } 

            virtual bool Push (const int from ) override
            {
            return true;//can be pushed in all directions
            }
        };



    struct block_horizontal : public block //inherit from base class block
        {
            void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable skin) override
            {
            pge -> FillRect(pos * size, size, olc::GREEN);
            } 

            virtual bool Push (const int from ) override
            {
            return from == EAST || from ==WEST;
            }
        };


    struct block_vertical : public block //inherit from base class block
        {
            void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable skin) override
            {
            pge -> FillRect(pos * size, size, olc::YELLOW);
            } 

            virtual bool Push (const int from ) override
            {
            return from == NORTH || from == SOUTH;
            }
        };



    struct block_countdown : public block //inherit from base class block
        {
            block_countdown(int m )
            {
            nMoves = m; 
            }

            void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable skin) override
            {
            pge -> FillRect(pos * size, size, olc::CYAN);
            pge -> DrawString (pos * size + olc::vi2d(4,4), std::to_string(nMoves), olc::BLACK);
            } 

            virtual bool Push (const int from ) override
            {
            return nMoves > 0;
            }

            void Move() override
            {
            nMoves--;
            }

            int nMoves = 0; 
        };

    olc::vi2d vPlayer;
    std::vector< std::unique_ptr<block>> vLevel;

public:

    void LoadLevel(int n)
    {
    //turns our string into something that loads the level
        vLevel.clear();
        vGoals.clear();

        for (int y = 0; y < vLevelSize.y; y++){
        //vLevelSize.y = the height
            for (int x = 0; x < vLevelSize.x; x++)
            {
                switch(sLevel [y* vLevelSize.x + x ]) //convert 2d into 1d
                {
                case '#':
                    vLevel.emplace_back(std::make_unique <block_solid>());
                    break;
                 case 'p':
                    vLevel.emplace_back(std::make_unique <block_player>());
                    vPlayer = { x, y };
                    break;
                case '+':
                    vLevel.emplace_back(std::make_unique <block_simple>());
                    break;
                case '-':
                    vLevel.emplace_back(std::make_unique <block_horizontal>());
                    break;
                case '|':
                    vLevel.emplace_back(std::make_unique <block_vertical>());
                    break;
                case '5':
                    vLevel.emplace_back(std::make_unique <block_countdown>(5));
                    break;
                case '@':
                    vGoals.push_back({x, y});
                    vLevel.emplace_back(nullptr);
                    break;
                default: //for empty block '.'
                    vLevel.emplace_back(nullptr);
                }
            }
        }
    }


	bool OnUserCreate() override
	{
    gfxTiles.Load("./gfx/blocks.png");
    LoadLevel(0);
	// Called once at the start, so create things here
	return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
    bool bPushing = false; //not pushing anything unless we press the keys
    int dirPush = 0;     

        if(GetKey(olc::Key::W).bPressed)
        {
            dirPush = NORTH;
            bPushing = true;
        }
        if(GetKey(olc::Key::S).bPressed)
        {
            dirPush = SOUTH;
            bPushing = true;
        }
        if(GetKey(olc::Key::A).bPressed)
        {
            dirPush = WEST;
            bPushing = true;
        }
        if(GetKey(olc::Key::D).bPressed)
        {
            dirPush = SOUTH;
            bPushing = true;
        };
        if(GetKey(olc::Key::R).bPressed)
        {
            LoadLevel(0);
            //clears the 2 vectors and reconstructors everything 
        };
	
        auto id = [&](olc::vi2d& pos){return pos.y * vLevelSize.x + pos.x; };

        if ( bPushing )
        {
        olc::vi2d vBlock = vPlayer; 

        bool bAllowPush = false; 
        bool bTest = true; 

            while(bTest)
            {
                if(vLevel[id(vBlock)] != nullptr)
                {
                    if(vLevel[id(vBlock)]->Push( (dirPush + 2 ) % 4) )
                    {
                    //block allows to be pushed, move the cursor in that direction
                        switch (dirPush)
                        {
                        case NORTH: vBlock.y--; break;
                        case SOUTH: vBlock.y++; break;
                        case EAST: vBlock.x++; break;
                        case WEST: vBlock.x--; break; 
                        }
                    }
                    else 
                    bTest = false;
                }
                else
                {
                //if cursor is in nullptr area
                bAllowPush = true;
                bTest = false;
                }
            }
            if (bAllowPush)
            {
                while (vBlock != vPlayer)
                {
                    olc::vi2d vSource = vBlock;
                    switch (dirPush)
                    {
                    case NORTH: vBlock.y++; break;
                    case SOUTH: vBlock.y--; break;
                    case EAST: vBlock.x--; break;
                    case WEST: vBlock.x++; break;   
                    }

                    if (vLevel[id(vSource)] != nullptr) vLevel[id(vSource)]->Move();
                    std::swap (vLevel[id(vSource)], vLevel[id(vBlock)]);

                    //updates cursor position
                    vBlock = vSource;
                }
                //updating player position
                switch (dirPush)
                {
                case NORTH: vPlayer.y--; break;
                case SOUTH: vPlayer.y++; break;
                case EAST: vPlayer.x++; break;
                case WEST: vPlayer.x--; break; 
                }
            }
        }

        int nGoals = 0; 

        for (auto& g :vGoals)
            if(vLevel[id(g)]) nGoals++;

        Clear(olc::BLACK);

        for (auto& g :vGoals)
            FillCircle(g * vBlockSize + vBlockSize/2, vBlockSize.x/2 -2, olc::YELLOW);

        olc::vi2d vTilePos = { 0, 0};
        for(vTilePos.y = 0; vTilePos.y < vLevelSize.y; vTilePos.y++)
        {
            for(vTilePos.x = 0; vTilePos.x < vLevelSize.x; vTilePos.x++)
            {
            auto &b = vLevel[id(vTilePos)]; //gives pointer to the block at the particular location, takes an x and a y and returns an index
                
                if (b)
                {
                b->DrawSelf(this, vTilePos, vBlockSize, gfxTiles);
                }
            }
        }
		DrawString(4, 4, "Goals: " + std::to_string(nGoals) + " / " + std::to_string(vGoals.size()), olc::WHITE);
        
        return true;
	}
};


int main()
{
	Sokoban demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();

	return 0;
}
