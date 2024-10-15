/* 
	������־
	1.������Ŀ
	2.�����ز�
	3.ʵ����Ϸ����
	4.ʵ����Ϸ����������
	5.ʵ�ֹ�����ֲ�￨��ʾ
	6.ʵ�������������
	7.ʵ�������ռ������
*/
#include <vector>
#include <fstream>

// �ṩ���߳����������
#include <iostream>
#include <thread>

#include <Windows.h>
#include <stdio.h>
#include <graphics.h>
#include <time.h>
#include "tools.h" 
#include <mmsystem.h> // Ϊʵ�ֲ������ֵ���Ŀ� �����ý���ļ�
#pragma comment(lib, "winmm.lib") // Windows ��ý����صľ�̬���ӿ�

#define WIN_WIDTH  900
#define WIN_HEIGHT 600
//#define PLANT_COUNT 4

// ���ڴ洢��Ƶ���ݵ����� �ѷ���
//std::vector<char> audioData;

// ö��
enum {Pea,Sunflower,PlantCount};

IMAGE imgBar;
IMAGE imgBg;
IMAGE imgPlantCards[PlantCount];
// ָ��ָ���Ӧ������
IMAGE* imgPlant[PlantCount][20];

int curX, curY; // ��ǰѡ�е�ֲ����ƶ����̵�λ��
int curPlant; // 0��û��ѡ�� 1��ѡ���һ��ֲ��

// ֲ��ṹ��
struct plant {
	int type; // 0����ʾû��ֲ�� 1����ʾѡ���һ��ֲ��
	int frameIndex; // ֲ�ﶯ������֡�����
};

struct plant map[3][9];

// ����ṹ��
struct sunshineBall {
	int x, y; // �������� x���� y��
	int frameIndex; // ���⶯������֡�����
	int destY; // ����Ʈ��Ŀ��λ�õ�y����
	bool used; // �Ƿ���ʹ��
	int timer; // ��ʱ��
};

// ��˿��� �ڴ��
struct sunshineBall balls[10];
IMAGE imgSunshineBall[29];

// ��ʼ����
int sunshine;

// ��ʬ�ṹ��
struct zombie{
	int x, y;
	int frameIndex;
	bool used;
	int speed;
};
struct zombie zombies[10];
IMAGE imgZombie[22];

bool fileExist(const char* name) {
	// ���Թر�sdl���
	FILE* fp = fopen(name, "r");
	if (fp == NULL) {
		return false;
	}else{
		fclose(fp);
		return true;
	}
	/*if (fp) {
		fclose(fp);
	}

	return fp != NULL;*/
}

// ��Ϸ��ʼ��
void gameInit()
{	
	// ���ر���ͼƬ
	// ���ַ����޸�Ϊ���ֽ��ַ���
	loadimage(&imgBg,"res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	// memset() ������ָ����ֵ c ���Ƶ� str ��ָ����ڴ������ǰ n ���ֽ��У���������ڽ��ڴ�����������Ϊ�ض�ֵ����һЩ����£���Ҫ���ٳ�ʼ������ڴ�Ϊ������ض�ֵ��memset() �����ṩ��Ч��ʵ�֡�
	// ��ʼ��
	memset(imgPlant,0,sizeof(imgPlant));
	memset(map, 0, sizeof(map));
	memset(balls,0,sizeof(balls));
	memset(zombies, 0, sizeof(zombies));

	// ��ʼ��ֲ�￨��
	char name[64];
	for (int i = 0; i < PlantCount; i++) {
		// ����ֲ�￨�Ƶ��ļ���
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgPlantCards[i],name);

		for (int j = 0; j < 20;j++) {
			sprintf_s(name, sizeof(name), "res/plant/%d/%d.png", i,j + 1);
			// �ж��ļ��Ƿ����
			if (fileExist(name)) {
				imgPlant[i][j] = new IMAGE;
				loadimage(imgPlant[i][j], name);
			}
			else {
				break;
			}
		}
	}

	curPlant = 0;
	sunshine = 100; // ��ʼ����

	// ��ʼ������
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}

	// ��ʼ����ʬ����
	for (int i = 0; i < 22; i++) {
		sprintf_s(name, sizeof(name), "res/zm/%d.png",i + 1);
		loadimage(&imgZombie[i], name);
	}

	// �����������
	srand(time(NULL));
	
	// ������Ϸ���� initgraph(���ڿ��,���ڸ߶�,����̨)
	initgraph(WIN_WIDTH,WIN_HEIGHT,1);

	// ��������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 28;
	f.lfWeight = 12;
	// �������� ���庯������
	strcpy(f.lfFaceName, "Segoe UI Black");
	// ���忹���
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);
	// �������屳��͸��
	setbkmode(TRANSPARENT);
	setcolor(BLACK);
}

// ��Ϸ���ݸ���
void updateWindow() {
	// ���û��� ��ֹ����ѭ����ͼƬ��˸
	BeginBatchDraw(); // ��ӡ�������ڴ��� ����

	// ��ȾͼƬ putimage (x�����꣬y������,ͼƬ·��)
	putimage(0, 0, &imgBg);
	//putimage(50, 0,&imgBar);
	putimagePNG(150, 0, &imgBar);

	// ��Ⱦ����
	for (int i = 0; i < PlantCount; i++) {
		int x=238+i*65;
		int y = 6;
		putimage(x, y, &imgPlantCards[i]);
	}

	// ��Ⱦ��ֲ�µ�ֲ��
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0){
				int x = 256 + j * 81;
				int y = 179 + i * 102 + 14;
				//printf("%d,%d\n", x, y);
				// ֲ��ѡ�е�����
				int plantType = map[i][j].type - 1;
				// ֲ�ﶯ������
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgPlant[plantType][index]);
			}
		}
	}

	// ��Ⱦ�϶������е�ֲ��
	if (curPlant > 0) {
		IMAGE* img = imgPlant[curPlant - 1][0];
		// img->get() ������ʽ���﷨ͨ�����ڷ��ʽṹ�����ĳ�Ա�������Ա����
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++){
		if(balls[i].used) {
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			putimagePNG(balls[i].x,balls[i].y,img);
		}
	}
	
	// ��ʾ������
	char sunshineScoreText[8];
	sprintf_s(sunshineScoreText, sizeof(sunshineScoreText), "%d", sunshine);
	// ������������ı�
	outtextxy(179,67,sunshineScoreText);

	// ��Ⱦ��ʬ
	int zombieMax = sizeof(zombies) / sizeof(zombies[0]);
	for (int i = 0; i < zombieMax; i++) {
		if (zombies[i].used) {
			IMAGE* img = &imgZombie[zombies[i].frameIndex];
			putimagePNG(zombies[i].x, zombies[i].y - img->getheight(), img);
		}
	}

	EndBatchDraw();// ��������
}



// ������Ƶ
void playAudioAsync() {
	mciSendString("play res/sunshine.mp3", 0, 0, 0);
}

// �ռ�����
void collectSunshine(ExMessage* msg) {
	int count = sizeof(balls) / sizeof(balls[0]);
	int w = imgSunshineBall[0].getwidth();
	int h = imgSunshineBall[0].getheight();
	for (int i = 0; i < count; i++){
		if (balls[i].used) {
			int x = balls[i].x;
			int y = balls[i].y;
			//msg->x ��һ�ַ��ʽṹ��ָ���Ա�ķ�ʽ���������÷��У�msg ��һ��ָ��ṹ���ָ�룬�� x �Ǹýṹ���е�һ����Ա������
			if (msg->x > x && msg->x < x + w && msg->y > y && msg->y < y + h) {
				balls[i].used = false;
				sunshine += 25;
				// mciSendString("play ��Ƶ�ļ���ַ volume(ϵͳ����)",����ֵ���ص��������û��������Զ������ݣ�)
				// volume to 0~1000 0��С 1000���
			    // mciSendString("status res/sunshine.mp3 volume", volumeBuffer, sizeof(volumeBuffer), 0);
				//mciSendString("play res/sunshine.mp3",0,0,0);
				// �첽������Ƶ
				std::thread audioThread(playAudioAsync);
				audioThread.detach(); // �����̣߳�ʹ���ں�̨����
			}
			
		}
	}
	
}


// �û��������
void userClick() {
	ExMessage msg;
	static int status = 0;

	// �жϵ�ǰ��û�е������
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) // ������
		{  
			if (msg.x > 238 && msg.x < 238+65*PlantCount && msg.y < 96)  //ֲ�￨�Ƶ����Χ
			{  
				int index = (msg.x - 238) / 65;
				//printf("%d\n",index);
				status = 1;
				curPlant = index + 1;
			}
			else {
				// �ж��Ƿ���������
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) // ����϶�
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP) // ���̧��
		{
			if (msg.x > 256 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) / 102;
				int col = (msg.x - 256) / 81;
				//printf("%d,%d\n", row, col);
				if (map[row][col].type == 0)
				{
					map[row][col].type = curPlant;
					map[row][col].frameIndex = 0;
				}
			}

			curPlant = 0;
			status = 0;
		}
		
	}
		
}

// ��������
void createSunshine() {
	static int count = 0;
	static int fre = 50;
	count++;

	if (count >= fre){
		fre = rand() % 200 + 201;
		count = 0;

		// ���������ȡһ������ʹ�õ�����
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;

		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax) return;

		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].x = 260 + rand() % 600; // 260-900
		balls[i].y = 60;
		balls[i].destY = 220 + (rand() % 4) * 90; // �������
		balls[i].timer = 0;
	}
}

// �������⶯��״̬
void updateSunshine() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++){
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
			
			if (balls[i].timer ==0){
				balls[i].y += 2;
			}

			if (balls[i].y > balls[i].destY) {
				balls[i].timer++;
				if (balls[i].timer > 100) {
					// ������ʧ
					balls[i].used = false;
				}
			}
		}
	}
}

// ������ʬ
void createZombie() {
	// ��ʬƵ�� �� ��ʬ����
	static int zombieFre = 200;
	static int zombieCount = 0;
	zombieCount++;
	if (zombieCount > zombieFre) {
		zombieCount = 0;
		zombieFre = rand() % 200 + 300;

		int i;
		int zombieMax = sizeof(zombies) / sizeof(zombies[0]);
		for (i = 0; i < zombieMax && zombies[i].used; i++);
			if (i < zombieMax) {
				zombies[i].used = true;
				zombies[i].x = WIN_WIDTH;
				zombies[i].y = 100 * (1 + rand() % 3) + 172;
				zombies[i].speed = 1;
			}


	}
}

// ���½�ʬ����״̬
void updateZombie() {
	int zombieMax = sizeof(zombies) / sizeof(zombies[0]);
	static int count = 0;
	count++;
	// ���� �ٶȱ��� �������� ����һ��
	if (count > 2) {
		count = 0;

		// ���½�ʬ��xyλ��
		for (int i = 0; i < zombieMax; i++) {
			if (zombies[i].used) {
				// ��ʬx���� ��ȥ ��ʬ�ƶ��ٶ�
				zombies[i].x -= zombies[i].speed;
				if (zombies[i].x < 170) {
					printf("GAME OVER\n");
					MessageBox(NULL, "over", "over", 0); // ��Ϸʧ�ܽ���
					exit(0);
				}
			}
		}
	}
	static int speedCount = 0;
	speedCount++;
	// ������ʬ֡�����ٶ�
	if (speedCount > 3){
		speedCount = 0;
		for (int i = 0; i < zombieMax; i++) {
			if (zombies[i].used) {
				zombies[i].frameIndex = (zombies[i].frameIndex + 1) % 22;
			}
		}
	}
	
}

// ��Ϸ���ݸ���
void updateGame() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0){
				map[i][j].frameIndex++;

				int plantType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;

				if (imgPlant[plantType][index] == NULL) {
					map[i][j].frameIndex = 0;
				}
			}
		}
	}

	createSunshine(); // �������⺯��
	updateSunshine(); // �������⶯��״̬
	createZombie(); // ������ʬ	
	updateZombie(); // ���½�ʬ����״̬
}

// �˵���ҳ��
void startUI() {
	IMAGE imgMenu,imgMenu2,imgMenu3;
	loadimage(&imgMenu, "res/menu.png");
	loadimage(&imgMenu2, "res/menu1.png");
	loadimage(&imgMenu3, "res/menu2.png");
	
	int flag = 0;

	while (1) {
		// �����Ͼ���һ����װ��˫���� �������һֱˢ����Ļ��˸����
		// ����BeginBatchDraw�����еĻ�ͼ��������ʾ����Ļ�ϣ��������ڴ��н���
		// ֱ������EndBatchDraw��֮ǰ�������ڴ��л�ͼ�ĳ�Ʒ��һ��չʾ����Ļ��
		BeginBatchDraw();
		putimage(0, 0, &imgMenu);
		putimagePNG(474, 75, flag ? &imgMenu3 : &imgMenu2);
		EndBatchDraw();

		ExMessage msg;
		if (peekmessage(&msg)){
			if (msg.message == WM_LBUTTONDOWN && msg.x > 474 && msg.x < 474 + 320 && msg.y > 75 && msg.y < 75 + 140) {
				flag = 1;
			}else if (msg.message == WM_LBUTTONUP && msg.x > 474 && msg.x < 474 + 320 && msg.y > 75 && msg.y < 75 + 140){
				return;
				}
			}

			EndBatchDraw();
	}
		
}

int main() {
	// ��Ϸ��ʼ��
	gameInit();
	// ����������
	startUI();

	int timer = 0;
	bool flag = true;

	printf("start");
	// ��ѭ��
	while (1) {
		userClick();
		// tools�ļ��� ������ʱ����ʱ����� ��ʱʱ��-�ϴε���ʱ��
		timer += getDelay();
		if (timer > 30) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			// ��ʾ����
			updateWindow();
			// �ı���Ϸ����
			updateGame();
			flag = false;
		}
		
	}

	

	system("pause");
	return 0;
}
