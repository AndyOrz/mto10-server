#include "../common.h"
#include "cmath"
#include <set>

struct PossiblePoint
{
    int x;
    int y;
    int value;
    int near;
};

class Game_Engine
{
private:
    int row, col, gameid, delay;
    int step, score, maxvalue;
    uint8_t *game_map;

    void str_to_map(string &s);
    string map_to_str(uint8_t *mmap);
    list<PossiblePoint> search_possiblepoints(uint8_t *mmap);
    list<PossiblePoint> find_area(PossiblePoint p, bool *flag_map, uint8_t *mmap);
    uint8_t * play(uint8_t *mmap, PossiblePoint p, int &max_value);
    int generate_random_value(int max_value);
    void printmap(FILE *file,uint8_t *mmap,bool *flag_map);
public:
    Game_Engine();

    map<string, string> InitGame(map<string, int> &init_game, int default_delay);

    //返回值0:MergeSucceeded,1:GameOver,2:GameFiniShed,3:GameTimeout
    map<string, string> Play(map<string, string> &coordinate);

    map<string, string> Solve(char row, int col);
    map<string, string> AutoSolve();

    // 输出
    void Flush(FILE *file);
    void FlushChoice(FILE *file, map<string, string> &choice);
    void FlushError(FILE *file, map<string, string> &game);
};