#include <iostream>
#include <queue>	//火车进出车间符合队列的概念
using namespace std;

#define DONE 2		//表示已经完成
#define SUCCEED 1	//表示成功进出
#define WAITING 0	//表示需要等待
#define FAIL -1		//表示操作失败

#define N_TRAIN 12*4	//火车总量
#define N_WORKSHOP 3	//车间总量

/*	考虑可能的后续条件的加入 先设定两个对象类型：[动车、车间]	*/

class WorkShop;

class Train
{
public:	//都使用public 不考虑不安全情况(懒
	int type_CRH = 0;		//Q1中 暂定所有动车类型相同
	int cost_time[N_WORKSHOP] = { 4,8,6 };		//Q1中 暂定一单位时间为15分钟 1/15min
	WorkShop* In_which_WorkShop;		//此火车所在车间 若已完成则指向空（NULL）

	int Done_work();	//完成一单位时的工作进度
};

class WorkShop
{
public:
	char type;				//车间类型
	int n;					//车间总数量
	queue<Train*> InsideTrain;	//车间火车队列

	WorkShop(char _type, int _n) {	//初始化车间信息（类型、总数量）
		type = _type;
		n = _n;
	}
	int Init(Train* t);		//将某一火车置入车间
	int Outit();			//将某一火车置出车间 且不置入其他车间（用于末尾车间）
	int MoveTrain(Train* t, WorkShop* Next_WorkShop);	//转换火车所在车间
};

int Train::Done_work() {
	if (In_which_WorkShop == NULL)	//已经完成
		return DONE;
	int i = In_which_WorkShop->type - 'a';	//得到当前火车处在哪一车间
	if (cost_time[i]) {		//当前工作量还没做完
		cost_time[i]--;		//做一单位工作量
		if (cost_time[i])	//仍未做完
			return SUCCEED;
		return WAITING;		//做完则进入等待状态
	}
	else
		return WAITING;		//当前工作量已经做完 维持等待状态
}

int WorkShop::Init(Train* t) {
	if (n == InsideTrain.size())
		return WAITING;		//所有车间已满，进站失败
	t->In_which_WorkShop = this;
	InsideTrain.push(t);		//火车进站
	return SUCCEED;
}

int WorkShop::Outit() {
	InsideTrain.front()->In_which_WorkShop = NULL;	//出站后不在任何车间
	InsideTrain.pop();
	return SUCCEED;
}

int WorkShop::MoveTrain(Train* t, WorkShop* Next_WorkShop) {
	if (Next_WorkShop->Init(t) == SUCCEED) {	//有空闲车间可以插入并且插入成功
		InsideTrain.pop();
		return SUCCEED;
	}
	return WAITING;		//无空闲车间 进入等待状态
}


int main()
{
	Train *train = new Train[N_TRAIN];

	WorkShop ws[N_WORKSHOP] = {		//车间初始化
		WorkShop('a',3),
		WorkShop('b',8),
		WorkShop('c',5) };

	int time, i, j, k = -1;
	for (i = 0, time = 0;;) {
		if (i < N_TRAIN) {
			if (ws[0].Init(&train[i]))		//尝试进a车间
				i++;		//成功则下一次让下一辆车尝试进a车间
		}
		else {		//所有车都已进入车间
			for (j = 0; j < N_WORKSHOP; j++)
				if (!ws[j].InsideTrain.empty())		//任意车间非空则退出（还有没维修完的火车
					break;
			if (j == N_WORKSHOP)	//所有火车已完成
				break;
		}

#if 0	//松开查看过程 回车继续
		//输出：
		//时间
		//火车编号	所在车间	所在车间剩余加工时长
		cout << time * 15 << "min" << endl;
		for (j = 0; j < i; j++)
			if(train[j].In_which_WorkShop)
				cout << j + 1 << '\t' << train[j].In_which_WorkShop->type << '\t'
				<< train[j].cost_time[train[j].In_which_WorkShop - ws] << endl;
		getchar();
#endif

		for (j = 0; j < i; j++) {
			if (train[j].In_which_WorkShop == NULL)		//已完成加工
				continue;
			int t = train[j].Done_work();		//做一单位时的工作
			if (t == WAITING) {			//火车处在等待状态
				k = train[j].In_which_WorkShop - ws;	//得到所在车间编号
				if (k == N_WORKSHOP - 1)		//处在最后一个车间
					ws[k].Outit();				//直接开出而不转移到其他车间
				else				//在最后一个车间之前
					ws[k].MoveTrain(&train[j], &ws[k + 1]);		//转移火车
			}
		}
		time++;		//计时器+1
	}

	cout << time * 15.0 / 60 << "Hours" << endl;
	return 0;
}