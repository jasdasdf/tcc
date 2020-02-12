#include "uart_io.h"



/// Look-up table for all enabled MFS instances and their internal data
stc_mfs_instance_data_t m_astcMfsInstanceDataLut[MFS_INSTANCE_COUNT] =
{
	{	
		{
				&UART0, 

		}, // pstcInstance 
		{
				MfsInitMode,            // MFS initial mode 
				{{0u}},                 // stcInternData (not initialized yet) 
		} 
	}, 
};



volatile stc_mfsn_uart_t* UartCh = &UART0;



#define UartInstanceToIndex(Instance) ((uint32_t)Instance - (uint32_t)&UART0)/0x100u

/**
 ******************************************************************************
 ** \brief Return the internal data for a certain UART instance.
 **
 ** \param pstcUart Pointer to UART instance
 **
 ** \return Pointer to internal data or NULL if instance is not enabled (or not known)
 **
 ******************************************************************************/
static stc_mfs_intern_data_t* MfsUartGetInternDataPtr(volatile stc_mfsn_uart_t* pstcUart)
{
    stc_mfs_intern_data_t* pstcInternDataPtr = NULL;
    uint32_t u32Instance;

    if (NULL != pstcUart)
    {
        for (u32Instance = 0u; u32Instance < (uint32_t)MfsInstanceIndexMax; u32Instance++)
        {
            if (pstcUart == m_astcMfsInstanceDataLut[u32Instance].stcInstance.pstcUartInstance)
            {
                pstcInternDataPtr = &m_astcMfsInstanceDataLut[u32Instance].stcInternData;
                break;
            }
        }
            
    }

    return (pstcInternDataPtr);
} /* MfsGetInternDataPtr */

/**
 ******************************************************************************
 ** \brief Enable UART functions
 **
 ** \param [in] pstcUart   Pointer to UART instance   
 ** \param [in] enFunc     UART function types
 ** \arg   UartTx    UART transfer function
 ** \arg   UartRx    UART receive function
 ** 
 ** \retval Ok                    Function has been enabled normally
 ** \retval ErrorInvalidParameter If one of following conditions are met:
 **                               - pstcUart == NULL
 **                               - Other invalid configuration                   
 ** 
 ******************************************************************************/
en_result_t Mfs_Uart_EnableFunc(volatile stc_mfsn_uart_t* pstcUart, en_uart_func_t enFunc)
{
    if (NULL == pstcUart)
    {
        return ErrorInvalidParameter;
    }
    
    switch(enFunc)
    {
        case UartTx:
            pstcUart->SCR_f.TXE = 1u;
            break;
        case UartRx:
            pstcUart->SCR_f.RXE = 1u;
            break;
        default:
            return ErrorInvalidParameter;
    }
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief Get status of UART according to status type
 **
 ** \param [in] pstcUart     Pointer to UART instance   
 ** \param [in] enStatus     UART status type
 ** \arg   UartParityError   UART parity error
 ** \arg   UartFrameError    UART frame error
 ** \arg   UartRxFull        UART receive buffer full
 ** \arg   UartTxEmpty       UART tranfer buffer empty
 ** \arg   UartTxIdle        UART tranfer idle status
 ** \arg   UartTxFifoRequest UART transfer FIFO request
 ** 
 ** \retval FALSE                 If one of following conditions are met:
 **                               - No UART parity error [enStatus = UartParityError]
 **                               - No UART frame error  [enStatus = UartFrameError]     
 **                               - UART receive buffer is not full [enStatus = UartRxFull]
 **                               - UART tranfer buffer is not empty [enStatus = UartTxEmpty]
 **                               - UART tranfer is on-going [enStatus = UartTxIdle]
 **                               - No UART transfer FIFO request [enStatus = UartTxFifoRequest] 
 ** \retval TRUE                 If one of following conditions are met:
 **                               - UART parity error occurs [enStatus = UartParityError]
 **                               - UART frame error occurs  [enStatus = UartFrameError]    
 **                               - UART receive buffer is full [enStatus = UartRxFull]
 **                               - UART tranfer buffer is empty [enStatus = UartTxEmpty]
 **                               - UART tranfer is idle [enStatus = UartTxIdle]
 **                               - UART transfer FIFO request issues [enStatus = UartTxFifoRequest] 
 ** 
 ******************************************************************************/
boolean_t Mfs_Uart_GetStatus(volatile stc_mfsn_uart_t* pstcUart, 
                             en_uart_status_t enStatus)
{
    boolean_t bResult = FALSE;
    
    switch(enStatus)
    {
        case UartParityError:
            bResult = (pstcUart->SSR_f.PE == 1u) ? TRUE : FALSE;
            break;  
        case UartFrameError:
            bResult = (pstcUart->SSR_f.FRE == 1u) ? TRUE : FALSE;
            break;  
        case UartOverrunError:
            bResult = (pstcUart->SSR_f.ORE == 1u) ? TRUE : FALSE;
            break;  
        case UartRxFull:
            bResult = (pstcUart->SSR_f.RDRF == 1u) ? TRUE : FALSE;
            break;  
        case UartTxEmpty:
            bResult = (pstcUart->SSR_f.TDRE == 1u) ? TRUE : FALSE;
            break;  
        case UartTxIdle:
            bResult = (pstcUart->SSR_f.TBI == 1u) ? TRUE : FALSE;
            break;  
        case UartTxFifoRequest:
            bResult = (pstcUart->FCR_f.FDRQ == 1u) ? TRUE : FALSE;
            break;  
        default:
            break;  
    }
    
    return bResult;
}

/**
 ******************************************************************************
 ** \brief Write UART data buffer
 **
 ** \param [in] pstcUart   Pointer to UART instance   
 ** \param [in] u16Data    Send data
 ** 
 ** \retval Ok                    Data has been successfully sent
 ** \retval ErrorInvalidParameter If one of following conditions are met:
 **                               - pstcUart == NULL           
 ** 
 ******************************************************************************/
en_result_t Mfs_Uart_SendData(volatile stc_mfsn_uart_t* pstcUart, uint16_t u16Data)
{
    if (NULL == pstcUart)
    {
        return ErrorInvalidParameter;
    }
    
    pstcUart->TDR = u16Data;
    
    return Ok;
}


/**
 ******************************************************************************
 ** \brief Set the baudrate of UART
 **
 ** \param [in] pstcUart    Pointer to UART instance   
 ** \param [in] u32BaudRate Baudrate value [bps]
 ** 
 ** \retval Ok                    UART baud rate has been successfully modified
 ** \retval ErrorInvalidParameter If one of following conditions are met:
 **                               - pstcUart == NULL
 **                               - u32BaudRate < 4         
 ** \note
 ** The UART baud rate can be initialized in the Mfs_Uart_Init() and be modified
 ** in the funciton.
 ** 
 ******************************************************************************/
en_result_t Mfs_Uart_SetBaudRate(volatile stc_mfsn_uart_t* pstcUart,
                                 uint32_t u32BaudRate)
{
    uint32_t u32Pclk1;
	
    SystemCoreClockUpdate();  
    if ((NULL == pstcUart) || (u32BaudRate < 4u))
    {
        return ErrorInvalidParameter;
    }
    
    u32Pclk1 = SystemCoreClock / (1ul << (FM4_CRG->APBC2_PSR & 0x03u)); /* MFS is attached on APB2 bus in FM3, FM4 device */
  
    pstcUart->BGR_f.BGR = (u32Pclk1/u32BaudRate) - 1;
    
    return Ok;
}


en_result_t Mfs_Uart_Init(volatile stc_mfsn_uart_t*         pstcUart,
                          const stc_mfs_uart_config_t* pstcConfig)
{
    stc_mfs_intern_data_t* pstcMfsInternData;
    uint8_t u8Ch;
    
    /* Preset local register variables to zero */
    stc_mfs_uart_smr_field_t  stcSMR;
    stc_mfs_uart_scr_field_t  stcSCR;
    stc_mfs_uart_escr_field_t stcESCR;
	
		/* Check for valid pointer and get pointer to internal data struct ... */
    pstcMfsInternData = MfsUartGetInternDataPtr(pstcUart);
    
    /* Parameter check and get ptr to internal data struct */
    if ((NULL == pstcMfsInternData)
    ||  (NULL == pstcConfig)
       )
    {
        return (ErrorInvalidParameter);
    }
		
		/* Calculate the channel number */
    u8Ch = UartInstanceToIndex(pstcUart);
		
		if(u8Ch > 15u)
    {
        return (ErrorInvalidParameter);
    }
    
    /* Set the MFS mode in the internal structure */
    pstcMfsInternData->enMode = MfsUartMode;
    
    /* Set UART mode */
    switch (pstcConfig->enMode)
    {
        case UartNormal:
            stcSMR.MD = 0u;   /* Normal mode */
            break;
        case UartMulti:
            stcSMR.MD = 1u;    /* Multi-processor mode */
            break;
        default:
            return (ErrorInvalidParameter);
    }

    /* Enable SOE */
    stcSMR.SOE = TRUE;
    /* Set Parity */
    switch(pstcConfig->enParity)
    {
        case UartParityNone:
            stcESCR.P   = FALSE; /* Parity type selection (0-even/1-odd) */
            stcESCR.PEN = FALSE; /* Parity disable */
            break;
        case UartParityEven:
            stcESCR.P   = FALSE; /* Parity type selection (0-even/1-odd) */
            stcESCR.PEN = TRUE;  /* Parity Enable */
            break;
        case UartParityOdd:
            stcESCR.P   = TRUE;  /* Parity type selection (0-even/1-odd) */
            stcESCR.PEN = TRUE;  /* Parity Enable */
            break;
        default:
            return (ErrorInvalidParameter);
    }
    /* Set Stop bit length */
    switch (pstcConfig->enStopBit)
    {
        case UartOneStopBit:
            stcSMR.SBL   = FALSE;
            stcESCR.ESBL = FALSE;
            break;
        case UartTwoStopBits:
            stcSMR.SBL   = TRUE;
            stcESCR.ESBL = FALSE;
            break;     
        case UartThreeStopBits:
            stcSMR.SBL   = FALSE;
            stcESCR.ESBL = TRUE;
            break;
        case UartFourStopBits:
            stcSMR.SBL   = TRUE;
            stcESCR.ESBL = TRUE;
            break;
        default:
            return (ErrorInvalidParameter);
    }    
    /* Set Data bit length (5 - 9bits) */
    switch(pstcConfig->enDataLength)
    {
        case UartFiveBits:
            stcESCR.L = UartFiveBits;
            break;
        case UartSixBits:
            stcESCR.L = UartSixBits;
            break;
        case UartSevenBits:
            stcESCR.L = UartSevenBits;
            break;   
        case UartEightBits:
            stcESCR.L = UartEightBits;
            break;
        case UartNineBits:
            stcESCR.L = UartNineBits;
            break;
        default:
            return (ErrorInvalidParameter);
    }
    /* Set Bit direction (LSB/MSB) */    
    switch(pstcConfig->enBitDirection)
    {
        case UartDataLsbFirst:
            stcSMR.BDS = 0;
            break;
        case UartDataMsbFirst:
            stcSMR.BDS = 1;
            break;  
        default:
            return ErrorInvalidParameter;  
    }

    /* HW Flow */
    if (TRUE == pstcConfig->bHwFlow)
    {
        stcESCR.FLWEN = TRUE;
    }
    else
    {
        stcESCR.FLWEN = FALSE;
    }

    /* Set Signal system (NRZ/Int-NRZ) */
    if (TRUE == pstcConfig->bInvertData)
    {
        stcESCR.INV = TRUE;
    }
    else
    {
        stcESCR.INV = FALSE;
    }
    
    
    /* Clear MFS by setting the Software Reset bit */
    pstcUart->SCR_f.UPCL = TRUE;

    /* Set Baudrate */
    (void)Mfs_Uart_SetBaudRate(pstcUart, pstcConfig->u32BaudRate);
    /* Set registers value */
    pstcUart->SMR_f  = stcSMR;
    pstcUart->SCR_f  = stcSCR;
    pstcUart->ESCR_f = stcESCR;
    
    /* Set external clock */
    pstcUart->BGR_f.EXT = ((pstcConfig->bUseExtClk == TRUE) ? 1u : 0u);
    
    /* Configue FIFO */
    if(pstcConfig->pstcFifoConfig != NULL)
    {
        /* Reset FIFO */
        pstcUart->FCR_f.FCL1 = 1;
        pstcUart->FCR_f.FCL2 = 1;
        /* Enable FIFO receive Idle detection */
        pstcUart->FCR_f.FRIIE = 1u;
        /* Selection TX and RX FIFO  */
        switch(pstcConfig->pstcFifoConfig->enFifoSel)
        {
            case MfsTxFifo1RxFifo2:
                pstcUart->FCR_f.FSEL = 0u;
                break;
            case MfsTxFifo2RxFifo1:
                pstcUart->FCR_f.FSEL = 1u;
                break;
            default:
               return (ErrorInvalidParameter);
        }
        /* Set FIFO count */
        pstcUart->FBYTE1 = pstcConfig->pstcFifoConfig->u8ByteCount1;
        pstcUart->FBYTE2 = pstcConfig->pstcFifoConfig->u8ByteCount2;
        /* Enable FIFO  */
        pstcUart->FCR_f.FE1 = 1u;
        pstcUart->FCR_f.FE2 = 1u;
    }
		
		return (Ok);
	
}

#define PINRELOC_SET_EPFR(epfr,pos,width,value)    \
          ((epfr) = ((epfr) & ~(((1u<<(width))-1u)<<(pos))) | \
          ((value) << (pos)))

#define SetPinFunc_SOT0_0(dummy)        do{ \
                                            bFM4_GPIO_ADE_AN31=0u; \
                                            PINRELOC_SET_EPFR( FM4_GPIO->EPFR07, 6u, 2u,  1u ); \
                                            bFM4_GPIO_PFR2_P2 = 1u; \
                                        }while (0u)

#define SetPinFunc_SIN0_0(dummy)        do{ \
                                            PINRELOC_SET_EPFR( FM4_GPIO->EPFR07, 4u, 2u,  1u ); \
                                            bFM4_GPIO_PFR2_P1 = 1u; \
                                        }while (0u)
																				
void Uart_Io_Init(void){

		stc_mfs_uart_config_t stcUartConfig;

		stcUartConfig.enMode						= UartNormal;
		stcUartConfig.u32BaudRate				= 115200;
		stcUartConfig.enDataLength			= UartEightBits;
		stcUartConfig.enParity					= UartParityNone;
		stcUartConfig.enStopBit					= UartOneStopBit;
		stcUartConfig.enBitDirection		= UartDataLsbFirst;
		stcUartConfig.bInvertData				= FALSE;
		stcUartConfig.bHwFlow						= FALSE;
		stcUartConfig.pstcFifoConfig		= NULL;
	
	
		Mfs_Uart_Init(UartCh, &stcUartConfig);
	
	  SetPinFunc_SOT0_0();    
    Mfs_Uart_EnableFunc(UartCh, UartTx);
		
	
		SetPinFunc_SIN0_0();    
    Mfs_Uart_EnableFunc(UartCh, UartRx);
	
}

int fputc(int ch, FILE *f){

    if(((uint8_t)ch) == '\n')
    {
        while (TRUE != Mfs_Uart_GetStatus(UartCh, UartTxEmpty));
        Mfs_Uart_SendData(UartCh, '\r');
    }
    while (TRUE != Mfs_Uart_GetStatus(UartCh, UartTxEmpty))
    {
        /* Wait for room in the Tx FIFO */
    }
    Mfs_Uart_SendData(UartCh, ch);
    return ch;

}



void uart_printf(char *ch){
	
	int i = 0;
	while (ch[i] != '\0')
	{
    if(((uint8_t)ch[i]) == '\n')
    {
        while (TRUE != Mfs_Uart_GetStatus(UartCh, UartTxEmpty));
        Mfs_Uart_SendData(UartCh, '\r');
    }
    while (TRUE != Mfs_Uart_GetStatus(UartCh, UartTxEmpty))
    {
        /* Wait for room in the Tx FIFO */
    }
    Mfs_Uart_SendData(UartCh, ch[i++]);
		
	}

}

/**
 ******************************************************************************
 ** \brief Read UART data buffer
 **
 ** \param [in] pstcUart   Pointer to UART instance   
 ** 
 ** \retval Receive data        
 ** 
 ******************************************************************************/
uint16_t Mfs_Uart_ReceiveData(volatile stc_mfsn_uart_t* pstcUart)
{    
    return (pstcUart->RDR);
}


void uart_scanf(char *ch){

				
			// espera digitar alguma coisa
//    while (TRUE != Mfs_Uart_GetStatus(UartCh, UartRxFull))
//    {
//        /* Wait until there is a character in the Rx FIFO */ 
//    }
	
			if (Mfs_Uart_GetStatus(UartCh, UartRxFull) == TRUE ){
			
					*ch = (int)Mfs_Uart_ReceiveData(UartCh);        
			}
			
}

//int fgetc(FILE *f);
