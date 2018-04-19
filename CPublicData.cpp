#include "stdafx.h"
#include "CPublicData.h"
#include "networkDlg.h"

CPublicData::CPublicData()
{
}


CPublicData::~CPublicData()
{
}
/*记录数据包*/

/********流量统计*******/
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
	//显示
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
	//printf("添加新的IP地址：%d.%d.%d.%d\n", ip.byte1, ip.byte2, ip.byte3, ip.byte4);
	list->size++;

}

int CPublicData::Flist_update(FList *list, ip_address ip_send, ip_address ip_receive)
{
	int i = 0;
	//源IP是否不在集合中
	int flag_send = 1;
	//目的IP是否不在集合中
	int flag_receive = 1;
	for (i = 0;i < list->size;i++)
	{
		//提取遍历对象的ip地址
		ip_address model = (list->buff_array + i)->ip;
		//判断源IP地址是否在集合中
		if (ip_send.byte1 == model.byte1&&ip_send.byte2 == model.byte2&&ip_send.byte3 == model.byte3&&ip_send.byte4 == model.byte4)
		{
			//源IP地址发送数据包数目增加
			(list->buff_array + i)->send++;
			fsend.Format(_T("%d"), (list->buff_array + i)->send);
			CnetworkDlg::flowList.SetItemText((list->buff_array + i)->id, 1, fsend);
			printf("现有IP地址：%d.%d.%d.%d 发送数据包数目为：%d\n", model.byte1, model.byte2, model.byte3, model.byte4, (list->buff_array + i)->send);
			//在集合中找到了源IP地址
			flag_send = 0;
		}
		//判断目的IP地址是否在集合中
		if (ip_receive.byte1 == model.byte1&&ip_receive.byte2 == model.byte2&&ip_receive.byte3 == model.byte3&&ip_receive.byte4 == model.byte4)
		{
			//目的IP地址接收数据包数目增加
			(list->buff_array + i)->receive++;
			freceive.Format(_T("%d"), (list->buff_array + i)->receive);
			CnetworkDlg::flowList.SetItemText((list->buff_array + i)->id, 2, freceive);
			printf("现有IP地址：%d.%d.%d.%d 接收数据包数目为：%d\n", model.byte1, model.byte2, model.byte3, model.byte4, (list->buff_array + i)->receive);
			//在集合中找到了目的IP地址
			flag_receive = 0;
		}
		//如果在集合中既找到了源IP又找到了目的IP，则返回
		if (!(flag_send || flag_receive))
			return 0;
	}

	//源IP与目的IP至少有一个不在集合
	//若源IP不在集合中
	if (flag_send)
	{
		//1表示增加的IP发送数据包数量初始化为1
		CPublicData::Flist_add(list, ip_send, 1);
	}
	//若目的IP不在集合中
	if (flag_receive)
	{
		//0表示增加的IP接收数据包数量初始化为1
		CPublicData::Flist_add(list, ip_receive, 0);
	}
	return 0;
}
/*记录数据包*/
/*初始化记录动态数组*/
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

/*添加记录数组元素*/
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


/*释放记录动态数组*/
void CPublicData::Plist_free(PList * list)
{
	free(list->buff_array);
	free(list);
}


void CPublicData::Flist_free(FList *list) {

	free(list->buff_array);
	free(list);
}

/*****流量统计结束******/
