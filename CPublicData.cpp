#include "stdafx.h"
#include "CPublicData.h"
#include "networkDlg.h"

CPublicData::CPublicData()
{
}


CPublicData::~CPublicData()
{
}
/*��¼���ݰ�*/

/********����ͳ��*******/
FList * CPublicData::Flist_init(int init_size)
{
	FList * list = (FList *)malloc(sizeof(FList));
	if (list == NULL) {
		perror("malloc slist error");
		exit(0);
	}
	list->size = 0;
	list->buff_size = init_size < 10 ? 10 : init_size;
	list->buff_array=(flow *)malloc(sizeof(flow) * list->buff_size);
	if (list->buff_array == NULL) {
		perror("malloc slist buff array error");
		exit(0);
	}
	return list;
}

void CPublicData::Flist_add(FList *list, ip_address ip, int flag)
{
	if (list->size == list->buff_size) {
		list->buff_size = list->buff_size / 2 * 3 + 1;
		list->buff_array = (flow *)realloc(list->buff_array, sizeof(flow) * list->buff_size);
	}
	(list->buff_array + list->size)->ip = ip;
	(list->buff_array + list->size)->id = CPublicData::ip_number++;
	//��ʾ
	fip1.Format(_T("%d"), ip.byte1);
	fip2.Format(_T("%d"), ip.byte2);
	fip3.Format(_T("%d"), ip.byte3);
	fip4.Format(_T("%d"), ip.byte4);
	fip = fip1 + "." + fip2 + "." + fip3 + "." + fip4;
	CnetworkDlg::flowList.InsertItem((list->buff_array + list->size)->id, fip);
	if (flag)
	{
		(list->buff_array + list->size)->send = 1;
		(list->buff_array + list->size)->receive = 0;
		CnetworkDlg::flowList.SetItemText((list->buff_array + list->size)->id, 1, _T("1"));
		CnetworkDlg::flowList.SetItemText((list->buff_array + list->size)->id, 2, _T("0"));
	}
	else
	{
		(list->buff_array + list->size)->send = 0;
		(list->buff_array + list->size)->receive = 1;
		CnetworkDlg::flowList.SetItemText((list->buff_array + list->size)->id, 1, _T("0"));
		CnetworkDlg::flowList.SetItemText((list->buff_array + list->size)->id, 2, _T("1"));
	}
	//printf("����µ�IP��ַ��%d.%d.%d.%d\n", ip.byte1, ip.byte2, ip.byte3, ip.byte4);
	list->size++;

}

int CPublicData::Flist_update(FList *list, ip_address ip_send, ip_address ip_receive)
{
	int i = 0;
	//ԴIP�Ƿ��ڼ�����
	int flag_send = 1;
	//Ŀ��IP�Ƿ��ڼ�����
	int flag_receive = 1;
	for (i = 0;i < list->size;i++)
	{
		//��ȡ���������ip��ַ
		ip_address model = (list->buff_array + i)->ip;
		//�ж�ԴIP��ַ�Ƿ��ڼ�����
		if (ip_send.byte1 == model.byte1&&ip_send.byte2 == model.byte2&&ip_send.byte3 == model.byte3&&ip_send.byte4 == model.byte4)
		{
			//ԴIP��ַ�������ݰ���Ŀ����
			(list->buff_array + i)->send++;
			fsend.Format(_T("%d"), (list->buff_array + i)->send);
			CnetworkDlg::flowList.SetItemText((list->buff_array + i)->id, 1, fsend);
			printf("����IP��ַ��%d.%d.%d.%d �������ݰ���ĿΪ��%d\n", model.byte1, model.byte2, model.byte3, model.byte4, (list->buff_array + i)->send);
			//�ڼ������ҵ���ԴIP��ַ
			flag_send = 0;
		}
		//�ж�Ŀ��IP��ַ�Ƿ��ڼ�����
		if (ip_receive.byte1 == model.byte1&&ip_receive.byte2 == model.byte2&&ip_receive.byte3 == model.byte3&&ip_receive.byte4 == model.byte4)
		{
			//Ŀ��IP��ַ�������ݰ���Ŀ����
			(list->buff_array + i)->receive++;
			freceive.Format(_T("%d"), (list->buff_array + i)->receive);
			CnetworkDlg::flowList.SetItemText((list->buff_array + i)->id, 2, freceive);
			printf("����IP��ַ��%d.%d.%d.%d �������ݰ���ĿΪ��%d\n", model.byte1, model.byte2, model.byte3, model.byte4, (list->buff_array + i)->receive);
			//�ڼ������ҵ���Ŀ��IP��ַ
			flag_receive = 0;
		}
		//����ڼ����м��ҵ���ԴIP���ҵ���Ŀ��IP���򷵻�
		if (!(flag_send || flag_receive))
			return 0;
	}

	//ԴIP��Ŀ��IP������һ�����ڼ���
	//��ԴIP���ڼ�����
	if (flag_send)
	{
		//1��ʾ���ӵ�IP�������ݰ�������ʼ��Ϊ1
		CPublicData::Flist_add(list, ip_send, 1);
	}
	//��Ŀ��IP���ڼ�����
	if (flag_receive)
	{
		//0��ʾ���ӵ�IP�������ݰ�������ʼ��Ϊ1
		CPublicData::Flist_add(list, ip_receive, 0);
	}
	return 0;
}
/*��¼���ݰ�*/
/*��ʼ����¼��̬����*/
PList * CPublicData::Plist_init(int init_size)
{
	PList *list = (PList *)malloc(sizeof(PList));
	if (list == NULL) {
		perror("malloc slist error");
		exit(0);
	}
	list->size = 0;
	list->buff_size = init_size < 10 ? 10 : init_size;
	list->buff_array = (pinfo *)malloc(sizeof(pinfo) * list->buff_size);
	if (list->buff_array == NULL) {
		perror("malloc slist buff array error");
		exit(0);
	}
	return list;
}

/*��Ӽ�¼����Ԫ��*/
void CPublicData::Plist_add(PList * list, u_char * data,int length)
{
	if (list->size == list->buff_size) {
		list->buff_size = list->buff_size / 2 * 3 + 1;
		list->buff_array = (pinfo *)realloc(list->buff_array, sizeof(pinfo) * list->buff_size);
	}
	(list->buff_array + list->size)->data = data;
	(list->buff_array + list->size)->length = length;
	list->size++;
}


/*�ͷż�¼��̬����*/
void CPublicData::Plist_free(PList * list)
{
	free(list->buff_array);
	free(list);
}


void CPublicData::Flist_free(FList *list) {

	free(list->buff_array);
	free(list);
}

/*****����ͳ�ƽ���******/
