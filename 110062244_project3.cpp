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
bool valid(int row, int col, int direction, int length = 4) {
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
    case 8:
        return row - length >= 0 && col + length < SIZE &&
               row + length < SIZE && col - length >= 0 ? true : false;
    }
    return false;
}
//state value function:
int SVF(int row, int col, int player) {
    //if (!valid(row,col,8,2)) return 0;
    int opnt = 3 - player;
    int win, alive4, die4, ddie4, alive3, die3, ddie3, alive2, bad_alive2, bbad_alive2, die2, ddie2, normal;
    win=alive4=die4=ddie4=alive3=die3=ddie3=alive2=bad_alive2=bbad_alive2=die2=ddie2=normal=0;
    for (int dir = 0; dir < 4; dir++) {
        int left[5], right[5], len=1; // left or right reflect the chess near the cont. line
        int l=1, r=1;
        bool l_stop = false, r_stop = false;
        for (int i = 1; i <= 4; i++) {
            // dir0:"|"  dir1:"/"  dir2:"-"  dir3:"\"
            // | down is left, up is right
            switch (dir) {
            case 0:
                if (valid(row, col, 4, i)) {
                    if (board[row+i][col] == player && !l_stop) len++;
                    else l_stop = true;
                    if (l_stop) left[l++] = board[row+i][col];
                }
                if (valid(row, col, 0, i)) {
                    if (board[row-i][col] == player && !r_stop) len++;
                    else r_stop = true;
                    if (r_stop) right[r++] = board[row-i][col];
                }
                break;
            case 1:
                if (valid(row, col, 5, i)) {
                    if (board[row+i][col-i] == player && !l_stop) len++;
                    else l_stop = true;
                    if (l_stop) left[l++] = board[row+i][col-i];
                }
                if (valid(row, col, 1, i)) {
                    if (board[row-i][col+i] == player && !r_stop) len++;
                    else r_stop = true;
                    if (r_stop) right[r++] = board[row-i][col+i];
                }
                break;
            case 2:
                if (valid(row, col, 6, i)) {
                    if (board[row][col-i] == player && !l_stop) len++;
                    else l_stop = true;
                    if (l_stop) left[l++] = board[row][col-i];
                }
                if (valid(row, col, 2, i)) {
                    if (board[row][col+i] == player && !r_stop) len++;
                    else r_stop = true;           
                    if (r_stop) right[r++] = board[row][col+i];
                }
                break;
            case 3:
                if (valid(row, col, 7, i)) {
                    if (board[row-i][col-i] == player && !l_stop) len++;
                    else l_stop = true;
                    if (l_stop) left[l++] = board[row-i][col-i];
                }
                if (valid(row, col, 3, i)) {
                    if (board[row+i][col+i] == player && !r_stop) len++;
                    else r_stop = true;            
                    if (r_stop) right[r++] = board[row+i][col+i];

                }
                break;
            }
        }
        //identify type
        if(len == 5) win=1;
        //p p p p
        else if (len == 4) { 
            if(left[1] == 0 && right[1] == 0) alive4 ++; //  0 p p p p 0
            else if(left[1] == 0 || right[1] == 0) die4 ++; // 0 p p p p x
            else normal++; 
        //p p p
        } else if (len == 3) { 
            if (left[1] == 0 && right[1] == 0) { // 0 p p p 0
                if (left[2] == 0 || right[2] == 0) alive3++; // 0 0 p p p 0
                else ddie3++;
            } else if (left[1] == 0 || right[1] == 0) { // x p p p 0
                if ((left[1]==0 && left[2]==0) || (right[1]==0 && right[2]==0)) die4++; // x p p p 0 0
                else ddie4++; // x p p p 0 x
            } else normal++; // x p p p x
        //p p
        } else if (len == 2) {
            if (player == 2) continue; 
            // 0 p p 0
            if (left[1] == 0 && right[1]==0) { 
                if (left[2]==0 && right[2]==0) alive2++;                                                    // 0 0 p p 0 0
                else if ((left[2]==0 || right[2]==0) && ((left[2]==opnt || right[2]==opnt))) bad_alive2++;  // 0 0 p p 0 x
                else if (left[2]==opnt && right[2]==opnt) ddie2++;                                          // x 0 p p 0 x
                else if (left[2]==player && right[2]==player) alive4++;                                     // p 0 p p 0 p
                else if (left[2]==player || right[2]==player)  alive3++;                                   // p 0 p p 0 x或0
            // 0 p p x
            } else if (left[1]==0) { 
                if (left[2]==0) { // 0 0 p p x
                    if (left[3]==0) die2++;                                                                 // 0 0 0 p p x
                    else if (left[3]==player) die2++;                                                       // p 0 0 p p x
                    else ddie2++;                                                                           // x 0 0 p p x
                } else if (left[2] == player) { // p 0 p p x
                    if (left[3]==0) alive3++;                                                               // 0 p 0 p p x
                    else if (left[3]==player) alive4++;                                                     // p p 0 p p x
                    else ddie3++;                                                                           // x p 0 p p x
                }
            //x p p 0
            } else if (right[1]==0) { 
                if (right[2]==0) { // x p p 0 0
                    if (right[3]==0) die2++;                                                                // x p p 0 0 0
                    else if (right[3]==player) die2++;                                                      // x p p 0 0 p
                    else ddie2++;                                                                           // x p p 0 0 x
                } else if (right[2] == player) { // x p p 0 p
                    if (right[3]==0) alive3++;
                    else if (right[3]==player) alive4++;
                    else ddie3++;
                }
            } else normal++; // x p p x
        //p
        } else if (len == 1) { 
            if (player == 2) continue;
            // 0 p 0
            if (right[1]==0 && left[1]==0) { 
                if (right[2]==0 && left[2]==0) { // 0 0 p 0 0
                    if (right[3]==0 || left[3]==0) normal++;                                                 // 0 0 p 0 0 0
                } else if (right[2]==0) {        // x 0 p 0 0
                    if (right[3]==0) normal++;
                    if (right[3]==player) alive2++;
                    else ddie2++;
                } else if (left[2]==0) {         // 0 0 p 0 x
                    if (left[3]==0) normal++;
                    if (left[3]==player) alive2++;
                    else ddie2++;
                }
            }
            // x p 0
            else if (right[1]==0) {
                if (right[2]==0) { // x p 0 0
                    if (right[3]==0) normal++;  // x p 0 0 0
                    else if (right[3]==player) { // x p 0 0 p
                        if (right[4]==0) alive2++;
                        else if (right[4]==player) die3++;
                        else ddie2++;
                    } else normal++; // x p 0 0 x
                } else if (right[2]==player) { // x p 0 p
                    if (right[3]==0) { // x p 0 p 0
                        if (right[4]==0) die2++;
                        else if (right[4]==player) die3++;
                        else ddie2++;
                    } else if (right[3]==player) { // x p 0 p p
                        if (right[4]==0) die3++;// x p 0 p p 0
                        else if (right[4]==player) alive4++; // x p 0 p p p
                        else ddie3++;   // x p 0 p p x
                    } else ddie2++; // x p 0 p x
                } else normal++;// x p 0 x
            }
            // 0 p x
            else if (left[1]==0) {
                if (left[2]==0) {
                    if (left[3]==0) normal++;
                    else if (left[3]==player) {
                        if (left[4]==0) alive2++;
                        else if (left[4]==player) die3++;
                        else ddie2++;
                    } else normal++;
                } else if (left[2]==player) {
                    if (left[3]==0) {
                        if (left[4]==0) die2++;
                        else if (left[4]==player) die3++;
                        else ddie2++;
                    } else if (left[3]==player) {
                        if (left[4]==0) die3++;
                        else if (left[4]==player) alive4++;
                        else ddie3++;
                    } else ddie2++;
                } else normal++;
            }
        }
    } //score over
    //win, alive4, die4, ddie4, alive3, die3, ddie3, alive2, bad_alive2, bbad_alive2, die2, ddie2, normal;  
    if (win >= 1) {
        if (player == 1) return 100; //win first, don't have to care opnt
        else return 60;
    }
    if (alive4 >= 1 || die4 >= 2 || (die4 >= 1 && alive3 >= 1)) return 40;
    if (alive3 >= 2) return 20;
    if (die3 >= 1 && alive3 >= 1) return 13;
    if (die4 >= 1) return 12;
    if (ddie4 >= 1) return 11;
    if (alive3 >= 1) return 10;
    if (alive2 >= 2) return 9;
    if (alive2 >= 1) return 8;
    if (die3 >= 1) return 7;
    if (ddie3 >= 1) return 6;
    if (bad_alive2 >=1) return 4;
    if (bbad_alive2 >= 1) return 3;
    if (die2 >= 1) return 2;
    if (ddie2 >= 1) return 1;
    return 0; 
}

int N_SVF (std::array<std::array<int, SIZE>, SIZE> MAP, int player) {
    int opnt = 3 - player;
    int value = 0;
    int xd[3][4] = {{0,0,0,0},
                    {1,2,3,4},
                    {1,2,3,4}};
    int yd[3][4] = {{1,2,3,4},
                    {1,2,3,4},
                    {0,0,0,0}};
    for (int x = 0; x < SIZE; x++) {
    for (int y = 0; y < SIZE; y++) {
    if (MAP[x][y]==player) {
    for (int dir = 0; dir < 3; dir++) {
        if (valid(x, y, dir+2)) {  //right -> downright -> down
            if (MAP[ x + xd[dir][0] ][ y + yd[dir][0] ] == EMPTY) {                                            // p .
                if (MAP[ x + xd[dir][1] ][ y + yd[dir][1] ] == EMPTY) {                                        // . p .
                    if (MAP[ x + xd[dir][2] ][ y + yd[dir][2] ] == EMPTY) {                                    // . p . .
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY) {}                               // . p . . .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {}                       // . p . . p
                        else {}                                                                                // . p . . o
                    }
                    else if (MAP[ x + xd[dir][2] ][ y + yd[dir][2] ] == player) {                              // . p . p
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 0 ;}           // . p . p .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 0 ;}           // . p . p p
                        else {}                                                                                // . p . p o
                    }
                    else {                                                                                     // . p . o
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY) {}                               // . p . o .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {}                       // . p . o p
                        else {}                                                                                // . p . o o
                    }
                }
                else if (MAP[ x + xd[dir][1] ][ y + yd[dir][1] ] == player) {                                  // . p p
                    if (MAP[ x + xd[dir][2] ][ y + yd[dir][2] ] == EMPTY) {                                    // . p p .
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 0 ;}           // . p p . .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 0 ;}           // . p p . p
                        else {}                                                                                // . p p . o
                    }
                    else if (MAP[ x + xd[dir][2] ][ y + yd[dir][2] ] == player) {                              // . p p p
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 1 ;}           // . p p p .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 3 ;}           // . p p p p
                        else                                                          {value += 2 ;}           // . p p p o
                    }
                    else {                                                                                     // . p p o
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 0 ;}           // . p p o .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 0 ;}           // . p p o p
                        else                                                          {value += 0 ;}           // . p p o o
                    }
                }
                else {}                                                                                        // . p o 
            }
            else if (MAP[ x + xd[dir][0] ][ y + yd[dir][0] ] == player) {                                      // p p
                if (MAP[ x + xd[dir][1] ][ y + yd[dir][1] ] == EMPTY) {                                        // p p .
                    if (MAP[ x + xd[dir][2] ][ y + yd[dir][2] ] == EMPTY) {                                    // p p . .
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 1 ;}           // p p . . .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 1 ;}           // p p . . p
                        else                                                          {value += 1 ;}           // p p . . o
                    }
                    else if (MAP[ x + xd[dir][2] ][ y + yd[dir][2] ] == player) {                              // p p . p
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 1 ;}           // p p . p .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 7 ;}           // p p . p p
                        else                                                          {value += 4 ;}           // p p . p o
                    }
                    else {                                                                                     // p p . o
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 0 ;}           // p p . o .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 0 ;}           // p p . o p
                        else                                                          {value += 0 ;}           // p p . o o
                    }
                }
                else if (MAP[ x + xd[dir][1] ][ y + yd[dir][1] ] == player) {                                  // p p p
                    if (MAP[ x + xd[dir][2] ][ y + yd[dir][2] ] == EMPTY) {                                    // p p p .
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 6 ;}           // p p p . .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 8 ;}           // p p p . p
                        else                                                          {value += 4 ;}           // p p p . o
                    }
                    else if (MAP[ x + xd[dir][2] ][ y + yd[dir][2] ] == player) {                              // p p p p
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 9 ;}           // p p p p .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 9 ;}           // p p p p p
                        else                                                          {value += 0 ;}           // p p p p o
                    }
                    else {                                                                                     // p p p o
                        if (MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 0 ;}           // p p p o .
                        else if ((MAP[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 0 ;}           // p p p o p
                        else                                                          {value += 0 ;}           // p p p o o
                    }
                }
                else {}                                                                                        // p p o 
            }
            else {}                                                                                            // p o
        }
    }}}}
    return value;
}
void write_valid_spot(std::ofstream& fout) {
    srand(time(NULL));
    // Keep updating the output until getting killed.
    ////////////////////////////////////////////////
    /*
    while(true) {
        // Choose a random spot. 
        int x = (rand() % SIZE);
        int y = (rand() % SIZE);
        if (board[x][y] == EMPTY) {
            fout << x << " " << y << std::endl;
            // Remember to flush the output to ensure the last action is written to file.
            fout.flush();
        }
    }
    */
    ////////////////////////////////////////////////
    //while(true) {
    int val[SIZE][SIZE], max=1;
    int best_x=SIZE/2, best_y=SIZE/2; // && !(x==y && x==SIZE/2) 
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            if (board[x][y] == EMPTY) {
                std::array<std::array<int, SIZE>, SIZE> MAP = board;
                MAP[x][y] = 1;
                val[x][y] = N_SVF(MAP,1) - N_SVF(MAP,2);
            }
            else val[x][y] = -1;
            if (val[x][y]>max) {
                max = val[x][y];
                best_x = x;
                best_y = y;
            } 
            if (val[x][y]>=10) std::cout<<val[x][y]<<" ";
            else if (val[x][y]==-1 && board[x][y]==1) std::cout<<" O"<<" ";
            else if (val[x][y]==-1 && board[x][y]==2) std::cout<<" X"<<" ";
            else std::cout<<" "<<val[x][y]<<" ";
        }
        std::cout<<std::endl;
    }
    fout << best_x << " " << best_y << std::endl;
    fout.flush();
    //}
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
