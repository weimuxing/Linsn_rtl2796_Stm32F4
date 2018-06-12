//#include <Net_Config.h>
#include <stdio.h>
#include <string.h>
#include "define.h"

#define ENABLE_UDP_COM 1 //

#define DHCP_CHECK 1
#define ENABLE_NETWORK 1

extern U8  rem_IP[4];
extern U8  my_udp_socket;
extern U16 udp_port;

#define UDP_PORT (1000)

void init_udp(void);

void from_pcudp	(int channel, U8 *buf, int len);


void get_remoteport( U8 *remip, U16 port) ;

U16 udp_callback (U8 socket, U8 *remip, U16 port, U8 *buf, U16 len) ;

#if 0
void to_pcudp(int ip, U8 *buf,int buflen) ;
#else
void to_pcudp(U8 *buf,int buflen) ;
#endif

void udp_task(void);

void init_dhcp_tout(void);



