//#include "Udp-rs232.h"
#include "define.h"
//#include <RTL.h>
//#include <Net_Config.h>
#include "userPref.h"
#include "systick.h"

#include "rs232command.h"
#include "lcd.h"
#include "menu.h"

U8  rem_IP[4] = {192,168,0,10};
U8  my_udp_socket;
U16 udp_port=1000;

BOOL LEDrun;
BOOL LCDupdate;
U8	 lcd_text[2][16+1];


u32 dhcp_tout;
BOOL tick;

//extern LOCALM localm[];                       /* Local Machine Settings      */
#define MY_IP localm[NETIF_ETH].IpAdr
#define LocM   localm[NETIF_ETH]

extern U8 own_hw_adr[];


#define DHCP_TOUT   50                        /* DHCP timeout 5 seconds      */

extern void dhcp_check (void);

static char enable_udp = 0;

void init_dhcp_tout(void)
{
  	dhcp_tout = DHCP_TOUT;
}

int get_enable_udp(void)
{
	return enable_udp;
}

void set_enable_udp(int enable)
{
	enable_udp = enable;
}

 void timer_poll () 
{
  /* System tick timer running in poll mode */

  static u32 startTime = 0;

  if (isSysTickTimeout(startTime, 100)) 
  {
  	startTime = GetSysTick();
    /* Timer tick every 100 ms */
    timer_tick ();
    tick = __TRUE;
	
  }
}

void SetStaticIP(void)
{
	int i;
	for(i=0; i<4; i++)
	{
		localm[NETIF_ETH].IpAdr[i] = GetUserPref()->netWorkSetting.IpAdr[i];
		localm[NETIF_ETH].DefGW[i] = GetUserPref()->netWorkSetting.DefGW[i];
		localm[NETIF_ETH].NetMask[i] = GetUserPref()->netWorkSetting.NetMask[i];
		localm[NETIF_ETH].PriDNS[i] = GetUserPref()->netWorkSetting.PriDNS[i];
		localm[NETIF_ETH].SecDNS[i] = GetUserPref()->netWorkSetting.SecDNS[i];
	}
	
}




/*--------------------------- dhcp_check ------------------------------------*/

void dhcp_check () {
  /* Monitor DHCP IP address assignment. */

  

  if (tick == __FALSE || dhcp_tout == 0) {
    return;
  }
//#ifdef RTX_KERNEL
if(GetUserPref()->netWorkSetting.DHCP)
{
	disableDispReflrshInputFormat();
	tick = __FALSE;
	if ((mem_test (&MY_IP, 0, IP_ADRLEN) == __FALSE) && !(dhcp_tout & 0x80000000)) 
	{
	  /* Success, DHCP has already got the IP address. */
	  dhcp_tout = 0;
	  //dhcp_tout = 30 | 0x80000000;
	  sprintf((char *)lcd_text[0],"IP address:");
	  sprintf((char *)lcd_text[1],"%d.%d.%d.%d", MY_IP[0], MY_IP[1],
												 MY_IP[2], MY_IP[3]);
	  LCDupdate = __TRUE;
	  printf("DHCP Success = %d\r\n", GetSysTick());
	  printf("%s\r\n", lcd_text[0]);
	  printf("%s\r\n", lcd_text[1]);
		#if 0 //后面一起做
	  my_udp_socket = udp_get_socket(0, UDP_OPT_SEND_CS | UDP_OPT_CHK_CS, udp_callback);
  
	  if (my_udp_socket != 0) 
	  {
		  //Open UDP port 1000 for communication 
		  printf("Open Udp Socket %d = %d\r\n", UDP_PORT, GetSysTick());
		  udp_open (my_udp_socket, UDP_PORT);
	  }
	  else
	  {
		  printf("OPEN UDP SOCKET FALSE\r\n");
	  }
		#endif
  
	  //return;
	}
}

  

//#endif
  if (--dhcp_tout == 0) 
  {
	  disableDispReflrshInputFormat();
    /* A timeout, disable DHCP and use static IP address. */
    dhcp_disable ();

	#if 1
	printf("Set Static IP = %d\r\n", GetSysTick());
	SetStaticIP();
	printf("IP address: %d.%d.%d.%d\r\n", getIPAddress()[0],getIPAddress()[1], 
		getIPAddress()[2], getIPAddress()[3]);
	#endif
	
    LCDupdate = __TRUE;
    dhcp_tout = 30 | 0x80000000;
    return;
  }
  if (dhcp_tout == 0x80000000) 
  {
    dhcp_tout = 0;
    sprintf((char *)lcd_text[0],"IP address:");
    sprintf((char *)lcd_text[1],"%d.%d.%d.%d", MY_IP[0], MY_IP[1],
                                               MY_IP[2], MY_IP[3]);
    LCDupdate = __TRUE;
  }
  #if 0
  if(dhcp_tout == 0)
  {
	LCDupdate = __FALSE;

	my_udp_socket = udp_get_socket(0, UDP_OPT_SEND_CS | UDP_OPT_CHK_CS, udp_callback);

    if (my_udp_socket != 0) 
	{
		//Open UDP port 1000 for communication 
		udp_open (my_udp_socket, UDP_PORT);
		printf("Open Udp Socket %d = %d\r\n", UDP_PORT, GetSysTick());
	}
	else
	{
		printf("OPEN UDP SOCKET FALSE\r\n");
	}
  }
  #else
  if(LCDupdate == __TRUE)
  {
  	  #if 0
	  disableDispReflrshInputFormat();
	  menuEnterPage(MENU_INDEX_NOTHING);
	  LCD_Clear();
	  SysDelay1ms(200);
	  LCD_WriteStr(0,0,(char *)lcd_text[0]);
	  LCD_WriteStr(1, 0, (char *)lcd_text[1]);
	  SysDelay1ms(1000);
	  if(dhcp_tout == 0)
	  {
		  my_udp_socket = udp_get_socket(0, UDP_OPT_SEND_CS | UDP_OPT_CHK_CS, udp_callback);
  
		  if (my_udp_socket != 0) 
		  {
			  //Open UDP port 1000 for communication 
			  udp_open (my_udp_socket, 1000);
		  }
		  else
		  {
			  LCD_WriteLine1("OPEN UDP SOCKET FALSE");
			  SysDelay1ms(1000);
		  }
		  menuProcessEvent(MENU_KEY_INFO);
	  }
	  LCDupdate = __FALSE;
	  #else
	  disableDispReflrshInputFormat();
	  if(dhcp_tout == 0)
	  {
		  my_udp_socket = udp_get_socket(0, UDP_OPT_SEND_CS | UDP_OPT_CHK_CS, udp_callback);
  
		  if (my_udp_socket != 0) 
		  {
			  //Open UDP port 1000 for communication 
			  udp_open (my_udp_socket, 1000);
		  }
		  else
		  {
			  LCD_WriteLine1("OPEN UDP SOCKET FALSE");
			  SysDelay1ms(1000);
		  }
		  menuEnterPage(MENU_INDEX_NOTHING);
		  LCD_Clear();
		  SysDelay1ms(200);
		  LCD_WriteStr(0,0,(char *)lcd_text[0]);
		  LCD_WriteStr(1, 0, (char *)lcd_text[1]);
		  SysDelay1ms(1000);
		  menuProcessEvent(MENU_KEY_INFO);
	  }
	  LCDupdate = __FALSE;
	  #endif
	  //enableDispReflrshInputFormat();
  }
  #endif
}



void init_udp(void)
{
	printf("initial_udp...\r\n");

#if 1	
	{
		own_hw_adr[2] = 0x32;
		own_hw_adr[3] = 0x00;
		own_hw_adr[4] = getDeviceSN();
		own_hw_adr[5] = getDeviceSN()>>8;
	}
#endif

	
	init_TcpNet ();

	printf("udp_init = %d\r\n", GetSysTick());
	udp_init();

	printf("init_dhcp = %d\r\n", GetSysTick());
	init_dhcp();

	printf("udp end init = %d\r\n", GetSysTick());

  	dhcp_tout = DHCP_TOUT;
}


U8 channel_ip[4];

#if 0
void to_pcudp(int ip, U8 *buf,int buflen) 
#else
void to_pcudp(U8 *buf,int buflen) 
#endif
{
  
	U8 *sendbuf;
	U16 i=0;

	sendbuf = udp_get_buf (buflen);
	if(sendbuf == NULL)
	{
		return;
	}
								
	for(i=0;i< buflen ;i++)
	{
		sendbuf[i]=buf[i];
	}	

	#if 0
	channel_ip[0] = ip&0xff;
	channel_ip[1] = (ip>>8)&0xff;
	channel_ip[2] = (ip>>16)&0xff;
	channel_ip[3] = (ip>>24)&0xff;
	udp_send (my_udp_socket, channel_ip, udp_port, sendbuf,buflen);
	#else
	udp_send (my_udp_socket, rem_IP, udp_port, sendbuf,buflen);
	#endif

}

void udp_task(void)
{
	if(enable_udp)
	{
		timer_poll ();
		main_TcpNet ();
		
		
		dhcp_check ();
		
		if(dhcp_tout > 0 && dhcp_tout < 100)
		{
			dhcp_run_client();
		}
	}
}



void from_pcudp	(int channel, U8 *buf, int len)
{
	//prcess up pc host send out
	//if((buf[10]==uc150addr+0x30)&&(UC150CMD_LEN==len))
	int i;
	for(i=0; i<len; i++)
	{
		IsrReceiveCommand(channel, buf[i]);
	}
}
void get_remoteport( U8 *remip, U16 port)
{
	U8 i=0;
	for(i=0;i<4;i++)
	{
		rem_IP[i]=remip[i];	
	}
	udp_port=port;
}


U16 udp_callback (U8 socket, U8 *remip, U16 port, U8 *buf, U16 len)
{   /* This function is called when UDP data has been received. */		

	int channel;
	int value;
	

	get_remoteport( remip,  port);

	value = remip[0];
	value += remip[1]<<8;
	value += remip[2]<<16;
	value += remip[3]<<24;

	//此段程序源自320的控制板,猜测大意是如果网络控制则拒绝其他控制方式(串口/USB)
	//channel = getUsedChannel(CHANNEL_TYPE_UDP, value);
	//if(channel == 0xff)
	//{
	//	channel = getFreeChannel(CHANNEL_TYPE_UDP, value);
	//}	

	//control dev
   	//from_pcudp(channel,  buf, len);
   	from_pcudp(1,  buf, len);
	return (0);
}

