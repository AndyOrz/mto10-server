#include "GameEngine.h"

Game_Engine::Game_Engine()
{
}

void Game_Engine::str_to_map(string &s)
{
    for (int i = 0; i < row * col; i++)
    {
        char c = s[i];
        if (c >= '0' && c <= '9')
            this->game_map[i] = c - '0';
        if (c >= 'a' && c <= 'f')
            this->game_map[i] = c - 'a' + 10;
        if (c >= 'A' && c <= 'F')
            this->game_map[i] = c - 'A' + 10;
    }
}
string Game_Engine::map_to_str(uint8_t *mmap)
{
    string s = "";
    for (int i = 0; i < row * col; i++)
    {
        char c[2];
        sprintf(c, "%X", mmap[i]);
        s += c;
    }
    return s;
}

list<PossiblePoint> Game_Engine::search_possiblepoints(uint8_t *mmap)
{
    bool *flag_map = new bool[this->row * this->col];
    bzero(flag_map, this->row * this->col);
    list<PossiblePoint> ppoints;
    int i = this->row * this->col - 1;
    while (i >= 0)
    {
        if (flag_map[i])
            ; //已被探索过
        else
        {
            flag_map[i] = true;
            PossiblePoint p = {
                i % this->col,
                i / this->col,
                mmap[i],
                1};
            list<PossiblePoint> plist = find_area(p, flag_map, mmap);

            if (plist.size() > 1)
            {
                set<int> selected_x;
                p.near = plist.size();
                ppoints.push_back(p);
                list<PossiblePoint>::iterator iter = plist.begin();
                while (iter != plist.end())
                {
                    if (selected_x.count((*iter).x) == 0)
                    {
                        (*iter).near = plist.size();
                        ppoints.push_back(*(iter));
                        selected_x.insert((*iter).x);
                    }
                    iter++;
                }
            }
        }
        i--;
    }
    delete[] flag_map;
    return ppoints;
}

list<PossiblePoint> Game_Engine::find_area(PossiblePoint p, bool *flag_map, uint8_t *mmap)
{
    queue<PossiblePoint> q;
    list<PossiblePoint> plist;
    q.push(p);
    plist.push_back(p);
    flag_map[p.x + p.y * this->col] = true;
    while (!q.empty())
    {
        PossiblePoint near_p = q.front();
        q.pop();
        //上
        if (near_p.y - 1 >= 0 &&
            mmap[near_p.x + (near_p.y - 1) * this->col] == mmap[near_p.x + near_p.y * this->col] && flag_map[near_p.x + (near_p.y - 1) * this->col] == false)
        {
            flag_map[near_p.x + (near_p.y - 1) * this->col] = true;
            q.push({near_p.x, near_p.y - 1, near_p.value, 0});
            plist.push_back({near_p.x, near_p.y - 1, near_p.value, 0});
        }
        //下
        if (near_p.y + 1 < this->row &&
            mmap[near_p.x + (near_p.y + 1) * this->col] == mmap[near_p.x + near_p.y * this->col] && flag_map[near_p.x + (near_p.y + 1) * this->col] == false)
        {
            flag_map[near_p.x + (near_p.y + 1) * this->col] = true;
            q.push({near_p.x, near_p.y + 1, near_p.value, 0});
            plist.push_back({near_p.x, near_p.y + 1, near_p.value, 0});
        }
        //左
        if (near_p.x - 1 >= 0 &&
            mmap[near_p.x - 1 + near_p.y * this->col] == mmap[near_p.x + near_p.y * this->col] && flag_map[near_p.x - 1 + near_p.y * this->col] == false)
        {
            flag_map[near_p.x - 1 + near_p.y * this->col] = true;
            q.push({near_p.x - 1, near_p.y, near_p.value, 0});
            plist.push_back({near_p.x - 1, near_p.y, near_p.value, 0});
        }
        //右
        if (near_p.x + 1 < this->col &&
            mmap[near_p.x + 1 + near_p.y * this->col] == mmap[near_p.x + near_p.y * this->col] && flag_map[near_p.x + 1 + near_p.y * this->col] == false)
        {
            flag_map[near_p.x + 1 + near_p.y * this->col] = true;
            q.push({near_p.x + 1, near_p.y, near_p.value, 0});
            plist.push_back({near_p.x + 1, near_p.y, near_p.value, 0});
        }
    }
    return plist;
}

int Game_Engine::generate_random_value(int max_value)
{
    int r;
    switch (max_value)
    {
    case 3:
        return rand() % 3 + 1;
        break;
    case 4:
        r = rand() % 10;
        if (r >= 0 && r <= 8)
            return r / 3 + 1;
        else
            return 4;
        break;
    case 5:
        r = rand() % 20;
        if (r >= 0 && r <= 14)
            return r / 5 + 1;
        else if (r >= 15 && r <= 17)
            return 3;
        else
            return 4;
        break;
    case 6:
        r = rand() % 20;
        if (r >= 0 && r <= 15)
            return r / 4 + 1;
        else if (r >= 16 && r <= 18)
            return 5;
        else
            return 6;
        break;
    default:
        r = rand() % 100;
        if (r >= 0 && r <= 79)
            return r % (max_value - 3) + 1;
        else if (r >= 80 && r <= 89)
            return max_value - 2;
        else if (r >= 90 && r <= 94)
            return max_value - 1;
        else
            return max_value;
        break;
    }
}

uint8_t *Game_Engine::play(uint8_t *mmap, PossiblePoint p, int &max_value, uint8_t *oldmap)
{
    uint8_t *m = new uint8_t[this->row * this->col];
    memcpy(m, mmap, this->row * this->col);
    bool *flag_map = new bool[this->row * this->col];
    bzero(flag_map, this->row * this->col);

    //消除p的邻域并下落，寻找最大值
    max_value = 0;
    find_area(p, flag_map, m);
    m[p.x + p.y * this->col] = m[p.x + p.y * this->col] + 1;
    flag_map[p.x + p.y * this->col] = false;
    for (int j = this->row - 1; j >= 0; j--)
        for (int i = 0; i < this->col; i++)
        {
            if (m[i + j * this->col] > max_value)
                max_value = m[i + j * this->col];
            while (flag_map[i + j * this->col] && m[i + j * this->col] != 0)
            {
                // printf("\n");
                // printmap(stdout,m,flag_map);
                for (int jj = j; jj >= 1; jj--)
                {
                    m[i + jj * this->col] = m[i + (jj - 1) * this->col];
                    flag_map[i + jj * this->col] = flag_map[i + (jj - 1) * this->col];
                }
                m[i] = 0;
                flag_map[i] = 1;
                // printmap(stdout,m,flag_map);
            }
        }

    memcpy(oldmap, m, this->row * this->col);
    //填充随机数
    for (int j = this->row - 1; j >= 0; j--)
        for (int i = 0; i < this->col; i++)
            if (flag_map[i + j * this->col])
                m[i + j * this->col] = generate_random_value(max_value);

    delete[] flag_map;
    return m;
}

map<string, string> Game_Engine::InitGame(map<string, int> &init_game, int default_delay)
{
    if (init_game["GameType"] == 0)
    {
        this->row = init_game["Row"];
        this->col = init_game["Col"];
        this->gameid = init_game["GameID"];
        this->delay = init_game["Delay"];

        if (this->row == -1 || this->row < 5 || this->row > 8)
        {
            this->row = rand() % 4 + 5;
            init_game["Row"] = this->row;
        }
        if (this->col == -1 || this->col < 5 || this->col > 10)
        {
            this->col = rand() % 6 + 5;
            init_game["Col"] = this->col;
        }
        if (this->gameid == -1 || this->gameid < -1)
        {
            this->gameid = time(0);
            init_game["GameID"] = this->gameid;
        }
        if (this->delay == -1 || this->delay < 2 || this->delay > 60)
        {
            this->delay = default_delay;
            init_game["Delay"] = this->delay;
        }

        this->game_map = new uint8_t[row * col];
        this->step = 0;
        this->score = 100;
        this->maxvalue = 3;
        srand(init_game["GameID"]);
        for (int i = 0; i < row * col; i++)
            this->game_map[i] = generate_random_value(this->maxvalue);
    }
    if (init_game["GameType"] == 1)
    {
    }
    map<string, string> gamestart;
    gamestart["Type"] = "GameProgress";
    gamestart["Content"] = "StartGame";
    gamestart["Row"] = to_string(this->row);
    gamestart["Col"] = to_string(this->col);
    gamestart["GameID"] = to_string(this->gameid);
    gamestart["Delay"] = to_string(this->delay);
    gamestart["Map"] = map_to_str(this->game_map);
    return gamestart;
}

map<string, string> Game_Engine::Play(map<string, string> &coordinate)
{
    map<string, string> block;
    block["Type"] = "GameProgress";
    block["GameID"] = to_string(this->gameid);

    if (coordinate["Content"] == "GameTimeout")
    {
        block["Content"] = "GameTimeout";
        block["Step"] = to_string(this->step);
        block["Score"] = to_string(this->score);
        block["MaxValue"] = to_string(this->maxvalue);
        block["NewMap"] = map_to_str(this->game_map);
        return block;
    }

    int x = coordinate["Col"][0] - '0';
    int y = coordinate["Row"][0] - 'A';
    if (x < 0 || y < 0 || x >= this->col || y >= this->row)
    {
        block["Content"] = "MergeFailed";
        this->step++;
        this->score -= 50;

        block["Step"] = to_string(this->step);
        block["Score"] = to_string(this->score);
        block["MaxValue"] = to_string(this->maxvalue);
        block["NewMap"] = map_to_str(this->game_map);
        return block;
    }

    bool *flag_map = new bool[this->row * this->col];
    bzero(flag_map, this->row * this->col);
    PossiblePoint p = {
        x,
        y,
        this->game_map[x + y * this->col]};
    list<PossiblePoint> plist = find_area(p, flag_map, this->game_map);
    if (plist.size() > 1)
    {
        uint8_t *oldmap = new uint8_t[this->col * this->row];
        uint8_t *mmap = play(this->game_map, {x, y}, this->maxvalue, oldmap);
        memcpy(this->game_map, mmap, this->col * this->row);
        block["OldMap"] = map_to_str(oldmap);
        delete[] mmap;
        delete[] oldmap;
        this->step++;
        this->score += plist.size() * p.value * 3;

        block["Step"] = to_string(this->step);
        block["Score"] = to_string(this->score);
        block["MaxValue"] = to_string(this->maxvalue);
        block["NewMap"] = map_to_str(this->game_map);

        if (this->maxvalue >= 16)
            block["Content"] = "GameFiniShed";
        if (search_possiblepoints(this->game_map).size() == 0 || this->score < 0)
            block["Content"] = "GameOver";
        else
            block["Content"] = "MergeSucceeded";
    }
    else
    {
        block["Content"] = "MergeFailed";
        this->step++;
        this->score -= 10;

        block["Step"] = to_string(this->step);
        block["Score"] = to_string(this->score);
        block["MaxValue"] = to_string(this->maxvalue);
    }
    return block;
}

void Game_Engine::Flush(FILE *file)
{
    time_t curtime;
    time(&curtime);
    fprintf(file, "--------%s", ctime(&curtime));

    fprintf(file, "\nGameID=%d\n", this->gameid);
    fprintf(file, "Step=%d  Score=%d  MaxValue=%d\n", this->step, this->score, this->maxvalue);
    fprintf(file, "    ");
    for (int i = 0; i < this->col; i++)
        fprintf(file, " %d", i);
    fprintf(file, "\n");
    fprintf(file, "----");
    for (int i = 0; i < this->col; i++)
        fprintf(file, "--");
    fprintf(file, "\n");
    for (int i = 0; i < this->row; i++)
    {
        fprintf(file, "%c | ", i + 'A');
        for (int j = 0; j < this->col; j++)
            fprintf(file, "%2d", this->game_map[j + i * col]);
        fprintf(file, "\n");
    }
}

void Game_Engine::FlushChoice(FILE *file, map<string, string> &choice)
{
    time_t curtime;
    time(&curtime);
    fprintf(file, "--------%s", ctime(&curtime));

    fprintf(file, "Make Choice:");
    fprintf(file, "Row=%s  Col=%s\n", choice["Row"].c_str(), choice["Col"].c_str());
}

void Game_Engine::FlushError(FILE *file, map<string, string> &game)
{
    time_t curtime;
    time(&curtime);
    fprintf(file, "--------%s", ctime(&curtime));

    fprintf(file, "Game Over! Message:\n");
    map<string, string>::iterator iter;
    iter = game.begin();
    while (iter != game.end())
    {
        fprintf(file, "%s=%s\n", iter->first.c_str(), iter->second.c_str());
        iter++;
    }
}

void Game_Engine::printmap(FILE *file, uint8_t *mmap, bool *flag_map)
{
    fprintf(file, "\nsmap");
    for (int i = 0; i < this->col; i++)
        fprintf(file, " %d", i);
    fprintf(file, "\n");
    fprintf(file, "----");
    for (int i = 0; i < this->col; i++)
        fprintf(file, "--");
    fprintf(file, "\n");
    for (int i = 0; i < this->row; i++)
    {
        fprintf(file, "%c | ", i + 'A');
        for (int j = 0; j < this->col; j++)
            if (flag_map == NULL)
                fprintf(file, "%2X", mmap[j + i * col]);
            else
                fprintf(file, "%c%X", (flag_map[j + i * col] ? '_' : ' '), mmap[j + i * col]);
        fprintf(file, "\n");
    }
}

int Game_Engine::GetDelay()
{
    return this->delay;
}