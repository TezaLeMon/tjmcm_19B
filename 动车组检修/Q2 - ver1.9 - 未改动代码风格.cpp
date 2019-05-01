#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <time.h>
#include <algorithm>
using namespace std;
ofstream outfile;

#define is_prompt	//是否在屏幕打印提示

#define workpiecesnumber 11	//工件总数
#define ordernumber (3+1)     //工序数
#define parallel workpiecesnumber	//所有工序的最大并行机个数
#define parallel_0 workpiecesnumber	//将列车到达时间模拟为一个工序
#define parallel_a 3       //工序a的最大并行机个数
#define parallel_b 8       //工序b的最大并行机个数
#define parallel_c 5       //工序c的最大并行机个数
#define parallel_d        //工序d的最大并行机个数
#define parallel_e        //工序e的最大并行机个数
#define parallel_f        //工序f的最大并行机个数
const int parallel_arr[ordernumber] = { workpiecesnumber,parallel_a,parallel_b,parallel_c };

#define populationnumber 300	//每一代种群的个体数
const double crossoverrate = 0.1;		//交叉概率
const double mutationrate = 0.01;		//变异概率
int G = 1000;		//循环代数100
int usetime[workpiecesnumber][ordernumber];	//第几个工件第几道工序的加工用时；
int machinetime[ordernumber][parallel] = { 0 };	//第几道工序的第几台并行机器的统计时间；
int starttime[workpiecesnumber][ordernumber][parallel];	//第几个工件第几道工序在第几台并行机上开始加工的时间；
int finishtime[workpiecesnumber][ordernumber][parallel];	//第几个工件第几道工序在第几台并行机上完成加工的时间；
int single_time_makespan[populationnumber];	//个体的time_makespan_total；
int order_chr[populationnumber][workpiecesnumber];	//第几代的染色体顺序，即工件加工顺序；
int time_makespan_total;	//总的流程加工时间；
int temp_time_makespan;   //暂时存储流程加工时间；
double fits[populationnumber];//存储每一代种群每一个个体的适应度，便于进行选择操作；

int initialization()   //初始化种群；
{
	for (int i = 0; i < populationnumber; i++)     //首先生成一个工件个数的全排列的个体；
		for (int j = 0; j < workpiecesnumber; j++)
			order_chr[i][j] = j + 1;

	for (int i = 0; i < populationnumber; i++)     //将全排列的个体中随机选取两个基因位交换，重复工件个数次，以形成随机初始种群；
		for (int j = 0; j < workpiecesnumber; j++){
			int t1 = rand() % workpiecesnumber;
			int t2 = rand() % workpiecesnumber;
			int t = order_chr[i][t1];
			order_chr[i][t1] = order_chr[i][t2];
			order_chr[i][t2] = t;
		}

#ifdef is_prompt
	for (int i = 0; i < populationnumber; i++){
		for (int j = 0; j < workpiecesnumber; j++)
			cout << order_chr[i][j] << " ";
		cout << endl;
	}
#endif
	return 0;
}

int fitness(int c)   //计算适应度函数，c代表某个体；
{
	int totaltime;      //总的加工流程时间（time_makespan_total）；
	int temp1[workpiecesnumber] = { 0 };	//temp1暂时存储个体c的基因序列，以便进行不同流程之间的加工时记录工件加工先后顺序；
	int temp2[workpiecesnumber] = { 0 };	//temp2暂时存储每个工件的完工时间赋予，便于决定下一道工序的工件加工顺序；

	memcpy(temp1, order_chr[c], workpiecesnumber * sizeof(int));

	for (int i = 0; i < ordernumber; i++){
		for (int j = 0; j < workpiecesnumber; j++)  //该循环的目的是通过比较所有机器的当前工作时间，找出最先空闲的机器，便于新的工件生产；
		{
			int m = machinetime[i][0];        //先记录第i道工序的第一台并行机器的当前工作时间；
			int n = 0;
			for (int p = 0; p < parallel_arr[i]; p++){ //与其他并行机器进行比较，找出时间最小的机器；
				if (m > machinetime[i][p]){
					m = machinetime[i][p];
					n = p;
				}
			}
			int q = temp1[j];                 //按顺序提取temp1中的工件号，对工件进行加工；
			starttime[q - 1][i][n] = max(machinetime[i][n], temp2[j]);  //开始加工时间取该机器的当前时间和该工件上一道工序完工时间的最大值；
			machinetime[i][n] = starttime[q - 1][i][n] + usetime[q - 1][i]; //机器的累计加工时间等于机器开始加工的时刻，加上该工件加工所用的时间；
			finishtime[q - 1][i][n] = machinetime[i][n];                 //工件的完工时间就是该机器当前的累计加工时间；
			temp2[j] = finishtime[q - 1][i][n];       //将每个工件的完工时间赋予temp2，根据完工时间的快慢，便于决定下一道工序的工件加工顺序；
		}

		int temp3[workpiecesnumber];		//生成暂时数组，便于将temp1和temp2中的工件重新排列；
		memcpy(temp3, temp1, workpiecesnumber * sizeof(int));

		for (int e = 0; e < workpiecesnumber - 1; e++){
			for (int ee = 0; ee < workpiecesnumber - 1 - e; ee++){	//由于temp2存储工件上一道工序的完工时间，在进行下一道工序生产时，按照先完工先生产的原则，
				if (temp2[ee] > temp2[ee + 1]){						//因此，该循环的目的在于将temp2中按照加工时间从小到大排列，同时temp1相应进行变换来记录temp2中的工件号；
					int t1 = temp2[ee];
					int t2 = temp3[ee];
					temp2[ee] = temp2[ee + 1];
					temp3[ee] = temp3[ee + 1];
					temp2[ee + 1] = t1;
					temp3[ee + 1] = t2;
				}
			}
		}
		memcpy(temp1, temp3, workpiecesnumber * sizeof(int));
	}

	totaltime = 0;
	for (int i = 0; i < parallel_arr[ordernumber - 1]; i++) //比较最后一道工序机器的累计加工时间，最大时间就是该流程的加工时间；
		if (totaltime < machinetime[ordernumber - 1][i])
			totaltime = machinetime[ordernumber - 1][i];

	for (int i = 0; i < workpiecesnumber; i++)  //将数组归零，便于下一个个体的加工时间统计；
		for (int j = 0; j < ordernumber; j++)
			for (int t = 0; t < parallel_arr[j]; t++){
				starttime[i][j][t] = 0;
				finishtime[i][j][t] = 0;
				machinetime[j][t] = 0;
			}
	time_makespan_total = totaltime;
	fits[c] = 1.000 / time_makespan_total;          //将time_makespan_total取倒数作为适应度函数；
	return 0;
}


int gant(int c)                   //该函数是为了将最后的结果便于清晰明朗的展示并做成甘特图，对问题的结果以及问题的解决并没有影响；
{
	int totaltime;
	char machine[ordernumber * parallel][1000];
	memset(machine, '0', ordernumber * parallel * 1000 * sizeof(char));

	int temp1[workpiecesnumber] = { 0 }; //加工顺序
	int temp2[workpiecesnumber] = { 0 }; //上一步骤的完成时间

	for (int j = 0; j < workpiecesnumber; j++)
		temp1[j] = order_chr[c][j];

	for (int i = 0; i < ordernumber; i++){
		for (int j = 0; j < workpiecesnumber; j++){
			int m = machinetime[i][0];
			int n = 0;

			for (int p = 0; p < parallel_arr[i]; p++) //找出时间最小的机器；
				if (m > machinetime[i][p]){
					m = machinetime[i][p];
					n = p;
				}
			int q = temp1[j];

			starttime[q - 1][i][n] = max(machinetime[i][n], temp2[j]);
			machinetime[i][n] = starttime[q - 1][i][n] + usetime[q - 1][i];
			finishtime[q - 1][i][n] = machinetime[i][n];
			temp2[j] = finishtime[q - 1][i][n];
			//cout<<"start:"<<starttime[q-1][i][n]<<"   use:"<<usetime[q-1][i]<<"  machine:"<<machinetime[i][n]<<"   finish:"<<finishtime[q-1][i][n]<<endl;
			int h;
			for (h = starttime[q - 1][i][n]; h < finishtime[q - 1][i][n]; h++)
				machine[i * workpiecesnumber + n][h] = '0' + q;
		}

		int temp3[workpiecesnumber];
		for (int s = 0; s < workpiecesnumber; s++)
			temp3[s] = temp1[s];

		for (int e = 0; e < workpiecesnumber - 1; e++)
			for (int ee = 0; ee < workpiecesnumber - 1 - e; ee++)
				if (temp2[ee] > temp2[ee + 1]){
					int t1 = temp2[ee];
					int t2 = temp3[ee];
					temp2[ee] = temp2[ee + 1];
					temp3[ee] = temp3[ee + 1];
					temp2[ee + 1] = t1;
					temp3[ee + 1] = t2;
				}

		for (int e = 0; e < workpiecesnumber; e++)
			temp1[e] = temp3[e];
	}

	totaltime = 0;
	for (int i = 0; i < parallel_arr[ordernumber - 1]; i++)
		if (totaltime < machinetime[ordernumber - 1][i])
			totaltime = machinetime[ordernumber - 1][i];

#ifdef is_prompt
	cout << "total=" << totaltime << endl;
	outfile << totaltime << endl;
#endif

	temp_time_makespan = totaltime;
	for (int e = parallel; e < ordernumber * parallel; e++){	//不输出到达时间对应的工序
		if (e % parallel > parallel_arr[e / parallel])
			continue;
		for (int ee = 0; ee < 1000; ee++){
			char ch_out = machine[e][ee] > '9' ? 'a' + machine[e][ee] - '9' - 1 : machine[e][ee];
			if (machine[e][ee] == '0')
				ch_out = ' ';
			outfile << ch_out;
#ifdef is_prompt
			cout << ch_out;
#endif
		}
		outfile << endl;
#ifdef is_prompt
		cout << endl;
#endif
	}

	return 0;
}


int select()
{
	double roulette[populationnumber + 1] = { 0 };	//记录轮盘赌的每一个概率区间；
	double pro_single[populationnumber];			//记录每个个体出现的概率，即个体的适应度除以总体适应度之和；
	double totalfitness = 0;                	//种群所有个体的适应度之和；
	int temp1_order_chr[populationnumber][workpiecesnumber];	//存储order_chr中所有个体的染色体；

	for (int i = 0; i < populationnumber; i++)     		//计算所有个体适应度的总和；
		totalfitness = totalfitness + fits[i];

	for (int i = 0; i < populationnumber; i++){
		pro_single[i] = fits[i] / totalfitness;   	//计算每个个体适应度与总体适应度之比；
		roulette[i + 1] = roulette[i] + pro_single[i]; //将每个个体的概率累加，构造轮盘赌；
	}

	for (int i = 0; i < populationnumber; i++)
		for (int j = 0; j < workpiecesnumber; j++)
			temp1_order_chr[i][j] = order_chr[i][j];               //temp1_order_chr暂时存储order_chr的值；

	for (int i = 0; i < populationnumber; i++){
		int temp_order;   //当识别出所属区间之后，temp_order记录区间的序号；
		double p = rand() % (10000) / 10000.0;
		for (int j = 0; j < populationnumber; j++)
			if (p >= roulette[j] && p < roulette[j + 1])
				temp_order = j;
		for (int m = 0; m < workpiecesnumber; m++)
			order_chr[i][m] = temp1_order_chr[temp_order][m];
	}
	return 0;
}

int crossover()
/*	种群中的个体随机进行两两配对，配对成功的两个个体作为父代1和父代2进行交叉操作。
随机生成两个不同的基因点位，子代1继承父代2基因位之间的基因片段，其余基因按顺序集成父代1中未重复的基因；
子代2继承父代1基因位之间的基因片段，其余基因按顺序集成父代2中未重复的基因。	*/
{
	for (int i = 0; i < populationnumber / 2; i++){ //将所有个体平均分成两部分，一部分为交叉的父代1，一部分为进行交叉的父代2；
		int n1 = 1 + rand() % workpiecesnumber / 2;    //生成两个不同的基因位；
		int n2 = n1 + rand() % (workpiecesnumber - n1 - 1) + 1;
		double n_rand = rand() % 10000 / 10000.0;
		if (n_rand - crossoverrate < 1e-9)   //发生交叉操作的概率
		{
			int temp1[workpiecesnumber] = { 0 };
			int temp2[workpiecesnumber] = { 0 };
			for (int j = 0; j < workpiecesnumber; j++)
			{
				int flg1 = 0, flg2 = 0;
				for (int p = n1; p < n2; p++)          //将交叉点位之间的基因片段进行交叉，temp1和temp2记录没有发生重复的基因；
					if (order_chr[2 * i + 1][p] == order_chr[2 * i][j])
						flg1 = 1;
				if (!flg1)
					temp1[j] = order_chr[2 * i][j];

				for (int p = n1; p < n2; p++)
					if (order_chr[2 * i][p] == order_chr[2 * i + 1][j])
						flg2 = 1;

				if (!flg2)
					temp2[j] = order_chr[2 * i + 1][j];
			}


			for (int j = n1; j < n2; j++){             //子代1继承父代2交叉点位之间的基因；子代2继承父代1交叉点位之间的基因；
				int t = order_chr[2 * i][j];
				order_chr[2 * i][j] = order_chr[2 * i + 1][j];
				order_chr[2 * i + 1][j] = t;
			}
			for (int p = 0; p < n1; p++)               //子代1第一交叉点之前的基因片段，按顺序依次继承父代1中未与子代1重复的基因；
				for (int q = 0; q < workpiecesnumber; q++)
					if (temp1[q]){
						order_chr[2 * i][p] = temp1[q];
						temp1[q] = 0;
						break;
					}

			for (int p = 0; p < n1; p++)               //子代2第一交叉点之前的基因片段，按顺序依次继承父代2中未与子代2重复的基因；
				for (int m = 0; m < workpiecesnumber; m++)
					if (temp2[m]){
						order_chr[2 * i + 1][p] = temp2[m];
						temp2[m] = 0;
						break;
					}

			for (int p = n2; p < workpiecesnumber; p++)             //子代1第2交叉点之后的基因片段，按顺序依次继承父代1中未与子代1重复的基因；
				for (int q = 0; q < workpiecesnumber; q++)
					if (temp1[q]){
						order_chr[2 * i][p] = temp1[q]; temp1[q] = 0;
						break;
					}

			for (int p = n2; p < workpiecesnumber; p++)               //子代2第2交叉点之后的基因片段，按顺序依次继承父代2中未与子代2重复的基因；
				for (int m = 0; m < workpiecesnumber; m++)
					if (temp2[m]){
						order_chr[2 * i + 1][p] = temp2[m]; temp2[m] = 0;
						break;
					}
		}
	}
	return 0;

}

int mutation()  //变异操作为两点变异，随机生成两个基因位，并交换两个基因的位置；
{
	double n_rand = rand() % 10000 / 10000.0;
	if (n_rand - mutationrate < 1e-9)	//发生变异的概率
		for (int i = 0; i < populationnumber; i++){
			int t1 = rand() % workpiecesnumber;
			int t2 = rand() % workpiecesnumber;
			int t = order_chr[i][t1];
			order_chr[i][t1] = order_chr[i][t2];
			order_chr[i][t2] = t;
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

	for (int i = 0; i < workpiecesnumber; i++) {
		for (int j = 0; j < ordernumber; j++) {
			ifs >> usetime[i][j];
#if defined is_prompt
			cout << usetime[i][j] << "  ";
#endif
		}
#if defined is_prompt
		cout << endl;
#endif
	}
	ifs.close();  //读入已知的加工时间；

#if defined is_prompt
	cout << endl << endl;
#endif
	srand((unsigned)time(NULL));

	initialization();    //初始化种群；
	for (int g = 0; g < G; g++){
		for (int c = 0; c < populationnumber; c++){//计算每个个体适应度并存在single_time_makespan中；
			fitness(c);
			single_time_makespan[c] = time_makespan_total;
		}
		select();     //选择操作；
		crossover();  //交叉操作；
		mutation();   //变异操作；
	}

	int time_min = single_time_makespan[0];
	int n_min = 0;
	for (int c = 0; c < populationnumber - 1; c++)  //计算最后一代每个个体的适应度，并找出最优个体；
		if (single_time_makespan[c] < time_min)		{
			time_min = single_time_makespan[c];
			n_min = c;
		}

	gant(n_min);   //画出简易的流程图；
	outfile.close();
	return 0;
}