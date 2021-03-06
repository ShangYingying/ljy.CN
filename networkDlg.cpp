
// networkDlg.cpp: 实现文件
//

#include "stdafx.h"


#include "network.h"
#include "networkDlg.h"
#include "afxdialogex.h"
#include "Capture.h"

/*输出文件测试用*/
#pragma warning(disable:4996)


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CnetworkDlg 对话框

/* 全局变量声明*/
//帧序号
int count = 0;
//适配器指针
pcap_if_t *d;
//所选适配器序号
int inum = 0;
//错误信息
char errbuf[PCAP_ERRBUF_SIZE];
//适配器链表指针
pcap_if_t *alldevs;
//监听的适配器
pcap_t * adhandle;
//是否停止捕获
int stopNow = 0;

//流量统计结果
FList* CPublicData::flow_list = NULL;
//记录数据包内容
PList* CPublicData::package_list = NULL;

//ip地址序号
int CPublicData::ip_number = 0;
CString CPublicData::fip;
CString CPublicData::fsend;
CString CPublicData::freceive;
CString CPublicData::fip1;
CString CPublicData::fip2;
CString CPublicData::fip3;
CString CPublicData::fip4;
/*全局变量结束*/

//显示列表所需变量
CListCtrl Capture::packageList;
CListCtrl CnetworkDlg::adapterList;
CListCtrl CnetworkDlg::packageList;
CListCtrl CnetworkDlg::flowList;

/*全局变量声明结束*/

/******抓包函数声明*************/
/* 包处理函数原型 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

/*协议识别函数*/
char* protocol(int a);
/******抓包函数声明结束**********/

CnetworkDlg::CnetworkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NETWORK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CnetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, adapterList);
	DDX_Control(pDX, IDC_LIST2, packageList);
	DDX_Control(pDX, IDC_LIST4, flowList);
}

BEGIN_MESSAGE_MAP(CnetworkDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CnetworkDlg::OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CnetworkDlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CnetworkDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &CnetworkDlg::OnEnChangeEdit1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CnetworkDlg::OnLvnItemchangedList2)
	ON_BN_CLICKED(IDCANCEL, &CnetworkDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CnetworkDlg 消息处理程序

BOOL CnetworkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	/*初始化适配器列表*/
	CRect rect;
	adapterList.GetClientRect(&rect);   //获得客户区
	adapterList.SetExtendedStyle(adapterList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);    //设置风格，LVS_EX_FULLROWSELECT是有线条，后面那个是充满
																										  //插入列：
	adapterList.InsertColumn(0, _T("适配器序号"), LVCFMT_CENTER, rect.Width() / 3, 0);
	adapterList.InsertColumn(1, _T("适配器名称"), LVCFMT_CENTER, rect.Width() / 3, 1);
	adapterList.InsertColumn(2, _T("适配器描述"), LVCFMT_CENTER, rect.Width() / 3, 2);

	/*初始化流量统计列表*/
	CRect rect2;
	CnetworkDlg::flowList.GetClientRect(&rect2);   //获得客户区
	CnetworkDlg::flowList.SetExtendedStyle(CnetworkDlg::packageList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);    //设置风格，LVS_EX_FULLROWSELECT是有线条，后面那个是充满
																																	  /*初始化抓包信息列表*/
	CnetworkDlg::flowList.InsertColumn(0, _T("IP地址"), LVCFMT_CENTER, rect2.Width() / 3, 0);
	CnetworkDlg::flowList.InsertColumn(1, _T("发送数据包数目"), LVCFMT_CENTER, rect2.Width() / 3, 1);
	CnetworkDlg::flowList.InsertColumn(2, _T("接收数据包数目"), LVCFMT_CENTER, rect2.Width() / 3, 2);


	/* 获得设备列表 */
	if (pcap_findalldevs_ex((char*)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		getchar();
		exit(1);
	}

	int i = 1;

	/* 打印列表 */
	for (d = alldevs; d; d = d->next)
	{
		CString name(d->name);
		CString des(d->description);
		CString num;
		num.Format(_T("%d"), i);
		//插入第i-1行数据，行首内容为适配器序号
		adapterList.InsertItem(i-1, num);
		//设置第i-1行第1列内容，为适配器名称
		adapterList.SetItemText(i-1, 1, name);

		if (d->description)
			//设置第i-1行第1列内容，为适配器描述
			adapterList.SetItemText(i-1, 2, des);
		else
			adapterList.SetItemText(i-1, 2, _T("无描述"));
		i ++;
	}

	if (i == 0)
	{
		adapterList.DeleteAllItems();
		adapterList.InsertColumn(0, _T("\nNo interfaces found! Make sure WinPcap is installed.\n"), LVCFMT_CENTER, rect.Width() , 0);
		adapterList.InsertItem(0, _T("No interfaces found! Make sure WinPcap is installed."));
		return -1;
	}

	
	/*初始化适配器列表结束*/


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CnetworkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CnetworkDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CnetworkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CnetworkDlg::OnBnClickedOk()
{

	// TODO: 在此添加控件通知处理程序代码
	//初始化流量统计表
	CPublicData::flow_list = CPublicData::Flist_init(50);
	CPublicData::package_list = CPublicData::Plist_init(100);

	/* 跳转到已选设备 */
	//inum为全局变量，为用户选择的网卡序号
	int i = 0;
	for (d = alldevs, i = 0; i< inum - 1; d = d->next, i++);

	/* 打开适配器 */
	if ((adhandle = pcap_open(d->name,  // 设备名
		65536,     // 要捕捉的数据包的部分
				   // 65535保证能捕获到不同数据链路层上的每个数据包的全部内容
		PCAP_OPENFLAG_PROMISCUOUS,         // 混杂模式
		1000,      // 读取超时时间
		NULL,      // 远程机器验证
		errbuf     // 错误缓冲池
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
	}

	/*初始化抓包信息列表*/
	CRect rect;
	CnetworkDlg::packageList.GetClientRect(&rect);   //获得客户区
	CnetworkDlg::packageList.SetExtendedStyle(CnetworkDlg::packageList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);    //设置风格，LVS_EX_FULLROWSELECT是有线条，后面那个是充满

	CnetworkDlg::packageList.InsertColumn(0, _T("帧序号"), LVCFMT_CENTER, rect.Width() / 8, 0);
	CnetworkDlg::packageList.InsertColumn(1, _T("捕捉时间"), LVCFMT_CENTER, rect.Width() / 8, 1);
	CnetworkDlg::packageList.InsertColumn(2, _T("数据包标识"), LVCFMT_CENTER, rect.Width() / 8, 2);
	CnetworkDlg::packageList.InsertColumn(3, _T("数据包长度"), LVCFMT_CENTER, rect.Width() / 8, 3);
	CnetworkDlg::packageList.InsertColumn(4, _T("数据包协议"), LVCFMT_CENTER, rect.Width() / 8, 4);
	CnetworkDlg::packageList.InsertColumn(5, _T("数据包源IP地址"), LVCFMT_CENTER, rect.Width() / 8, 5);
	CnetworkDlg::packageList.InsertColumn(6, _T("数据包目的IP地址"), LVCFMT_CENTER, rect.Width() / 8, 6);
	CnetworkDlg::packageList.InsertColumn(7, _T("UDP端口信息"), LVCFMT_CENTER, rect.Width() / 8, 7);


	/* 检查数据链路层，为了简单，我们只考虑以太网 */
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		
	}

	/* 释放设备列表 */
	pcap_freealldevs(alldevs);

	/* 开始捕捉 */
	pcap_loop(adhandle, 0, packet_handler, NULL);

}


void CnetworkDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 
	NMLISTVIEW *pnml = (NMLISTVIEW*)pNMHDR;//新建一个指针
	if (-1 != pnml->iItem)//如果指针无误
	{
		CString str;
		str = adapterList.GetItemText(pnml->iItem, 0);//如果鼠标选中了某行，将选中的行的第0列赋给str
		inum = _ttoi(str);
	}

	*pResult = 0;
}

/* 每次捕获到数据包时，libpcap都会自动调用这个回调函数 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	//及时更新显示
	UpdateWindow(AfxGetMainWnd()->m_hWnd);

	struct tm ltime;
	char timestr[16];
	ip_header *ih;
	udp_header *uh;
	u_int ip_len;
	u_short sport, dport;
	time_t local_tv_sec;

	/*抓包列表中的显示变量初始化*/
	/* 将时间戳转换成可识别的格式 */
	local_tv_sec = header->ts.tv_sec;
	localtime_s(&ltime, &local_tv_sec);
	strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);

	CString pframe;          //帧序号  
	int temp = count +1;
	pframe.Format(_T("%d"), temp);
	CString ptime(timestr);  //捕捉时间
	CString pnumber;         //数据包标识
	CString plength;         //数据包长度

	CString sp;              //源IP地址
	CString dp;              //目的IP地址
	CString Info;            //UDP端口信息
	CString sp1;
	CString sp2;
	CString sp3;
	CString sp4;
	CString dp1;
	CString dp2;
	CString dp3;
	CString dp4;
	CString info1;
	CString info2;
	CString info3;

	/* 获得IP数据包头部的位置 */
	ih = (ip_header *)(pkt_data + 14); //以太网头部长度
									   // 获得UDP首部的位置 
	ip_len = (ih->ver_ihl & 0xf) * 4;
	uh = (udp_header *)((u_char*)ih + ip_len);

	/* 将网络字节序列转换成主机字节序列 */
	//源应用程序端口
	sport = ntohs(uh->sport);
	//目的应用程序端口
	dport = ntohs(uh->dport);

	/* 打印IP地址和UDP端口 */
	pnumber.Format(_T("%d"), ih->identification);
	plength.Format(_T("%d"), ntohs(ih->tlen) + 14);
	CString pprotocol(protocol(ih->proto));       //数据包协议
	sp1.Format(_T("%d"), ih->saddr.byte1);
	sp2.Format(_T("%d"), ih->saddr.byte2);
	sp3.Format(_T("%d"), ih->saddr.byte3);
	sp4.Format(_T("%d"), ih->saddr.byte4);
	sp = sp1 + "." + sp2 + "." + sp3 + "." + sp4;
	dp1.Format(_T("%d"), ih->daddr.byte1);
	dp2.Format(_T("%d"), ih->daddr.byte2);
	dp3.Format(_T("%d"), ih->daddr.byte3);
	dp4.Format(_T("%d"), ih->daddr.byte4);
	dp = dp1 + "." + dp2 + "." + dp3 + "." + dp4;

	info1.Format(_T("%d"), sport);
	info2.Format(_T("%d"), dport);
	Info = info1 + "->" + info2;
	/*抓包列表显示变量初始化结束*/

	//显示抓包结果
	CnetworkDlg::packageList.InsertItem(count, pframe);
	CnetworkDlg::packageList.SetItemText(count, 1, ptime);
	CnetworkDlg::packageList.SetItemText(count, 2, pnumber);
	CnetworkDlg::packageList.SetItemText(count, 3, plength);
	CnetworkDlg::packageList.SetItemText(count, 4, pprotocol);
	CnetworkDlg::packageList.SetItemText(count, 5, sp);
	CnetworkDlg::packageList.SetItemText(count, 6, dp);
	CnetworkDlg::packageList.SetItemText(count, 7, Info);
	CPublicData::Flist_update(CPublicData::flow_list, ih->saddr, ih->daddr);

	/*记录帧序号*/
	count++;

	//十六进制显示抓包内容
	CString hexStr, tempStr;
	u_char * piece = (u_char *)pkt_data;
	for (int i = 0; i < header->caplen; i++)
	{
		tempStr.Format(_T("%x "), * (piece++) );
		hexStr += tempStr;
	}
	CPublicData::Plist_add(CPublicData::package_list,piece,header->caplen);
	SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_EDIT1, hexStr);

	//字符显示抓包内容
	CString chaStr, ctempStr;
	piece = (u_char *)pkt_data;
	for (int i = 0; i < header->caplen; i++)
	{
		ctempStr.Format(_T("%c "), *(piece++));
		chaStr += ctempStr;
	}
	SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_EDIT2, chaStr);

	//判断是否停止
	AfxGetApp()->PumpMessage();
	if (stopNow == 1)
	{
		pcap_breakloop(adhandle);  ///right here ahandle turned into 0
		//CPublicData::Flist_free(CPublicData::flow_list);
		//CPublicData::Plist_free(CPublicData::package_list);
	}
}

/*****解析向IP传送数据的协议****/
char*  protocol(int a)
{
	//协议字段：8个bit。用来标识是哪个协议向IP传送数据。ICMP为1，IGMP为2，TCP为6，UDP为17，GRE为47，ESP为50。
	switch (a)
	{
	case 1:return "ICMP协议\n";break;
	case 2:return "IGMP协议\n";break;
	case 6:return "TCP协议\n";break;
	case 17:return "UDP协议\n";break;
	case 47:return "GRE协议\n";break;
	case 50:return "ESP协议\n";break;
	default:return "未知协议\n";
	}
}


void CnetworkDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	stopNow = 1;
}


void CnetworkDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CnetworkDlg::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int pnum;

	int i = packageList.GetSelectionMark();//获得选 中行的行标
	CString s = packageList.GetItemText(i, 0);//i是行，j是列。m_list是个list control 对象
	pnum = _ttoi(s);

	//显示十六进制内容
	CString hexStr, tempStr;
	u_char * piece = (CPublicData::package_list->buff_array + (pnum - 1))->data;
	for (int i = 0; i < (CPublicData::package_list->buff_array + (pnum - 1))->length; i++)
	{
		tempStr.Format(_T("%x "), *(piece++));
		hexStr += tempStr;
	}
	this->SetDlgItemText(IDC_EDIT1, hexStr);
	//显示字符内容
	CString chaStr, ctempStr;
	piece = (CPublicData::package_list->buff_array + (pnum - 1))->data;
	for (int i = 0; i < (CPublicData::package_list->buff_array + (pnum - 1))->length; i++)
	{
		ctempStr.Format(_T("%c "), *(piece++));
		chaStr += ctempStr;
	}
	this->SetDlgItemText(IDC_EDIT2, chaStr);
	*pResult = 0;
}


void CnetworkDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}
