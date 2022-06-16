#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

int player;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

///////////////////////////////////////
//check if in the board
//up 0, upright 1, right 2, downright 3
//down 4, down left 5, left 6, upleft 7
//all dir 8

bool valid(int row, int col, int direction=8, int length = 0) {
    switch (direction) {
    case 0: // dir 0 up
        return row - length >= 0 ? true : false;
    case 1: // dir 1 upright
        return row - length >= 0 && col + length < SIZE ? true : false;
    case 2: // dir 2 right
        return col + length < SIZE ? true : false;
    case 3: // dir 3 downright
        return col + length < SIZE && row + length < SIZE ? true : false;
    case 4: // dir 4 down
        return row + length < SIZE ? true : false;
    case 5: // dir 5 downleft
        return row + length < SIZE && col - length >= 0 ? true : false;
    case 6: // dir 6 left
        return col - length >= 0 ? true : false;
    case 7: // dir 7 upleft 
        return col - length >= 0 && row - length >= 0 ? true : false;
    case 8: // all dir
        return row - length >= 0 && col + length < SIZE &&
               row + length < SIZE && col - length >= 0 ? true : false;
    }
    return false;
}
//state value function:

//direction: go to left first.
int dx[4][8] = {{1,2,3,4,-1,-2,-3,-4},
                {1,2,3,4,-1,-2,-3,-4},
                {0,0,0,0, 0, 0, 0, 0},
                {-1,-2,-3,-4,1,2,3,4}};
int dy[4][8] = {{ 0, 0, 0, 0,0,0,0,0},
                {-1,-2,-3,-4,1,2,3,4},
                {-1,-2,-3,-4,1,2,3,4},
                {-1,-2,-3,-4,1,2,3,4}};

int SVF(int x, int y, int player) {
    int opnt = 3 - player;
    int win5, alive4, die4, ddie4, alive3, jalive3, alive2, jalive2, die3, die2;
        win5= alive4= die4= ddie4= alive3= jalive3= alive2= jalive2= die3= die2=0;
    for (int dir = 0; dir < 4; dir++) { // dir0:"|"  dir1:"/"  dir2:"-"  dir3:"\"
        int left[5] = {opnt,opnt,opnt,opnt,opnt}, right[5]= {opnt,opnt,opnt,opnt,opnt};
        int len=1, l=1, r=1; // left or right reflect the chess near the cont. line
        bool stop = false;
        // | down is left, up is right (git ver3)
        for (int i = 0; i < 8; i++) {
            int traX = x+dx[dir][i], traY = y+dy[dir][i];
            if (valid(traX, traY)) { //in the board
                if (i==4) stop = false; //turn direction
                if (board[traX][traY] == player && !stop) {
                    len++;
                } else if (i<4) { //left
                    stop = true;
                    left[l++] = board[traX][traY];
                } else { //right
                    stop = true;
                    right[r++] = board[traX][traY];
                }
            }
        }
        //identify type (git ver3)
        if(len == 5) win5=1;
        //p p p p
        else if (len == 4) { 
            if(left[1] == 0 && right[1] == 0)                                                     alive4 ++;// 0 p p p p 0
            else if(left[1] == 0 || right[1] == 0)                                                  die4 ++;// 0 p p p p x
        //p p p
        } else if (len == 3) { 
            if (left[1] == 0 && right[1] == 0) {                                                            // 0 p p p 0
                if (left[2] == 0)                                                                  alive3++;// 0 0 p p p 0
                else if (right[2] == player)                                                        ddie4++;// p 0 p p p 0
                else                                                                               alive3++;// x 0 p p p 0 (x)
            } else if (left[1] == 0 || right[1] == 0) {                                                     // x p p p 0
                if ((left[1]==0 && left[2]==0) || (right[1]==0 && right[2]==0))                      die3++;// x p p p 0 0
                else                                                                                 die3++;// x p p p 0 x
            }
        //p p
        } else if (len == 2) {
            if (player == 2) continue; 
            // 0 p p 0
            if (left[1] == 0 && right[1]==0) { 
                if (left[2]==0 && right[2]==0)                                                     alive2++;// 0 0 p p 0 0
                else if ((left[2]==0 || right[2]==0) && ((left[2]==opnt || right[2]==opnt)))       alive2++;// 0 0 p p 0 x
                else if (left[2]==player && right[2]==player)                                      alive4++;// p 0 p p 0 p
                else if (left[2]==player || right[2]==player)                                     jalive3++;// p 0 p p 0 x或0
            // 0 p p x
            } else if (left[1]==0) { 
                if (left[2]==0) {                                                                           // 0 0 p p x
                    if (left[3]==0)                                                                  die2++;// 0 0 0 p p x
                    else if (left[3]==player)                                                        die2++;// p 0 0 p p x
                } else if (left[2] == player) {                                                             // p 0 p p x
                    if (left[3]==0)                                                               jalive3++;// 0 p 0 p p x
                    else if (left[3]==player)                                                        die4++;// p p 0 p p x
                }
            //x p p 0
            } else if (right[1]==0) { 
                if (right[2]==0) {                                                                          // x p p 0 0
                    if (right[3]==0)                                                                 die2++;// x p p 0 0 0
                    else if (right[3]==player)                                                       die2++;// x p p 0 0 p
                } else if (right[2] == player) {                                                            // x p p 0 p
                    if (right[3]==0)                                                              jalive3++;// x p p 0 p 0
                    else if (right[3]==player)                                                       die4++;// x p p 0 p p
                }
            }
        //p
        } else if (len == 1){
            if (!left[1] && !right[1]) { //0i0
                if ((left[2] == player && left[3] == player && left[4] == 0) || (right[2] == player && right[3] == player && right[4] == 0)) jalive3 ++;//0pp0i0
                else if ((left[2] == player && left[3] == player) || (right[2] == player && right[3] == player)) die3++; //pp0i0
                else if ((left[3] == 0 && left[2] == player) && (right[2] == 0 || left[4] == 0)) jalive2 ++; //0p0i0
                else if ((right[2] == 0 && right[4] == 0 && right[3] == player)||(left[2] == 0 && left[4] == 0 && left[3] == player)) jalive2 ++; //0i00p0
            }
           else if((left[1] == 0 && left[2] == player && left[3] == player && left[4] == player) || //ppp0i?
                 (right[1] == 0 && right[2] == player && right[3] == player && right[4] == player)) ddie4 ++;
           else if((left[1] == 0 && left[4] == 0 && left[2] == player && left[3] == player) ||    //0pp0i?
                   (right[1] == 0 && right[4] == 0 && right[2] == player && right[3] == player)){
                   die3 ++;
           }
           else if((left[1] == 0 && left[2] == 0 && left[3] == player && left[4] == player) ||   //pp00i?
                   (right[1] == 0 && right[2] == 0 && right[3] == player && right[4] == player)){
                   die3 ++;
           } 
           else if((left[1] == 0 && left[3] == 0 && left[2] == player && left[4] == player) ||   //p0p0i?
                   (right[1] == 0 && right[3] == 0 && right[2] == player && right[4] == player)){
                   die3 ++;
           }
           else if((left[1] == 0 && right[1] == 0 && right[3] == 0 && right[2] == player) && (left[2] == 0 || right[4] == 0)){ //0i0p0
                   jalive2 ++;
           }
           else if((left[1] == 0 && left[3] == 0 && left[4] == 0 && left[2] == player) || // 00p0?
                   (right[1] == 0 && right[3] == 0 && right[4] == 0 && right[2] == player)){
                   die2 ++;
           }
           else if((left[1] == 0 && right[1] == 0 && right[2] == 0 && left[2] == player) || // p0?00
                   (right[1] == 0 && left[1] == 0 && left[2] == 0 && right[2] == player)){
                   die2 ++;
           }
           else if((left[1] == 0 && left[2] == 0 && left[4] == 0 && left[3] == player) || // 0p00?
                   (right[1] == 0 && right[2] == 0 && right[4] == 0 && right[3] == player)){
                   die2 ++;
           }
           else if((left[1] == 0 && left[2] == 0 && right[1] == 0 && left[3] == player) || // p00?0
                   (right[1] == 0 && right[2] == 0 && left[1] == 0 && right[3] == player)){
                   die2 ++;
           }
           else if((left[1] == 0 && left[2] == 0 && left[3] == 0 && left[4] == player) || // p000?
                   (right[1] == 0 && right[2] == 0 && right[3] == 0 && right[4] == player)){
                   die2 ++;
           }
           else;
        }
    //identify chess board over
    //win, alive4, die4, ddie4, alive3, die3, ddie3, alive2, bad_alive2, bbad_alive2, die2, ddie2, normal; 
    } 
    if (win5 >= 1) {
        if (player == 1) return 1594323; //win first, don't have to care opnt
        else return 531441;
    }
    if (alive4 >= 1 || die4 >= 2 || (die4 >= 1 && alive3 >= 1)) return 177147;
    if (alive3 >= 2) return 59049;
    if (die3 >= 1 && alive3 >= 1) return 19683;
    if (die4 >= 1) return 6561;
    if (ddie4 >= 1) return 2187;
    if (alive3 >= 1) return 729;
    if (jalive3 >= 1) return 243;
    if (alive2 >= 2) return 81;
    if (alive2 >= 1) return 27;
    if (jalive2 >= 1) return 9;
    if (die3 >= 1) return 3;
    if (die2 >= 1) return 1;
    return 0; 
}

void write_valid_spot(std::ofstream& fout) {
    srand(time(NULL));
    // Keep updating the output until getting killed.
    int val[SIZE][SIZE], max=0;
    int best_x=SIZE/2, best_y=SIZE/2;
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            if (board[x][y] == EMPTY) {
                val[x][y] = std::max(SVF(x,y,1),SVF(x,y,2)+1)-1;
            } else val[x][y] = -1;
            if (val[x][y]>max) {
                max = val[x][y];
                best_x = x;
                best_y = y;
            } 
            //print
            if (val[x][y]>=10000) std::cout<<val[x][y]<<" ";
            else if (val[x][y]>=1000) std::cout<<"  "<<val[x][y]<<" ";
            else if (val[x][y]>=100) std::cout<<"  "<<val[x][y]<<" ";
            else if (val[x][y]>=10) std::cout<<"   "<<val[x][y]<<" ";
            else if (val[x][y]>=0) std::cout<<"    "<<val[x][y]<<" ";
            else if (val[x][y]==-1 && board[x][y]==1) std::cout<<"    P"<<" ";
            else if (val[x][y]==-1 && board[x][y]==2) std::cout<<"    X"<<" ";
            else std::cout<<" "<<val[x][y]<<" ";
        }
        std::cout<<std::endl;
    }
    fout << best_x << " " << best_y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
