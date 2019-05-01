#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <time.h>
#include <algorithm>
using namespace std;
ofstream outfile;

#define is_prompt	//�Ƿ�����Ļ��ӡ��ʾ

#define N_train 11	//�г�����
#define ordernumber (3+1)     //��������	+1��������ʱ��Ҳ��������
#define parallel N_train	//���й��������л�����
#define parallel_0 N_train	//���г�����ʱ��ģ��Ϊһ������
#define parallel_a 3       //����a�ĳ������
#define parallel_b 8       //����b�ĳ������
#define parallel_c 5       //����c�ĳ������
#define parallel_d 3       //����d�ĳ������
#define parallel_e 2       //����e�ĳ������
const int parallel_arr[ordernumber] = { N_train,parallel_a,parallel_b,parallel_c };

#define populationnumber 300	//ÿһ����Ⱥ�ĸ�����
#define G 1000	//ѭ������
#define crossoverrate 0.1		//�������
#define mutationrate 0.01		//�������

int usetime[N_train][ordernumber];	//i�г���j����ļӹ�ʱ��
int machinetime[ordernumber][parallel] = { 0 };	//i����ĵ�j�����ͳ��ʱ��
int starttime[N_train][ordernumber][parallel];	//i�г�j�����ڵ�k�����Ͽ�ʼ�ӹ���ʱ��
int finishtime[N_train][ordernumber][parallel];	//i�г�j�����ڵ�k��������ɼӹ���ʱ��
int single_time_makespan[populationnumber];	//i�г��������̼ӹ�ʱ��
int order_chr[populationnumber][N_train];	//�ڼ�����Ⱦɫ��˳�򣬼������ӹ�˳��
int time_makespan_total;	//�����̼ӹ�ʱ��
double fits[populationnumber];//�洢ÿһ����Ⱥÿһ���������Ӧ�ȣ����ڽ���ѡ�����

int initialization()	//��ʼ����Ⱥ
{
	for (int i = 0; i < populationnumber; i++)	//����һ���г�������ȫ���еĸ���
		for (int j = 0; j < N_train; j++)
			order_chr[i][j] = j + 1;

	for (int i = 0; i < populationnumber; i++)	//��ȫ���еĸ��������ѡȡ��������λ�������ظ�����������
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

int fitness(int x)	//������Ӧ��
{
	int totaltime;	//�ܼӹ�����ʱ�䣨time_makespan_total��
	int temp1[N_train] = { 0 };	//temp1��ʱ�洢����x�Ļ������У��Ա���в�ͬ����֮��ļӹ�ʱ��¼�г������Ⱥ�˳��
	int temp2[N_train] = { 0 };	//temp2��ʱ�洢ÿ���г����깤ʱ�丳�裬���ھ�����һ��������г�����˳��
	int i;

	memcpy(temp1, order_chr[x], N_train * sizeof(int));

	for (i = 0; i < ordernumber; i++){
		for (int j = 0; j < N_train; j++)	//��ѭ����Ŀ����ͨ���Ƚ����г���ĵ�ǰ����ʱ�䣬�ҳ����ȿ��еĳ���
		{
			int m = machinetime[i][0];		//�ȼ�¼i����ĵ�һ���г��ĵ�ǰ����ʱ��
			int n = 0;
			for (int p = 0; p < parallel_arr[i]; p++){	//�����������г����бȽϣ��ҳ�ʱ����С���г�
				if (m > machinetime[i][p]){
					m = machinetime[i][p];
					n = p;
				}
			}
			int q = temp1[j];		//��˳����ȡtemp1�е��г��ţ����г����м���
			starttime[q - 1][i][n] = max(machinetime[i][n], temp2[j]);	//��ʼ����ʱ��ȡ�û����ĵ�ǰʱ��͸��г���һ�������깤ʱ������ֵ
			machinetime[i][n] = starttime[q - 1][i][n] + usetime[q - 1][i];	//�������ۼƼӹ�ʱ����ڻ�����ʼ�ӹ���ʱ�̣����ϸ��г��������õ�ʱ��
			finishtime[q - 1][i][n] = machinetime[i][n];			//�г����깤ʱ����Ǹó��䵱ǰ���ۼƼӹ�ʱ��
			temp2[j] = finishtime[q - 1][i][n];		//���г����깤ʱ�丳��temp2�������깤ʱ��Ŀ��������ھ�����һ������Ĺ����ӹ�˳��
		}

		int temp3[N_train];		//������ʱ���飬���ڽ�temp1��temp2�еĹ�����������
		memcpy(temp3, temp1, N_train * sizeof(int));

		for (int e = 0; e < N_train - 1; e++){
			for (int ee = 0; ee < N_train - 1 - e; ee++){	//����temp2�洢�г���һ��������깤ʱ�䣬�ڽ�����һ����������ʱ���������깤��������ԭ��
				if (temp2[ee] > temp2[ee + 1]){				//��ˣ���ѭ����Ŀ�����ڽ�temp2�а��ռӹ�ʱ���С�������У�ͬʱtemp1��Ӧ���б任����¼temp2�еĹ�����
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
	for (i = 0; i < parallel_arr[ordernumber - 1]; i++)	//�Ƚ����һ�����򳵼���ۼƼӹ�ʱ�䣬���ʱ�伴�ù���ļӹ�ʱ��
		if (totaltime < machinetime[ordernumber - 1][i])
			totaltime = machinetime[ordernumber - 1][i];

	for (i = 0; i < N_train; i++)	//��������㣬������һ������ļӹ�ʱ��ͳ��
		for (int j = 0; j < ordernumber; j++)
			for (int t = 0; t < parallel_arr[j]; t++){
				starttime[i][j][t] = 0;
				finishtime[i][j][t] = 0;
				machinetime[j][t] = 0;
			}
	time_makespan_total = totaltime;
	fits[x] = 1.0 / time_makespan_total;		//��time_makespan_totalȡ������Ϊ��Ӧ�Ⱥ���
	return 0;
}

int select()
{
	double roulette[populationnumber + 1] = { 0 };	//��¼���̶ĵ�ÿһ����������
	double pro_single[populationnumber];			//��¼ÿ��������ֵĸ��ʣ����������Ӧ�ȳ���������Ӧ��֮��
	double totalfitness = 0;						//��Ⱥ���и������Ӧ��֮��
	int temp1_order_chr[populationnumber][N_train];	//�洢order_chr�����и����Ⱦɫ��
	int i;

	for (i = 0; i < populationnumber; i++)		//�������и�����Ӧ�ȵ��ܺ�
		totalfitness = totalfitness + fits[i];

	for (i = 0; i < populationnumber; i++){
		pro_single[i] = fits[i] / totalfitness;		//����ÿ��������Ӧ����������Ӧ��֮��
		roulette[i + 1] = roulette[i] + pro_single[i];	//��ÿ������ĸ����ۼӣ��������̶�
	}

	for (i = 0; i < populationnumber; i++)
		for (int j = 0; j < N_train; j++)
			temp1_order_chr[i][j] = order_chr[i][j];	//temp1_order_chr��ʱ�洢order_chr��ֵ

	for (i = 0; i < populationnumber; i++){
		int temp_order;		//��ʶ�����������֮��temp_order��¼��������
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
/*	��Ⱥ�еĸ����������������ԣ���Գɹ�������������Ϊ����1�͸���2���н��������
�������������ͬ�Ļ����λ���Ӵ�1�̳и���2����λ֮��Ļ���Ƭ�Σ��������˳�򼯳ɸ���1��δ�ظ��Ļ���
�Ӵ�2�̳и���1����λ֮��Ļ���Ƭ�Σ��������˳�򼯳ɸ���2��δ�ظ��Ļ���	*/
{
	for (int i = 0; i < populationnumber / 2; i++){	//�����и���ƽ���ֳ������֣�һ����Ϊ����ĸ���1��һ����Ϊ���н���ĸ���2
		int n1 = 1 + rand() % N_train / 2;		//����������ͬ�Ļ���λ
		int n2 = n1 + rand() % (N_train - n1 - 1) + 1;
		double n_rand = rand() % 1000 / 1000.0;
		if (n_rand - crossoverrate < 1e-9)		//������������ĸ���
		{
			int temp1[N_train] = { 0 };
			int temp2[N_train] = { 0 };
			int j;
			for (j = 0; j < N_train; j++)
			{
				int flg1 = 0, flg2 = 0;
				for (int p = n1; p < n2; p++)	//�������λ֮��Ļ���Ƭ�ν��н��棬temp1��temp2��¼û�з����ظ��Ļ���
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


			for (j = n1; j < n2; j++){		//�Ӵ�1�̳и���2�����λ֮��Ļ���,�Ӵ�2�̳и���1�����λ֮��Ļ���
				int t = order_chr[2 * i][j];
				order_chr[2 * i][j] = order_chr[2 * i + 1][j];
				order_chr[2 * i + 1][j] = t;
			}

			int p, q;
			for (p = 0; p < n1; p++)		//�Ӵ�1��һ�����֮ǰ�Ļ���Ƭ�Σ���˳�����μ̳и���1��δ���Ӵ�1�ظ��Ļ���
				for (q = 0; q < N_train; q++)
					if (temp1[q]){
						order_chr[2 * i][p] = temp1[q];
						temp1[q] = 0;
						break;
					}

			for (p = 0; p < n1; p++)		//�Ӵ�2��һ�����֮ǰ�Ļ���Ƭ�Σ���˳�����μ̳и���2��δ���Ӵ�2�ظ��Ļ���
				for (q = 0; q < N_train; q++)
					if (temp2[q]){
						order_chr[2 * i + 1][p] = temp2[q];
						temp2[q] = 0;
						break;
					}

			for (p = n2; p < N_train; p++)		//�Ӵ�1�ڶ������֮��Ļ���Ƭ�Σ���˳�����μ̳и���1��δ���Ӵ�1�ظ��Ļ���
				for (q = 0; q < N_train; q++)
					if (temp1[q]){
						order_chr[2 * i][p] = temp1[q];
						break;
					}

			for (p = n2; p < N_train; p++)		//�Ӵ�2�ڶ������֮��Ļ���Ƭ�Σ���˳�����μ̳и���2��δ���Ӵ�2�ظ��Ļ���
				for (q = 0; q < N_train; q++)
					if (temp2[q]){
						order_chr[2 * i + 1][p] = temp2[q];
						break;
					}
		}
	}
	return 0;

}

int mutation()	//������죬���������������λ�����������������λ��
{
	double n_rand = rand() % 1000 / 1000.0;
	if (n_rand - mutationrate < 1e-9)	//��������ĸ���
		for (int i = 0; i < populationnumber; i++){
			int t1 = rand() % N_train;
			int t2 = rand() % N_train;
			int t = order_chr[i][t1];
			order_chr[i][t1] = order_chr[i][t2];
			order_chr[i][t2] = t;
		}
	return 0;
}

int gant(int c)		//�����չʾ�����ɸ���ͼ
{
	int totaltime;
	char machine[ordernumber * parallel][1000];
	memset(machine, '0', ordernumber * parallel * 1000 * sizeof(char));

	int temp1[N_train] = { 0 };	//ά��˳��
	int temp2[N_train] = { 0 };	//��һ��������ʱ��
	int i;

	for (i = 0; i < N_train; i++)
		temp1[i] = order_chr[c][i];

	for (i = 0; i < ordernumber; i++) {
		for (int j = 0; j < N_train; j++) {
			int m = machinetime[i][0];
			int n = 0;

			for (int p = 0; p < parallel_arr[i]; p++)	//�ҳ�ʱ����С�ĳ���
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
		cout << "���ļ�ʧ�ܣ�" << endl;
		return -1;
	}

	for (int i = 0; i < N_train; i++) {
		for (int j = 0; j < ordernumber; j++) {
			ifs >> usetime[i][j];	//������֪�ļӹ�ʱ��
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

	initialization();    //��ʼ����Ⱥ
	for (int g = 0; g < G; g++){
		for (int c = 0; c < populationnumber; c++){	//����ÿ��������Ӧ�Ȳ�����single_time_makespan��
			fitness(c);
			single_time_makespan[c] = time_makespan_total;
		}
		select();     //ѡ��
		crossover();  //����
		mutation();   //����
	}

	int time_min = single_time_makespan[0];
	int n_min = 0;
	for (int c = 0; c < populationnumber - 1; c++)  //�������һ��ÿ���������Ӧ�ȣ����ҳ����Ÿ���
		if (single_time_makespan[c] < time_min)		{
			time_min = single_time_makespan[c];
			n_min = c;
		}

	gant(n_min);   //��������ͼ
	outfile.close();
	return 0;
}