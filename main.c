#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>

#include <string.h>
#include <conio.h>


#define U 1
#define D 2
#define L 3
#define R 4       //蛇的状态，U：上 ；D：下；L:左 R：右



#define MAX_USERS 100 // 最大用户数
#define LOG_FILE "user_log.txt" // 用户日志文件路径
time_t start_time; // 添加全局变量

FILE *log_file; // 用户日志文件指针

//定义用户表和用户日志的结构
typedef struct {
    char username[20];
    char password[20];
} User;

typedef struct {
    int id;
    char username[20];
    time_t start_time;
    int duration;
    int score;
} UserLog;

User users[MAX_USERS]; // 用户表
int num_users = 0; // 当前用户数


typedef struct SNAKE //蛇身的一个节点
{
    int x;
    int y;
    struct SNAKE* next;
}snake;

//全局变量//
int score = 0, add = 10;//总得分与每次吃食物得分。
int status, sleeptime = 200;//每次运行的时间间隔
snake* head, * food;//蛇头指针，食物指针
snake* q;//遍历蛇的时候用到的指针
int endgamestatus = 0; //游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。

//声明全部函数//
void Pos();
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pause();
void gamecircle();
void welcometogame();
void endgame();
void gamestart();

void registerUser();
int loginUser();
void openLogFile();
void closeLogFile();
void logUserGame();
void gametip();
void gameF5();








void Pos(int x, int y)//设置光标位置
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//创建地图
{
    int i;
    for (i = 0; i < 58; i += 2)//打印上下边框
    {
        Pos(i, 0);
        printf("■");
        Pos(i, 26);
        printf("■");
    }
    for (i = 1; i < 26; i++)//打印左右边框
    {
        Pos(0, i);
        printf("■");
        Pos(56, i);
        printf("■");
    }
}

void initsnake()//初始化蛇身
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//从蛇尾开始，头插法，以x,y设定开始的位置//
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)
    {
        head = (snake*)malloc(sizeof(snake));
        head->next = tail;
        head->x = 24 + 2 * i;
        head->y = 5;
        tail = head;
    }
    while (tail != NULL)//从头到为，输出蛇身
    {
        Pos(tail->x, tail->y);
        printf("■");
        tail = tail->next;
    }
}

int biteself()//判断是否咬到了自己
{
    snake* self;
    self = head->next;
    while (self != NULL)
    {
        if (self->x == head->x && self->y == head->y)
        {
            return 1;
        }
        self = self->next;
    }
    return 0;
}

void createfood() {
    snake* food_1;
    srand((unsigned)time(NULL));
    food_1 = (snake*)malloc(sizeof(snake));
    while (1) {
        food_1->x = rand() % 52 + 2;
        food_1->y = rand() % 24 + 1;
        q = head;
        int food_overlap = 0; // Flag to check if food overlaps with snake
        while (q != NULL) {
            if (q->x == food_1->x && q->y == food_1->y) {
                food_overlap = 1;
                break;
            }
            q = q->next;
        }
        if (!food_overlap) {
            break; // If no overlap, break the loop
        }
    }
    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("■"); // Display the food
}


void cantcrosswall()//不能穿墙
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()//蛇前进,上U,下D,左L,右R
{
    snake* nexthead;
    cantcrosswall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//如果下一个有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                               //如果没有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == D)
    {
        nexthead->x = head->x;
        nexthead->y = head->y + 1;
        if (nexthead->x == food->x && nexthead->y == food->y)  //有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                               //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == L)
    {
        nexthead->x = head->x - 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == R)
    {
        nexthead->x = head->x + 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                         //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)       //判断是否会咬到自己
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()//暂停
{
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            break;
        }

    }
}

void gamecircle()//控制游戏
{

    Pos(64, 15);
    printf("不能穿墙，不能咬到自己\n");
    Pos(64, 16);
    printf("用↑.↓.←.→分别控制蛇的移动.");
    Pos(64, 17);
    printf("F1 为加速，F2 为减速\n");
    Pos(64, 18);
    printf("ESC ：退出游戏.space：暂停游戏.");
    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("得分：%d  ", score);
        Pos(64, 11);
        printf("每个食物得分：%d分", add);
        if (GetAsyncKeyState(VK_UP) && status != D)
        {
            status = U;
        }
        else if (GetAsyncKeyState(VK_DOWN) && status != U)
        {
            status = D;
        }
        else if (GetAsyncKeyState(VK_LEFT) && status != R)
        {
            status = L;
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status != L)
        {
            status = R;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE))
        {
            endgamestatus = 3;
            break;
        }
        else if (GetAsyncKeyState(VK_F1))
        {
            if (sleeptime >= 50)
            {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320)
                {
                    add = 2;//防止减到1之后再加回来有错
                }
            }
        }
        else if (GetAsyncKeyState(VK_F2))
        {
            if (sleeptime < 350)
            {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350)
                {
                    add = 1;  //保证最低分为1
                }
            }
        }
        Sleep(sleeptime);
        snakemove();
    }
    logUserGame(num_users, users[num_users - 1].username, start_time, time(NULL) - start_time, score);
}

void welcometogame()//开始界面
{
    Pos(40, 12);
    printf("欢迎来到贪食蛇游戏！");
    Pos(40, 25);
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("用↑.↓.←.→分别控制蛇的移动， F1 为加速，2 为减速\n");
    Pos(25, 13);
    printf("加速将能得到更高的分数。\n");
    system("pause");
    system("cls");
}

void gamestart()//游戏初始化
{
    start_time = time(NULL); // 在游戏开始时记录时间
    system("mode con cols=100 lines=30");
    welcometogame();
    creatMap();
    initsnake();
    createfood();
}


//注册功能
void registerUser() {
    printf("请输入用户名：");
    scanf("%s", users[num_users].username);
    printf("请输入密码：");
    scanf("%s", users[num_users].password);
    printf("注册成功！\n");
    num_users++;
}


//登录功能
int loginUser() {
    char username[20];
    char password[20];
    printf("请输入用户名：");
    scanf("%s", username);
    printf("请输入密码：");
    scanf("%s", password);
    int i = 0;
    for (i = 0; i < num_users; i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            printf("登录成功！\n");
            return 1;
        }
    }
    printf("用户名或密码错误！\n");
    return 0;
}




//打开用户日志文件
void openLogFile() {
    log_file = fopen("user_log.txt", "a");
    if (log_file == NULL) {
        printf("无法打开用户日志文件！\n");
        exit(1);
    }
}

//关闭用户日志文件
void closeLogFile() {
    fclose(log_file);
}

//记录用户游戏日志
void logUserGame(int id, const char *username, time_t start_time, int duration, int score) {
    // 将时间转换为字符串形式
    char* time_str = asctime(localtime(&start_time));
    // 写入日志文件
    fprintf(log_file, "%d\t%s\t%s\t%d\t%d\n", id, username, time_str, duration, score);
}

//添加了用户注册或登录的流程
void gametip() {
    openLogFile(); // 打开用户日志文件
    printf("欢迎来到贪食蛇游戏！\n");

    // 用户注册或登录
    while (1) {
        printf("请选择操作：1. 注册  2. 登录\n");
        int choice;
        scanf("%d", &choice);
        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            if (loginUser()) {
                break;
            }
        } else {
            printf("无效的操作！\n");
        }
    }

    creatMap();
    initsnake();
    createfood();
}

//在游戏主界面显示当前正在游戏中的用户；添加了按F5显示游戏用户日志的功能，按下F5键后清屏显示用户日志。
void gameF5() {
    system("cls");
    printf("*********当前用户：%s正在游戏中*********\n", users[num_users - 1].username);
    printf("按F5显示游戏用户日志\n");

    if (GetAsyncKeyState(VK_F5) & 0x8000) { // Check if F5 key is pressed
        system("cls");
        printf("游戏用户日志：\n");
        // Open user log file for reading
        FILE *user_log_file = fopen("user_log.txt", "r");
        if (user_log_file == NULL) {
            printf("无法打开用户日志文件！\n");
            return;
        }
        // Read and display user log file contents
        char line[100];
        while (fgets(line, sizeof(line), user_log_file)) {
            printf("%s", line);
        }
        fclose(user_log_file); // Close user log file
        printf("\n按任意键继续...\n");
        _getch();
    }
}




//返回登录页面或退出
void returnToMenuOrExit() {
    printf("\n游戏结束，您要返回登录注册页面还是退出游戏？\n");
    printf("1. 返回登录注册页面\n");
    printf("2. 退出游戏\n");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
        case 1:
            // 返回登录注册页面
            system("cls");
            main();
            break;
        case 2:
            // 退出游戏
            exit(0);
            break;
        default:
            printf("无效的选择，程序退出。\n");
            exit(1);
    }
}


//游戏结束
//新增得分提示，游戏用户表展示
void endgame() {
    int duration = time(NULL) - start_time; // 计算游戏持续时间
    UserLog log;
    log.id = num_users; // 假设当前用户的ID为用户表的大小
    strcpy(log.username, users[num_users - 1].username); // 记录当前用户的用户名
    log.start_time = start_time;
    log.duration = duration;
    log.score = score;
    printf("游戏日志：\n");
    printf("用户ID：%d，用户名：%s，开始时间：%s，持续时长：%d秒，得分：%d\n",
           log.id, log.username, asctime(localtime(&log.start_time)), log.duration, log.score);
    fprintf(log_file, "%d\t%s\t%s\t%d\t%d\n", log.id, log.username, asctime(localtime(&log.start_time)), log.duration, log.score);

    // 游戏结束后提示用户选择返回登录注册页面或退出游戏
    returnToMenuOrExit();
}




int main() {
    gametip();
    gamestart();
    while (1) {
        gameF5(); // 游戏进行中也需要检测按键以显示游戏用户日志
        gamecircle();
        endgame();
        // 游戏结束后提示用户选择返回登录注册页面或退出游戏
        printf("游戏结束，您要返回登录注册页面还是退出游戏？\n");
        printf("1. 返回登录注册页面\n");
        printf("2. 退出游戏\n");
        int choice;
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                // 返回登录注册页面
                system("cls");
                gametip();
                break;
            case 2:
                // 退出游戏
                closeLogFile(); // 关闭用户日志文件
                exit(0);
                break;
            default:
                printf("无效的选择，程序退出。\n");
                closeLogFile(); // 关闭用户日志文件
                exit(1);
        }
    }
    return 0;
}


