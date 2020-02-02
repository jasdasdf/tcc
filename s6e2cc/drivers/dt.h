// dual timer (importado da biblioteca pdl)
#include "mcu.h"
#include "s6e2cc.h"
#include "arm_math.h"

#ifndef TRUE
  #define TRUE 1 
#endif

#ifndef FALSE
  #define FALSE  0
#endif  


typedef struct stc_dt_timer2control_field
{
    union {
        struct {
            __IO  uint32_t  ONESHOT              :1;
            __IO  uint32_t  TIMERSIZE            :1;
            __IO  uint32_t  TIMERPRE             :2;
            __IO  uint32_t  RESERVED1            :1;
            __IO  uint32_t  INTENABLE            :1;
            __IO  uint32_t  TIMERMODE            :1;
            __IO  uint32_t  TIMEREN              :1;
            __IO  uint32_t  RESERVED2            :24;
        };
        struct {
            __IO  uint32_t  RESERVED0            :2;
            __IO  uint32_t  TIMERPRE0            :1;
            __IO  uint32_t  TIMERPRE1            :1;
            __IO  uint32_t  RESERVED3            :28;
        };
    };
} stc_dt_timer2control_field_t;

typedef struct stc_dt_timer1mis_field
{
        __IO  uint32_t  TIMER1MIS                :1;
        __IO  uint32_t  RESERVED0                :31;
} stc_dt_timer1mis_field_t;

typedef struct stc_dt_timer2ris_field
{
        __IO  uint32_t  TIMER2RIS                :1;
        __IO  uint32_t  RESERVED0                :31;
} stc_dt_timer2ris_field_t;


typedef struct stc_dt_timer2mis_field
{
        __IO  uint32_t  TIMER2MIS                :1;
        __IO  uint32_t  RESERVED0                :31;
} stc_dt_timer2mis_field_t;

typedef struct stc_dt_timer1ris_field
{
        __IO  uint32_t  TIMER1RIS                :1;
        __IO  uint32_t  RESERVED0                :31;
} stc_dt_timer1ris_field_t;

/*******************************************************************************
* DT_MODULE
*******************************************************************************/
typedef struct stc_dt_timer1control_field
{
    union {
        struct {
            __IO  uint32_t  ONESHOT              :1;
            __IO  uint32_t  TIMERSIZE            :1;
            __IO  uint32_t  TIMERPRE             :2;
            __IO  uint32_t  RESERVED1            :1;
            __IO  uint32_t  INTENABLE            :1;
            __IO  uint32_t  TIMERMODE            :1;
            __IO  uint32_t  TIMEREN              :1;
            __IO  uint32_t  RESERVED2            :24;
        };
        struct {
            __IO  uint32_t  RESERVED0            :2;
            __IO  uint32_t  TIMERPRE0            :1;
            __IO  uint32_t  TIMERPRE1            :1;
            __IO  uint32_t  RESERVED3            :28;
        };
    };
} stc_dt_timer1control_field_t;

/*
*******************************************************************************
* DT_MODULE
*   register structur
*******************************************************************************/
typedef struct
{
    union {
        __IO uint32_t TIMER1LOAD;
        struct {
          union {
            __IO uint16_t TIMER1LOADL;
            struct {
              __IO uint8_t TIMER1LOADLL;
              __IO uint8_t TIMER1LOADLH;
            };
          };
          union {
            __IO uint16_t TIMER1LOADH;
            struct {
              __IO uint8_t TIMER1LOADHL;
              __IO uint8_t TIMER1LOADHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER1VALUE;
        struct {
          union {
            __IO uint16_t TIMER1VALUEL;
            struct {
              __IO uint8_t TIMER1VALUELL;
              __IO uint8_t TIMER1VALUELH;
            };
          };
          union {
            __IO uint16_t TIMER1VALUEH;
            struct {
              __IO uint8_t TIMER1VALUEHL;
              __IO uint8_t TIMER1VALUEHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER1CONTROL;
        stc_dt_timer1control_field_t TIMER1CONTROL_f;
        struct {
          union {
            __IO uint16_t TIMER1CONTROLL;
            struct {
              __IO uint8_t TIMER1CONTROLLL;
              __IO uint8_t TIMER1CONTROLLH;
            };
          };
          union {
            __IO uint16_t TIMER1CONTROLH;
            struct {
              __IO uint8_t TIMER1CONTROLHL;
              __IO uint8_t TIMER1CONTROLHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER1INTCLR;
        struct {
          union {
            __IO uint16_t TIMER1INTCLRL;
            struct {
              __IO uint8_t TIMER1INTCLRLL;
              __IO uint8_t TIMER1INTCLRLH;
            };
          };
          union {
            __IO uint16_t TIMER1INTCLRH;
            struct {
              __IO uint8_t TIMER1INTCLRHL;
              __IO uint8_t TIMER1INTCLRHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER1RIS;
        stc_dt_timer1ris_field_t TIMER1RIS_f;
        struct {
          union {
            __IO uint16_t TIMER1RISL;
            struct {
              __IO uint8_t TIMER1RISLL;
              __IO uint8_t TIMER1RISLH;
            };
          };
          union {
            __IO uint16_t TIMER1RISH;
            struct {
              __IO uint8_t TIMER1RISHL;
              __IO uint8_t TIMER1RISHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER1MIS;
        stc_dt_timer1mis_field_t TIMER1MIS_f;
        struct {
          union {
            __IO uint16_t TIMER1MISL;
            struct {
              __IO uint8_t TIMER1MISLL;
              __IO uint8_t TIMER1MISLH;
            };
          };
          union {
            __IO uint16_t TIMER1MISH;
            struct {
              __IO uint8_t TIMER1MISHL;
              __IO uint8_t TIMER1MISHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER1BGLOAD;
        struct {
          union {
            __IO uint16_t TIMER1BGLOADL;
            struct {
              __IO uint8_t TIMER1BGLOADLL;
              __IO uint8_t TIMER1BGLOADLH;
            };
          };
          union {
            __IO uint16_t TIMER1BGLOADH;
            struct {
              __IO uint8_t TIMER1BGLOADHL;
              __IO uint8_t TIMER1BGLOADHH;
            };
          };
        };
    };
        __IO  uint8_t RESERVED0[4];
    union {
        __IO uint32_t TIMER2LOAD;
        struct {
          union {
            __IO uint16_t TIMER2LOADL;
            struct {
              __IO uint8_t TIMER2LOADLL;
              __IO uint8_t TIMER2LOADLH;
            };
          };
          union {
            __IO uint16_t TIMER2LOADH;
            struct {
              __IO uint8_t TIMER2LOADHL;
              __IO uint8_t TIMER2LOADHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER2VALUE;
        struct {
          union {
            __IO uint16_t TIMER2VALUEL;
            struct {
              __IO uint8_t TIMER2VALUELL;
              __IO uint8_t TIMER2VALUELH;
            };
          };
          union {
            __IO uint16_t TIMER2VALUEH;
            struct {
              __IO uint8_t TIMER2VALUEHL;
              __IO uint8_t TIMER2VALUEHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER2CONTROL;
        stc_dt_timer2control_field_t TIMER2CONTROL_f;
        struct {
          union {
            __IO uint16_t TIMER2CONTROLL;
            struct {
              __IO uint8_t TIMER2CONTROLLL;
              __IO uint8_t TIMER2CONTROLLH;
            };
          };
          union {
            __IO uint16_t TIMER2CONTROLH;
            struct {
              __IO uint8_t TIMER2CONTROLHL;
              __IO uint8_t TIMER2CONTROLHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER2INTCLR;
        struct {
          union {
            __IO uint16_t TIMER2INTCLRL;
            struct {
              __IO uint8_t TIMER2INTCLRLL;
              __IO uint8_t TIMER2INTCLRLH;
            };
          };
          union {
            __IO uint16_t TIMER2INTCLRH;
            struct {
              __IO uint8_t TIMER2INTCLRHL;
              __IO uint8_t TIMER2INTCLRHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER2RIS;
        stc_dt_timer2ris_field_t TIMER2RIS_f;
        struct {
          union {
            __IO uint16_t TIMER2RISL;
            struct {
              __IO uint8_t TIMER2RISLL;
              __IO uint8_t TIMER2RISLH;
            };
          };
          union {
            __IO uint16_t TIMER2RISH;
            struct {
              __IO uint8_t TIMER2RISHL;
              __IO uint8_t TIMER2RISHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER2MIS;
        stc_dt_timer2mis_field_t TIMER2MIS_f;
        struct {
          union {
            __IO uint16_t TIMER2MISL;
            struct {
              __IO uint8_t TIMER2MISLL;
              __IO uint8_t TIMER2MISLH;
            };
          };
          union {
            __IO uint16_t TIMER2MISH;
            struct {
              __IO uint8_t TIMER2MISHL;
              __IO uint8_t TIMER2MISHH;
            };
          };
        };
    };
    union {
        __IO uint32_t TIMER2BGLOAD;
        struct {
          union {
            __IO uint16_t TIMER2BGLOADL;
            struct {
              __IO uint8_t TIMER2BGLOADLL;
              __IO uint8_t TIMER2BGLOADLH;
            };
          };
          union {
            __IO uint16_t TIMER2BGLOADH;
            struct {
              __IO uint8_t TIMER2BGLOADHL;
              __IO uint8_t TIMER2BGLOADHH;
            };
          };
        };
    };
} FM_DT_TypeDef, FM4_DT_TypeDef;



#define FM4_DT_BASE                               (0x40015000UL) /* DT Base Address */
#define FM_DT_BASE                                (0x40015000UL) /* DT Base Address */


/*******************************************************************************
* Global definitions
*******************************************************************************/
#define DT0     (*((volatile stc_dtn_t *) FM_DT_BASE))

/* for TimerPre of TimerXControl */
#define DT_PRE_TIMER_DIV_1      (0x00)
#define DT_PRE_TIMER_DIV_16     (0x01)
#define DT_PRE_TIMER_DIV_256    (0x02)

/** \} GroupDT_Macros */

/**
* \addtogroup GroupDT_Types
* \{
*/

/******************************************************************************
 * Global type definitions
 ******************************************************************************/
 
 /**
 ******************************************************************************
 ** \brief Dt mode
 ** 
 ** To select between Free-run, Periodic, and One-Shot mode
 ******************************************************************************/
typedef enum en_dt_mode
{
    DtFreeRun      = 0u,  ///< Free-running mode
    DtPeriodic     = 1u,  ///< Periodic mode
    DtOneShot      = 2u   ///< One-shot mode
} en_dt_mode_t;

/**
 ******************************************************************************
 ** \brief Dt Prescaler
 ** 
 ** To select clock divider
 ******************************************************************************/
typedef enum en_dt_prescaler
{
    DtPrescalerDiv1     = 0u,  ///< Prescaler divisor 1
    DtPrescalerDiv16    = 1u,  ///< Prescaler divisor 16
    DtPrescalerDiv256   = 2u   ///< Prescaler divisor 256
} en_dt_prescaler_t;

/**
 ******************************************************************************
 ** \brief Dt Counter Size
 ** 
 ** To select the size of the counter
 ******************************************************************************/
typedef enum en_dt_countersize
{
    DtCounterSize16     = 0u,  ///< 16 Bit counter size
    DtCounterSize32     = 1u   ///< 32 Bit counter size
} en_dt_countersize_t;

/**
 ******************************************************************************
 ** \brief Dt channel number
 ******************************************************************************/
typedef enum en_dt_channel
{
    DtChannel0     = 0u,  ///< channel 0
    DtChannel1     = 1u,  ///< channel 1
    DtMaxChannels  = 2u   ///< Number of channels
} en_dt_channel_t;
/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/// Enumeration to define an index for each enabled Dual timer instance
typedef enum en_dt_instance_index
{
#if (PDL_PERIPHERAL_ENABLE_DT0 == PDL_ON)
    DtInstanceIndexDt0 = 0u,
#endif
    DtInstanceIndexMax
} en_dt_instance_index_t;

/** \} GroupDT_Types */

/**
* \addtogroup GroupDT_DataStructures
* \{
*/

/**
 ******************************************************************************
 ** \brief Redefinition of DT register structure 
 ******************************************************************************/
typedef FM_DT_TypeDef stc_dtn_t;

/**
 *****************************************************************************
 ** \brief Dt channel configuration
 ** 
 ** The DT configuration is done on a per channel basis
 *****************************************************************************/
typedef struct stc_dt_channel_config
{
    uint8_t u8Mode;           ///< Mode, see description of #en_dt_mode_t
    uint8_t u8PrescalerDiv;   ///< Prescaler, see description of #en_dt_prescaler_t
    uint8_t u8CounterSize;    ///< Counter size, see description of #en_dt_countersize_t
#if (PDL_INTERRUPT_ENABLE_DT0 == PDL_ON)    
    boolean_t  bIrqEnable;      ///< TRUE: enable interrupt, FALSE: disable interrupt
    func_ptr_t pfnIrqCallback;  ///< Pointer to interrupt callback function
    boolean_t  bTouchNvic;      ///< TRUE: enable MVIC, FALSE: don't enable NVIC
#endif    
} stc_dt_channel_config_t;

/// Datatype for holding internal data needed for DT
typedef struct stc_dt_intern_data
{
    /// Callback for interrupts of DT channel 0
    func_ptr_t  pfnIrqCallbackIntern[DtMaxChannels] ;
} stc_dt_intern_data_t ;

/// DT instance data type
typedef struct stc_dt_instance_data
{
    volatile stc_dtn_t*  pstcInstance;  ///< pointer to registers of an instance
    stc_dt_intern_data_t stcInternData; ///< module internal data of instance
} stc_dt_instance_data_t;

/** \} GroupDT_DataStructures */

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/

/**
* \addtogroup GroupDT_Functions
* \{
*/

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
#if (PDL_INTERRUPT_ENABLE_DT0 == PDL_ON)
  /* External function  */
  void DtIrqHandler1(void);
  void DtIrqHandler2(void);
  #define DtIrqHandler(u8Channel)      (((u8Channel) == (0)) ? ( DtIrqHandler1() ) : ( DtIrqHandler2() ))
  en_result_t Dt_EnableIrq (uint8_t u8Channel);
  en_result_t Dt_DisableIrq(uint8_t u8Channel);
#endif

/* Init/Deinit */
en_result_t Dt_Init(const stc_dt_channel_config_t* pstcConfig,
                    uint8_t u8Channel);
en_result_t Dt_DeInit(uint8_t u8Channel, boolean_t bTouchNvic);

/* Function Enable/Disable */
en_result_t Dt_EnableCount(uint8_t u8Channel);
en_result_t Dt_DisableCount(uint8_t u8Channel);

/* Get/Clr irq flag */
boolean_t Dt_GetIrqFlag(uint8_t u8Channel);
boolean_t Dt_GetMaskIrqFlag(uint8_t u8Channel);
en_result_t Dt_ClrIrqFlag(uint8_t u8Channel);

/* Write/Read count value */
en_result_t Dt_WriteLoadVal(uint32_t u32LoadVal,
                            uint8_t  u8Channel);
en_result_t Dt_WriteBgLoadVal(uint32_t u32BgLoadVal,
                              uint8_t  u8Channel);
uint32_t Dt_ReadCurCntVal(uint8_t u8Channel);


/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/

/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/
/* Configuration for channel 0 */
static const stc_dt_channel_config_t stcDtChannelConfig0 = {
    DtPeriodic,         /* Periodic mode */
    DtPrescalerDiv256,  /* Prescaler dividor f/256 */
    DtCounterSize32     /* 32bits counter size */
};

/* Configuration for channel 1 */
static const stc_dt_channel_config_t stcDtChannelConfig1 = {
    DtOneShot,          /* One-shot mode */
    DtPrescalerDiv256,  /* Prescaler dividor f/256 */
    DtCounterSize32     /* 32bits counter size */
};

int32_t my_dt(void);
