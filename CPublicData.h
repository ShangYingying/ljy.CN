#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>
#include "time.h"

/* 4�ֽڵ�IP��ַ */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
} ip_address;

//ͳ��ÿ��IP��ַ�ķ������ݰ������Ľṹ��
typedef struct flow
{
	//��IP��ַ���
	int id;
	//ip��ַ
	ip_address ip;
	//�������ݰ���Ŀ
	int send;
	//�������ݰ���Ŀ
	int receive;
}flow;

//ͳ��������̬����
typedef struct {
	int buff_size;    // ��������
	int size;    // ��������Ԫ�ظ���
	flow * buff_array;    // ����Ԫ��ͷָ��
} FList;

//��¼ÿ�����ݰ���Ϣ
typedef struct pinfo
{
	//֡���
	int id;
	//����
	u_char * data;
	//����
	int length;
}pinfo;

//��¼���ݰ����ݶ�̬����
typedef struct 
{
	int buff_size;    // ��������
	int size;    // ��������Ԫ�ظ���
	pinfo * buff_array;    // ����Ԫ��ͷָ��
} PList;

/* IPv4 �ײ� */
typedef struct ip_header
{
	u_char  ver_ihl;        // �汾 (4 bits) + �ײ����� (4 bits)
	u_char  tos;            // ��������(Type of service) (8 bits)
	u_short tlen;           // �ܳ�(Total length)  (16 bits)
	u_short identification; // ��ʶ(Identification) (16 bits)
	u_short flags_fo;       // ��־λ(Flags) (3 bits) + ��ƫ����(Fragment offset) (13 bits)
	u_char  ttl;            // ���ʱ��(Time to live) (8 bits)
	u_char  proto;          // Э��(Protocol) (8 bits)
	u_short crc;            // �ײ�У���(Header checksum) (16 bits)
	ip_address  saddr;      // Դ��ַ(Source address) (32 bits)
	ip_address  daddr;      // Ŀ�ĵ�ַ(Destination address) (32 bits)
	u_int   op_pad;         // ѡ�������(Option + Padding)  (�����п���û��)
} ip_header;

/* UDP �ײ�*/
typedef struct udp_header
{
	u_short sport;          // Դ�˿�(Source port)
	u_short dport;          // Ŀ�Ķ˿�(Destination port)
	u_short len;            // UDP���ݰ�����(Datagram length)
	u_short crc;            // У���(Checksum)
} udp_header;

class CPublicData
{
public:
	//����ͳ�ƽ��
	static FList * flow_list;
	//���ݰ�����ͳ��
	static PList * package_list;

	/*�����б���ʾ��ʼ��*/
	static CString fip;
	static CString fsend;
	static CString freceive;
	static CString fip1;
	static CString fip2;
	static CString fip3;
	static CString fip4;
	/*�����б���ʾ��ʼ������*/

public:
	/*��ʼ��������̬����*/
	static FList * Flist_init(int);

	/*����������̬����*/
	static int Flist_update(FList *, ip_address, ip_address);

	/*�����������Ԫ��*/
	static void Flist_add(FList *list, ip_address ip, int flag);

	/*�ͷ�������̬����*/
	static void Flist_free(FList *);

	/*��ʼ����¼��̬����*/
	static PList * Plist_init(int);

	/*��Ӽ�¼����Ԫ��*/
	static void Plist_add(PList *, u_char * data,int);

	/*�ͷż�¼��̬����*/
	static void Plist_free(PList *);

	/*����*/
	static int ip_number;

	CPublicData();
	~CPublicData();
};

