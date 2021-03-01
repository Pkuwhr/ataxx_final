//
//  main.cpp
//  ataxx
//
//  Created by 王浩然 on 2017/12/7.
//  Copyright © 2017年 王浩然. All rights reserved.
//

#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
using namespace std;

int beginPos[1000][2], possiblePos[1000][2], posCount = 0;
int save_policy=0;
int search_depth=4;
float score_rate;
int dif;
int currBotColor,MyColor;// 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[7][7] = { 0 }; // 先x后y，记录棋盘状态
int blackPieceCount = 2, whitePieceCount = 2;
int countdis(int player)//估值函数
{
    int i,j;
    int black=0,white=0;
    int dis;
    for(i=0;i<7;i++)
    {
        for(j=0;j<7;j++)
        {
            if(gridInfo[i][j]==1)
            {
                black++;
            }
            if(gridInfo[i][j]==-1)
            {
                white++;
            }
        }
    }
    dis=player*(black-white);
    if(dis>=0)
    {
        dif=1;
    }
    else
    {
        dif=0;
    }
    return dis;
}

static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
    { -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
    { 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
    { 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
    { -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
    { 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } };

// 判断是否在地图内
inline bool inMap(int x, int y)
{
    if (x < 0 || x > 6 || y < 0 || y > 6)
        return false;
    return true;
}

// 向Direction方向改动坐标，并返回是否越界
inline bool MoveStep(int &x, int &y, int Direction)
{
    x = x + delta[Direction][0];
    y = y + delta[Direction][1];
    return inMap(x, y);
}

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int color)
{
    if (color == 0)
        return false;
    if (x1 == -1) // 无路可走，跳过此回合
        return false;
    if (!inMap(x0, y0) || !inMap(x1, y1)) // 超出边界
        return false;
    if (gridInfo[x0][y0] != color)
        return false;
    int dx, dy, x, y, dir;
    dx = abs((x0 - x1));
    dy = abs((y0 - y1));
    if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // 保证不会移动到原来位置，而且移动始终在5×5区域内
        return false;
    if (gridInfo[x1][y1] != 0) // 保证移动到的位置为空
        return false;
    if (dx == 2 || dy == 2) // 如果走的是5×5的外围，则不是复制粘贴
        gridInfo[x0][y0] = 0;
    else
    {
        if (color == 1)
            blackPieceCount++;
        else
            whitePieceCount++;
    }
    
    gridInfo[x1][y1] = color;
    for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
    {
        x = x1 + delta[dir][0];
        y = y1 + delta[dir][1];
        if (!inMap(x, y))
            continue;
        if (gridInfo[x][y] == -color)
        {
            gridInfo[x][y] = color;
        }
    }
    return true;
}

int step_policy(int depth,int alpha,int beta,int player)//含参数版本的决策策略
{
    int value;
    int hi,hj;
    int temp[7][7];
    int i;
    int x,y;
    int dir;
    for(hi=0;hi<7;hi++)
    {
        for(hj=0;hj<7;hj++)
        {
            temp[hi][hj]=gridInfo[hi][hj];
        }
    }
    if(depth==0)
    {
        /*//debug
         cout<<"层数："<<depth<<endl;
         for(hi=0;hi<7;hi++)
         {
         for(hj=0;hj<7;hj++)
         {
         cout<<gridInfo[hi][hj]<<" ";
         }
         cout<<endl;
         }
         //debug
         cout<<"叶节点得分："<<countdis(currBotColor)<<endl;*/
        return score_rate*countdis(currBotColor);
    }
    if(depth==search_depth)
    {
        for(i=0;i<posCount;i++)
        {
            if(ProcStep(beginPos[i][0], beginPos[i][1], possiblePos[i][0], possiblePos[i][1], player))
            {
                
                /* //debug
                 cout<<"层数："<<depth<<endl;
                 for(hi=0;hi<7;hi++)
                 {
                 for(hj=0;hj<7;hj++)
                 {
                 cout<<gridInfo[hi][hj]<<" ";
                 }
                 cout<<endl;
                 }
                 //debug*/
                if(abs(beginPos[i][0]-possiblePos[i][0])==2||abs(beginPos[i][1]-possiblePos[i][1])==2)
                {
                    score_rate=1+0.01*pow(-1,dif+search_depth);//评估结果正数 dif为1 否则为0
                }
                else
                {
                    score_rate=1-0.01*pow(-1,dif+search_depth);
                }
                value=step_policy(depth-1, alpha, beta, -player);
                for(hi=0;hi<7;hi++)
                {
                    for(hj=0;hj<7;hj++)
                    {
                        gridInfo[hi][hj]=temp[hi][hj];
                    }
                }
                if(player==currBotColor)
                {
                    if(value>alpha)
                    {
                        alpha=value;
                        save_policy=i;
                    }
                }
                if(player==-currBotColor)
                {
                    if(value<beta)
                    {
                        beta=value;
                    }
                }
                //                cout<<depth<<"层的"<<"alpha:"<<alpha<<"beta:"<<beta<<endl;
                if(alpha>=beta)
                {
                    //                    cout<<endl<<"剪枝！！！！！！"<<endl;
                    if(player==currBotColor)
                    {
                        return 100;
                    }
                    else
                    {
                        return -100;
                    }
                }
            }
        }
    }
    if(depth>0&&depth<search_depth)
    {
        for(x=0;x<7;x++)
        {
            for(y=0;y<7;y++)
            {
                if(gridInfo[x][y]==player)
                {
                    for(dir=0;dir<24;dir++)
                    {
                        if(ProcStep(x, y, x+delta[dir][0], y+delta[dir][1], player))
                        {
                            /* //debug
                             cout<<"层数："<<depth<<endl;
                             for(hi=0;hi<7;hi++)
                             {
                             for(hj=0;hj<7;hj++)
                             {
                             cout<<gridInfo[hi][hj]<<" ";
                             }
                             cout<<endl;
                             }
                             //debug*/
                            value=step_policy(depth-1, alpha, beta, -player);
                            for(hi=0;hi<7;hi++)
                            {
                                for(hj=0;hj<7;hj++)
                                {
                                    gridInfo[hi][hj]=temp[hi][hj];
                                }
                            }
                            for(hi=0;hi<7;hi++)
                            {
                                for(hj=0;hj<7;hj++)
                                {
                                    gridInfo[hi][hj]=temp[hi][hj];
                                }
                            }
                            if(player==currBotColor)
                            {
                                if(value>alpha)
                                {
                                    alpha=value;
                                }
                            }
                            if(player==-currBotColor)
                            {
                                if(value<beta)
                                {
                                    beta=value;
                                }
                            }
                            //                            cout<<depth<<"层的"<<"alpha:"<<alpha<<"beta:"<<beta<<endl;
                            if(alpha>=beta)
                            {
                                //                                cout<<endl<<depth<<"层剪枝！！！！！！"<<endl;
                                if(player==currBotColor)
                                {
                                    return 100;
                                }
                                else
                                {
                                    return -100;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if(player==currBotColor)
    {
        return alpha;
    }
    else
    {
        return beta;
    }
}
void mac_player(int color)//电脑会收到一个自己所执的棋子颜色
{
    istream::sync_with_stdio(false);
    int x0, y0, x1, y1;
    int dir;
    posCount=0;
    for (y0 = 0; y0 < 7; y0++)
    {
        for (x0 = 0; x0 < 7; x0++)
        {
            if (gridInfo[x0][y0] != currBotColor)
                continue;
            for (dir = 0; dir < 24; dir++)
            {
                x1 = x0 + delta[dir][0];
                y1 = y0 + delta[dir][1];
                if (!inMap(x1, y1))
                    continue;
                if (gridInfo[x1][y1] != 0)
                    continue;
                beginPos[posCount][0] = x0;
                beginPos[posCount][1] = y0;
                possiblePos[posCount][0] = x1;
                possiblePos[posCount][1] = y1;
                posCount++;
            }
        }
    }
    // 做出决策（你只需修改以下部分）
    step_policy(search_depth, -100, 100, currBotColor);
    int startX, startY, resultX, resultY;
    if (posCount > 0)
    {
        startX = beginPos[save_policy][0];
        startY = beginPos[save_policy][1];
        resultX = possiblePos[save_policy][0];
        resultY = possiblePos[save_policy][1];
    }
    else
    {
        startX = -1;
        startY = -1;
        resultX = -1;
        resultY = -1;
    }
    
    // 决策结束，输出结果（你只需修改以上部分）
    
    ProcStep(startX, startY, resultX, resultY, color);
}

int judge_winner(int color)//color下完棋之后开始形式判断，假如胜负已分输出胜者的颜色，否则输出0
{
    int x0, y0, x1, y1;
    int dir;
    int cnt=0;
    for (y0 = 0; y0 < 7; y0++)
    {
        for (x0 = 0; x0 < 7; x0++)
        {
            if (gridInfo[x0][y0] != -color)
                continue;
            for (dir = 0; dir < 24; dir++)
            {
                x1 = x0 + delta[dir][0];
                y1 = y0 + delta[dir][1];
                if (!inMap(x1, y1))
                    continue;
                if (gridInfo[x1][y1] != 0)
                    continue;
                cnt++;
            }
        }
    }//判断对手还有没有落子点
    if(cnt==0)
    {
        for(y0 = 0; y0 < 7; y0++)
        {
            for(x0 = 0; x0 < 7;x0++)
            {
                if(gridInfo[x0][y0]==0)
                {
                    gridInfo[x0][y0]=color;
                }
            }
        }
        if(countdis(color)>0)
        {
            if(color==MyColor)
            {
                cout<<"You Win!"<<endl;
            }
            else
            {
                cout<<"You Loss!"<<endl;
            }
        }
        else
        {
            if(color==MyColor)
            {
                cout<<"You Loss!"<<endl;
            }
            else
            {
                cout<<"You Win!"<<endl;
            }
        }
        return 0;
    }//如果我走完这一步对手没有棋下了，那么剩下的空格都是我的
    return 1;
}//如果返回值为零，说明棋局结束，否则返回值为1，棋局继续

void play_chess()
{
    int x,y;
    int n;
    cout<<"你希望用什么颜色的棋子:"<<endl;
    cout<<"   1. 黑棋"<<endl;
    cout<<"   2. 白棋"<<endl;
    cout<<"   3. 退出"<<endl;
    cin>>n;
    switch (n)
    {
        case 1:
        {
            MyColor=1;
            currBotColor=-1;
            break;
        }
        case 2:
        {
            MyColor=-1;
            currBotColor=1;
            break;
        }
        default:
            return;
    }
    if(currBotColor==1)
    {
        while(judge_winner(MyColor)&&judge_winner(currBotColor))
        {
            for(y=0;y<7;y++)
            {
                for(x=0;x<7;x++)
                {
                    if(gridInfo[x][y]==1)
                    {
                        cout<<"●";
                    }
                    if(gridInfo[x][y]==0)
                    {
                        cout<<" ";
                    }
                    if(gridInfo[x][y]==-1)
                    {
                        cout<<"○";
                    }
                }
                cout<<endl;
            }//打印棋盘
            mac_player(1);//电脑下棋；
            for(y=0;y<7;y++)
            {
                for(x=0;x<7;x++)
                {
                    if(gridInfo[x][y]==1)
                    {
                        cout<<"●";
                    }
                    if(gridInfo[x][y]==0)
                    {
                        cout<<" ";
                    }
                    if(gridInfo[x][y]==-1)
                    {
                        cout<<"○";
                    }
                }
                cout<<endl;
            }//打印棋盘
            int sx,sy,ex,ey;
            cout<<"请输入你想要移动的棋子坐标"<<endl;
            cout<<"X: ";
            cin>>sx;
            cout<<"Y: ";
            cin>>sy;
            cout<<"请输入你想要移动的终点位置"<<endl;
            cout<<"X: ";
            cin>>ex;
            cout<<"Y: ";
            cin>>ey;
            ProcStep(sx, sy, ex, ey, MyColor);
            for(y=0;y<7;y++)
            {
                for(x=0;x<7;x++)
                {
                    if(gridInfo[x][y]==1)
                    {
                        cout<<"●";
                    }
                    if(gridInfo[x][y]==0)
                    {
                        cout<<" ";
                    }
                    if(gridInfo[x][y]==-1)
                    {
                        cout<<"○";
                    }
                }
                cout<<endl;
            }//打印棋盘
        }
    }
    if(currBotColor==-1)
    {
        while(judge_winner(currBotColor)&&judge_winner(MyColor))
        {
            system("cls");
            for(y=0;y<7;y++)
            {
                for(x=0;x<7;x++)
                {
                    if(gridInfo[x][y]==1)
                    {
                        cout<<"●";
                    }
                    if(gridInfo[x][y]==0)
                    {
                        cout<<"□";
                    }
                    if(gridInfo[x][y]==-1)
                    {
                        cout<<"○";
                    }
                }
                cout<<endl;
            }//打印棋盘
            cout<<endl<<endl;
            int sx,sy,ex,ey;
      loop: cout<<"请输入你想要移动的棋子坐标"<<endl;
            cout<<"X: ";
            cin>>sx;
            cout<<"Y: ";
            cin>>sy;
            cout<<"请输入你想要移动的终点位置"<<endl;
            cout<<"X: ";
            cin>>ex;
            cout<<"Y: ";
            cin>>ey;
            if(!ProcStep(sx, sy, ex, ey, MyColor))
            {
                cout<<"非法棋步，请重新落子！！！"<<endl;
                goto loop;
            }
            system("cls");
            for(y=0;y<7;y++)
            {
                for(x=0;x<7;x++)
                {
                    if(gridInfo[x][y]==1)
                    {
                        cout<<"●";
                    }
                    if(gridInfo[x][y]==0)
                    {
                        cout<<"□";
                    }
                    if(gridInfo[x][y]==-1)
                    {
                        cout<<"○";
                    }
                }
                cout<<endl;
            }//打印棋盘
            cout<<endl<<endl;
            mac_player(-1);//电脑下棋；
            system("cls");
            for(y=0;y<7;y++)
            {
                for(x=0;x<7;x++)
                {
                    if(gridInfo[x][y]==1)
                    {
                        cout<<"●";
                    }
                    if(gridInfo[x][y]==0)
                    {
                        cout<<"□";
                    }
                    if(gridInfo[x][y]==-1)
                    {
                        cout<<"○";
                    }
                }
                cout<<endl;
            }//打印棋盘
            cout<<endl<<endl;
        }
    }
}

void restore()
{
    
}
int main()
{
    int xx,yy;
    for(xx=0;xx<7;xx++)
    {
        for(yy=0;yy<7;yy++)
        {
            gridInfo[xx][yy]=0;
        }
    }
    int n=0;
    while(n!=3)
    {
        system("cls");
        cout<<"欢迎来玩同化棋"<<endl;
        cout<<"   1.开始   "<<endl;
        cout<<"   2.复盘   "<<endl;
        cout<<"   3.退出   "<<endl;
        cout<<"输入序号以选择："<<endl;
        cin>>n;
        switch (n)
        {
            case 1:
            {
                
                gridInfo[0][0]=1;
                gridInfo[6][6]=1;
                gridInfo[0][6]=-1;
                gridInfo[6][0]=-1;//初始化棋盘
                play_chess();
                break;
                
            }
            case 2:
                restore();
                break;
            default:
            {
                n=3;
                break;
            }
        }
    }
    return 0;
}

