/* 
	开发日志
	1.创建项目
	2.导入素材
	3.实现游戏背景
	4.实现游戏顶部工具栏
	5.实现工具栏植物卡显示
*/

#include <stdio.h>
#include <graphics.h>
#include <time.h>
#include "tools.h" 


#define WIN_WIDTH  900
#define WIN_HEIGHT 600
//#define PLANT_COUNT 4

// 枚举
enum {Pea,Sunflower,PlantCount};

IMAGE imgBar;
IMAGE imgBg;
IMAGE imgPlantCards[PlantCount];
// 指针指向对应的数据
IMAGE* imgPlant[PlantCount][20];

int curX, curY; // 当前选中的植物，在移动过程的位置
int curPlant; // 0：没有选中 1：选择第一种植物

// 定义结构体
struct plant {
	int type; // 0：表示没有植物 1：表示选择第一种植物
	int frameIndex; // 植物动画序列帧数序号
};

struct plant map[3][9];

struct sunshineBall {
	int x, y; // 阳光坐标 x不变 y变
	int frameIndex; // 阳光动画序列帧数序号
	int destY; // 阳光飘落目标位置的y坐标
	bool used; // 是否在使用
	int timer; // 计时器
};

// 后端开发 内存池
struct sunshineBall balls[10];
IMAGE imgSunshineBall[29];

bool fileExist(const char* name) {
	// 属性关闭sdl检测
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

// 游戏初始化
void gameInit()
{	
	// 加载背景图片
	// 把字符集修改为多字节字符集
	loadimage(&imgBg,"res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	// memset() 函数将指定的值 c 复制到 str 所指向的内存区域的前 n 个字节中，这可以用于将内存块清零或设置为特定值。在一些情况下，需要快速初始化大块内存为零或者特定值，memset() 可以提供高效的实现。
	memset(imgPlant,0,sizeof(imgPlant));
	memset(map, 0, sizeof(map));
	memset(balls,0,sizeof(balls));

	// 初始化植物卡牌
	char PlantName[64];
	for (int i = 0; i < PlantCount; i++) {
		// 生成植物卡牌的文件名
		sprintf_s(PlantName, sizeof(PlantName), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgPlantCards[i],PlantName);

		for (int j = 0; j < 20;j++) {
			sprintf_s(PlantName, sizeof(PlantName), "res/plant/%d/%d.png", i,j + 1);
			// 判断文件是否存在
			if (fileExist(PlantName)) {
				imgPlant[i][j] = new IMAGE;
				loadimage(imgPlant[i][j], PlantName);
			}
			else {
				break;
			}
		}
	}

	curPlant = 0;

	// 初始化阳光
	for (int i = 0; i < 29; i++) {
		sprintf_s(PlantName, sizeof(PlantName), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], PlantName);
	}

	// 配置随机种子
	srand(time(NULL));
	
	// 创建游戏窗口 initgraph(窗口宽度,窗口高度,控制台)
	initgraph(WIN_WIDTH,WIN_HEIGHT,1);
}


// 游戏数据更新
void updateWindow() {
	// 设置缓冲 防止在死循环中图片闪烁
	BeginBatchDraw(); // 打印在特殊内存中 缓存

	// 渲染图片 putimage (x轴坐标，y轴坐标,图片路径)
	putimage(0, 0, &imgBg);
	//putimage(50, 0,&imgBar);
	putimagePNG(150, 0, &imgBar);

	// 渲染卡牌
	for (int i = 0; i < PlantCount; i++) {
		int x=238+i*65;
		int y = 6;
		putimage(x, y, &imgPlantCards[i]);
	}

	// 渲染种植下的植物
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0){
				int x = 256 + j * 81;
				int y = 179 + i * 102 + 14;
				//printf("%d,%d\n", x, y);
				// 植物选中的类型
				int plantType = map[i][j].type - 1;
				// 植物动画序列
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgPlant[plantType][index]);
			}
		}
	}

	// 渲染拖动过程中的植物
	if (curPlant > 0) {
		IMAGE* img = imgPlant[curPlant - 1][0];
		// img->get() 这种形式的语法通常用于访问结构体或类的成员函数或成员变量
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++){
		if(balls[i].used) {
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			putimagePNG(balls[i].x,balls[i].y,img);
		}
	}
	
	EndBatchDraw();// 结束缓存
}


// 用户点击操作
void userClick() {
	ExMessage msg;
	static int status = 0;

	// 判断当前有没有点击操作
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) // 鼠标左键
		{  
			if (msg.x > 238 && msg.x < 238+65*PlantCount && msg.y < 96)  //植物卡牌点击范围
			{  
				int index = (msg.x - 238) / 65;
				//printf("%d\n",index);
				status = 1;
				curPlant = index + 1;
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) // 鼠标拖动
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP) // 鼠标抬起
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

// 创建阳光
void createSunshine() {
	static int count = 0;
	static int fre = 50;
	count++;

	if (count >= fre){
		fre = rand() % 200 + 201;
		count = 0;

		// 从阳光池中取一个可以使用的阳光
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;

		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax) return;

		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].x = 260 + rand() % 600; // 260-900
		balls[i].y = 60;
		balls[i].destY = 220 + (rand() % 4) * 90; // 阳光落点
		balls[i].timer = 0;
	}
}

// 更新阳光动画状态
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
					// 阳光消失
					balls[i].used = false;
				}
			}
		}
	}
}

// 游戏数据更新
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

	createSunshine(); // 创建阳光函数
	updateSunshine();// 更新阳光动画状态
}

// 菜单主页面
void startUI() {
	IMAGE imgMenu,imgMenu2,imgMenu3;
	loadimage(&imgMenu, "res/menu.png");
	loadimage(&imgMenu2, "res/menu1.png");
	loadimage(&imgMenu3, "res/menu2.png");
	
	int flag = 0;

	while (1) {
		// 本质上就是一个封装的双缓存 解决动画一直刷新屏幕闪烁问题
		// 运行BeginBatchDraw后，所有的绘图都不再显示在屏幕上，而是在内存中进行
		// 直到碰到EndBatchDraw，之前所有在内存中绘图的成品将一并展示在屏幕中
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
	// 游戏初始化
	gameInit();
	// 主界面启动
	startUI();

	int timer = 0;
	bool flag = true;

	printf("start");
	// 死循环
	while (1) {
		userClick();
		// tools文件中 作运行时进行时间计数 当时时间-上次调用时间
		timer += getDelay();
		if (timer > 30) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			// 显示窗口
			updateWindow();
			// 改变游戏数据
			updateGame();
			flag = false;
		}
		
		//Sleep(10);
	}

	

	system("pause");
	return 0;
}