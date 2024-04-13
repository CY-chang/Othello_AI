#include <iostream>
#include <algorithm>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>


struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

int root_player;
const int size = 8;
std::array<std::array<int, size>, size> root_board;
std::vector<Point> root_next_valid_spots;
int minimax_depth = 5;
std::array<Point, 4> X_spot{Point(1, 1), Point(1, 6), Point(6, 1), Point(6, 6)};
std::array<Point, 4> corner_spot{Point(0, 0), Point(0, 7), Point(7, 0), Point(7, 7)};
std::array<Point, 8> C_spot{Point(0, 1), Point(0, 6), Point(7, 1), Point(7, 6), Point(1, 0), Point(1, 7), Point(6, 0), Point(6, 7)};
std::array<Point, 8> A_spot{Point(0, 2), Point(0, 5), Point(7, 2), Point(7, 5), Point(2, 0), Point(2, 7), Point(5, 0), Point(5, 7)};

class node{
    public:

    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;

    node(std::array<std::array<int, SIZE>, SIZE> board){
        int cnt0=0,cnt1=0,cnt2=0;
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                this->board[i][j]=board[i][j];
                if(board[i][j]==EMPTY)
                cnt0++;
                else if(board[i][j]==BLACK)
                cnt1++;
                else if(board[i][j]==WHITE)
                cnt2++;
            }
        }
        cur_player=root_player;
        done=false;
        winner=-1;
        for(int i=0;i<root_next_valid_spots.size();i++){
            next_valid_spots.push_back(root_next_valid_spots[i]);
        }
        disc_count[0]=cnt0;
        disc_count[1]=cnt1;
        disc_count[2]=cnt2;
    }

    node(const node& cmp){
        for(int i=0;i<SIZE;i++){
            for(int j=0;j<SIZE;j++){
                board[i][j]=cmp.board[i][j];
            }
        }
        for(int i=0;i<cmp.next_valid_spots.size();i++){
            next_valid_spots.push_back(cmp.next_valid_spots[i]);
        }
        for(int i=0;i<3;i++)
        disc_count[i]=cmp.disc_count[i];
        cur_player=cmp.cur_player;
        done=cmp.done;
        winner=cmp.winner;
    }

    
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }

    int evaluate() {
        double state_value = 0;
        int opponent = get_next_player(root_player);

        state_value += disc_count[root_player] - disc_count[opponent];
        
        int saved_player = cur_player;

        cur_player = root_player;
        state_value += (int)get_valid_spots().size() * 4;
        cur_player = opponent;
        state_value -= (int)get_valid_spots().size() * 4;

        cur_player = saved_player;

        for (auto pos: corner_spot){
            if(get_disc(pos) == root_player) state_value += 50;
            if(get_disc(pos) == opponent) state_value -= 50;
        }
        for (auto pos: X_spot) {
            if (get_disc(pos) == root_player) state_value -= 30;
            else if (get_disc(pos) == opponent) state_value += 30;
        }
        for (int i = 0; i < 8; i++) {
            if (get_disc(corner_spot[i % 4]) == EMPTY) {
                if (get_disc(A_spot[i]) == root_player && get_disc(C_spot[i]) == opponent)
                    state_value+=30;
                if (get_disc(C_spot[i]) == root_player && get_disc(A_spot[i]) == opponent)
                    state_value-=30;
            }
        }
        return state_value;
    }
};  



void read_board(std::ifstream& fin) {
    fin >> root_player;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fin >> root_board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        root_next_valid_spots.push_back({x, y});
    }
}


void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = root_next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = root_next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int minimax(node cur,int depth,int alpha,int beta,bool maximize,std::ofstream& fout){     
    if (depth == 0){
        return cur.evaluate();
    }
    if (cur.done){
        if (cur.winner == root_player) return 10000;
        else return -10000;
    }
    if (maximize) {
        for (int i = 0; i < (int)cur.next_valid_spots.size(); i++) {   
            node next_state = cur;
            next_state.put_disc(cur.next_valid_spots[i]);
            int value = minimax(next_state, depth - 1, alpha, beta, false, fout);
            if (value > alpha) {
                alpha = value;
                if (depth == minimax_depth) {
                    fout << cur.next_valid_spots[i].x << " " << cur.next_valid_spots[i].y << std::endl;
                    fout.flush();
                }
            }
            if (alpha >= beta) break;
        }
        return std::min(alpha, beta);
    }
    else {
        for (int i = 0; i < (int)cur.next_valid_spots.size(); i++) {   
            node next_state = cur;
            next_state.put_disc(cur.next_valid_spots[i]);
            int value = minimax(next_state, depth - 1, alpha, beta, true, fout);
            if (value < beta) beta = value;
            if (alpha >= beta) break;
        }
        return std::max(alpha, beta);
    }
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);

    //write_valid_spot(fout);
    node cur_state(root_board);
    minimax(cur_state, minimax_depth, INT_MIN, INT_MAX, true, fout);
    
    fin.close();
    fout.close();
    return 0;
}