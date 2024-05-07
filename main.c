#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>

#include <string.h>
#include <conio.h>


#define U 1
#define D 2
#define L 3
#define R 4       //�ߵ�״̬��U���� ��D���£�L:�� R����



#define MAX_USERS 100 // ����û���
#define LOG_FILE "user_log.txt" // �û���־�ļ�·��
time_t start_time; // ���ȫ�ֱ���

FILE *log_file; // �û���־�ļ�ָ��

//�����û�����û���־�Ľṹ
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

User users[MAX_USERS]; // �û���
int num_users = 0; // ��ǰ�û���


typedef struct SNAKE //�����һ���ڵ�
{
    int x;
    int y;
    struct SNAKE* next;
}snake;

//ȫ�ֱ���//
int score = 0, add = 10;//�ܵ÷���ÿ�γ�ʳ��÷֡�
int status, sleeptime = 200;//ÿ�����е�ʱ����
snake* head, * food;//��ͷָ�룬ʳ��ָ��
snake* q;//�����ߵ�ʱ���õ���ָ��
int endgamestatus = 0; //��Ϸ�����������1��ײ��ǽ��2��ҧ���Լ���3�������˳���Ϸ��

//����ȫ������//
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








void Pos(int x, int y)//���ù��λ��
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//������ͼ
{
    int i;
    for (i = 0; i < 58; i += 2)//��ӡ���±߿�
    {
        Pos(i, 0);
        printf("��");
        Pos(i, 26);
        printf("��");
    }
    for (i = 1; i < 26; i++)//��ӡ���ұ߿�
    {
        Pos(0, i);
        printf("��");
        Pos(56, i);
        printf("��");
    }
}

void initsnake()//��ʼ������
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//����β��ʼ��ͷ�巨����x,y�趨��ʼ��λ��//
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
    while (tail != NULL)//��ͷ��Ϊ���������
    {
        Pos(tail->x, tail->y);
        printf("��");
        tail = tail->next;
    }
}

int biteself()//�ж��Ƿ�ҧ�����Լ�
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
    printf("��"); // Display the food
}


void cantcrosswall()//���ܴ�ǽ
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()//��ǰ��,��U,��D,��L,��R
{
    snake* nexthead;
    cantcrosswall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//�����һ����ʳ��//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                               //���û��ʳ��//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
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
        if (nexthead->x == food->x && nexthead->y == food->y)  //��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                               //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
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
        if (nexthead->x == food->x && nexthead->y == food->y)//��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
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
        if (nexthead->x == food->x && nexthead->y == food->y)//��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                         //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)       //�ж��Ƿ��ҧ���Լ�
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()//��ͣ
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

void gamecircle()//������Ϸ
{

    Pos(64, 15);
    printf("���ܴ�ǽ������ҧ���Լ�\n");
    Pos(64, 16);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ�.");
    Pos(64, 17);
    printf("F1 Ϊ���٣�F2 Ϊ����\n");
    Pos(64, 18);
    printf("ESC ���˳���Ϸ.space����ͣ��Ϸ.");
    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("�÷֣�%d  ", score);
        Pos(64, 11);
        printf("ÿ��ʳ��÷֣�%d��", add);
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
                    add = 2;//��ֹ����1֮���ټӻ����д�
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
                    add = 1;  //��֤��ͷ�Ϊ1
                }
            }
        }
        Sleep(sleeptime);
        snakemove();
    }
    logUserGame(num_users, users[num_users - 1].username, start_time, time(NULL) - start_time, score);
}

void welcometogame()//��ʼ����
{
    Pos(40, 12);
    printf("��ӭ����̰ʳ����Ϸ��");
    Pos(40, 25);
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ��� F1 Ϊ���٣�2 Ϊ����\n");
    Pos(25, 13);
    printf("���ٽ��ܵõ����ߵķ�����\n");
    system("pause");
    system("cls");
}

void gamestart()//��Ϸ��ʼ��
{
    start_time = time(NULL); // ����Ϸ��ʼʱ��¼ʱ��
    system("mode con cols=100 lines=30");
    welcometogame();
    creatMap();
    initsnake();
    createfood();
}


//ע�Ṧ��
void registerUser() {
    printf("�������û�����");
    scanf("%s", users[num_users].username);
    printf("���������룺");
    scanf("%s", users[num_users].password);
    printf("ע��ɹ���\n");
    num_users++;
}


//��¼����
int loginUser() {
    char username[20];
    char password[20];
    printf("�������û�����");
    scanf("%s", username);
    printf("���������룺");
    scanf("%s", password);
    int i = 0;
    for (i = 0; i < num_users; i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            printf("��¼�ɹ���\n");
            return 1;
        }
    }
    printf("�û������������\n");
    return 0;
}




//���û���־�ļ�
void openLogFile() {
    log_file = fopen("user_log.txt", "a");
    if (log_file == NULL) {
        printf("�޷����û���־�ļ���\n");
        exit(1);
    }
}

//�ر��û���־�ļ�
void closeLogFile() {
    fclose(log_file);
}

//��¼�û���Ϸ��־
void logUserGame(int id, const char *username, time_t start_time, int duration, int score) {
    // ��ʱ��ת��Ϊ�ַ�����ʽ
    char* time_str = asctime(localtime(&start_time));
    // д����־�ļ�
    fprintf(log_file, "%d\t%s\t%s\t%d\t%d\n", id, username, time_str, duration, score);
}

//������û�ע����¼������
void gametip() {
    openLogFile(); // ���û���־�ļ�
    printf("��ӭ����̰ʳ����Ϸ��\n");

    // �û�ע����¼
    while (1) {
        printf("��ѡ�������1. ע��  2. ��¼\n");
        int choice;
        scanf("%d", &choice);
        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            if (loginUser()) {
                break;
            }
        } else {
            printf("��Ч�Ĳ�����\n");
        }
    }

    creatMap();
    initsnake();
    createfood();
}

//����Ϸ��������ʾ��ǰ������Ϸ�е��û�������˰�F5��ʾ��Ϸ�û���־�Ĺ��ܣ�����F5����������ʾ�û���־��
void gameF5() {
    system("cls");
    printf("*********��ǰ�û���%s������Ϸ��*********\n", users[num_users - 1].username);
    printf("��F5��ʾ��Ϸ�û���־\n");

    if (GetAsyncKeyState(VK_F5) & 0x8000) { // Check if F5 key is pressed
        system("cls");
        printf("��Ϸ�û���־��\n");
        // Open user log file for reading
        FILE *user_log_file = fopen("user_log.txt", "r");
        if (user_log_file == NULL) {
            printf("�޷����û���־�ļ���\n");
            return;
        }
        // Read and display user log file contents
        char line[100];
        while (fgets(line, sizeof(line), user_log_file)) {
            printf("%s", line);
        }
        fclose(user_log_file); // Close user log file
        printf("\n�����������...\n");
        _getch();
    }
}




//���ص�¼ҳ����˳�
void returnToMenuOrExit() {
    printf("\n��Ϸ��������Ҫ���ص�¼ע��ҳ�滹���˳���Ϸ��\n");
    printf("1. ���ص�¼ע��ҳ��\n");
    printf("2. �˳���Ϸ\n");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
        case 1:
            // ���ص�¼ע��ҳ��
            system("cls");
            main();
            break;
        case 2:
            // �˳���Ϸ
            exit(0);
            break;
        default:
            printf("��Ч��ѡ�񣬳����˳���\n");
            exit(1);
    }
}


//��Ϸ����
//�����÷���ʾ����Ϸ�û���չʾ
void endgame() {
    int duration = time(NULL) - start_time; // ������Ϸ����ʱ��
    UserLog log;
    log.id = num_users; // ���赱ǰ�û���IDΪ�û���Ĵ�С
    strcpy(log.username, users[num_users - 1].username); // ��¼��ǰ�û����û���
    log.start_time = start_time;
    log.duration = duration;
    log.score = score;
    printf("��Ϸ��־��\n");
    printf("�û�ID��%d���û�����%s����ʼʱ�䣺%s������ʱ����%d�룬�÷֣�%d\n",
           log.id, log.username, asctime(localtime(&log.start_time)), log.duration, log.score);
    fprintf(log_file, "%d\t%s\t%s\t%d\t%d\n", log.id, log.username, asctime(localtime(&log.start_time)), log.duration, log.score);

    // ��Ϸ��������ʾ�û�ѡ�񷵻ص�¼ע��ҳ����˳���Ϸ
    returnToMenuOrExit();
}




int main() {
    gametip();
    gamestart();
    while (1) {
        gameF5(); // ��Ϸ������Ҳ��Ҫ��ⰴ������ʾ��Ϸ�û���־
        gamecircle();
        endgame();
        // ��Ϸ��������ʾ�û�ѡ�񷵻ص�¼ע��ҳ����˳���Ϸ
        printf("��Ϸ��������Ҫ���ص�¼ע��ҳ�滹���˳���Ϸ��\n");
        printf("1. ���ص�¼ע��ҳ��\n");
        printf("2. �˳���Ϸ\n");
        int choice;
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                // ���ص�¼ע��ҳ��
                system("cls");
                gametip();
                break;
            case 2:
                // �˳���Ϸ
                closeLogFile(); // �ر��û���־�ļ�
                exit(0);
                break;
            default:
                printf("��Ч��ѡ�񣬳����˳���\n");
                closeLogFile(); // �ر��û���־�ļ�
                exit(1);
        }
    }
    return 0;
}


