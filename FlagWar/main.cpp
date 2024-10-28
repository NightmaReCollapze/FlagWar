/**
* FlagWar
* by WUNAN frEd
* 
* 在一个n*m的矩形地图中有两个玩家(使用字母A和B)分别在地图的左上角与右下角，
* 两个玩家每回合可以通过输入w/a/s/d加一个数字(<=3)来决定自己如何移动（如d1表示向右移动一格，
* w2表示向上移动两格）。
* 每场游戏的开局玩家可以选择将自己队伍的旗帜放在地图自己那半边（对角线分割）
* 除玩家出生点外的任意一个位置，当两名玩家都选择好旗帜位置后开始游戏。游戏的目标是夺取对方的旗帜
* （即将自己移动到对手旗帜的位置上），当一名玩家移动到另一名玩家上方时，后者会被“杀死”，
* 当玩家的旗帜还在时，被”杀死“后会回到出生点，而当旗帜被夺取后被杀死即游戏结束。
* 此外，还有分数机制。杀死玩家+10, 被杀死-7，夺取到旗帜+50
* In an n*m rectangular map there are two players (using the letters A and B)
* in the upper left and lower right corner of the map,
* Two players can decide how they want to move each turn by entering
* w/a/s/d plus a number (<=3) (e.g. d1 means move one space to the right,
* w2 means move up two Spaces).
* At the beginning of each game, players can choose to place their team's flag on their side of the map (split diagonally).
* Any location other than the player's birth point, when both players have selected
* the flag position to start the game. The object of the game is to capture the opponent's flag
* (i.e. move yourself to the opponent's flag), when a player moves over another player, the latter is "killed",
* When the player's flag is still there, being "killed" will return to the birth point, 
* and when the flag is captured and killed, the game ends.
* In addition, there is a scoring mechanism. Kill +10, get killed -7, capture flag +50
*
* 241026
* 241027 demo1 - 胜负判定 结算
*/
#include <iostream>
#include <vector>
#include <utility> // pair
#include <String>
#include <Windows.h> // WindowsAPI
#include <algorithm>
#include <cctype> //check type

using namespace std;

const int map_sizeX = 10, map_sizeY = 15;
char map[map_sizeX][map_sizeY] = {{{' '}}};
const int max_move_dist = 3;
const char map_space = '~';

// Player class is better to write in Player.h, 
// but in order to show the program conveniently I put it here.
class Player
{
private:
	string name;
	char sign;
	bool flag;
	char flagSign;
	int Treasure_num;
	bool Treasure_use;
	int score;
	pair<int, int> kill_die;
	pair<int, int> pos; // position
	pair<int, int> flag_pos;
	pair<int, int> init_pos;

public:
	bool out; // lose when true

	Player(string name, char sign, char flagSign,
		pair<int, int> init_pos, pair<int, int> flag_pos)
	{
		this->name = name;
		this->sign = sign;
		this->flagSign = flagSign;
		this->init_pos = init_pos;
		this->pos = init_pos;
		this->flag_pos = flag_pos;
		score = 0;
		kill_die = { 0,0 };
		flag = true;
		out = false;
		Treasure_num = 0;
		Treasure_use = false;
	}
	~Player()
	{
		// to be complete if needed in the future
	}

	//accessors
	char getSign()
	{
		return sign;
	}
	string getName()
	{
		return name;
	}
	int getTreasNum()
	{
		return Treasure_num;
	}
	pair<int, int> getPos()
	{
		return pos;
	}
	char getFlagSign()
	{
		return flagSign;
	}
	pair<int, int> getInitPos()
	{
		return init_pos;
	}
	pair<int, int> getFlagPos()
	{
		return flag_pos;
	}
	pair<int, int> getKD()
	{
		return kill_die;
	}
	bool getFlagStatus()
	{
		return flag;
	}
	int getScore()
	{
		return score;
	}

	//mutators
	void setPos(pair<int, int> pos)
	{
		this->pos = pos;
	}
	void addTreasure()
	{
		Treasure_num += 1;
	}
	void flagBeenCaptured()
	{
		flag = false;
		flagSign = map_space;
	}
	void reborn_my_flag_lol()
	{
		flag = true;
	}
	void setKD(pair<int, int> KD)
	{
		kill_die = KD;
	}
	void addScore(int score)
	{
		this->score = score;
	}

	bool useTreasure()
	{
		if (!Treasure_use && Treasure_num >= 1)
		{
			Treasure_use = true;
			Treasure_num--;
			return true;
		}
		else
			return false;
	}
	void die()
	{
		if (flag)
			pos = init_pos;
		else
		{
			pos = init_pos;
			out = true;
		}	
	}
	bool move(char orin, int num, char gameMap[map_sizeX][map_sizeY], Player& anotherPlayer)
	{ 
		int newX = pos.first, newY = pos.second;
		switch (orin) 
		{
		case 'w':
			newX -= num;
			break;
		case 's':
			newX += num;
			break;
		case 'a':
			newY -= num;
			break;
		case 'd':  
			newY += num;
			break;
		default:
			return false;
		}
		if (newX < 0 || newX >= map_sizeX || newY < 0 || newY >= map_sizeY ||
			num <= 0 || num > max_move_dist)
			return false;
		pos.first = newX;
		pos.second = newY;
		return true;
	}
};

void mapInit()
{
	// map init.
	for (int i = 0; i < map_sizeX; ++i)
	{
		for (int j = 0; j < map_sizeY; ++j)
			map[i][j] = map_space;
	}
}

bool containsSpace(const string& str) 
{
	return str.find(' ') != string::npos || str==""; // name should not contain space or is null
}

bool isValidPos(int x, int y)
{
	return (x-1 >= 0 && x-1 < map_sizeX && y-1 >= 0 && y-1 < map_sizeY && 
		!(x == 1 && y == 1) && !(x == map_sizeX - 1 && y == map_sizeY - 1));
}
bool isValidPos(int x, int y, pair<int, int> ano_flag)
{
	return (x - 1 >= 0 && x - 1 < map_sizeX && y - 1 >= 0 && y - 1 < map_sizeY &&
		!(x == 1 && y == 1) && !(x == map_sizeX - 1 && y == map_sizeX - 1) && !(x == ano_flag.first && y == ano_flag.second));
}
void showLoading(string text)
{
	system("cls");
	for (int i = 0; i <= 20; ++i)
	{
		system("cls");
		cout << text << endl;
		cout << "<";
		for (int k = 0; k <= i; ++k)
			cout << "=";
		for (int k = 0; k <= 20 - i - 1; ++k)
			cout << " ";
		cout << ">";
		Sleep(30);	
	}
}

// return 2: use the treasure
pair<bool, pair<char, int>> doMove(string str)
{
	if (str == "@")
	{
		return { true, {'n', 0} };
	}

	char letter = str[0];
	char digitChar = str[1];
	if (str.length() != 2 || !isalpha(letter) || !isdigit(digitChar))
	{
		throw "invalid format of moving!";
	}
	int number = digitChar - '0'; // Convert char digit to int  
	if (number > max_move_dist)
	{
		throw "invalid distance of moving!";
	}
	return { false, {letter, number} };
}
void drawMap(Player& p1, Player& p2)
{
	mapInit();

	// draw the map
	// draw the flag first to prevent flags covering players
	map[p1.getFlagPos().first][p1.getFlagPos().second] = p1.getFlagSign();
	map[p2.getFlagPos().first][p2.getFlagPos().second] = p2.getFlagSign();
	map[p1.getPos().first][p1.getPos().second] = p1.getSign();
	map[p2.getPos().first][p2.getPos().second] = p2.getSign();
	for (int i = 0; i < map_sizeX; ++i)
	{
		for (int j = 0; j < map_sizeY; ++j)
		{
			cout << map[i][j];
		}
		cout << endl;
	}
}
void statusUpdate(Player& p, Player& p_ano)
{
	if (map[p.getPos().first][p.getPos().second] == '@')
	{
		p.addTreasure();
		p.addScore(20);
		map[p.getPos().first][p.getPos().second] = map_space;
	}
	if (map[p.getPos().first][p.getPos().second] == p_ano.getSign())
	{
		p.setKD({ p.getKD().first + 1, p.getKD().second });
		p.addScore(10);
		p_ano.setKD({ p_ano.getKD().first, p.getKD().second + 1});
		p_ano.addScore(-7);
		p_ano.die();
	}	
	if (map[p.getPos().first][p.getPos().second] == p_ano.getFlagSign())
	{
		p.addScore(50);
		p_ano.flagBeenCaptured(); 
	}
	if (map[p.getPos().first][p.getPos().second] == p.getFlagSign() && p.getFlagStatus())
	{
		p.setPos(p.getInitPos());
	}
}

void win(Player& winer, Player& loser)
{
	Sleep(1000);
	system("cls");
	system("color 64");
	cout << "\n\n\t\t\t" << winer.getName() << "WIN THE WAR!!" << endl;
	auto get_kdr = [](pair<int, int> kd) -> double // lambda expression
		{
			return kd.second == 0 ? kd.first : ((double)kd.first / (double)kd.second);
		};
	cout << "\n\nSUMMARY: " << endl;
	cout << winer.getName() << ": " << winer.getKD().first << "-" <<
		winer.getKD().second << " (" << get_kdr(winer.getKD()) << ")" << 
		"["<<winer.getScore() << "]" << endl;
	cout << loser.getName() << ": " << loser.getKD().first << "-" <<
		loser.getKD().second << " (" << get_kdr(loser.getKD()) << ")" <<
		"[" << winer.getScore() << "]" << endl;
	cout << endl;

	double kdr_diff = get_kdr(winer.getKD()) - get_kdr(loser.getKD());
	if (kdr_diff == 0)
		cout << "KDRMVP: NECK AND NECK! 势均力敌!" << endl;
	else if(kdr_diff > 0)
		cout << "KDRMVP:" << winer.getName() << endl;
	else cout << "KDRMVP:" << loser.getName() << endl;
	
	double score_diff = winer.getScore() - loser.getScore();
	if (score_diff == 0)
		cout << "ScoringMVP: NECK AND NECK! 势均力敌!" << endl;
	else if (kdr_diff > 0)
		cout << "ScoringMVP:" << winer.getName() << endl;
	else cout << "ScoringMVP:" << loser.getName() << endl;

	Sleep(3000);
	system("color 75");
	Sleep(1000);
	system("color 86");
	Sleep(1000);
	system("color 71");
	Sleep(1000);
	system("pause");
}

int main()
{
	cout << "\t\t\tWelcome to FLAGWAR!!" << endl;
	showLoading("Welcome to FLAGWAR!!");
	Sleep(1000); // wait for 1000ms (WindowsAPI)
	system("cls"); // run cmd command "cls" clear screen (WinodwsAPI)

	// P1 init.
	system("color 3");
	string name = "";
	do
	{
		cout << "Input P1 name: (should not contain space) ";
		getline(cin, name);
		cout << endl;
	} while (containsSpace(name)); // validation
	pair<int, int> pf_pos;
	do
	{
		cout << "Input the position of " << name << "'s flag: " << endl;
		cout << "1 <= x y <= 9 (1 1 or 9 9 are not allowed)" << endl;
		cin >> pf_pos.first >> pf_pos.second;
	} while (!isValidPos(pf_pos.first, pf_pos.second)); // validation
	pf_pos.first--; // correspond to array
	pf_pos.second--;
	Player* p1 = new Player(name, '1', 'A', { 0, 0 }, pf_pos);
	system("cls");
	auto nth = getchar(); // use this to prevent cin mishandle the last input

	// P2 init.
	system("color 6");
	do
	{
		cout << "Input P2 name: (should not contain space) ";
		getline(cin, name);
		cout << endl;
	} while (containsSpace(name)); // validation
	pair<int, int> p2f_pos;
	do
	{
		cout << "Input the position of " << name << "'s flag: " << endl;
		cout << "1 <= x y <= 9 (1 1 or 9 9 are not allowed)" << endl;
		cin >> p2f_pos.first >> p2f_pos.second;
	} while (!isValidPos(p2f_pos.first, p2f_pos.second, pf_pos)); // validation
	p2f_pos.first--; // correspond to array
	p2f_pos.second--;
	Player* p2 = new Player(name, '2', 'B', { map_sizeX-1, map_sizeY-1 }, p2f_pos);
	system("cls");
	nth = getchar(); // use this to prevent cin mishandle the last input

	system("color 7");
	showLoading(p1->getName() + " vs. " + p2->getName());
	system("cls");

	bool isP1 = true;
	string move = "";
	int treas_use_cnter = 0;
	while (1) // start the main game loop
	{
		drawMap(*p1, *p2);
		cout << "\n\n";
		cout << (isP1 ?
			("> P1: " + p1->getName() + "(" + (to_string(p1->getFlagStatus())) + ")" +
				"\nP2: " + p2->getName() + "(" + to_string(p2->getFlagStatus()) + ")") :
			("> P2: " + p2->getName() + "(" + to_string(p2->getFlagStatus()) + ")" +
				"\nP1: " + p1->getName() + "(" + to_string(p1->getFlagStatus()) + ")")) << endl;
		cout << "\n\nmove: ";
		
		try
		{
			getline(cin, move);
			auto [use_treas, tmp_move] = doMove(move); // need C++17
			if (use_treas)
			{
				treas_use_cnter = 2;
				system("cls");
				continue;
			}
			else
			{
				if (!(isP1 ? p1->move(tmp_move.first, tmp_move.second, map, *p2) :
					p2->move(tmp_move.first, tmp_move.second, map, *p1)))
				{
					throw "invalid move!";
					cout << "re-enter your move!";
					Sleep(1000); // time for user to read
					system("cls");
					continue;
				}

				isP1 ? statusUpdate(*p1, *p2) : statusUpdate(*p2, *p1);

				if ((*p1).out)
				{
					win(*p2, *p1);
					break;
				}		
				if ((*p2).out)
				{
					win(*p1, *p2);
					break;
				}
					
				if (treas_use_cnter <= 1)
					isP1 = !isP1;
				else treas_use_cnter--;
			}
		}
		catch (const char* err)
		{
			cerr << err << endl;
			Sleep(1000);
		}
		system("cls");
	}
	delete p1, p2;
	return 0;
}