#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <time.h>
#include <algorithm>
using namespace std;
ofstream outfile;
ofstream outdata;

//#define is_prompt	//�Ƿ�����Ļ��ӡ��ʾ

#define N_train 11	//��������


#define parallel N_train	//���й��������л�����
#define parallel_0 N_train	//���г�����ʱ��ģ��Ϊһ������
#define parallel_a 3       //����a������л�����
#define parallel_b 8       //����b������л�����
#define parallel_c 5       //����c������л�����
#define parallel_d 3       //����d������л�����
#define parallel_e 2       //����e������л�����

/*************��Ҫ�ֶ����ڵĲ���**************/
#define ordernumber (1+1)     //������
#define input_data "input3.txt"
#define output_data "outdata3.txt"
#define output_gant "output3.txt"

const int parallel_arr[ordernumber] = { parallel_0,parallel_c };
/*************��Ҫ�ֶ����ڵĲ���**************/

#define populationnumber 300	//ÿһ����Ⱥ�ĸ�����
#define crossoverrate 0.1		//�������
#define mutationrate 0.01		//�������
#define G 500		//ѭ������

int usetime[N_train][ordernumber];	//�ڼ��������ڼ�������ļӹ���ʱ��
int machinetime[ordernumber][parallel] = { 0 };	//�ڼ�������ĵڼ�̨���л�����ͳ��ʱ�䣻
int starttime[N_train][ordernumber][parallel];	//�ڼ��������ڼ��������ڵڼ�̨���л��Ͽ�ʼ�ӹ���ʱ�䣻
int finishtime[N_train][ordernumber][parallel];	//�ڼ��������ڼ��������ڵڼ�̨���л�����ɼӹ���ʱ�䣻
int single_time_makespan[populationnumber];	//�����time_makespan_total��
int order_chr[populationnumber][N_train];	//�ڼ�����Ⱦɫ��˳�򣬼������ӹ�˳��
int time_makespan_total;	//�ܵ����̼ӹ�ʱ�䣻
double fits[populationnumber];//�洢ÿһ����Ⱥÿһ���������Ӧ�ȣ����ڽ���ѡ�������

int initialization()   //��ʼ����Ⱥ��
{
	int i, j;
	for (i = 0; i < populationnumber; i++)     //��������һ������������ȫ���еĸ��壻
		for (j = 0; j < N_train; j++)
			order_chr[i][j] = j + 1;

	for (i = 0; i < populationnumber; i++)     //��ȫ���еĸ��������ѡȡ��������λ�������ظ����������Σ����γ������ʼ��Ⱥ��
		for (j = 0; j < N_train; j++) {
			int t1 = rand() % N_train;
			int t2 = rand() % N_train;
			int t = order_chr[i][t1];
			order_chr[i][t1] = order_chr[i][t2];
			order_chr[i][t2] = t;
		}

#ifdef is_prompt
	for (i = 0; i < populationnumber; i++) {
		for (j = 0; j < N_train; j++)
			cout << order_chr[i][j] << " ";
		cout << endl;
	}
#endif
	return 0;
}

int fitness(int c)   //������Ӧ�Ⱥ�����c����ĳ���壻
{
	int totaltime;      //�ܵļӹ�����ʱ�䣨time_makespan_total����
	int temp1[N_train] = { 0 };	//temp1��ʱ�洢����c�Ļ������У��Ա���в�ͬ����֮��ļӹ�ʱ��¼�����ӹ��Ⱥ�˳��
	int temp2[N_train] = { 0 };	//temp2��ʱ�洢ÿ���������깤ʱ�丳�裬���ھ�����һ������Ĺ����ӹ�˳��
	int i, j;

	memcpy(temp1, order_chr[c], N_train * sizeof(int));

	for (i = 0; i < ordernumber; i++) {
		for (j = 0; j < N_train; j++)  //��ѭ����Ŀ����ͨ���Ƚ����л����ĵ�ǰ����ʱ�䣬�ҳ����ȿ��еĻ����������µĹ���������
		{
			int q = temp1[j];			//��˳����ȡtemp1�еĹ����ţ��Թ������мӹ���
			if (usetime[q - 1][i] == 0)		//����ʱΪ0/��ʾ����Ҫ���д˹���
				continue;
			int m = machinetime[i][0];        //�ȼ�¼��i������ĵ�һ̨���л����ĵ�ǰ����ʱ�䣻
			int n = 0;
			for (int p = 0; p < parallel_arr[i]; p++) { //���������л������бȽϣ��ҳ�ʱ����С�Ļ�����
				if (m > machinetime[i][p]) {
					m = machinetime[i][p];
					n = p;
				}
			}
			starttime[q - 1][i][n] = max(machinetime[i][n], temp2[j]);  //��ʼ�ӹ�ʱ��ȡ�û����ĵ�ǰʱ��͸ù�����һ�������깤ʱ������ֵ��
			machinetime[i][n] = starttime[q - 1][i][n] + usetime[q - 1][i]; //�������ۼƼӹ�ʱ����ڻ�����ʼ�ӹ���ʱ�̣����ϸù����ӹ����õ�ʱ�䣻
			finishtime[q - 1][i][n] = machinetime[i][n];                 //�������깤ʱ����Ǹû�����ǰ���ۼƼӹ�ʱ�䣻
			temp2[j] = finishtime[q - 1][i][n];       //��ÿ���������깤ʱ�丳��temp2�������깤ʱ��Ŀ��������ھ�����һ������Ĺ����ӹ�˳��
		}

		//int temp3[N_train];		//������ʱ���飬���ڽ�temp1��temp2�еĹ����������У�
		//memcpy(temp3, temp1, N_train * sizeof(int));

		//for (int e = 0; e < N_train - 1; e++) {
		//	for (int ee = 0; ee < N_train - 1 - e; ee++) {	//����temp2�洢������һ��������깤ʱ�䣬�ڽ�����һ����������ʱ���������깤��������ԭ��
		//		if (temp2[ee] > temp2[ee + 1]) {						//��ˣ���ѭ����Ŀ�����ڽ�temp2�а��ռӹ�ʱ���С�������У�ͬʱtemp1��Ӧ���б任����¼temp2�еĹ����ţ�
		//			int t1 = temp2[ee];
		//			int t2 = temp3[ee];
		//			temp2[ee] = temp2[ee + 1];
		//			temp3[ee] = temp3[ee + 1];
		//			temp2[ee + 1] = t1;
		//			temp3[ee + 1] = t2;
		//		}
		//	}
		//}
		//memcpy(temp1, temp3, N_train * sizeof(int));
	}

	totaltime = 0;
	for (i = 0; i < parallel_arr[ordernumber - 1]; i++) //�Ƚ����һ������������ۼƼӹ�ʱ�䣬���ʱ����Ǹ����̵ļӹ�ʱ�䣻
		if (totaltime < machinetime[ordernumber - 1][i])
			totaltime = machinetime[ordernumber - 1][i];

	for (i = 0; i < N_train; i++)  //��������㣬������һ������ļӹ�ʱ��ͳ�ƣ�
		for (int j = 0; j < ordernumber; j++)
			for (int t = 0; t < parallel_arr[j]; t++) {
				starttime[i][j][t] = 0;
				finishtime[i][j][t] = 0;
				machinetime[j][t] = 0;
			}
	time_makespan_total = totaltime;
	fits[c] = 1.000 / time_makespan_total;          //��time_makespan_totalȡ������Ϊ��Ӧ�Ⱥ�����
	return 0;
}

int select()
{
	double roulette[populationnumber + 1] = { 0 };	//��¼���̶ĵ�ÿһ���������䣻
	double pro_single[populationnumber];			//��¼ÿ��������ֵĸ��ʣ����������Ӧ�ȳ���������Ӧ��֮�ͣ�
	double totalfitness = 0;                	//��Ⱥ���и������Ӧ��֮�ͣ�
	int temp1_order_chr[populationnumber][N_train];	//�洢order_chr�����и����Ⱦɫ�壻
	int i, j;

	for (i = 0; i < populationnumber; i++)     		//�������и�����Ӧ�ȵ��ܺͣ�
		totalfitness = totalfitness + fits[i];

	for (i = 0; i < populationnumber; i++) {
		pro_single[i] = fits[i] / totalfitness;   	//����ÿ��������Ӧ����������Ӧ��֮�ȣ�
		roulette[i + 1] = roulette[i] + pro_single[i]; //��ÿ������ĸ����ۼӣ��������̶ģ�
	}

	for (i = 0; i < populationnumber; i++)
		for (j = 0; j < N_train; j++)
			temp1_order_chr[i][j] = order_chr[i][j];               //temp1_order_chr��ʱ�洢order_chr��ֵ��

	for (i = 0; i < populationnumber; i++) {
		int temp_order;   //��ʶ�����������֮��temp_order��¼�������ţ�
		double p = rand() % (10000) / 10000.0;
		for (j = 0; j < populationnumber; j++)
			if (p >= roulette[j] && p < roulette[j + 1])
				temp_order = j;
		for (j = 0; j < N_train; j++)
			order_chr[i][j] = temp1_order_chr[temp_order][j];
	}
	return 0;
}

int crossover()
/*	��Ⱥ�еĸ����������������ԣ���Գɹ�������������Ϊ����1�͸���2���н��������
�������������ͬ�Ļ����λ���Ӵ�1�̳и���2����λ֮��Ļ���Ƭ�Σ��������˳�򼯳ɸ���1��δ�ظ��Ļ���
�Ӵ�2�̳и���1����λ֮��Ļ���Ƭ�Σ��������˳�򼯳ɸ���2��δ�ظ��Ļ���	*/
{
	for (int i = 0; i < populationnumber / 2; i++) { //�����и���ƽ���ֳ������֣�һ����Ϊ����ĸ���1��һ����Ϊ���н���ĸ���2��
		int n1 = 1 + rand() % N_train / 2;    //����������ͬ�Ļ���λ��
		int n2 = n1 + rand() % (N_train - n1 - 1) + 1;
		double n_rand = rand() % 10000 / 10000.0;
		if (n_rand - crossoverrate < 1e-9)   //������������ĸ���
		{
			int temp1[N_train] = { 0 };
			int temp2[N_train] = { 0 };
			for (int j = 0; j < N_train; j++)
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


			for (int j = n1; j < n2; j++) {             //�Ӵ�1�̳и���2�����λ֮��Ļ����Ӵ�2�̳и���1�����λ֮��Ļ���
				int t = order_chr[2 * i][j];
				order_chr[2 * i][j] = order_chr[2 * i + 1][j];
				order_chr[2 * i + 1][j] = t;
			}
			for (int p = 0; p < n1; p++)               //�Ӵ�1��һ�����֮ǰ�Ļ���Ƭ�Σ���˳�����μ̳и���1��δ���Ӵ�1�ظ��Ļ���
				for (int q = 0; q < N_train; q++)
					if (temp1[q]) {
						order_chr[2 * i][p] = temp1[q];
						temp1[q] = 0;
						break;
					}

			for (int p = 0; p < n1; p++)               //�Ӵ�2��һ�����֮ǰ�Ļ���Ƭ�Σ���˳�����μ̳и���2��δ���Ӵ�2�ظ��Ļ���
				for (int m = 0; m < N_train; m++)
					if (temp2[m]) {
						order_chr[2 * i + 1][p] = temp2[m];
						temp2[m] = 0;
						break;
					}

			for (int p = n2; p < N_train; p++)             //�Ӵ�1��2�����֮��Ļ���Ƭ�Σ���˳�����μ̳и���1��δ���Ӵ�1�ظ��Ļ���
				for (int q = 0; q < N_train; q++)
					if (temp1[q]) {
						order_chr[2 * i][p] = temp1[q]; temp1[q] = 0;
						break;
					}

			for (int p = n2; p < N_train; p++)               //�Ӵ�2��2�����֮��Ļ���Ƭ�Σ���˳�����μ̳и���2��δ���Ӵ�2�ظ��Ļ���
				for (int m = 0; m < N_train; m++)
					if (temp2[m]) {
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
		for (int i = 0; i < populationnumber; i++) {
			int t1 = rand() % N_train;
			int t2 = rand() % N_train;
			int t = order_chr[i][t1];
			order_chr[i][t1] = order_chr[i][t2];
			order_chr[i][t2] = t;
		}
	return 0;
}

int gant(int c)		//�ú�����Ϊ�˽����Ľ�������������ʵ�չʾ�����ɸ���ͼ��������Ľ���Լ�����Ľ����û��Ӱ�죻
{
	int totaltime;
	int i, j;
	char machine[ordernumber * parallel][1000];
	memset(machine, '0', ordernumber * parallel * 1000 * sizeof(char));

	int temp1[N_train] = { 0 }; //�ӹ�˳��
	int temp2[N_train] = { 0 }; //��һ��������ʱ��

	for (i = 0; i < N_train; i++)
		temp1[i] = order_chr[c][i];

	for (i = 0; i < ordernumber; i++) {
		for (j = 0; j < N_train; j++) {
			int m = machinetime[i][0];
			int n = 0;

			for (int p = 0; p < parallel_arr[i]; p++) //�ҳ�ʱ����С�Ļ�����
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

		//int temp3[N_train];
		//memcpy(temp3, temp1, N_train * sizeof(int));

		//for (int e = 0; e < N_train - 1; e++)
		//	for (int ee = 0; ee < N_train - 1 - e; ee++)
		//		if (temp2[ee] > temp2[ee + 1]) {
		//			int t1 = temp2[ee];
		//			int t2 = temp3[ee];
		//			temp2[ee] = temp2[ee + 1];
		//			temp3[ee] = temp3[ee + 1];
		//			temp2[ee + 1] = t1;
		//			temp3[ee + 1] = t2;
		//		}

		//memcpy(temp1, temp3, N_train * sizeof(int));
	}
	for (j = 0; j < N_train; j++)
		outdata << temp2[j] << endl;

	totaltime = 0;
	for (int i = 0; i < parallel_arr[ordernumber - 1]; i++)
		if (totaltime < machinetime[ordernumber - 1][i])
			totaltime = machinetime[ordernumber - 1][i];

#if 1
	cout << "total=" << totaltime << endl;
#endif
	outfile << totaltime << endl;

	for (int e = parallel; e < ordernumber * parallel; e++) {	//���������ʱ���Ӧ�Ĺ���
		if (e % parallel > parallel_arr[e / parallel])
			continue;
		for (int ee = 0; ee < 1000; ee++) {
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

int main()
{
	ifstream ifs(input_data, ios::in);
	outfile.open(output_gant, ios::out);
	outdata.open(output_data, ios::out);
	if (!ifs.is_open() || !outfile.is_open() || !outdata.is_open()) {
		cout << "���ļ�ʧ�ܣ�" << endl;
		return -1;
	}

	for (int i = 0; i < N_train; i++) {
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
	for (int g = 0; g < G; g++) {
		for (int c = 0; c < populationnumber; c++) {//����ÿ��������Ӧ�Ȳ�����single_time_makespan�У�
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
		if (single_time_makespan[c] < time_min) {
			time_min = single_time_makespan[c];
			n_min = c;
		}

	gant(n_min);   //�������׵�����ͼ��
	outfile.close(); 
	outdata.close();
	return 0;
}