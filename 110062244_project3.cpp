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
    int win5, alive4, die4, ddie4, alive3, jalive3, alive2, jalive2, die3, die2;
        win5= alive4= die4= ddie4= alive3= jalive3= alive2= jalive2= die3= die2=0;
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
        if(len == 5) win5=1;
        //p p p p
        else if (len == 4) { 
            if(left[1] == 0 && right[1] == 0)                                                     alive4 ++;// 0 p p p p 0
            else if(left[1] == 0 || right[1] == 0)                                                  die4 ++;// 0 p p p p x
            else;                                                                                           // x p p p p x
        //p p p
        } else if (len == 3) { 
            if (left[1] == 0 && right[1] == 0) {                                                            // 0 p p p 0
                if (left[2] == 0)                                                                  alive3++;// 0 0 p p p 0
                else if (right[2] == player)                                                        ddie4++;// p 0 p p p 0
                else                                                                               alive3++;// x 0 p p p 0 (x)
            } else if (left[1] == 0 || right[1] == 0) {                                                     // x p p p 0
                if ((left[1]==0 && left[2]==0) || (right[1]==0 && right[2]==0))                      die3++;// x p p p 0 0
                else                                                                                 die3++;// x p p p 0 x
            } else;                                                                                         // x p p p x
        //p p
        } else if (len == 2) {
            if (player == 2) continue; 
            // 0 p p 0
            if (left[1] == 0 && right[1]==0) { 
                if (left[2]==0 && right[2]==0)                                                     alive2++;// 0 0 p p 0 0
                else if ((left[2]==0 || right[2]==0) && ((left[2]==opnt || right[2]==opnt)))       alive2++;// 0 0 p p 0 x
                else if (left[2]==opnt && right[2]==opnt);                                                  // x 0 p p 0 x
                else if (left[2]==player && right[2]==player)                                      alive4++;// p 0 p p 0 p
                else if (left[2]==player || right[2]==player)                                     jalive3++;// p 0 p p 0 x或0
            // 0 p p x
            } else if (left[1]==0) { 
                if (left[2]==0) {                                                                           // 0 0 p p x
                    if (left[3]==0)                                                                  die2++;// 0 0 0 p p x
                    else if (left[3]==player)                                                        die2++;// p 0 0 p p x
                    else;                                                                                   // x 0 0 p p x
                } else if (left[2] == player) {                                                             // p 0 p p x
                    if (left[3]==0)                                                               jalive3++;// 0 p 0 p p x
                    else if (left[3]==player)                                                        die4++;// p p 0 p p x
                    else;                                                                                   // x p 0 p p x
                }
            //x p p 0
            } else if (right[1]==0) { 
                if (right[2]==0) {                                                                          // x p p 0 0
                    if (right[3]==0)                                                                 die2++;// x p p 0 0 0
                    else if (right[3]==player)                                                       die2++;// x p p 0 0 p
                    else;                                                                                   // x p p 0 0 x
                } else if (right[2] == player) {                                                            // x p p 0 p
                    if (right[3]==0)                                                              jalive3++;// x p p 0 p 0
                    else if (right[3]==player)                                                       die4++;// x p p 0 p p
                    else;                                                                                   // x p p 0 p x
                }
            } else;                                                                                         // x p p x
        //p
        } else if (len == 1){
              if((left[1] == 0 && left[2] == player && left[3] == player && left[4] == player) || 
                 (right[1] == 0 && right[2] == player && right[3] == player && right[4] == player)){//ddie4
                   ddie4 ++;
           }
           else if((left[1] == 0 && right[1] == 0) && ((left[2] == player && left[3] == player && left[4] == 0) || 
                   (right[2] == player && right[3] == player && right[4] == 0))){//jalive3
                   jalive3 ++;
           }
           else if((left[1] == 0 && right[1] == 0) && 
                   ((left[2] == player && left[3] == player) || (right[2] == player && right[3] == player))){//die3
                   die3 ++;
           }
           else if((left[1] == 0 && left[4] == 0 && left[2] == player && left[3] == player) || 
                   (right[1] == 0 && right[4] == 0 && right[2] == player && right[3] == player)){//die3
                   die3 ++;
           }
           else if((left[1] == 0 && left[2] == 0 && left[3] == player && left[4] == player) || 
                   (right[1] == 0 && right[2] == 0 && right[3] == player && right[4] == player)){//die3
                   die3 ++;
           } 
           else if((left[1] == 0 && left[3] == 0 && left[2] == player && left[4] == player) || 
                   (right[1] == 0 && right[3] == 0 && right[2] == player && right[4] == player)){//die3
                   die3 ++;
           }
           else if((left[1] == 0 && right[1] == 0 && right[3] == 0 && right[2] == player) && (left[2] == 0 || right[4] == 0)){//jalive2
                   jalive2 ++;
           }
           else if((right[1] == 0 && left[1] == 0 && left[3] == 0 && left[2] == player) && 
                   (right[2] == 0 || left[4] == 0)){//jalive2
                   jalive2 ++;
           }
           else if((left[1] == 0 && right[1] == 0 && right[2] == 0 && right[4] == 0 && right[3] == player) || 
                   (right[1] == 0 && left[1] == 0 && left[2] == 0 && left[4] == 0 && left[3] == player)){//jalive2
                   jalive2 ++;
           }
           else if((left[1] == 0 && left[3] == 0 && left[4] == 0 && left[2] == player) || 
                   (right[1] == 0 && right[3] == 0 && right[4] == 0 && right[2] == player)){//die2
                   die2 ++;
           }
           else if((left[1] == 0 && right[1] == 0 && right[2] == 0 && left[2] == player) || 
                   (right[1] == 0 && left[1] == 0 && left[2] == 0 && right[2] == player)){//die2
                   die2 ++;
           }
           else if((left[1] == 0 && left[2] == 0 && left[4] == 0 && left[3] == player) || 
                   (right[1] == 0 && right[2] == 0 && right[4] == 0 && right[3] == player)){//die2
                   die2 ++;
           }
           else if((left[1] == 0 && left[2] == 0 && right[1] == 0 && left[3] == player) || 
                   (right[1] == 0 && right[2] == 0 && left[1] == 0 && right[3] == player)){//die2
                   die2 ++;
           }
           else if((left[1] == 0 && left[2] == 0 && left[3] == 0 && left[4] == player) || 
                   (right[1] == 0 && right[2] == 0 && right[3] == 0 && right[4] == player)){//die2
                   die2 ++;
           }
           else;
        }
    } //identify chess board over

    //win, alive4, die4, ddie4, alive3, die3, ddie3, alive2, bad_alive2, bbad_alive2, die2, ddie2, normal;  
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

int N_SVF (int player) {
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
    if (board[x][y]==EMPTY) {
    for (int dir = 0; dir < 3; dir++) {
        if (valid(x, y, dir+2)) {  //right -> downright -> down
            if (board[ x + xd[dir][0] ][ y + yd[dir][0] ] == player) {                                           // . p
                if (board[ x + xd[dir][1] ][ y + yd[dir][1] ] == EMPTY) {                                        // . p .
                    if (board[ x + xd[dir][2] ][ y + yd[dir][2] ] == EMPTY) {                                    // . p . .
                        if (board[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 1 ;}           // . p . . .
                        else if ((board[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 1 ;}           // . p . . p
                        else                                                            {value += 1 ;}           // . p . . o
                    }
                    else if (board[ x + xd[dir][2] ][ y + yd[dir][2] ] == player) {                              // . p . p
                        if (board[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 2 ;}           // . p . p .
                        else if ((board[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 9 ;}           // . p . p p
                        else                                                            {value += 2 ;}           // . p . p o
                    }
                    else {                                                                                       // . p . o
                        if (board[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 1 ;}           // . p . o .
                        else if ((board[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 1 ;}           // . p . o p
                        else                                                            {value += 1 ;}           // . p . o o
                    }
                }
                else if (board[ x + xd[dir][1] ][ y + yd[dir][1] ] == player) {                                  // . p p
                    if (board[ x + xd[dir][2] ][ y + yd[dir][2] ] == EMPTY) {                                    // . p p .
                        if (board[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 7 ;}           // . p p . .
                        else if ((board[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 9 ;}           // . p p . p
                        else                                                            {value += 4 ;}           // . p p . o
                    }
                    else if (board[ x + xd[dir][2] ][ y + yd[dir][2] ] == player) {                              // . p p p
                        if (board[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 9 ;}           // . p p p .
                        else if ((board[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value +=14 ;}           // . p p p p
                        else                                                            {value += 6 ;}           // . p p p o
                    }
                    else {                                                                                       // . p p o
                        if (board[ x + xd[dir][3] ][ y + yd[dir][3] ] == EMPTY)         {value += 2 ;}           // . p p o .
                        else if ((board[ x + xd[dir][3] ][ y + yd[dir][3] ] == player)) {value += 2 ;}           // . p p o p
                        else                                                            {value += 2 ;}           // . p p o o
                    }
                }
                else                                                                    {value += 1 ;}            // . p o 
            }
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
    int val[SIZE][SIZE], max=0;
    int best_x=SIZE/2, best_y=SIZE/2;
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            if (board[x][y] == EMPTY) {
                val[x][y] = SVF(x,y,1)/2*2 + SVF(x,y,2)/2*2;
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
