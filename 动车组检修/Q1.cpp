#include <iostream>
#include <queue>	//�𳵽���������϶��еĸ���
using namespace std;

#define DONE 2		//��ʾ�Ѿ����
#define SUCCEED 1	//��ʾ�ɹ�����
#define WAITING 0	//��ʾ��Ҫ�ȴ�
#define FAIL -1		//��ʾ����ʧ��

#define N_TRAIN 12*4	//������
#define N_WORKSHOP 3	//��������

/*	���ǿ��ܵĺ��������ļ��� ���趨�����������ͣ�[����������]	*/

class WorkShop;

class Train
{
public:	//��ʹ��public �����ǲ���ȫ���(��
	int type_CRH = 0;		//Q1�� �ݶ����ж���������ͬ
	int cost_time[N_WORKSHOP] = { 4,8,6 };		//Q1�� �ݶ�һ��λʱ��Ϊ15���� 1/15min
	WorkShop* In_which_WorkShop;		//�˻����ڳ��� ���������ָ��գ�NULL��

	int Done_work();	//���һ��λʱ�Ĺ�������
};

class WorkShop
{
public:
	char type;				//��������
	int n;					//����������
	queue<Train*> InsideTrain;	//����𳵶���

	WorkShop(char _type, int _n) {	//��ʼ��������Ϣ�����͡���������
		type = _type;
		n = _n;
	}
	int Init(Train* t);		//��ĳһ�����복��
	int Outit();			//��ĳһ���ó����� �Ҳ������������䣨����ĩβ���䣩
	int MoveTrain(Train* t, WorkShop* Next_WorkShop);	//ת�������ڳ���
};

int Train::Done_work() {
	if (In_which_WorkShop == NULL)	//�Ѿ����
		return DONE;
	int i = In_which_WorkShop->type - 'a';	//�õ���ǰ�𳵴�����һ����
	if (cost_time[i]) {		//��ǰ��������û����
		cost_time[i]--;		//��һ��λ������
		if (cost_time[i])	//��δ����
			return SUCCEED;
		return WAITING;		//���������ȴ�״̬
	}
	else
		return WAITING;		//��ǰ�������Ѿ����� ά�ֵȴ�״̬
}

int WorkShop::Init(Train* t) {
	if (n == InsideTrain.size())
		return WAITING;		//���г�����������վʧ��
	t->In_which_WorkShop = this;
	InsideTrain.push(t);		//�𳵽�վ
	return SUCCEED;
}

int WorkShop::Outit() {
	InsideTrain.front()->In_which_WorkShop = NULL;	//��վ�����κγ���
	InsideTrain.pop();
	return SUCCEED;
}

int WorkShop::MoveTrain(Train* t, WorkShop* Next_WorkShop) {
	if (Next_WorkShop->Init(t) == SUCCEED) {	//�п��г�����Բ��벢�Ҳ���ɹ�
		InsideTrain.pop();
		return SUCCEED;
	}
	return WAITING;		//�޿��г��� ����ȴ�״̬
}


int main()
{
	Train *train = new Train[N_TRAIN];

	WorkShop ws[N_WORKSHOP] = {		//�����ʼ��
		WorkShop('a',3),
		WorkShop('b',8),
		WorkShop('c',5) };

	int time, i, j, k = -1;
	for (i = 0, time = 0;;) {
		if (i < N_TRAIN) {
			if (ws[0].Init(&train[i]))		//���Խ�a����
				i++;		//�ɹ�����һ������һ�������Խ�a����
		}
		else {		//���г����ѽ��복��
			for (j = 0; j < N_WORKSHOP; j++)
				if (!ws[j].InsideTrain.empty())		//���⳵��ǿ����˳�������ûά����Ļ�
					break;
			if (j == N_WORKSHOP)	//���л������
				break;
		}

#if 0	//�ɿ��鿴���� �س�����
		//�����
		//ʱ��
		//�𳵱��	���ڳ���	���ڳ���ʣ��ӹ�ʱ��
		cout << time * 15 << "min" << endl;
		for (j = 0; j < i; j++)
			if(train[j].In_which_WorkShop)
				cout << j + 1 << '\t' << train[j].In_which_WorkShop->type << '\t'
				<< train[j].cost_time[train[j].In_which_WorkShop - ws] << endl;
		getchar();
#endif

		for (j = 0; j < i; j++) {
			if (train[j].In_which_WorkShop == NULL)		//����ɼӹ�
				continue;
			int t = train[j].Done_work();		//��һ��λʱ�Ĺ���
			if (t == WAITING) {			//�𳵴��ڵȴ�״̬
				k = train[j].In_which_WorkShop - ws;	//�õ����ڳ�����
				if (k == N_WORKSHOP - 1)		//�������һ������
					ws[k].Outit();				//ֱ�ӿ�������ת�Ƶ���������
				else				//�����һ������֮ǰ
					ws[k].MoveTrain(&train[j], &ws[k + 1]);		//ת�ƻ�
			}
		}
		time++;		//��ʱ��+1
	}

	cout << time * 15.0 / 60 << "Hours" << endl;
	return 0;
}