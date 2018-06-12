/*----------------------------------------------------------------------------
 *      RL-ARM - TCPnet
 *----------------------------------------------------------------------------
 *      Name:    ETH_STM32F2XX.C
 *      Purpose: Driver for ST STM32F2xx Ethernet Controller
 *      Rev.:    V4.20
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <Net_Config.h>
#include "stm32f2xx.h"
#include "ETH_STM32F2xx.h"

/* The following macro definitions may be used to select the speed
   of the physical link:

  _10MBIT_   - connect at 10 MBit only
  _100MBIT_  - connect at 100 MBit only

  By default an autonegotiation of the link speed is used. This may take 
  longer to connect, but it works for 10MBit and 100MBit physical links.
  
    The following macro definitions may be used to select PYH interface:
  -MII_      - use MII interface instead of RMII
                                                                              */
//#define _MII_ 1

/* Net_Config.c */
extern U8 own_hw_adr[];

/* Local variables */
static U8 TxBufIndex;
static U8 RxBufIndex;

/* ENET local DMA Descriptors. */
static RX_Desc Rx_Desc[NUM_RX_BUF];
static TX_Desc Tx_Desc[NUM_TX_BUF];

/* ENET local DMA buffers. */
static U32 rx_buf[NUM_RX_BUF][ETH_BUF_SIZE>>2];
static U32 tx_buf[NUM_TX_BUF][ETH_BUF_SIZE>>2];

/*----------------------------------------------------------------------------
 *      ENET Ethernet Driver Functions
 *----------------------------------------------------------------------------
 *  Required functions for Ethernet driver module:
 *  a. Polling mode: - void init_ethernet ()
 *                   - void send_frame (OS_FRAME *frame)
 *                   - void poll_ethernet (void)
 *  b. Interrupt mode: - void init_ethernet ()
 *                     - void send_frame (OS_FRAME *frame)
 *                     - void int_enable_eth ()
 *                     - void int_disable_eth ()
 *                     - interrupt function 
 *---------------------------------------------------------------------------*/

/* Local Function Prototypes */
static void rx_descr_init (void);
static void tx_descr_init (void);
static void write_PHY (U32 PhyReg, U16 Value);
static U16  read_PHY (U32 PhyReg);

/*--------------------------- init_ethernet ---------------------------------*/
void init_ethernet (void) {
  /* Initialize the ETH ethernet controller. */
  U32 regv,tout,id1,id2;  

  /* Enable System configuration controller clock */
  RCC->APB2ENR |= (1 << 14);
  
  /* Reset Ethernet MAC */
  RCC->AHB1RSTR |=  0x02000000;
#ifdef _MII_
  SYSCFG->PMC &= ~(1 << 23);
#else
  SYSCFG->PMC |=  (1 << 23);
#endif
  RCC->AHB1RSTR &= ~0x02000000;

#if _MII_
  /* Enable Ethernet and GPIOA, GPIOB, GPIOC, GPIOG, GPIOH clocks */
  RCC->AHB1ENR |= 0x1E0000C7;
#else
  /* Enable Ethernet and GPIOA, GPIOB, GPIOC, GPIOG clocks */
  RCC->AHB1ENR |= 0x1E000047;
#endif
  
  /* Configure Port A ethernet pins (PA.1, PA.2, PA.7, PA.8) */
  GPIOA->MODER   &= ~0x0003C03C;
  GPIOA->MODER   |=  0x00028028;              /* Pins to alternate function */
  GPIOA->OTYPER  &= ~0x00000186;              /* Pins in push-pull mode     */
  GPIOA->OSPEEDR |=  0x0003C03C;              /* Slew rate as 100MHz pin    */
  GPIOA->PUPDR   &= ~0x0003C03C;              /* No pull up, no pull down   */

  GPIOA->AFR[0]  &= ~0xF0000FF0;
  GPIOA->AFR[0]  |=  0xB0000BB0;              /* Pins to AF11 (Ethernet)    */
  GPIOA->AFR[1]  &= ~0x0000000F;              
  GPIOA->AFR[1]  |=  0x00000000;              /* Pin to AF0 (MCO1)          */

#ifdef _MII_
  /* Configure Port B ethernet pin (PB8) */
  GPIOB->MODER   &= ~0x00030000;
  GPIOB->MODER   |=  0x00020000;              /* Pins to alternate function */
  GPIOB->OTYPER  &= ~0x00000100;              /* Pin in push-pull mode      */
  GPIOB->OSPEEDR |=  0x00030000;              /* Slew rate as 100MHz pin    */
  GPIOB->PUPDR   &= ~0x00030000;              /* No pull up, no pull down   */

  GPIOB->AFR[1]  &= ~0x0000000F;
  GPIOB->AFR[1]  |=  0x0000000B;              /* Pin to AF11 (Ethernet)     */

  /* Configure Port C ethernet pins (PC.1, PC.2, PC.3, PC.4, PC.5) */
  GPIOC->MODER   &= ~0x00000FFC;
  GPIOC->MODER   |=  0x00000AA8;              /* Pins to alternate function */
  GPIOC->OTYPER  &= ~0x0000003E;              /* Pins in push-pull mode     */
  GPIOC->OSPEEDR |=  0x00000FFC;              /* Slew rate as 100MHz pin    */
  GPIOC->PUPDR   &= ~0x00000FFC;              /* No pull up, no pull down   */

  GPIOC->AFR[0]  &= ~0x00FFFFF0;
  GPIOC->AFR[0]  |=  0x00BBBBB0;              /* Pins to AF11 (Ethernet)    */
#else
  /* Configure Port C ethernet pins (PC.1, PC.4, PC.5) */
  GPIOC->MODER   &= ~0x00000F0C;
  GPIOC->MODER   |=  0x00000A08;              /* Pins to alternate function */
  GPIOC->OTYPER  &= ~0x00000032;              /* Pins in push-pull mode     */
  GPIOC->OSPEEDR |=  0x00000F0C;              /* Slew rate as 100MHz pin    */
  GPIOC->PUPDR   &= ~0x00000F0C;              /* No pull up, no pull down   */

  GPIOC->AFR[0]  &= ~0x00FF00F0;
  GPIOC->AFR[0]  |=  0x00BB00B0;              /* Pins to AF11 (Ethernet)    */
#endif
  
#if 1 
	/* Configure Port B ethernet pins (PB.12, PB.13) */  // used in VSP360 board added by anve
	GPIOB->MODER   &= ~(0xF<<24);
	GPIOB->MODER   |=  (0xa<<24);				/* Pin to alternate function  */
	GPIOB->OTYPER  &= ~(0x03<<12);				/* Pin in push-pull mode	  */
	GPIOB->OSPEEDR |=  (0xF<<24);				/* Slew rate as 100MHz pin	  */
	GPIOB->PUPDR   &= ~(0xF<<24);				/* No pull up, no pull down   */

	GPIOB->AFR[1]  &= ~(0xFF<<16);
	GPIOB->AFR[1]  |=  (0xBB<<16);				/* Pin to AF11 (Ethernet)	  */

	/* Configure Port G ethernet pins (PG.11) */// used in VSP360 board added by anve
	GPIOG->MODER   &= ~0x00C00000;
	GPIOG->MODER   |=  0x00800000;              /* Pin to alternate function  */
	GPIOG->OTYPER  &= ~0x00000800;              /* Pin in push-pull mode      */
	GPIOG->OSPEEDR |=  0x00C00000;              /* Slew rate as 100MHz pin    */
	GPIOG->PUPDR   &= ~0x00C00000;              /* No pull up, no pull down   */

	GPIOG->AFR[1]  &= ~0x0000F000;
	GPIOG->AFR[1]  |=  0x0000B000;              /* Pin to AF11 (Ethernet)     */

#elif 0
	/* Configure Port B ethernet pins (PB.11, PB.12, PB.13) */  // used in 3U board added by anve
	GPIOB->MODER   &= ~(0x3F<<22);
	GPIOB->MODER   |=  (0x2a<<22);				/* Pin to alternate function  */
	GPIOB->OTYPER  &= ~(0x07<<11);				/* Pin in push-pull mode	  */
	GPIOB->OSPEEDR |=  (0x3F<<22);				/* Slew rate as 100MHz pin	  */
	GPIOB->PUPDR   &= ~(0x3F<<22);				/* No pull up, no pull down   */

	GPIOB->AFR[1]  &= ~(0xFFF<<12);
	GPIOB->AFR[1]  |=  (0xBBB<<12);				/* Pin to AF11 (Ethernet)	  */

#else
  /* Configure Port G ethernet pins (PG.11, PG.13, PG.14) */
  GPIOG->MODER   &= ~0x3CC00000;
  GPIOG->MODER   |=  0x28800000;              /* Pin to alternate function  */
  GPIOG->OTYPER  &= ~0x00006800;              /* Pin in push-pull mode      */
  GPIOG->OSPEEDR |=  0x3CC00000;              /* Slew rate as 100MHz pin    */
  GPIOG->PUPDR   &= ~0x3CC00000;              /* No pull up, no pull down   */

  GPIOG->AFR[1]  &= ~0x0FF0F000;
  GPIOG->AFR[1]  |=  0x0BB0B000;              /* Pin to AF11 (Ethernet)     */
#endif

#ifdef _MII_
  /* Configure Port H ethernet pins (PH.0, PH.2, PH.3, PH.6, PH.7) */
  GPIOH->MODER   &= ~0x0000F0F3;
  GPIOH->MODER   |=  0x0000A0A2;              /* Pins to alternate function */
  GPIOH->OTYPER  &= ~0x000000CD;              /* Pins in push-pull mode     */
  GPIOH->OSPEEDR |=  0x0000F0F3;              /* Slew rate as 100MHz pin    */
  GPIOH->PUPDR   &= ~0x0000F0F3;              /* No pull up, no pull down   */

  GPIOH->AFR[0]  &= ~0xFF00FF0F;
  GPIOH->AFR[0]  |=  0xBB00BB0B;              /* Pins to AF11 (Ethernet)    */  
#endif

	ETH->DMABMR  |= DBMR_SR;
  while (ETH->DMABMR & DBMR_SR);

  /* HCLK Clock range 100-120MHz. */
  ETH->MACMIIAR = 0x00000004;
  /* Put the DP83848C in reset mode */
  write_PHY (PHY_REG_BMCR, 0x8000);

  /* Wait for hardware reset to end. */
  for (tout = 0; tout < 0x10000; tout++) {
    regv = read_PHY (PHY_REG_BMCR);
    if (!(regv & 0x8800)) {
      /* Reset complete, device not Power Down. */
      break;
    }
  }
  /* Check if this is a DP83848C PHY. */
  id1 = read_PHY (PHY_REG_IDR1);
  id2 = read_PHY (PHY_REG_IDR2);
  if (((id1 << 16) | (id2 & 0xFFF0)) == DP83848C_ID) {
    /* Configure the PHY device */
#if defined (_10MBIT_)
    /* Connect at 10MBit */
    write_PHY (PHY_REG_BMCR, PHY_FULLD_10M);
#elif defined (_100MBIT_)
    /* Connect at 100MBit */
    write_PHY (PHY_REG_BMCR, PHY_FULLD_100M);
#else
    /* Use autonegotiation about the link speed. */
    write_PHY (PHY_REG_BMCR, PHY_AUTO_NEG);
    /* Wait to complete Auto_Negotiation. */
    for (tout = 0; tout < 0x10000; tout++) {
      regv = read_PHY (PHY_REG_BMSR);
      if (regv & 0x0020) {
        /* Autonegotiation Complete. */
        break;
      }
    }
#endif
  }

  /* Check the link status. */
  for (tout = 0; tout < 0x10000; tout++) {
    regv = read_PHY (PHY_REG_STS);
    if (regv & 0x0001) {
      /* Link is on. */
      break;
    }
  }

  /* Initialize MAC control register */
  ETH->MACCR  = MCR_ROD;

  /* Configure Full/Half Duplex mode. */
  if (regv & 0x0004) {
    /* Full duplex is enabled. */
    ETH->MACCR |= MCR_DM;
  }

  /* Configure 100MBit/10MBit mode. */
  if ((regv & 0x0002) == 0) {
    /* 100MBit mode. */
    ETH->MACCR |= MCR_FES;
  }

  /* MAC address filtering, accept multicast packets. */
  ETH->MACFFR = MFFR_HPF | MFFR_PAM;
  ETH->MACFCR = MFCR_ZQPD;

  /* Set the Ethernet MAC Address registers */
  ETH->MACA0HR = ((U32)own_hw_adr[5] <<  8) | (U32)own_hw_adr[4];
  ETH->MACA0LR = ((U32)own_hw_adr[3] << 24) | (U32)own_hw_adr[2] << 16 |
                 ((U32)own_hw_adr[1] <<  8) | (U32)own_hw_adr[0];

  /* Initialize Tx and Rx DMA Descriptors */
  rx_descr_init ();
  tx_descr_init ();

  /* Flush FIFO, start DMA Tx and Rx */
  ETH->DMAOMR = DOMR_FTF | DOMR_ST | DOMR_SR;

  /* Enable receiver and transmiter */
  ETH->MACCR |= MCR_TE | MCR_RE;

  /* Reset all interrupts */
  ETH->DMASR  = 0xFFFFFFFF;

  /* Enable Rx and Tx interrupts. */
  ETH->DMAIER = INT_NISE | INT_AISE | INT_RBUIE | INT_RIE;
}

/*--------------------------- int_enable_eth --------------------------------*/

void int_enable_eth (void) {
  /* Ethernet Interrupt Enable function. */
  NVIC->ISER[1] = 1 << 29;
}


/*--------------------------- int_disable_eth -------------------------------*/

void int_disable_eth (void) {
  /* Ethernet Interrupt Disable function. */
  NVIC->ICER[1] = 1 << 29;
}


/*--------------------------- send_frame ------------------------------------*/

void send_frame (OS_FRAME *frame) {
  /* Send frame to ETH ethernet controller */
  U32 *sp,*dp;
  U32 i,j;

  j = TxBufIndex;
  /* Wait until previous packet transmitted. */
  while (Tx_Desc[j].CtrlStat & DMA_TX_OWN);

  sp = (U32 *)&frame->data[0];
  dp = (U32 *)(Tx_Desc[j].Addr & ~3);

  /* Copy frame data to ETH IO buffer. */
  for (i = (frame->length + 3) >> 2; i; i--) {
    *dp++ = *sp++;
  }
  Tx_Desc[j].Size      = frame->length;
  Tx_Desc[j].CtrlStat |= DMA_TX_OWN;
  if (++j == NUM_TX_BUF) j = 0;
  TxBufIndex = j;
  /* Start frame transmission. */
  ETH->DMASR   = DSR_TPSS;
  ETH->DMATPDR = 0;
}


/*--------------------------- interrupt_ethernet ----------------------------*/

void ETH_IRQHandler (void) {
  OS_FRAME *frame;
  U32 i, RxLen;
  U32 *sp,*dp;

  i = RxBufIndex;
  do {
    if (Rx_Desc[i].Stat & DMA_RX_ERROR_MASK) {
      goto rel;
    }
    if ((Rx_Desc[i].Stat & DMA_RX_SEG_MASK) != DMA_RX_SEG_MASK) {
      goto rel;
    }
    RxLen = ((Rx_Desc[i].Stat >> 16) & 0x3FFF) - 4;
    if (RxLen > ETH_MTU) {
      /* Packet too big, ignore it and free buffer. */
      goto rel;
    }
    /* Flag 0x80000000 to skip sys_error() call when out of memory. */
    frame = alloc_mem (RxLen | 0x80000000);
    /* if 'alloc_mem()' has failed, ignore this packet. */
    if (frame != NULL) {
      sp = (U32 *)(Rx_Desc[i].Addr & ~3);
      dp = (U32 *)&frame->data[0];
      for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
        *dp++ = *sp++;
      }
      put_in_queue (frame);
    }
    /* Release this frame from ETH IO buffer. */
rel:Rx_Desc[i].Stat = DMA_RX_OWN;

    if (++i == NUM_RX_BUF) i = 0;
    RxBufIndex = i;  
  }
  while (!(Rx_Desc[i].Stat & DMA_RX_OWN));
  RxBufIndex = i;

  if (ETH->DMASR & INT_RBUIE) {
    /* Receive buffer unavailable, resume DMA */
    ETH->DMASR = INT_RBUIE;
    ETH->DMARPDR = 0;       
  }
  /* Clear pending interrupt bits */
  ETH->DMASR = INT_NISE | INT_RIE;
}


/*--------------------------- rx_descr_init ---------------------------------*/

static void rx_descr_init (void) {
  /* Initialize Receive DMA Descriptor array. */
  U32 i,next;

  RxBufIndex = 0;
  for (i = 0, next = 0; i < NUM_RX_BUF; i++) {
    if (++next == NUM_RX_BUF) next = 0;
    Rx_Desc[i].Stat = DMA_RX_OWN;
    Rx_Desc[i].Ctrl = DMA_RX_RCH | ETH_BUF_SIZE;
    Rx_Desc[i].Addr = (U32)&rx_buf[i];
    Rx_Desc[i].Next = (U32)&Rx_Desc[next];
  }
  ETH->DMARDLAR = (U32)&Rx_Desc[0];
}



/*--------------------------- tx_descr_init ---------------------------------*/

static void tx_descr_init (void) {
  /* Initialize Transmit DMA Descriptor array. */
  U32 i,next;

  TxBufIndex = 0;
  for (i = 0, next = 0; i < NUM_TX_BUF; i++) {
    if (++next == NUM_TX_BUF) next = 0;
    Tx_Desc[i].CtrlStat = DMA_TX_TCH | DMA_TX_LS | DMA_TX_FS;
    Tx_Desc[i].Addr     = (U32)&tx_buf[i];
    Tx_Desc[i].Next     = (U32)&Tx_Desc[next];
  }
  ETH->DMATDLAR = (U32)&Tx_Desc[0];
}


/*--------------------------- write_PHY -------------------------------------*/

static void write_PHY (U32 PhyReg, U16 Value) {
  /* Write a data 'Value' to PHY register 'PhyReg'. */
  U32 tout;

  ETH->MACMIIDR = Value;
  ETH->MACMIIAR = DP83848C_DEF_ADR << 11 | PhyReg << 6 | MMAR_MW | MMAR_MB;

  /* Wait utill operation completed */
  tout = 0;
  for (tout = 0; tout < MII_WR_TOUT; tout++) {
    if ((ETH->MACMIIAR & MMAR_MB) == 0) {
      break;
    }
  }
}


/*--------------------------- read_PHY --------------------------------------*/

static U16 read_PHY (U32 PhyReg) {
  /* Read a PHY register 'PhyReg'. */
  U32 tout;

  ETH->MACMIIAR = DP83848C_DEF_ADR << 11 | PhyReg << 6 | MMAR_MB;

  /* Wait until operation completed */
  tout = 0;
  for (tout = 0; tout < MII_RD_TOUT; tout++) {
    if ((ETH->MACMIIAR & MMAR_MB) == 0) {
      break;
    }
  }
  return (ETH->MACMIIDR & MMDR_MD);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

