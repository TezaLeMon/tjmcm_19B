#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <time.h>
#include <algorithm>
using namespace std;
ofstream outfile;

#define is_prompt	//�Ƿ�����Ļ��ӡ��ʾ

#define workpiecesnumber 11	//��������
#define ordernumber (3+1)     //������
#define parallel workpiecesnumber	//���й��������л�����
#define parallel_0 workpiecesnumber	//���г�����ʱ��ģ��Ϊһ������
#define parallel_a 3       //����a������л�����
#define parallel_b 8       //����b������л�����
#define parallel_c 5       //����c������л�����
#define parallel_d        //����d������л�����
#define parallel_e        //����e������л�����
#define parallel_f        //����f������л�����
const int parallel_arr[ordernumber] = { workpiecesnumber,parallel_a,parallel_b,parallel_c };

#define populationnumber 300	//ÿһ����Ⱥ�ĸ�����
const double crossoverrate = 0.1;		//�������
const double mutationrate = 0.01;		//�������
int G = 1000;		//ѭ������100
int usetime[workpiecesnumber][ordernumber];	//�ڼ��������ڼ�������ļӹ���ʱ��
int machinetime[ordernumber][parallel] = { 0 };	//�ڼ�������ĵڼ�̨���л�����ͳ��ʱ�䣻
int starttime[workpiecesnumber][ordernumber][parallel];	//�ڼ��������ڼ��������ڵڼ�̨���л��Ͽ�ʼ�ӹ���ʱ�䣻
int finishtime[workpiecesnumber][ordernumber][parallel];	//�ڼ��������ڼ��������ڵڼ�̨���л�����ɼӹ���ʱ�䣻
int single_time_makespan[populationnumber];	//�����time_makespan_total��
int order_chr[populationnumber][workpiecesnumber];	//�ڼ�����Ⱦɫ��˳�򣬼������ӹ�˳��
int time_makespan_total;	//�ܵ����̼ӹ�ʱ�䣻
int temp_time_makespan;   //��ʱ�洢���̼ӹ�ʱ�䣻
double fits[populationnumber];//�洢ÿһ����Ⱥÿһ���������Ӧ�ȣ����ڽ���ѡ�������

int initialization()   //��ʼ����Ⱥ��
{
	for (int i = 0; i < populationnumber; i++)     //��������һ������������ȫ���еĸ��壻
		for (int j = 0; j < workpiecesnumber; j++)
			order_chr[i][j] = j + 1;

	for (int i = 0; i < populationnumber; i++)     //��ȫ���еĸ��������ѡȡ��������λ�������ظ����������Σ����γ������ʼ��Ⱥ��
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

int fitness(int c)   //������Ӧ�Ⱥ�����c����ĳ���壻
{
	int totaltime;      //�ܵļӹ�����ʱ�䣨time_makespan_total����
	int temp1[workpiecesnumber] = { 0 };	//temp1��ʱ�洢����c�Ļ������У��Ա���в�ͬ����֮��ļӹ�ʱ��¼�����ӹ��Ⱥ�˳��
	int temp2[workpiecesnumber] = { 0 };	//temp2��ʱ�洢ÿ���������깤ʱ�丳�裬���ھ�����һ������Ĺ����ӹ�˳��

	memcpy(temp1, order_chr[c], workpiecesnumber * sizeof(int));

	for (int i = 0; i < ordernumber; i++){
		for (int j = 0; j < workpiecesnumber; j++)  //��ѭ����Ŀ����ͨ���Ƚ����л����ĵ�ǰ����ʱ�䣬�ҳ����ȿ��еĻ����������µĹ���������
		{
			int m = machinetime[i][0];        //�ȼ�¼��i������ĵ�һ̨���л����ĵ�ǰ����ʱ�䣻
			int n = 0;
			for (int p = 0; p < parallel_arr[i]; p++){ //���������л������бȽϣ��ҳ�ʱ����С�Ļ�����
				if (m > machinetime[i][p]){
					m = machinetime[i][p];
					n = p;
				}
			}
			int q = temp1[j];                 //��˳����ȡtemp1�еĹ����ţ��Թ������мӹ���
			starttime[q - 1][i][n] = max(machinetime[i][n], temp2[j]);  //��ʼ�ӹ�ʱ��ȡ�û����ĵ�ǰʱ��͸ù�����һ�������깤ʱ������ֵ��
			machinetime[i][n] = starttime[q - 1][i][n] + usetime[q - 1][i]; //�������ۼƼӹ�ʱ����ڻ�����ʼ�ӹ���ʱ�̣����ϸù����ӹ����õ�ʱ�䣻
			finishtime[q - 1][i][n] = machinetime[i][n];                 //�������깤ʱ����Ǹû�����ǰ���ۼƼӹ�ʱ�䣻
			temp2[j] = finishtime[q - 1][i][n];       //��ÿ���������깤ʱ�丳��temp2�������깤ʱ��Ŀ��������ھ�����һ������Ĺ����ӹ�˳��
		}

		int temp3[workpiecesnumber];		//������ʱ���飬���ڽ�temp1��temp2�еĹ����������У�
		memcpy(temp3, temp1, workpiecesnumber * sizeof(int));

		for (int e = 0; e < workpiecesnumber - 1; e++){
			for (int ee = 0; ee < workpiecesnumber - 1 - e; ee++){	//����temp2�洢������һ��������깤ʱ�䣬�ڽ�����һ����������ʱ���������깤��������ԭ��
				if (temp2[ee] > temp2[ee + 1]){						//��ˣ���ѭ����Ŀ�����ڽ�temp2�а��ռӹ�ʱ���С�������У�ͬʱtemp1��Ӧ���б任����¼temp2�еĹ����ţ�
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
	for (int i = 0; i < parallel_arr[ordernumber - 1]; i++) //�Ƚ����һ������������ۼƼӹ�ʱ�䣬���ʱ����Ǹ����̵ļӹ�ʱ�䣻
		if (totaltime < machinetime[ordernumber - 1][i])
			totaltime = machinetime[ordernumber - 1][i];

	for (int i = 0; i < workpiecesnumber; i++)  //��������㣬������һ������ļӹ�ʱ��ͳ�ƣ�
		for (int j = 0; j < ordernumber; j++)
			for (int t = 0; t < parallel_arr[j]; t++){
				starttime[i][j][t] = 0;
				finishtime[i][j][t] = 0;
				machinetime[j][t] = 0;
			}
	time_makespan_total = totaltime;
	fits[c] = 1.000 / time_makespan_total;          //��time_makespan_totalȡ������Ϊ��Ӧ�Ⱥ�����
	return 0;
}


int gant(int c)                   //�ú�����Ϊ�˽����Ľ�������������ʵ�չʾ�����ɸ���ͼ��������Ľ���Լ�����Ľ����û��Ӱ�죻
{
	int totaltime;
	char machine[ordernumber * parallel][1000];
	memset(machine, '0', ordernumber * parallel * 1000 * sizeof(char));

	int temp1[workpiecesnumber] = { 0 }; //�ӹ�˳��
	int temp2[workpiecesnumber] = { 0 }; //��һ��������ʱ��

	for (int j = 0; j < workpiecesnumber; j++)
		temp1[j] = order_chr[c][j];

	for (int i = 0; i < ordernumber; i++){
		for (int j = 0; j < workpiecesnumber; j++){
			int m = machinetime[i][0];
			int n = 0;

			for (int p = 0; p < parallel_arr[i]; p++) //�ҳ�ʱ����С�Ļ�����
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
	for (int e = parallel; e < ordernumber * parallel; e++){	//���������ʱ���Ӧ�Ĺ���
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
	double roulette[populationnumber + 1] = { 0 };	//��¼���̶ĵ�ÿһ���������䣻
	double pro_single[populationnumber];			//��¼ÿ��������ֵĸ��ʣ����������Ӧ�ȳ���������Ӧ��֮�ͣ�
	double totalfitness = 0;                	//��Ⱥ���и������Ӧ��֮�ͣ�
	int temp1_order_chr[populationnumber][workpiecesnumber];	//�洢order_chr�����и����Ⱦɫ�壻

	for (int i = 0; i < populationnumber; i++)     		//�������и�����Ӧ�ȵ��ܺͣ�
		totalfitness = totalfitness + fits[i];

	for (int i = 0; i < populationnumber; i++){
		pro_single[i] = fits[i] / totalfitness;   	//����ÿ��������Ӧ����������Ӧ��֮�ȣ�
		roulette[i + 1] = roulette[i] + pro_single[i]; //��ÿ������ĸ����ۼӣ��������̶ģ�
	}

	for (int i = 0; i < populationnumber; i++)
		for (int j = 0; j < workpiecesnumber; j++)
			temp1_order_chr[i][j] = order_chr[i][j];               //temp1_order_chr��ʱ�洢order_chr��ֵ��

	for (int i = 0; i < populationnumber; i++){
		int temp_order;   //��ʶ�����������֮��temp_order��¼�������ţ�
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
/*	��Ⱥ�еĸ����������������ԣ���Գɹ�������������Ϊ����1�͸���2���н��������
�������������ͬ�Ļ����λ���Ӵ�1�̳и���2����λ֮��Ļ���Ƭ�Σ��������˳�򼯳ɸ���1��δ�ظ��Ļ���
�Ӵ�2�̳и���1����λ֮��Ļ���Ƭ�Σ��������˳�򼯳ɸ���2��δ�ظ��Ļ���	*/
{
	for (int i = 0; i < populationnumber / 2; i++){ //�����и���ƽ���ֳ������֣�һ����Ϊ����ĸ���1��һ����Ϊ���н���ĸ���2��
		int n1 = 1 + rand() % workpiecesnumber / 2;    //����������ͬ�Ļ���λ��
		int n2 = n1 + rand() % (workpiecesnumber - n1 - 1) + 1;
		double n_rand = rand() % 10000 / 10000.0;
		if (n_rand - crossoverrate < 1e-9)   //������������ĸ���
		{
			int temp1[workpiecesnumber] = { 0 };
			int temp2[workpiecesnumber] = { 0 };
			for (int j = 0; j < workpiecesnumber; j++)
			{
				int flg1 = 0, flg2 = 0;
				for (int p = n1; p < n2; p++)          //�������λ֮��Ļ���Ƭ�ν��н��棬temp1��temp2��¼û�з����ظ��Ļ���
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


			for (int j = n1; j < n2; j++){             //�Ӵ�1�̳и���2�����λ֮��Ļ����Ӵ�2�̳и���1�����λ֮��Ļ���
				int t = order_chr[2 * i][j];
				order_chr[2 * i][j] = order_chr[2 * i + 1][j];
				order_chr[2 * i + 1][j] = t;
			}
			for (int p = 0; p < n1; p++)               //�Ӵ�1��һ�����֮ǰ�Ļ���Ƭ�Σ���˳�����μ̳и���1��δ���Ӵ�1�ظ��Ļ���
				for (int q = 0; q < workpiecesnumber; q++)
					if (temp1[q]){
						order_chr[2 * i][p] = temp1[q];
						temp1[q] = 0;
						break;
					}

			for (int p = 0; p < n1; p++)               //�Ӵ�2��һ�����֮ǰ�Ļ���Ƭ�Σ���˳�����μ̳и���2��δ���Ӵ�2�ظ��Ļ���
				for (int m = 0; m < workpiecesnumber; m++)
					if (temp2[m]){
						order_chr[2 * i + 1][p] = temp2[m];
						temp2[m] = 0;
						break;
					}

			for (int p = n2; p < workpiecesnumber; p++)             //�Ӵ�1��2�����֮��Ļ���Ƭ�Σ���˳�����μ̳и���1��δ���Ӵ�1�ظ��Ļ���
				for (int q = 0; q < workpiecesnumber; q++)
					if (temp1[q]){
						order_chr[2 * i][p] = temp1[q]; temp1[q] = 0;
						break;
					}

			for (int p = n2; p < workpiecesnumber; p++)               //�Ӵ�2��2�����֮��Ļ���Ƭ�Σ���˳�����μ̳и���2��δ���Ӵ�2�ظ��Ļ���
				for (int m = 0; m < workpiecesnumber; m++)
					if (temp2[m]){
						order_chr[2 * i + 1][p] = temp2[m]; temp2[m] = 0;
						break;
					}
		}
	}
	return 0;

}

int mutation()  //�������Ϊ������죬���������������λ�����������������λ�ã�
{
	double n_rand = rand() % 10000 / 10000.0;
	if (n_rand - mutationrate < 1e-9)	//��������ĸ���
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
		cout << "���ļ�ʧ�ܣ�" << endl;
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
	ifs.close();  //������֪�ļӹ�ʱ�䣻

#if defined is_prompt
	cout << endl << endl;
#endif
	srand((unsigned)time(NULL));

	initialization();    //��ʼ����Ⱥ��
	for (int g = 0; g < G; g++){
		for (int c = 0; c < populationnumber; c++){//����ÿ��������Ӧ�Ȳ�����single_time_makespan�У�
			fitness(c);
			single_time_makespan[c] = time_makespan_total;
		}
		select();     //ѡ�������
		crossover();  //���������
		mutation();   //���������
	}

	int time_min = single_time_makespan[0];
	int n_min = 0;
	for (int c = 0; c < populationnumber - 1; c++)  //�������һ��ÿ���������Ӧ�ȣ����ҳ����Ÿ��壻
		if (single_time_makespan[c] < time_min)		{
			time_min = single_time_makespan[c];
			n_min = c;
		}

	gant(n_min);   //�������׵�����ͼ��
	outfile.close();
	return 0;
}