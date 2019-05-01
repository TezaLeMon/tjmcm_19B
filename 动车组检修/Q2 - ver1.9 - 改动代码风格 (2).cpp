#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <time.h>
#include <algorithm>
using namespace std;
ofstream outfile;

#define is_prompt	//是否在屏幕打印提示

#define N_train 11	//列车总数
#define ordernumber (3+1)     //工序总数	+1（将到达时间也看作工序）
#define parallel N_train	//所有工序的最大并行机个数
#define parallel_0 N_train	//将列车到达时间模拟为一个工序
#define parallel_a 3       //工序a的车间个数
#define parallel_b 8       //工序b的车间个数
#define parallel_c 5       //工序c的车间个数
#define parallel_d 3       //工序d的车间个数
#define parallel_e 2       //工序e的车间个数
const int parallel_arr[ordernumber] = { N_train,parallel_a,parallel_b,parallel_c };

#define populationnumber 300	//每一代种群的个体数
#define G 1000	//循环代数
#define crossoverrate 0.1		//交叉概率
#define mutationrate 0.01		//变异概率

int usetime[N_train][ordernumber];	//i列车在j工序的加工时间
int machinetime[ordernumber][parallel] = { 0 };	//i工序的第j车间的统计时间
int starttime[N_train][ordernumber][parallel];	//i列车j工序在第k车间上开始加工的时间
int finishtime[N_train][ordernumber][parallel];	//i列车j工序在第k车间上完成加工的时间
int single_time_makespan[populationnumber];	//i列车的总流程加工时间
int order_chr[populationnumber][N_train];	//第几代的染色体顺序，即工件加工顺序
int time_makespan_total;	//总流程加工时间
double fits[populationnumber];//存储每一代种群每一个个体的适应度，便于进行选择操作

int initialization()	//初始化种群
{
	for (int i = 0; i < populationnumber; i++)	//生成一个列车个数的全排列的个体
		for (int j = 0; j < N_train; j++)
			order_chr[i][j] = j + 1;

	for (int i = 0; i < populationnumber; i++)	//将全排列的个体中随机选取两个基因位交换，重复工件个数次
		for (int j = 0; j < N_train; j++){
			int t1 = rand() % N_train;
			int t2 = rand() % N_train;
			int t = order_chr[i][t1];
			order_chr[i][t1] = order_chr[i][t2];
			order_chr[i][t2] = t;
		}

#if defined is_prompt
	for (int i = 0; i < populationnumber; i++){
		for (int j = 0; j < N_train; j++)
			cout << order_chr[i][j] << " ";
		cout << endl;
	}
#endif
	return 0;
}

int fitness(int x)	//计算适应度
{
	int totaltime;	//总加工流程时间（time_makespan_total）
	int temp1[N_train] = { 0 };	//temp1暂时存储个体x的基因序列，以便进行不同工序之间的加工时记录列车检修先后顺序
	int temp2[N_train] = { 0 };	//temp2暂时存储每辆列车的完工时间赋予，便于决定下一道工序的列车检修顺序；
	int i;

	memcpy(temp1, order_chr[x], N_train * sizeof(int));

	for (i = 0; i < ordernumber; i++){
		for (int j = 0; j < N_train; j++)	//该循环的目的是通过比较所有车间的当前工作时间，找出最先空闲的车间
		{
			int m = machinetime[i][0];		//先记录i工序的第一辆列车的当前工作时间
			int n = 0;
			for (int p = 0; p < parallel_arr[i]; p++){	//与其他并行列车进行比较，找出时间最小的列车
				if (m > machinetime[i][p]){
					m = machinetime[i][p];
					n = p;
				}
			}
			int q = temp1[j];		//按顺序提取temp1中的列车号，对列车进行检修
			starttime[q - 1][i][n] = max(machinetime[i][n], temp2[j]);	//开始检修时间取该机器的当前时间和该列车上一道工序完工时间的最大值
			machinetime[i][n] = starttime[q - 1][i][n] + usetime[q - 1][i];	//机器的累计加工时间等于机器开始加工的时刻，加上该列车检修所用的时间
			finishtime[q - 1][i][n] = machinetime[i][n];			//列车的完工时间就是该车间当前的累计加工时间
			temp2[j] = finishtime[q - 1][i][n];		//将列车的完工时间赋予temp2，根据完工时间的快慢，便于决定下一道工序的工件加工顺序
		}

		int temp3[N_train];		//生成暂时数组，便于将temp1和temp2中的工件重新排列
		memcpy(temp3, temp1, N_train * sizeof(int));

		for (int e = 0; e < N_train - 1; e++){
			for (int ee = 0; ee < N_train - 1 - e; ee++){	//由于temp2存储列车上一道工序的完工时间，在进行下一道工序生产时，按照先完工先生产的原则
				if (temp2[ee] > temp2[ee + 1]){				//因此，该循环的目的在于将temp2中按照加工时间从小到大排列，同时temp1相应进行变换来记录temp2中的工件号
					int t1 = temp2[ee];
					int t2 = temp3[ee];
					temp2[ee] = temp2[ee + 1];
					temp3[ee] = temp3[ee + 1];
					temp2[ee + 1] = t1;
					temp3[ee + 1] = t2;
				}
			}
		}
	}

	totaltime = 0;
	for (i = 0; i < parallel_arr[ordernumber - 1]; i++)	//比较最后一道工序车间的累计加工时间，最大时间即该工序的加工时间
		if (totaltime < machinetime[ordernumber - 1][i])
			totaltime = machinetime[ordernumber - 1][i];

	for (i = 0; i < N_train; i++)	//将数组归零，便于下一个个体的加工时间统计
		for (int j = 0; j < ordernumber; j++)
			for (int t = 0; t < parallel_arr[j]; t++){
				starttime[i][j][t] = 0;
				finishtime[i][j][t] = 0;
				machinetime[j][t] = 0;
			}
	time_makespan_total = totaltime;
	fits[x] = 1.0 / time_makespan_total;		//将time_makespan_total取倒数作为适应度函数
	return 0;
}

int select()
{
	double roulette[populationnumber + 1] = { 0 };	//记录轮盘赌的每一个概率区间
	double pro_single[populationnumber];			//记录每个个体出现的概率，即个体的适应度除以总体适应度之和
	double totalfitness = 0;						//种群所有个体的适应度之和
	int temp1_order_chr[populationnumber][N_train];	//存储order_chr中所有个体的染色体
	int i;

	for (i = 0; i < populationnumber; i++)		//计算所有个体适应度的总和
		totalfitness = totalfitness + fits[i];

	for (i = 0; i < populationnumber; i++){
		pro_single[i] = fits[i] / totalfitness;		//计算每个个体适应度与总体适应度之比
		roulette[i + 1] = roulette[i] + pro_single[i];	//将每个个体的概率累加，构造轮盘赌
	}

	for (i = 0; i < populationnumber; i++)
		for (int j = 0; j < N_train; j++)
			temp1_order_chr[i][j] = order_chr[i][j];	//temp1_order_chr暂时存储order_chr的值

	for (i = 0; i < populationnumber; i++){
		int temp_order;		//当识别出所属区间之后，temp_order记录区间的序号
		double p = rand() % (10000) / 10000.0;
		for (int j = 0; j < populationnumber; j++)
			if (p >= roulette[j] && p < roulette[j + 1])
				temp_order = j;
		for (int m = 0; m < N_train; m++)
			order_chr[i][m] = temp1_order_chr[temp_order][m];
	}
	return 0;
}

int crossover()
/*	种群中的个体随机进行两两配对，配对成功的两个个体作为父代1和父代2进行交叉操作。
随机生成两个不同的基因点位，子代1继承父代2基因位之间的基因片段，其余基因按顺序集成父代1中未重复的基因；
子代2继承父代1基因位之间的基因片段，其余基因按顺序集成父代2中未重复的基因。	*/
{
	for (int i = 0; i < populationnumber / 2; i++){	//将所有个体平均分成两部分，一部分为交叉的父代1，一部分为进行交叉的父代2
		int n1 = 1 + rand() % N_train / 2;		//生成两个不同的基因位
		int n2 = n1 + rand() % (N_train - n1 - 1) + 1;
		double n_rand = rand() % 1000 / 1000.0;
		if (n_rand - crossoverrate < 1e-9)		//发生交叉操作的概率
		{
			int temp1[N_train] = { 0 };
			int temp2[N_train] = { 0 };
			int j;
			for (j = 0; j < N_train; j++)
			{
				int flg1 = 0, flg2 = 0;
				for (int p = n1; p < n2; p++)	//将交叉点位之间的基因片段进行交叉，temp1和temp2记录没有发生重复的基因
					if (order_chr[2 * i + 1][p] == order_chr[2 * i][j]) {
						flg1 = 1;
						break;
					}
				if (!flg1)
					temp1[j] = order_chr[2 * i][j];

				for (int p = n1; p < n2; p++)
					if (order_chr[2 * i][p] == order_chr[2 * i + 1][j]){
						flg2 = 1;
						break;
					}
				if (!flg2)
					temp2[j] = order_chr[2 * i + 1][j];
			}


			for (j = n1; j < n2; j++){		//子代1继承父代2交叉点位之间的基因,子代2继承父代1交叉点位之间的基因
				int t = order_chr[2 * i][j];
				order_chr[2 * i][j] = order_chr[2 * i + 1][j];
				order_chr[2 * i + 1][j] = t;
			}

			int p, q;
			for (p = 0; p < n1; p++)		//子代1第一交叉点之前的基因片段，按顺序依次继承父代1中未与子代1重复的基因
				for (q = 0; q < N_train; q++)
					if (temp1[q]){
						order_chr[2 * i][p] = temp1[q];
						temp1[q] = 0;
						break;
					}

			for (p = 0; p < n1; p++)		//子代2第一交叉点之前的基因片段，按顺序依次继承父代2中未与子代2重复的基因
				for (q = 0; q < N_train; q++)
					if (temp2[q]){
						order_chr[2 * i + 1][p] = temp2[q];
						temp2[q] = 0;
						break;
					}

			for (p = n2; p < N_train; p++)		//子代1第二交叉点之后的基因片段，按顺序依次继承父代1中未与子代1重复的基因
				for (q = 0; q < N_train; q++)
					if (temp1[q]){
						order_chr[2 * i][p] = temp1[q];
						break;
					}

			for (p = n2; p < N_train; p++)		//子代2第二交叉点之后的基因片段，按顺序依次继承父代2中未与子代2重复的基因
				for (q = 0; q < N_train; q++)
					if (temp2[q]){
						order_chr[2 * i + 1][p] = temp2[q];
						break;
					}
		}
	}
	return 0;

}

int mutation()	//两点变异，随机生成两个基因位，并交换两个基因的位置
{
	double n_rand = rand() % 1000 / 1000.0;
	if (n_rand - mutationrate < 1e-9)	//发生变异的概率
		for (int i = 0; i < populationnumber; i++){
			int t1 = rand() % N_train;
			int t2 = rand() % N_train;
			int t = order_chr[i][t1];
			order_chr[i][t1] = order_chr[i][t2];
			order_chr[i][t2] = t;
		}
	return 0;
}

int gant(int c)		//将结果展示并做成甘特图
{
	int totaltime;
	char machine[ordernumber * parallel][1000];
	memset(machine, '0', ordernumber * parallel * 1000 * sizeof(char));

	int temp1[N_train] = { 0 };	//维修顺序
	int temp2[N_train] = { 0 };	//上一步骤的完成时间
	int i;

	for (i = 0; i < N_train; i++)
		temp1[i] = order_chr[c][i];

	for (i = 0; i < ordernumber; i++) {
		for (int j = 0; j < N_train; j++) {
			int m = machinetime[i][0];
			int n = 0;

			for (int p = 0; p < parallel_arr[i]; p++)	//找出时间最小的车间
				if (m > machinetime[i][p]) {
					m = machinetime[i][p];
					n = p;
				}
			int q = temp1[j];

			starttime[q - 1][i][n] = max(machinetime[i][n], temp2[j]);
			machinetime[i][n] = starttime[q - 1][i][n] + usetime[q - 1][i];
			finishtime[q - 1][i][n] = machinetime[i][n];
			temp2[j] = finishtime[q - 1][i][n];
			for (int h = starttime[q - 1][i][n]; h < finishtime[q - 1][i][n]; h++)
				machine[i * N_train + n][h] = '0' + q;
		}

		int temp3[N_train];
		memcpy(temp3, temp1, N_train * sizeof(int));

		for (int e = 0; e < N_train - 1; e++)
			for (int ee = 0; ee < N_train - 1 - e; ee++)
				if (temp2[ee] > temp2[ee + 1]) {
					int t1 = temp2[ee];
					int t2 = temp3[ee];
					temp2[ee] = temp2[ee + 1];
					temp3[ee] = temp3[ee + 1];
					temp2[ee + 1] = t1;
					temp3[ee + 1] = t2;
				}
	}

	totaltime = 0;
	for (i = 0; i < parallel_arr[ordernumber - 1]; i++)
		if (totaltime < machinetime[ordernumber - 1][i])
			totaltime = machinetime[ordernumber - 1][i];

#if defined is_prompt
	cout << "total=" << totaltime << endl;
	outfile << totaltime << endl;
#endif

	for (int e = parallel; e < ordernumber * parallel; e++) {
		if (e % parallel > parallel_arr[e / parallel])
			continue;
		for (int ee = 0; ee < 1000; ee++) {
			char ch_out = machine[e][ee] > '9' ? 'a' + machine[e][ee] - '9' - 1 : machine[e][ee];
			if (machine[e][ee] == '0')
				ch_out = ' ';
			outfile << ch_out;
#if defined is_prompt
			cout << ch_out;
#endif
		}
		outfile << endl;
#if defined is_prompt
		cout << endl;
#endif
	}
	return 0;
}

int main()
{
	ifstream ifs("input.txt", ios::in);
	outfile.open("output.txt", ios::out);
	if (!ifs.is_open()){
		cout << "打开文件失败！" << endl;
		return -1;
	}

	for (int i = 0; i < N_train; i++) {
		for (int j = 0; j < ordernumber; j++) {
			ifs >> usetime[i][j];	//读入已知的加工时间
#if defined is_prompt
			cout << usetime[i][j] << "  ";
#endif
		}
#if defined is_prompt
		cout << endl;
#endif
	}
	ifs.close();

#if defined is_prompt
	cout << endl << endl;
#endif
	srand((unsigned)time(NULL));

	initialization();    //初始化种群
	for (int g = 0; g < G; g++){
		for (int c = 0; c < populationnumber; c++){	//计算每个个体适应度并存在single_time_makespan中
			fitness(c);
			single_time_makespan[c] = time_makespan_total;
		}
		select();     //选择
		crossover();  //交叉
		mutation();   //变异
	}

	int time_min = single_time_makespan[0];
	int n_min = 0;
	for (int c = 0; c < populationnumber - 1; c++)  //计算最后一代每个个体的适应度，并找出最优个体
		if (single_time_makespan[c] < time_min)		{
			time_min = single_time_makespan[c];
			n_min = c;
		}

	gant(n_min);   //画出甘特图
	outfile.close();
	return 0;
}