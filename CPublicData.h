#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>
#include "time.h"

/* 4字节的IP地址 */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
} ip_address;

//统计每个IP地址的发送数据包数量的结构体
typedef struct flow
{
	//该IP地址序号
	int id;
	//ip地址
	ip_address ip;
	//发送数据包数目
	int send;
	//接收数据包数目
	int receive;
}flow;

//统计流量动态数组
typedef struct {
	int buff_size;    // 数组上限
	int size;    // 数组已有元素个数
	flow * buff_array;    // 已有元素头指针
} FList;

//记录每个数据包信息
typedef struct pinfo
{
	//帧序号
	int id;
	//内容
	u_char * data;
	//长度
	int length;
}pinfo;

//记录数据包内容动态数组
typedef struct 
{
	int buff_size;    // 数组上限
	int size;    // 数组已有元素个数
	pinfo * buff_array;    // 已有元素头指针
} PList;

/* IPv4 首部 */
typedef struct ip_header
{
	u_char  ver_ihl;        // 版本 (4 bits) + 首部长度 (4 bits)
	u_char  tos;            // 服务类型(Type of service) (8 bits)
	u_short tlen;           // 总长(Total length)  (16 bits)
	u_short identification; // 标识(Identification) (16 bits)
	u_short flags_fo;       // 标志位(Flags) (3 bits) + 段偏移量(Fragment offset) (13 bits)
	u_char  ttl;            // 存活时间(Time to live) (8 bits)
	u_char  proto;          // 协议(Protocol) (8 bits)
	u_short crc;            // 首部校验和(Header checksum) (16 bits)
	ip_address  saddr;      // 源地址(Source address) (32 bits)
	ip_address  daddr;      // 目的地址(Destination address) (32 bits)
	u_int   op_pad;         // 选项与填充(Option + Padding)  (可能有可能没有)
} ip_header;

/* UDP 首部*/
typedef struct udp_header
{
	u_short sport;          // 源端口(Source port)
	u_short dport;          // 目的端口(Destination port)
	u_short len;            // UDP数据包长度(Datagram length)
	u_short crc;            // 校验和(Checksum)
} udp_header;

class CPublicData
{
public:
	//流量统计结果
	static FList * flow_list;
	//数据包内容统计
	static PList * package_list;

	/*流量列表显示初始化*/
	static CString fip;
	static CString fsend;
	static CString freceive;
	static CString fip1;
	static CString fip2;
	static CString fip3;
	static CString fip4;
	/*流量列表显示初始化结束*/

public:
	/*初始化流量动态数组*/
	static FList * Flist_init(int);

	/*更新流量动态数组*/
	static int Flist_update(FList *, ip_address, ip_address);

	/*添加流量数组元素*/
	static void Flist_add(FList *list, ip_address ip, int flag);

	/*释放流量动态数组*/
	static void Flist_free(FList *);

	/*初始化记录动态数组*/
	static PList * Plist_init(int);

	/*添加记录数组元素*/
	static void Plist_add(PList *, u_char * data,int);

	/*释放记录动态数组*/
	static void Plist_free(PList *);

	/*计数*/
	static int ip_number;

	CPublicData();
	~CPublicData();
};

