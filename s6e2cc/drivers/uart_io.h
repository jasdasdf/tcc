#include <stdio.h>
#include "mcu.h"
#include "arm_math.h"
#include "s6e2cc.h"

#define FALSE       ((boolean_t) 0)
#define TRUE        ((boolean_t) 1)


#define FM_MFS0_UART_BASE                         (0x40038000UL) /* MFS0 Base Address */
#define FM4_MFS0_UART_BASE                        (0x40038000UL) /* MFS0 Base Address */
#define FM_MFS1_UART_BASE                         (0x40038100UL) /* MFS1 Base Address */
#define FM4_MFS1_UART_BASE                        (0x40038100UL) /* MFS1 Base Address */
#define FM_MFS10_UART_BASE                        (0x40038A00UL) /* MFS10 Base Address */
#define FM4_MFS10_UART_BASE                       (0x40038A00UL) /* MFS10 Base Address */
#define FM_MFS11_UART_BASE                        (0x40038B00UL) /* MFS11 Base Address */
#define FM4_MFS11_UART_BASE                       (0x40038B00UL) /* MFS11 Base Address */
#define FM_MFS12_UART_BASE                        (0x40038C00UL) /* MFS12 Base Address */
#define FM4_MFS12_UART_BASE                       (0x40038C00UL) /* MFS12 Base Address */
#define FM_MFS13_UART_BASE                        (0x40038D00UL) /* MFS13 Base Address */
#define FM4_MFS13_UART_BASE                       (0x40038D00UL) /* MFS13 Base Address */
#define FM_MFS14_UART_BASE                        (0x40038E00UL) /* MFS14 Base Address */
#define FM4_MFS14_UART_BASE                       (0x40038E00UL) /* MFS14 Base Address */
#define FM_MFS15_UART_BASE                        (0x40038F00UL) /* MFS15 Base Address */
#define FM4_MFS15_UART_BASE                       (0x40038F00UL) /* MFS15 Base Address */
#define FM_MFS2_UART_BASE                         (0x40038200UL) /* MFS2 Base Address */
#define FM4_MFS2_UART_BASE                        (0x40038200UL) /* MFS2 Base Address */
#define FM_MFS3_UART_BASE                         (0x40038300UL) /* MFS3 Base Address */
#define FM4_MFS3_UART_BASE                        (0x40038300UL) /* MFS3 Base Address */
#define FM_MFS4_UART_BASE                         (0x40038400UL) /* MFS4 Base Address */
#define FM4_MFS4_UART_BASE                        (0x40038400UL) /* MFS4 Base Address */
#define FM_MFS5_UART_BASE                         (0x40038500UL) /* MFS5 Base Address */
#define FM4_MFS5_UART_BASE                        (0x40038500UL) /* MFS5 Base Address */
#define FM_MFS6_UART_BASE                         (0x40038600UL) /* MFS6 Base Address */
#define FM4_MFS6_UART_BASE                        (0x40038600UL) /* MFS6 Base Address */
#define FM_MFS6_UART_BASE                         (0x40038600UL) /* MFS6 Base Address */
#define FM4_MFS6_UART_BASE                        (0x40038600UL) /* MFS6 Base Address */
#define FM_MFS7_UART_BASE                         (0x40038700UL) /* MFS7 Base Address */
#define FM4_MFS7_UART_BASE                        (0x40038700UL) /* MFS7 Base Address */
#define FM_MFS8_UART_BASE                         (0x40038800UL) /* MFS8 Base Address */
#define FM4_MFS8_UART_BASE                        (0x40038800UL) /* MFS8 Base Address */
#define FM_MFS9_UART_BASE                         (0x40038900UL) /* MFS9 Base Address */
#define FM4_MFS9_UART_BASE                        (0x40038900UL) /* MFS9 Base Address */


typedef struct stc_mfs_uart_smr_field
{
    union {
        struct {
            __IO   uint8_t  SOE                  :1;
            __IO   uint8_t  RESERVED7            :1;
            __IO   uint8_t  BDS                  :1;
            __IO   uint8_t  SBL                  :1;
            __IO   uint8_t  RESERVED8            :1;
            __IO   uint8_t  MD                   :3;
        };
        struct {
            __IO   uint8_t  RESERVED9            :5;
            __IO   uint8_t  MD0                  :1;
            __IO   uint8_t  MD1                  :1;
            __IO   uint8_t  MD2                  :1;
        };
    };
} stc_mfs_uart_smr_field_t;

typedef struct stc_mfs_uart_scr_field
{
        __IO   uint8_t  TXE                      :1;
        __IO   uint8_t  RXE                      :1;
        __IO   uint8_t  TBIE                     :1;
        __IO   uint8_t  TIE                      :1;
        __IO   uint8_t  RIE                      :1;
        __IO   uint8_t  RESERVED3                :2;
        __IO   uint8_t  UPCL                     :1;
} stc_mfs_uart_scr_field_t;

typedef struct stc_mfs_uart_escr_field
{
    union {
        struct {
            __IO   uint8_t  L                    :3;
            __IO   uint8_t  P                    :1;
            __IO   uint8_t  PEN                  :1;
            __IO   uint8_t  INV                  :1;
            __IO   uint8_t  ESBL                 :1;
            __IO   uint8_t  FLWEN                :1;
        };
        struct {
            __IO   uint8_t  L0                   :1;
            __IO   uint8_t  L1                   :1;
            __IO   uint8_t  L2                   :1;
            __IO   uint8_t  RESERVED5            :5;
        };
    };
} stc_mfs_uart_escr_field_t;

typedef struct stc_mfs_uart_ssr_field
{
        __IO   uint8_t  TBI                      :1;
        __IO   uint8_t  TDRE                     :1;
        __IO   uint8_t  RDRF                     :1;
        __IO   uint8_t  ORE                      :1;
        __IO   uint8_t  FRE                      :1;
        __IO   uint8_t  PE                       :1;
        __IO   uint8_t  RESERVED5                :1;
        __IO   uint8_t  REC                      :1;
} stc_mfs_uart_ssr_field_t;

typedef struct stc_mfs_uart_bgr_field
{
    union {
        struct {
            __IO  uint16_t  BGR                  :15;
            __IO  uint16_t  EXT                  :1;
        };
        struct {
            __IO  uint16_t  BGR0                 :1;
            __IO  uint16_t  BGR1                 :1;
            __IO  uint16_t  BGR2                 :1;
            __IO  uint16_t  BGR3                 :1;
            __IO  uint16_t  BGR4                 :1;
            __IO  uint16_t  BGR5                 :1;
            __IO  uint16_t  BGR6                 :1;
            __IO  uint16_t  BGR7                 :1;
            __IO  uint16_t  BGR8                 :1;
            __IO  uint16_t  BGR9                 :1;
            __IO  uint16_t  BGR10                :1;
            __IO  uint16_t  BGR11                :1;
            __IO  uint16_t  BGR12                :1;
            __IO  uint16_t  BGR13                :1;
            __IO  uint16_t  BGR14                :1;
            __IO  uint16_t  RESERVED5            :1;
        };
    };
} stc_mfs_uart_bgr_field_t;

typedef struct stc_mfs_uart_fcr_field
{
        __IO  uint16_t  FE1                      :1;
        __IO  uint16_t  FE2                      :1;
        __IO  uint16_t  FCL1                     :1;
        __IO  uint16_t  FCL2                     :1;
        __IO  uint16_t  FSET                     :1;
        __IO  uint16_t  FLD                      :1;
        __IO  uint16_t  FLST                     :1;
        __IO  uint16_t  RESERVED9                :1;
        __IO  uint16_t  FSEL                     :1;
        __IO  uint16_t  FTIE                     :1;
        __IO  uint16_t  FDRQ                     :1;
        __IO  uint16_t  FRIIE                    :1;
        __IO  uint16_t  FLSTE                    :1;
        __IO  uint16_t  RESERVED10               :3;
} stc_mfs_uart_fcr_field_t;

typedef struct stc_mfs_uart_fbyte1_field
{
    union {
        struct {
            __IO   uint8_t  FD                   :8;
        };
        struct {
            __IO   uint8_t  FD0                  :1;
            __IO   uint8_t  FD1                  :1;
            __IO   uint8_t  FD2                  :1;
            __IO   uint8_t  FD3                  :1;
            __IO   uint8_t  FD4                  :1;
            __IO   uint8_t  FD5                  :1;
            __IO   uint8_t  FD6                  :1;
            __IO   uint8_t  FD7                  :1;
        };
    };
} stc_mfs_uart_fbyte1_field_t;

typedef struct stc_mfs_uart_fbyte2_field
{
    union {
        struct {
            __IO   uint8_t  FD                   :8;
        };
        struct {
            __IO   uint8_t  FD0                  :1;
            __IO   uint8_t  FD1                  :1;
            __IO   uint8_t  FD2                  :1;
            __IO   uint8_t  FD3                  :1;
            __IO   uint8_t  FD4                  :1;
            __IO   uint8_t  FD5                  :1;
            __IO   uint8_t  FD6                  :1;
            __IO   uint8_t  FD7                  :1;
        };
    };
} stc_mfs_uart_fbyte2_field_t;


/*******************************************************************************
* MFS_UART_MODULE
*   register structur
*******************************************************************************/
typedef struct
{
    union {
        __IO  uint8_t SMR;
        stc_mfs_uart_smr_field_t SMR_f;
    };
    union {
        __IO  uint8_t SCR;
        stc_mfs_uart_scr_field_t SCR_f;
    };
        __IO  uint8_t RESERVED43[2];
    union {
        __IO  uint8_t ESCR;
        stc_mfs_uart_escr_field_t ESCR_f;
    };
    union {
        __IO  uint8_t SSR;
        stc_mfs_uart_ssr_field_t SSR_f;
    };
        __IO  uint8_t RESERVED44[2];
    union {
        __IO uint16_t RDR;
        stc_mfs_uart_rdr_field_t RDR_f;
        struct {
            __IO  uint8_t RDRL;
            __IO  uint8_t RDRH;
        };
        __IO uint16_t TDR;
        stc_mfs_uart_tdr_field_t TDR_f;
        struct {
            __IO  uint8_t TDRL;
            __IO  uint8_t TDRH;
        };
    };
        __IO  uint8_t RESERVED45[2];
    union {
        __IO uint16_t BGR;
        stc_mfs_uart_bgr_field_t BGR_f;
        struct {
            __IO  uint8_t BGRL;
            __IO  uint8_t BGRH;
        };
    };
        __IO  uint8_t RESERVED46[6];
    union {
        __IO uint16_t FCR;
        stc_mfs_uart_fcr_field_t FCR_f;
        struct {
            __IO  uint8_t FCRL;
            __IO  uint8_t FCRH;
        };
    };
        __IO  uint8_t RESERVED47[2];
    union {
        __IO  uint8_t FBYTE1;
        stc_mfs_uart_fbyte1_field_t FBYTE1_f;
    };
    union {
        __IO  uint8_t FBYTE2;
        stc_mfs_uart_fbyte2_field_t FBYTE2_f;
    };
} FM_MFS_UART_TypeDef, FM4_MFS_UART_TypeDef;

/******************************************************************************
 * Global type definitions
 ******************************************************************************/
#define stc_mfsn_uart_t FM_MFS_UART_TypeDef 

#define UART0       (*((volatile stc_mfsn_uart_t *) FM_MFS0_UART_BASE))
#define UART1       (*((volatile stc_mfsn_uart_t *) FM_MFS1_UART_BASE))
#define UART2       (*((volatile stc_mfsn_uart_t *) FM_MFS2_UART_BASE))
#define UART3       (*((volatile stc_mfsn_uart_t *) FM_MFS3_UART_BASE))
#define UART4       (*((volatile stc_mfsn_uart_t *) FM_MFS4_UART_BASE))
#define UART5       (*((volatile stc_mfsn_uart_t *) FM_MFS5_UART_BASE))
#define UART6       (*((volatile stc_mfsn_uart_t *) FM_MFS6_UART_BASE))
#define UART7       (*((volatile stc_mfsn_uart_t *) FM_MFS7_UART_BASE))
#define UART8       (*((volatile stc_mfsn_uart_t *) FM_MFS8_UART_BASE))
#define UART9       (*((volatile stc_mfsn_uart_t *) FM_MFS9_UART_BASE))
#define UART10      (*((volatile stc_mfsn_uart_t *) FM_MFS10_UART_BASE))
#define UART11      (*((volatile stc_mfsn_uart_t *) FM_MFS11_UART_BASE))
#define UART12      (*((volatile stc_mfsn_uart_t *) FM_MFS12_UART_BASE))
#define UART13      (*((volatile stc_mfsn_uart_t *) FM_MFS13_UART_BASE))
#define UART14      (*((volatile stc_mfsn_uart_t *) FM_MFS14_UART_BASE))
#define UART15      (*((volatile stc_mfsn_uart_t *) FM_MFS15_UART_BASE))
	
// the numbers of enabled MFS's
#define MFS_INSTANCE_COUNT  1u // just the MFS0

#define MFS0_DATA_REG_ADDR   (uint32_t)(&FM_MFS0_UART->TDR)
#define MFS1_DATA_REG_ADDR   (uint32_t)(&FM_MFS1_UART->TDR)
#define MFS2_DATA_REG_ADDR   (uint32_t)(&FM_MFS2_UART->TDR)
#define MFS3_DATA_REG_ADDR   (uint32_t)(&FM_MFS3_UART->TDR)
#define MFS4_DATA_REG_ADDR   (uint32_t)(&FM_MFS4_UART->TDR)
#define MFS5_DATA_REG_ADDR   (uint32_t)(&FM_MFS5_UART->TDR)
#define MFS6_DATA_REG_ADDR   (uint32_t)(&FM_MFS6_UART->TDR)
#define MFS7_DATA_REG_ADDR   (uint32_t)(&FM_MFS7_UART->TDR)
#define MFS8_DATA_REG_ADDR   (uint32_t)(&FM_MFS8_UART->TDR)
#define MFS9_DATA_REG_ADDR   (uint32_t)(&FM_MFS9_UART->TDR)
#define MFS10_DATA_REG_ADDR  (uint32_t)(&FM_MFS10_UART->TDR)
#define MFS11_DATA_REG_ADDR  (uint32_t)(&FM_MFS11_UART->TDR)
#define MFS12_DATA_REG_ADDR  (uint32_t)(&FM_MFS12_UART->TDR)
#define MFS13_DATA_REG_ADDR  (uint32_t)(&FM_MFS13_UART->TDR)
#define MFS14_DATA_REG_ADDR  (uint32_t)(&FM_MFS14_UART->TDR)
#define MFS15_DATA_REG_ADDR  (uint32_t)(&FM_MFS15_UART->TDR) 



/// MFS mode
typedef enum en_mfs_mode
{
    MfsInitMode = 0u,    ///< MFS initial mode
    MfsUartMode = 1u,    ///< MFS UART mode
    MfsCsioMode = 2u,    ///< MFS CSIO mode
    MfsI2cMode  = 3u,    ///< MFS I2C mode
    MfsLinMode  = 4u,    ///< MFS LIN mode
  
}en_mfs_mode_t;


/**
 ******************************************************************************
 ** \brief UART interrupt enable structure
 ******************************************************************************/
typedef struct stc_uart_irq_en
{
    boolean_t bTxIrq;          ///< UART TX interrupt
    boolean_t bRxIrq;          ///< UART RX interrupt
    boolean_t bTxIdleIrq;      ///< UART TX idle interrupt
    boolean_t bTxFifoIrq;      ///< UART TX FIFO interrupt
    
}stc_uart_irq_en_t;

/**
 ******************************************************************************
 ** \brief UART interrupt callback function
 ******************************************************************************/
typedef struct stc_uart_irq_cb
{
    func_ptr_t pfnTxIrqCb;      ///< UART TX interrupt callback function pointer
    func_ptr_t pfnRxIrqCb;      ///< UART RX interrupt callback function pointer
    func_ptr_t pfnTxIdleCb;     ///< UART TX idle interrupt callback function pointer
    func_ptr_t pfnTxFifoIrqCb;  ///< UART TX FIFO interrupt callback function pointer
    
}stc_uart_irq_cb_t;

/**
 ******************************************************************************
 ** \brief UART configuration structure
 ******************************************************************************/


/// MFS common instance structure
typedef struct 
{

    volatile stc_mfsn_uart_t* pstcUartInstance;   ///< Pointer to UART instance



}stc_mfsn_t;

/// MFS module internal data, storing internal information for each enabled MFS instance.
typedef struct stc_mfs_intern_data
{
    en_mfs_mode_t               enMode;                 ///< MFS mode
    union
    {
        func_ptr_t fnMfsInternIntCb[6];
        stc_uart_irq_cb_t stcUartInternIrqCb;           ///< Uart internal interrupt callback function
//        stc_csio_irq_cb_t stcCsioInternIrqCb;           ///< CSIO internal interrupt callback function
//        stc_i2c_irq_cb_t stcI2cInternIrqCb;             ///< I2C internal interrupt callback function
//        stc_lin_irq_cb_t stcLinInternIrqCb;             ///< LIN internal interrupt callback function
    };

} stc_mfs_intern_data_t;

/// MFS instance data type
typedef struct stc_mfs_instance_data
{
    stc_mfsn_t   stcInstance;     ///< pointer to registers of an instance
    stc_mfs_intern_data_t stcInternData;    ///< module internal data of instance
} stc_mfs_instance_data_t;

/**
 ******************************************************************************
 ** \brief Mfs FIFO Number
 ******************************************************************************/
typedef enum en_mfs_fifo
{
    MfsFifo1 = 0u,                   ///< FIFO No.1
    MfsFifo2 = 1u,                   ///< FIFO No.2
} en_mfs_fifo_t;  









/******************************************************************************
 * UART type definitions
 ******************************************************************************/

/**
 ******************************************************************************
 ** \brief MFS UART mode
 ******************************************************************************/
typedef enum en_mfs_uart_mode
{
    UartNormal = 0u,          ///< Normal mode
    UartMulti  = 1u,          ///< Multi-Processor Mode
} en_uart_mode_t;

/**
 ******************************************************************************
 ** \brief UART data length
 ******************************************************************************/
typedef enum en_uart_data_len
{
    UartEightBits      = 0u,      ///<  8 Bit character length
    UartFiveBits       = 1u,      ///<  5 Bit character length
    UartSixBits        = 2u,      ///<  6 Bit character length
    UartSevenBits      = 3u,      ///<  7 Bit character length
    UartNineBits       = 4u,      ///<  9 Bit character length
} en_uart_data_len_t;

/**
 ******************************************************************************
 ** \brief UART parity format
 ******************************************************************************/
typedef enum en_uart_parity
{
    UartParityNone  = 0u,         ///< No parity bit is used.
    UartParityEven  = 2u,         ///< Even parity bit is used.
    UartParityOdd   = 3u,         ///< Odd parity bit is used.
} en_uart_parity_t;

/**
 ******************************************************************************
 ** \brief UART stop bits length
 ******************************************************************************/
typedef enum en_uart_stop_bit
{
    UartOneStopBit    = 0u,       ///< 1 Stop Bit
    UartTwoStopBits   = 1u,       ///< 2 Stop Bits
    UartThreeStopBits = 2u,       ///< 3 Stop Bits
    UartFourStopBits  = 3u,       ///< 4 Stop Bits
} en_uart_stop_bit_t;

/**
 ******************************************************************************
 ** \brief UART data direction
 ******************************************************************************/
typedef enum en_uart_data_dir
{
    UartDataLsbFirst = 0u,       ///< LSB first
    UartDataMsbFirst = 1u,       ///< MSB first
}en_uart_data_dir_t; 

/**
 ******************************************************************************
 ** \brief UART functions
 ******************************************************************************/
typedef enum en_uart_func
{
    UartTx = 0u,         ///< UART TX
    UartRx = 1u,         ///< UART RX
  
}en_uart_func_t;

/**
 ******************************************************************************
 ** \brief UART interrupt selection
 ******************************************************************************/
typedef enum en_uart_irq_sel
{
    UartTxIrq       = 0u,          ///< UART TX interrupt
    UartRxIrq       = 1u,          ///< UART RX interrupt
    UartTxIdleIrq   = 2u,          ///< UART TX idle interrupt
    UartTxFifoIrq   = 3u,          ///< UART TX FIFO interrupt
    
}en_uart_irq_sel_t;

/**
 ******************************************************************************
 ** \brief UART status types
 ******************************************************************************/
typedef enum en_uart_status
{
    UartParityError     = 0u,   ///< Parity error
    UartFrameError      = 1u,   ///< Frame error
    UartOverrunError    = 2u,   ///< Overrun error
    UartRxFull          = 3u,   ///< RX completion
    UartTxEmpty         = 4u,   ///< TX buffer empty
    UartTxIdle          = 5u,   ///< TX idle
    UartTxFifoRequest   = 6u,   ///< TX FIFO request
  
}en_uart_status_t;

/**
 ******************************************************************************
 ** \brief Mfs FIFO Selection
 ******************************************************************************/
typedef enum en_mfs_fifo_sel
{
    MfsTxFifo1RxFifo2 = 0u,          ///< Transmit FIFO:FIFO1, Received FIFO:FIFO2
    MfsTxFifo2RxFifo1 = 1u,          ///< Transmit FIFO:FIFO2, Received FIFO:FIFO1
} en_mfs_fifo_sel_t;

/**
 ******************************************************************************
 ** \brief Mfs FIFO configuration.
 ******************************************************************************/
typedef struct stc_mfs_fifo_config
{
    en_mfs_fifo_sel_t enFifoSel;    ///< FIFO selection, see #en_mfs_fifo_sel_t for details
    uint8_t u8ByteCount1;           ///< Transfer data count for FIFO1
    uint8_t u8ByteCount2;           ///< Transfer data count for FIFO2
} stc_mfs_fifo_config_t;

typedef struct stc_mfs_uart_config
{
    en_uart_mode_t      enMode;           ///< UART mode
    uint32_t            u32BaudRate;      ///< Baud rate (bps)
    en_uart_parity_t    enParity;         ///< Parity format
    en_uart_stop_bit_t  enStopBit;        ///< Stop bit
    en_uart_data_len_t  enDataLength;     ///< 5..9 Bit Character Length
    en_uart_data_dir_t  enBitDirection;   ///< UART data direction
    boolean_t bInvertData;                ///< FALSE: NRZ, TRUE : Inverted NRZ
    boolean_t bHwFlow;                    ///< FALSE: Not use Hardware Flow, TRUE : Use Hardware Flow
    boolean_t bUseExtClk;                 ///< FALSE: use internal clock, TRUE: use external clock which input via SCK pin
    
    stc_mfs_fifo_config_t* pstcFifoConfig;  ///< Pointer to FIFO configuration structure, if set to NULL, FIFO function will not be enabled.
   
} stc_mfs_uart_config_t;

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/
/// Enumeration to define an index for each enabled MFS instance
typedef enum en_mfs_instance_index
{  
    MfsInstanceIndexMfs0,
 
    MfsInstanceIndexMax,
    MfsInstanceIndexUnknown = 0xFFu,
    
} en_mfs_instance_index_t;

void Uart_Io_Init(void);
int fputc(int ch, FILE *f);
//int fgetc(FILE *f);


void uart_printf(char *ch);
void uart_scanf(char *ch);


/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
en_result_t Mfs_Uart_Init(volatile stc_mfsn_uart_t*  pstcUart,
                          const stc_mfs_uart_config_t* pstcConfig);
en_result_t Mfs_Uart_DeInit(volatile stc_mfsn_uart_t* pstcUart, boolean_t bTouchNvic);
// Baud rate
en_result_t Mfs_Uart_SetBaudRate(volatile stc_mfsn_uart_t* pstcUart,
                                 uint32_t u32BaudRate);
// Function enable/disable
en_result_t Mfs_Uart_EnableFunc(volatile stc_mfsn_uart_t* pstcUart, en_uart_func_t enFunc);
en_result_t Mfs_Uart_DisableFunc(volatile stc_mfsn_uart_t* pstcUart, en_uart_func_t enFunc);
// Status read/clear
boolean_t Mfs_Uart_GetStatus(volatile stc_mfsn_uart_t* pstcUart, 
                             en_uart_status_t enStatus);
en_result_t Mfs_Uart_ClrStatus(volatile stc_mfsn_uart_t* pstcUart,
                               en_uart_status_t enStatus);
// Data read/write
en_result_t Mfs_Uart_SendData(volatile stc_mfsn_uart_t* pstcUart, uint16_t Data);
uint16_t Mfs_Uart_ReceiveData(volatile stc_mfsn_uart_t* pstcUart);
// FIFO 
en_result_t Mfs_Uart_ResetFifo (volatile stc_mfsn_uart_t* pstcUart, 
                                en_mfs_fifo_t enFifo);
en_result_t Mfs_Uart_SetFifoCount(volatile stc_mfsn_uart_t* pstcUart,
                                  en_mfs_fifo_t enFifo,
                                  uint8_t u8Count);
uint8_t Mfs_Uart_GetFifoCount(volatile stc_mfsn_uart_t* pstcUart,
                              en_mfs_fifo_t enFifo);

