#include "game_engine.h"
#include "../common.h"

int main()
{
    map<string, string> init_game;
    init_game["Type"]="GameProgress";
    init_game["Content"] = "StartGame";
    init_game["Row"] = "6";
    init_game["Col"] = "6";
    init_game["GameID"] = "1585559852";
    init_game["Delay"] = "5000";
    init_game["Map"] = "313333212212323542332122231211231255";
    Game_Engine G(init_game);

    G.Flush(stdout);
    map<string, string> toServerBlock=G.AutoSolve();
    cout<<"Row="<<toServerBlock["Row"]<<"  Col="<<toServerBlock["Col"]<<endl;
}

