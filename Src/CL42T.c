/**
 * @file        CL42T.c
 * @brief       Driver Module for Driver CL42T.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      sde
 * @date        23/01/2025
 * @version     1.0
 */
// ********************************************************************
// *                      Includes
// ********************************************************************
#include "TypeCommon.h"
#include "APP_CFG/ConfigFiles/CL42T_ConfigPrivate.h"
#include "./CL42T.h"
#include "FMK_HAL/FMK_CPU/Src/FMK_CPU.h"
#include "APP_CTRL/APP_SYS/Src/APP_SYS.h"
#include "APP_CTRL/APP_SIG/Src/APP_SIG.h"
#include "FMK_HAL/FMK_SRL/Src/FMK_SRL.h"
#include "Library/QUEUE/Src/LIBQueue.h"
// ********************************************************************
// *                      Defines
// ********************************************************************
#if defined(__arm__) || defined(__thumb__)
    #define CL42T_IRQ_DISABLE() __disable_irq()
    #define CL42T_IRQ_ENABLE()  __enable_irq()
#else
    #define CL42T_IRQ_DISABLE() ((void)0)
    #define CL42T_IRQ_ENABLE()  ((void)0)
#endif

///@brief mapping 
#define CL42T_LOG   FMKSRL_LOG
// ********************************************************************
// *                      Types
// ********************************************************************

/* CAUTION : Automatic generated code section for Enum: Start */

/* CAUTION : Automatic generated code section for Enum: End */

//-----------------------------ENUM TYPES-----------------------------//
/* CAUTION : Automatic generated code section for Structure: Start */

/* CAUTION : Automatic generated code section for Structure: End */
//-----------------------------STRUCT TYPES---------------------------//
typedef struct 
{
    t_uint8 signal_u8;
    t_uint32 value_u32;
} t_sCL42T_MotorSignalInfo;

typedef struct __t_sCL42T_MotorInfo
{
    t_eCL42T_MotorId selfId_e;                                      //---- store the motor Id ----//
    t_uint8 signalId_au8[CL42T_SIGTYPE_NB];                         //---- signal id ----//
    t_eCL42T_DiagError Health_e;                                    //---- Health of the motor ----//
    t_uint32 InhibTime_u32;                                         //---- Inhibition Time Managment ----//
    t_uint32 startPulseTime_u32;                                    //---- save when the pulse are launch ----//
    t_float32 estimPulseTime_f32;                                   //---- Estimate the duration of one the sequence pulse 
                                                                    //---- to readt if the callback end pulse is not called ----//
    t_bool isConfigured_b;                                          //---- flag to know if the motor is configrued ---//
    volatile t_uint16 maskInfo_u16;                                 //---- motor status mask @ref t_eCL42T_BitMotorInfo
    t_bool flagErrorDetected_b;                                     //---- Flag to know if a error callback has been called ----//
    t_bool enableDeadtime_b;                                        //---- store the parameter to enable dedtime ----//
    t_eCL42T_MotorDirection endStoptrigger_e;                       //---- End Stop trigger information ----//
    t_sLIBQUEUE_QueueCore HwCmdFifo_s;                              //---- hardware Fifo Managment ----//
    t_cbCL42T_Diagnostic * diagCallback_pcb;                        //---- User callabck error ----//  
    t_cbCL42T_PulseDropped * pulseDroppCallback_pcb;                //---- store the callback that we call whenever we dropped command ----//                    
    
} t_sCL42T_MotorInfo;

typedef struct 
{
    t_bool flagReception_b;
    t_uint16 counterDiag_u16;
    t_uint32 startTime_u32;
} t_sCL42T_DiagMngmt;

///@brief Structure for queuing command 
typedef struct 
{
    t_eCL42T_MotorDirection direction_e;        //---- CL42T Driver Direction pin value ----//
    t_eCL42T_MotorState state_e;                //---- CL42T Driver state pin value ----//
    t_uint32 nbPulses_u32;                      //---- CL42T Driver nb pulses pin value ----//
    t_float32 frequency_f32;                    //---- CL42T Driver Frequency pin value ----//
    t_uint32 triggerTimer_u32;                  //---- Information to plan when to send pulses  ----//
} t_sCL42T_HwSignalCmd;
/* CAUTION : Automatic generated code section : Start */

/* CAUTION : Automatic generated code section : End */

//-----------------------------TYPEDEF TYPES---------------------------//

// ********************************************************************
// *                      Variables
// ********************************************************************
///@brief flag to know when varaible has been initiialed
t_bool g_isModuleInit_b = (t_bool)FALSE;
/**
* @brief Motor Inforrmation
*/
static t_sCL42T_MotorInfo g_MotorInfo_as[CL42T_MOTOR_NB];
/**
* @brief Cyclic Module State
*/
static t_eCyclicModState g_CL42T_ModState_e = STATE_CYCLIC_PREOPE;
/**
* @brief Diag Managemnt to receive Pulse from Driver
*/
static t_sCL42T_DiagMngmt g_diagMngmt_as[CL42T_MOTOR_NB];

///@brief Queue Buffer Command 
static t_sCL42T_HwSignalCmd g_BufferHwCmd_as[CL42T_MOTOR_NB][CL42T_CMD_QUEUE_SIZE];
//********************************************************************************
//                      Local functions - Prototypes
//********************************************************************************
/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
static t_eReturnCode s_CL42T_PreOpeState(void);
/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
static t_eReturnCode s_CL42T_OperationalState(void);
/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
static t_eReturnCode s_CL42T_ErrorState(void);

/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
static void s_CL42T_SigErrorMngmt(t_eFMKIO_SigType f_type_e, 
                                t_uint8 f_signalId_u8, 
                                t_uint16 f_debugInfo1_u16,
                                t_uint16 f_debugInfo2_u16);

/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
static t_eReturnCode s_CL42T_PerformDiagnostic( t_eCL42T_MotorId f_idMotor_e, 
                                                t_uint16 f_cntDiag_u16);
/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
static t_eReturnCode s_CL42T_GetDiagErrorFromCnt(t_uint16 f_counter_u8, t_eCL42T_DiagError * f_diagErr_pe);
/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
static void s_CL42T_PulseEventMngmt(t_eFMKIO_OutPwmSig f_signal_e);

/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
static void s_CL42T_SigErrorMngmt(  t_eFMKIO_SigType f_typeSig_e, 
                                    t_uint8 f_signalId_au8, 
                                    t_uint16 f_debugInfo1, 
                                    t_uint16 f_debugInfo2);
/**
 *
 *	@brief
 *	@note   faire un switchcase avec wise (low) et clockwise (high)
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */

static t_eReturnCode s_CL42T_AddPulseSignal(t_sCL42T_MotorInfo * f_motorInfo_ps,
                                            t_sCL42T_PwmSignalCfg * f_pulseCfg_ps);
/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_AddDirSignal(  t_sCL42T_MotorInfo * f_motorInfo_ps,
                                            t_eFMKIO_OutDigSig * f_DirSignal_pe);

/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_AddStateSignal(t_sCL42T_MotorInfo * f_motorInfo_ps,
                                            t_eFMKIO_OutDigSig  * f_StateSignal_pe);
/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_AddDiagSignal(  t_sCL42T_MotorInfo * f_motorInfo_ps,
                                            t_eFMKIO_InFreqSig * f_FreqSig_pe);
/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_AddEndStopSignal(  t_sCL42T_MotorInfo * f_motorInfo_ps,
                                                t_sCL42T_EndStopignalCfg * f_endStopCfg_ps);
/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static void s_CL42T_EvntEndStopCallback(t_eFMKIO_InEvntSig f_evntSig_e);
/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_MotorCommandMngmt(t_sCL42T_MotorInfo * f_MotorInfo_ps);
/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_FormatHwCmd(t_sCL42T_SetMotorValue f_MotorVal_s, t_sCL42T_HwSignalCmd * f_SigCmdVal_ps);
/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_SendHwCommand(t_sCL42T_MotorInfo * f_MotorInfo_ps, t_sCL42T_HwSignalCmd * f_SigCmdVal_ps);

/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_GetStateSignal(    t_sCL42T_MotorInfo * f_motorInfo_ps,
                                                t_eCL42T_MotorState *f_state_pe);   


/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_CounterDiagMngmt(t_eCL42T_MotorId f_motorId_e, t_uint8 * counter_pu8);
/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_DebugUpdateSignal(t_sCL42T_MotorInfo * f_motorInfo_ps);
/**
 *
 *	@brief
*	@note   
*
*
*	@param[in] 
*	@param[out]
*	 
*
*
*/
static t_eReturnCode s_CL42T_DroppAllPulses(t_sCL42T_MotorInfo * f_motorInfo_ps);
//********************************************************************************
//                      Public functions - Implementation
//********************************************************************************
/*********************************
 * CL42T_Init
 *********************************/
t_eReturnCode CL42T_Init(void)
{
    t_eReturnCode Ret_e;
    t_uint8 idxMotor_u8;
    t_uint8 idxSignal_u8;
    t_sLIBQUEUE_QueueCfg HwQueueCfg_s = {
        .bufferHead_pv = NULL,
        .actualSize_u16 = (t_uint8)CL42T_CMD_QUEUE_SIZE,
        .elementSize_u16 = sizeof(t_sCL42T_HwSignalCmd),
        .enableOverwrite_b = (t_bool)FALSE
    };

    Ret_e = RC_OK;
    for(idxMotor_u8 = (t_uint8)0 ; 
    (idxMotor_u8 < CL42T_MOTOR_NB) && (Ret_e == RC_OK) ;
     idxMotor_u8++)
    {
        g_MotorInfo_as[idxMotor_u8].diagCallback_pcb = NULL_FUNCTION;
        g_MotorInfo_as[idxMotor_u8].flagErrorDetected_b = (t_bool)FALSE;
        g_MotorInfo_as[idxMotor_u8].Health_e = CL42T_DIAGNOSTIC_OK;
        g_MotorInfo_as[idxMotor_u8].InhibTime_u32 = (t_uint32)0;
        g_MotorInfo_as[idxMotor_u8].startPulseTime_u32 = (t_float32)0;
        g_MotorInfo_as[idxMotor_u8].estimPulseTime_f32 = (t_float32)0;
        g_MotorInfo_as[idxMotor_u8].enableDeadtime_b = (t_bool)FALSE;
        g_MotorInfo_as[idxMotor_u8].isConfigured_b = (t_bool)FALSE;
        g_MotorInfo_as[idxMotor_u8].maskInfo_u16 = (t_uint8)0;
        g_MotorInfo_as[idxMotor_u8].pulseDroppCallback_pcb = NULL_FUNCTION;
        g_MotorInfo_as[idxMotor_u8].endStoptrigger_e = CL42T_MOTOR_DIRECTION_NB;
        g_MotorInfo_as[idxMotor_u8].selfId_e = (t_eCL42T_MotorId)idxMotor_u8;

        g_diagMngmt_as[idxMotor_u8].counterDiag_u16 = (t_uint8)0;
        g_diagMngmt_as[idxMotor_u8].flagReception_b = (t_bool)FALSE;
        g_diagMngmt_as[idxMotor_u8].startTime_u32 = (t_uint32)0;

        HwQueueCfg_s.bufferHead_pv = (void *)(&g_BufferHwCmd_as[idxMotor_u8][0]);

        Ret_e = LIBQUEUE_Create(&g_MotorInfo_as[idxMotor_u8].HwCmdFifo_s,
                                HwQueueCfg_s);

        for(idxSignal_u8 = (t_uint8)0 ; idxSignal_u8 < CL42T_SIGTYPE_NB ; idxSignal_u8++)
        {
            g_MotorInfo_as[idxMotor_u8].signalId_au8[idxSignal_u8] = (t_uint8)0xFF; 
        }
    }
    if(Ret_e == RC_OK)
    {
        g_isModuleInit_b = (t_bool)TRUE;
    }

    return Ret_e;
}
/*********************************
 * CL42T_Cyclic
 *********************************/
t_eReturnCode CL42T_Cyclic(void)
{
    t_eReturnCode Ret_e = RC_OK;

    switch (g_CL42T_ModState_e)
    {
    case STATE_CYCLIC_PREOPE:
        Ret_e = s_CL42T_PreOpeState();
        if(Ret_e == RC_OK)
        {
            g_CL42T_ModState_e = STATE_CYCLIC_OPE;
        }
    break;
    case STATE_CYCLIC_OPE: 
        Ret_e = s_CL42T_OperationalState();
        if(Ret_e < RC_OK)
        {
            g_CL42T_ModState_e = STATE_CYCLIC_ERROR;
        }
    break;    
    case STATE_CYCLIC_ERROR:
        Ret_e = s_CL42T_ErrorState();

        //---- try to get back in preope ----//
        if(Ret_e == RC_OK)
        {
            g_CL42T_ModState_e = STATE_CYCLIC_PREOPE;
        }
    break;
    case STATE_CYCLIC_CFG:
    case STATE_CYCLIC_BUSY:
    default:
        Ret_e = RC_OK;
        break;
    }
    return Ret_e;
}
/*********************************
 * CL42T_GetState
 *********************************/
t_eReturnCode CL42T_GetState(   t_eCyclicModState *f_State_pe)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_State_pe == (t_eCyclicModState *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        *f_State_pe = g_CL42T_ModState_e;
    }

    return Ret_e;
}
/*********************************
 * CL42T_SetState
 *********************************/
t_eReturnCode CL42T_SetState(   t_eCyclicModState f_State_e)
{
    g_CL42T_ModState_e = f_State_e;   
    return RC_OK;
}

/*********************************
 * s_CL42T_AddPulseSignal
 *********************************/
t_eReturnCode CL42T_AddMotorConfiguration(  t_eCL42T_MotorId f_motorId_e,
                                            t_sCL42T_MotorSigCfg f_MotorCfg_s,
                                            t_bool f_enableDeadtime_b,
                                            t_cbCL42T_Diagnostic *f_diagEvnt_pcb,
                                            t_cbCL42T_PulseDropped * f_pulseDropped_pcb)
{
    t_eReturnCode Ret_e;
    t_sCL42T_MotorInfo * motorInfo_ps;

    if(f_motorId_e >= CL42T_MOTOR_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    else if(g_isModuleInit_b == (t_bool)FALSE)
    {
        Ret_e = RC_ERROR_MODULE_NOT_INITIALIZED;
    }
    else
    {
        motorInfo_ps = (t_sCL42T_MotorInfo *)(&g_MotorInfo_as[f_motorId_e]);

        if(motorInfo_ps->isConfigured_b == (t_bool)TRUE)
        {
            Ret_e = RC_ERROR_ALREADY_CONFIGURED;
            ASSERT((t_uint16)f_motorId_e);
        }
        else 
        {
            Ret_e = s_CL42T_AddPulseSignal(motorInfo_ps, &f_MotorCfg_s.PulseSigCfg_s);

            if(Ret_e == RC_OK)
            {
                Ret_e = s_CL42T_AddDirSignal(motorInfo_ps, &f_MotorCfg_s.DirSignal_e);
            }
            if(Ret_e == RC_OK)
            {
                Ret_e = s_CL42T_AddStateSignal(motorInfo_ps, &f_MotorCfg_s.StateSignal_e);
            }
            if(Ret_e == RC_OK)
            {
                Ret_e = s_CL42T_AddDiagSignal(motorInfo_ps, &f_MotorCfg_s.DiagSignal_e);
            }
            if(Ret_e == RC_OK)
            {
                Ret_e = s_CL42T_AddEndStopSignal(motorInfo_ps, &f_MotorCfg_s.EndStopSigCW_s);

                if((Ret_e == RC_OK) || (Ret_e == RC_WARNING_NO_OPERATION))
                {
                    motorInfo_ps->signalId_au8[CL42T_SIGTYPE_ENDSTOP_CW] = 
                        (t_uint8)f_MotorCfg_s.EndStopSigCW_s.EndStopSignal_e;
                    Ret_e = s_CL42T_AddEndStopSignal(motorInfo_ps, &f_MotorCfg_s.EndStopSigCCW_s);
                }
                if((Ret_e == RC_OK) || (Ret_e == RC_WARNING_NO_OPERATION))
                {
                    Ret_e = RC_OK;
                    motorInfo_ps->signalId_au8[CL42T_SIGTYPE_ENDSTOP_CCW] = 
                        (t_uint8)f_MotorCfg_s.EndStopSigCCW_s.EndStopSignal_e;
                }            
            }
            if(Ret_e == RC_OK)
            {
                motorInfo_ps->enableDeadtime_b = (t_bool)f_enableDeadtime_b;
                motorInfo_ps->diagCallback_pcb = f_diagEvnt_pcb;
                motorInfo_ps->pulseDroppCallback_pcb = f_pulseDropped_pcb;
                motorInfo_ps->isConfigured_b = (t_bool)TRUE;
            }
        }
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_SetPulseSignal
 *********************************/
t_eReturnCode CL42T_SetMotorSigValue(   t_eCL42T_MotorId f_motorId_e,
                                        t_sCL42T_SetMotorValue f_MotorValue_s)
{
    t_eReturnCode Ret_e;
    t_sCL42T_MotorInfo * motorInfo_ps;
    t_sCL42T_HwSignalCmd hwSigCmd_s;

    if(f_motorId_e >= CL42T_MOTOR_NB) 
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    else if(g_isModuleInit_b == (t_bool)FALSE)
    {
        Ret_e = RC_ERROR_MODULE_NOT_INITIALIZED;
    }
    else if((g_CL42T_ModState_e != STATE_CYCLIC_OPE)
    ||     (g_MotorInfo_as[f_motorId_e].Health_e != CL42T_DIAGNOSTIC_OK))
    {
        Ret_e = RC_WARNING_BUSY;
    }
    else if(g_MotorInfo_as[f_motorId_e].isConfigured_b == (t_bool)False)
    {
        Ret_e = RC_ERROR_INSTANCE_NOT_INITIALIZED;
    }
    else
    {
        motorInfo_ps = (t_sCL42T_MotorInfo *)(&g_MotorInfo_as[f_motorId_e]);

        if(GETBIT(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ENABLE) == BIT_IS_RESET_16B)
        {
            Ret_e = RC_WARNING_NOT_ALLOWED;
            ASSERT(motorInfo_ps->maskInfo_u16);
        }
        else 
        {
                Ret_e = s_CL42T_FormatHwCmd(f_MotorValue_s, &hwSigCmd_s);

            if(Ret_e == RC_OK)
            {
                //---- protection of the queue ----//
                CL42T_IRQ_DISABLE();
                Ret_e = LIBQUEUE_WriteElement(  &motorInfo_ps->HwCmdFifo_s,
                                                &hwSigCmd_s,
                                                sizeof(hwSigCmd_s));
                CL42T_IRQ_ENABLE();
                //---- means no more place in queue ----//
                if(Ret_e == RC_WARNING_LIMIT_REACHED)
                {
                    Ret_e = RC_WARNING_BUSY;
                }
            }
        }
    }

    return Ret_e;
}

/*********************************
 * CL42T_SetMotorState
 *********************************/
t_eReturnCode CL42T_GetMotorInfo(   t_eCL42T_MotorId f_motorId_e,
                                    t_uint16 * f_MotorStsInfo_pu16)
{
    t_eReturnCode Ret_e;

    if(f_motorId_e >= CL42T_MOTOR_NB) 
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    else if(g_isModuleInit_b == (t_bool)FALSE)
    {
        Ret_e = RC_ERROR_MODULE_NOT_INITIALIZED;
    }
    else if((g_CL42T_ModState_e != STATE_CYCLIC_OPE)
    ||     (g_MotorInfo_as[f_motorId_e].Health_e != CL42T_DIAGNOSTIC_OK))
    {
        Ret_e = RC_WARNING_BUSY;
    }
    else if(g_MotorInfo_as[f_motorId_e].isConfigured_b == (t_bool)False)
    {
        Ret_e = RC_ERROR_INSTANCE_NOT_INITIALIZED;
    }
    else 
    {
        Ret_e = RC_OK;
        *f_MotorStsInfo_pu16 = g_MotorInfo_as[f_motorId_e].maskInfo_u16;

        //---- fixbug sometimes there is still cmd in queue and
        //       interruption has been called cyclic not yet 
        //          so the motor is OFF but in logic point of view it is still ON ----//
        if(g_MotorInfo_as[f_motorId_e].HwCmdFifo_s.actualSize_u16 > (t_uint16)0)
        {
            SETBIT_16B(*f_MotorStsInfo_pu16, CL42T_BITFIELD_MOTOR_ON);
        }
    }

    return Ret_e;
}

/*********************************
 * CL42T_SetMotorState
 *********************************/
t_eReturnCode CL42T_GetMotorSpeed(  t_eCL42T_MotorId f_motorId_e,
                                        t_float32 * f_motorSpeed_pf32)
{
    t_eReturnCode Ret_e;
    if(f_motorId_e >= CL42T_MOTOR_NB) 
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    else if(g_isModuleInit_b == (t_bool)FALSE)
    {
        Ret_e = RC_ERROR_MODULE_NOT_INITIALIZED;
    }
    else if((g_CL42T_ModState_e != STATE_CYCLIC_OPE)
    ||     (g_MotorInfo_as[f_motorId_e].Health_e != CL42T_DIAGNOSTIC_OK))
    {
        Ret_e = RC_WARNING_BUSY;
    }
    else if(g_MotorInfo_as[f_motorId_e].isConfigured_b == (t_bool)False)
    {
        Ret_e = RC_ERROR_INSTANCE_NOT_INITIALIZED;
    }
    else 
    {
        if(GETBIT(g_MotorInfo_as[f_motorId_e].maskInfo_u16, CL42T_BITFIELD_MOTOR_ON) == BIT_IS_SET_16B)
        {
            Ret_e = FMKIO_Get_OutPwmSigFrequency(   g_MotorInfo_as[f_motorId_e].signalId_au8[CL42T_SIGTYPE_PULSE],
                                                    f_motorSpeed_pf32);
        }
        else 
        {
            Ret_e = RC_OK;
            *f_motorSpeed_pf32 = 0.0f;
        }
    }

    return Ret_e;
}
/*********************************
 * CL42T_SetMotorState
 *********************************/
t_eReturnCode CL42T_SetMotorState(  t_eCL42T_MotorId f_motorId_e, 
                                    t_eCL42T_MotorState f_state_e,
                                    t_bool f_isEmergencyStop_b)
{
    t_eReturnCode Ret_e;
    t_sCL42T_MotorInfo * motorInfo_ps;

    if(f_motorId_e >= CL42T_MOTOR_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)0);
    }
    else if(g_MotorInfo_as[f_motorId_e].isConfigured_b == (t_bool)FALSE)
    {
        Ret_e = RC_ERROR_INSTANCE_NOT_INITIALIZED;
    }
    else if((g_CL42T_ModState_e != STATE_CYCLIC_OPE)
    &&      (f_isEmergencyStop_b == FALSE))
    {
        Ret_e = RC_WARNING_BUSY;
    }
    else 
    {
        motorInfo_ps = (t_sCL42T_MotorInfo *)(&g_MotorInfo_as[f_motorId_e]);

        if(f_state_e == CL42T_MOTOR_STATE_OFF)
        {
            if(f_isEmergencyStop_b == TRUE) // completely OFF Motor
            {
                Ret_e = FMKIO_Set_OutDigSigValue(   (t_eFMKIO_OutDigSig)motorInfo_ps->signalId_au8[CL42T_SIGTYPE_STATE],
                                                    FMKIO_DIG_VALUE_HIGH);
                if(Ret_e == RC_OK)
                {
                    RESETBIT_16B(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ENABLE);
                }
            }
            else // just stop pulse 
            {
                Ret_e = FMKIO_Set_OutPwmSigPulses((t_eFMKIO_OutPwmSig)motorInfo_ps->signalId_au8[CL42T_SIGTYPE_PULSE],
                                                    CL42T_NOMINATIVE_FREQUENCY,
                                                    (t_uint16)0,
                                                    (t_uint16)0);
            }
            if(Ret_e == RC_OK)
            {
                RESETBIT_16B(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ON);
                //---- flush the queue and call user with dropp !!! @todo ----//
                Ret_e = s_CL42T_DroppAllPulses(motorInfo_ps);
                        
            }
        }
        else // MOTOR ON
        {
            //---- datasheet says wait 200 ms before set direction or anything else 
            //      the timeout for deadtime is higher so we juste set the timeout 
            //      maybe  deal that in other way if deadtime < 200 ms 
            Ret_e = FMKIO_Set_OutDigSigValue(   (t_eFMKIO_OutDigSig)motorInfo_ps->signalId_au8[CL42T_SIGTYPE_STATE],
                                                FMKIO_DIG_VALUE_LOW);
            if(Ret_e == RC_OK)
            {
                SETBIT_16B(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ENABLE);
            }
            SETBIT_16B(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_IN_DEAD_TIME);
            FMKCPU_GetTick(&motorInfo_ps->InhibTime_u32);
        }        
    }

    return Ret_e;
}

void CL42T_Test_SetPerturb(t_eCL42T_MotorId f_MOTOR_e, t_bool isEndStopCW)
{
    if(isEndStopCW == TRUE)
    {
        g_MotorInfo_as[f_MOTOR_e].endStoptrigger_e = CL42T_MOTOR_DIRECTION_CW;
        SETBIT_16B(g_MotorInfo_as[f_MOTOR_e].maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CW);
    }
    else 
    {
        g_MotorInfo_as[f_MOTOR_e].endStoptrigger_e = CL42T_MOTOR_DIRECTION_CCW;
        SETBIT_16B(g_MotorInfo_as[f_MOTOR_e].maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CCW);
    }
}
//********************************************************************************
//                      Local functions - Implementation
//********************************************************************************
/*********************************
 * s_CL42T_ErrorState
 *********************************/
static t_eReturnCode s_CL42T_ErrorState(void)
{
    t_eReturnCode Ret_e;
    t_uint8 idxMotor_u8;

    Ret_e = RC_OK;
    for(idxMotor_u8 = (t_uint8)0 ; (idxMotor_u8 < CL42T_MOTOR_NB) && (Ret_e == RC_OK) ; idxMotor_u8++)
    {
        Ret_e = CL42T_SetMotorState((t_eCL42T_MotorId)idxMotor_u8, CL42T_MOTOR_STATE_OFF, TRUE);
    }

    return Ret_e;
}
/*********************************
 * CL42T_OperationnalState
 *********************************/
static t_eReturnCode s_CL42T_OperationalState(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 counter_u8 = 0;
    t_sCL42T_MotorInfo * motorInfo_ps;
    t_uint8 idxMotor_u8 = (t_uint8)0;
    

    for(idxMotor_u8 = (t_uint8)0 ; (idxMotor_u8 < CL42T_MOTOR_NB) && (Ret_e >= RC_OK) ; idxMotor_u8++)
    {
        if(g_MotorInfo_as[idxMotor_u8].isConfigured_b == (t_bool)True)
        {
            motorInfo_ps = (t_sCL42T_MotorInfo *)(&g_MotorInfo_as[idxMotor_u8]);

            //---- Check Diagnostic Value ----//
            Ret_e = s_CL42T_CounterDiagMngmt(idxMotor_u8, &counter_u8);                                           
                                            
            // diagrerromanagement

            //---- If return code not ok, perform diagnostic anyway ----//
            if((motorInfo_ps->flagErrorDetected_b == (t_bool)True)
            || (counter_u8 > (t_uint8)0))
            {
                Ret_e = s_CL42T_PerformDiagnostic((t_eCL42T_MotorId)idxMotor_u8, (t_uint16)counter_u8);
            }
            //---- Set Signal even if RetCode is a WARNING ----//
            if(Ret_e >= RC_OK)
            {
                Ret_e = s_CL42T_MotorCommandMngmt(motorInfo_ps);
            }
            
            (void)s_CL42T_DebugUpdateSignal(motorInfo_ps);
            
        }
    }
    if(Ret_e < RC_OK)
    {
        ASSERT((t_uint16)Ret_e);
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_PreOpeState
 *********************************/
static t_eReturnCode s_CL42T_PreOpeState(void)
{
    return RC_OK;
}

/*********************************
 * s_CL42T_PerformDiagnostic
 *********************************/
static t_eReturnCode s_CL42T_PerformDiagnostic( t_eCL42T_MotorId f_idMotor_e, 
                                                t_uint16 f_cntDiag_u16)
{
    t_eReturnCode Ret_e = RC_OK;
    t_sCL42T_MotorInfo * motorInfo_ps;

    if(f_idMotor_e >= CL42T_MOTOR_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(Ret_e == RC_OK)
    {
        motorInfo_ps = (t_sCL42T_MotorInfo *)(&g_MotorInfo_as[f_idMotor_e]);

        if(f_cntDiag_u16 > (t_uint16)0)
        {
            Ret_e = s_CL42T_GetDiagErrorFromCnt(f_cntDiag_u16, &motorInfo_ps->Health_e);
        }

        //---- check if the motor is still ON ----//
        if(GETBIT(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ON) == BIT_IS_SET_16B)
        {
            //---- try to shut down by cutting dutycycle ----//
            Ret_e = FMKIO_Set_OutPwmSigDutyCycle(   motorInfo_ps->signalId_au8[CL42T_SIGTYPE_PULSE],
                                                    (t_uint16)0);
            if(Ret_e == RC_OK)
            {
                RESETBIT_16B(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ON);
                //---- if the problem was infinite pulse, no more problem ----//
                if(motorInfo_ps->Health_e == CL42T_DIAGNOSTIC_PULSE_INFINITE)
                {
                    motorInfo_ps->Health_e = CL42T_DIAGNOSTIC_OK;
                }
            }
        }
        else
        {
            //--- mtoor off so dizagnostic also off
            if(motorInfo_ps->Health_e == CL42T_DIAGNOSTIC_PULSE_INFINITE)
            {
                motorInfo_ps->Health_e = CL42T_DIAGNOSTIC_OK;
            }
        }
        //---- If it's not countor error it is signal error or infinite pulse, already set in health variable ----//
        
        if((Ret_e == RC_OK)
        && (motorInfo_ps->Health_e != CL42T_DIAGNOSTIC_OK)
        && (motorInfo_ps->diagCallback_pcb != NULL_FUNCTION))
        {
            motorInfo_ps->diagCallback_pcb(f_idMotor_e, motorInfo_ps->Health_e);
        }
        else if((Ret_e == RC_OK)
        && (motorInfo_ps->Health_e == CL42T_DIAGNOSTIC_OK)
        && (motorInfo_ps->flagErrorDetected_b == (t_bool)TRUE)
        && (motorInfo_ps->diagCallback_pcb != NULL_FUNCTION))
        {
            motorInfo_ps->flagErrorDetected_b = (t_bool)FALSE;
            motorInfo_ps->diagCallback_pcb(f_idMotor_e, motorInfo_ps->Health_e);
        }
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_GetDiagErrorFromCnt
 *********************************/
static void s_CL42T_PulseEventMngmt(t_eFMKIO_OutPwmSig f_signal_e)
{
    t_uint8 idxMotor_u8 = (t_uint8)0;
    t_sCL42T_MotorInfo * motorInfo_ps = NULL;

    //---- Find Id Motor ----//
    for(idxMotor_u8 = (t_uint8)0 ; idxMotor_u8 < CL42T_MOTOR_NB ; idxMotor_u8++)
    {
        if(g_MotorInfo_as[idxMotor_u8].signalId_au8[CL42T_SIGTYPE_PULSE] == (t_uint8)f_signal_e)
        {
            motorInfo_ps = (t_sCL42T_MotorInfo *)(&g_MotorInfo_as[idxMotor_u8]);
            break;
        }
    }
    if(motorInfo_ps != (t_sCL42T_MotorInfo *) NULL)
    {        
        RESETBIT_16B(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ON);
        FMKCPU_GetTick(&motorInfo_ps->InhibTime_u32);
        CL42T_LOG(  "[CL42T] Motor %d, Pulse Finished, takes %d ms\r\n", motorInfo_ps->selfId_e,
                    (motorInfo_ps->InhibTime_u32 - motorInfo_ps->startPulseTime_u32));
    }
    else 
    {
        ASSERT((t_uint16)f_signal_e);
    }

    return;
}

/*********************************
 * s_CL42T_SendHwCommand
 *********************************/
static t_eReturnCode s_CL42T_SendHwCommand(t_sCL42T_MotorInfo * f_MotorInfo_ps, t_sCL42T_HwSignalCmd * f_SigCmdVal_ps)
{
    t_eReturnCode Ret_e;
    t_eFMKIO_DigValue digEnablevalue_e;
    t_eFMKIO_DigValue digDirectionvalue_e;

    if((f_SigCmdVal_ps == (t_sCL42T_HwSignalCmd *)NULL)
    || (f_MotorInfo_ps == (t_sCL42T_MotorInfo *)NULL))
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)0);
    }
    else
    {
        //---- start by ENABLE/DIABLE pin ----//
        digEnablevalue_e = (f_SigCmdVal_ps->state_e == CL42T_MOTOR_STATE_ON)
                        ? FMKIO_DIG_VALUE_LOW 
                        : FMKIO_DIG_VALUE_HIGH;

        Ret_e = FMKIO_Set_OutDigSigValue((t_eFMKIO_OutDigSig)f_MotorInfo_ps->signalId_au8[CL42T_SIGTYPE_STATE],
                                            digEnablevalue_e);
        //---- now direction ----//
        if(Ret_e == RC_OK)
        {
            digDirectionvalue_e = (f_SigCmdVal_ps->direction_e == CL42T_MOTOR_DIRECTION_CW)
                                ? FMKIO_DIG_VALUE_LOW
                                : FMKIO_DIG_VALUE_HIGH;
            Ret_e = FMKIO_Set_OutDigSigValue(   (t_eFMKIO_OutDigSig)f_MotorInfo_ps->signalId_au8[CL42T_SIGTYPE_DIR],
                                                digDirectionvalue_e);
        }
        //---- then the pulses ----//
        if(Ret_e == RC_OK)
        {
            //---- we double check the End Stop flag, 'cause sometimes an endStop Callback is called 
            //      after the first check ----//
            if(((GETBIT(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CW) == BIT_IS_SET_16B)
            && ((f_SigCmdVal_ps->direction_e == f_MotorInfo_ps->endStoptrigger_e)))
            || ((GETBIT(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CCW) == BIT_IS_SET_16B)
            && ((f_SigCmdVal_ps->direction_e == f_MotorInfo_ps->endStoptrigger_e))))
            {
                Ret_e = RC_WARNING_NOT_ALLOWED;
            }
            else 
            {
                //---- if infinite pulse, just set a PWM, user has to so send 0 ----//
                if(f_SigCmdVal_ps->nbPulses_u32 == CL42T_SEND_INFINITE_PULSE)
                {
                    Ret_e = FMKIO_Set_OutPwmSigFrequency(   (t_eFMKIO_OutPwmSig)f_MotorInfo_ps->signalId_au8[CL42T_SIGTYPE_PULSE],
                                                            f_SigCmdVal_ps->frequency_f32);
                    if(Ret_e == RC_OK)
                    {
                        Ret_e = FMKIO_Set_OutPwmSigDutyCycle(   (t_eFMKIO_OutPwmSig)f_MotorInfo_ps->signalId_au8[CL42T_SIGTYPE_PULSE],
                                                                CL42T_NOMINATIVE_DUTYCYCLE);
                    }
                }
                else 
                {
                    Ret_e = FMKIO_Set_OutPwmSigPulses(  (t_eFMKIO_OutPwmSig)f_MotorInfo_ps->signalId_au8[CL42T_SIGTYPE_PULSE],
                                                    f_SigCmdVal_ps->frequency_f32,
                                                    CL42T_NOMINATIVE_DUTYCYCLE,
                                                    f_SigCmdVal_ps->nbPulses_u32);
                }                
            }
        }
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_GetDiagErrorFromCnt
 *********************************/
static t_eReturnCode s_CL42T_FormatHwCmd(t_sCL42T_SetMotorValue f_MotorVal_s, t_sCL42T_HwSignalCmd * f_SigCmdVal_ps)
{
    t_eReturnCode Ret_e;

    if(f_SigCmdVal_ps == (t_sCL42T_HwSignalCmd *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)0);
    }
    else
    {
        Ret_e = RC_OK;
        //---- wrtie hardware signal ----//
        if(f_MotorVal_s.nbPulses_s32 > (t_sint32)0)
        {
            f_SigCmdVal_ps->direction_e = CL42T_MOTOR_DIRECTION_CCW;
        }
        else 
        {
            f_MotorVal_s.nbPulses_s32 *= (t_sint32)(-1);
            f_SigCmdVal_ps->direction_e = CL42T_MOTOR_DIRECTION_CW;
        }
        f_SigCmdVal_ps->state_e = CL42T_MOTOR_STATE_ON;
        f_SigCmdVal_ps->frequency_f32 = (t_float32)f_MotorVal_s.frequency_f32;
        f_SigCmdVal_ps->nbPulses_u32 = (t_uint32)f_MotorVal_s.nbPulses_s32;
        f_SigCmdVal_ps->triggerTimer_u32 = f_MotorVal_s.triggerTimer_u32;        
    }

    return Ret_e;
}
/*********************************
 * s_CL42T_GetDiagErrorFromCnt
 *********************************/
static void s_CL42T_SigErrorMngmt(t_eFMKIO_SigType f_type_e, 
                                t_uint8 f_signalId_u8, 
                                t_uint16 f_debugInfo1_u16,
                                t_uint16 f_debugInfo2_u16)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxMotor_u8;
    t_uint8 idxSignal_u8;

    if((f_type_e != FMKIO_SIGTYPE_OUTPUT_PWM)
    && (f_type_e != FMKIO_SIGTYPE_INPUT_FREQ))
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_type_e)
        {
            case FMKIO_SIGTYPE_OUTPUT_PWM:
            {
                for(idxMotor_u8 = (t_uint8)0 ; idxMotor_u8 < CL42T_MOTOR_NB ; idxMotor_u8++)
                {
                    for(idxSignal_u8 = (t_uint8)0 ; idxSignal_u8 < CL42T_SIGTYPE_NB ; idxSignal_u8++)
                    {
                        if(g_MotorInfo_as[idxMotor_u8].signalId_au8[idxSignal_u8] == f_signalId_u8)
                        {
                            CL42T_LOG("[CL42T] Error on Pwm %d, debug1 -> %d, debug2 ->%d",
                                        idxSignal_u8,
                                        f_debugInfo1_u16,
                                        f_debugInfo2_u16);
                            g_MotorInfo_as[idxMotor_u8].Health_e = CL42T_DIAGNOSTIC_SIGNAL_PULSE;
                            break;
                        }
                    }
                }
                break;
            }
            case FMKIO_SIGTYPE_INPUT_FREQ:
            {
                for(idxMotor_u8 = (t_uint8)0 ; idxMotor_u8 < CL42T_MOTOR_NB ; idxMotor_u8++)
                {
                    for(idxSignal_u8 = (t_uint8)0 ; idxSignal_u8 < CL42T_SIGTYPE_NB ; idxSignal_u8++)
                    {
                        if(g_MotorInfo_as->signalId_au8[idxSignal_u8] == f_signalId_u8)
                        {
                            CL42T_LOG("[CL42T] Error on Freq %d, debug1 -> %d, debug2 ->%d",
                                        idxSignal_u8,
                                        f_debugInfo1_u16,
                                        f_debugInfo2_u16);
                            g_MotorInfo_as[idxMotor_u8].Health_e = CL42T_DIAGNOSTIC_SIGNAL_FREQ;
                            break;
                        }
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }

    return;
}

/*********************************
 * s_CL42T_GetDiagErrorFromCnt
 *********************************/
static t_eReturnCode s_CL42T_GetDiagErrorFromCnt(t_uint16 f_counter_u8, t_eCL42T_DiagError * f_diagErr_pe)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_diagErr_pe == (t_eCL42T_DiagError *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch (f_counter_u8)
        {
            case CL42T_ONE_PULSE:
            {
                *f_diagErr_pe = CL42T_DIAGNOSTIC_OVER_CURRENT;
                break;
            }
            case CL42T_TWO_PULSE:
            {
                *f_diagErr_pe = CL42T_DIAGNOSTIC_OVER_VOLTAGE;
                break;
            }
            case CL42T_THREE_PULSE:
            {
                *f_diagErr_pe = CL42T_DIAGNOSTIC_CHIP_ERROR;
                break;
            }
            case CL42T_FOUR_PULSE:
            {
                *f_diagErr_pe = CL42T_DIAGNOSTIC_LOCK_MOTOR_SHAFT;
                break;
            }
            case CL42T_FIVE_PULSE:
            {
                *f_diagErr_pe = CL42T_DIAGNOSTIC_EEPROM;
                break;
            }
            case CL42T_SIX_PULSE:
            {
                *f_diagErr_pe = CL42T_DIAGNOSTIC_AUTO_TUNNING;
                break;
            }
            case CL42T_SEVEN_PULSE: 
            {
                *f_diagErr_pe = CL42T_DIAGNOSTIC_POSITION;
                break;
            }
            default:
            {
                *f_diagErr_pe = CL42T_DIAGNOSTIC_OK;
                break;
            }
        }
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_AddPulseSignal
 *********************************/
static t_eReturnCode s_CL42T_AddPulseSignal(t_sCL42T_MotorInfo * f_motorInfo_ps,
                                            t_sCL42T_PwmSignalCfg * f_pulseCfg_ps)
{
    t_eReturnCode Ret_e = RC_OK;
    t_sCL42T_MotorSignalInfo * SignalInfo_ps;

    if(f_motorInfo_ps == (t_sCL42T_MotorInfo *)NULL
    || f_pulseCfg_ps == (t_sCL42T_PwmSignalCfg *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    
    if(Ret_e == RC_OK)
    {
        SignalInfo_ps = (t_sCL42T_MotorSignalInfo *)(&f_motorInfo_ps->signalId_au8[CL42T_SIGTYPE_PULSE]);

        Ret_e = FMKIO_Set_OutPwmSigCfg( f_pulseCfg_ps->PulseSignal_e,
                                        f_pulseCfg_ps->pwmWaveForm_s,
                                        f_pulseCfg_ps->pwmCtrlPrm_s,
                                        s_CL42T_PulseEventMngmt,
                                        s_CL42T_SigErrorMngmt);       

        if(Ret_e == RC_OK)
        {
            SignalInfo_ps->signal_u8 = f_pulseCfg_ps->PulseSignal_e;
            
            //---- signal speed and pulse are the same pin -----//
            f_motorInfo_ps->signalId_au8[CL42T_SIGTYPE_PULSE] =  f_pulseCfg_ps->PulseSignal_e;
        }
        
    }
    return Ret_e;
}
/*********************************
 * CL42T_AddDirPulseSignal
 *********************************/
static t_eReturnCode s_CL42T_AddDirSignal(  t_sCL42T_MotorInfo * f_motorInfo_ps,
                                            t_eFMKIO_OutDigSig * f_DirSignal_pe)
{
    t_eReturnCode Ret_e = RC_OK;
    t_sCL42T_MotorSignalInfo * SignalInfo_ps;

    if(f_motorInfo_ps == (t_sCL42T_MotorInfo *)NULL
    || f_DirSignal_pe == (t_eFMKIO_OutDigSig *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    
    if(Ret_e == RC_OK)
    {
        SignalInfo_ps = (t_sCL42T_MotorSignalInfo *)(&f_motorInfo_ps->signalId_au8[CL42T_SIGTYPE_DIR]);

        Ret_e = FMKIO_Set_OutDigSigCfg( *f_DirSignal_pe,
                                        FMKIO_PULL_MODE_DISABLE,
                                        FMKIO_SPD_MODE_MEDIUM);

        if(Ret_e == RC_OK)
        {
            SignalInfo_ps->signal_u8 = *f_DirSignal_pe;
        }
    }

    return Ret_e;
}
/*********************************
 * s_CL42T_AddStateSignal
 *********************************/
static t_eReturnCode s_CL42T_AddStateSignal(t_sCL42T_MotorInfo * f_motorInfo_ps,
                                            t_eFMKIO_OutDigSig  * f_StateSignal_pe)
{
    t_eReturnCode Ret_e = RC_OK;
    t_sCL42T_MotorSignalInfo * SignalInfo_ps;

    if(f_motorInfo_ps == (t_sCL42T_MotorInfo *)NULL
    || f_StateSignal_pe == (t_eFMKIO_OutDigSig  *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    
    if(Ret_e == RC_OK)
    {
        SignalInfo_ps = (t_sCL42T_MotorSignalInfo *)(&f_motorInfo_ps->signalId_au8[CL42T_SIGTYPE_STATE]);


        Ret_e = FMKIO_Set_OutDigSigCfg( *f_StateSignal_pe, 
                                        FMKIO_PULL_MODE_DISABLE,
                                        FMKIO_SPD_MODE_MEDIUM);
        if(Ret_e == RC_OK)
        {
            SignalInfo_ps->signal_u8 = *f_StateSignal_pe;
        }
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_AddDiagSignal
 *********************************/
static t_eReturnCode s_CL42T_AddDiagSignal(  t_sCL42T_MotorInfo * f_motorInfo_ps,
                                            t_eFMKIO_InFreqSig * f_FreqSig_pe)
{
    t_eReturnCode Ret_e = RC_OK;
    t_sCL42T_MotorSignalInfo * SignalInfo_ps;

    if(f_motorInfo_ps == (t_sCL42T_MotorInfo *)NULL
    || f_FreqSig_pe == (t_eFMKIO_InFreqSig *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    
    if(Ret_e == RC_OK)
    {
        SignalInfo_ps = (t_sCL42T_MotorSignalInfo *)(&f_motorInfo_ps->signalId_au8[CL42T_SIGTYPE_DIAG]);

        Ret_e = FMKIO_Set_InFreqSigCfg( *f_FreqSig_pe, 
                                        FMKIO_STC_RISING_EDGE,
                                        FMKIO_FREQ_MEAS_COUNT,
                                        CL42T_IN_FREQ_SAMPLING, 
                                        s_CL42T_SigErrorMngmt);

        if(Ret_e == RC_OK)
        {
            SignalInfo_ps->signal_u8 = *f_FreqSig_pe;
        }
        
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_AddEndStopSignal
 *********************************/
static t_eReturnCode s_CL42T_AddEndStopSignal(  t_sCL42T_MotorInfo * f_motorInfo_ps,
                                                t_sCL42T_EndStopignalCfg * f_endStopCfg_ps)
{
    t_eReturnCode Ret_e;

    if((f_motorInfo_ps == (t_sCL42T_MotorInfo *)NULL)
    || (f_endStopCfg_ps == (t_sCL42T_EndStopignalCfg * )NULL))
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    else 
    {
        if(f_endStopCfg_ps->EndStopSignal_e < FMKIO_INPUT_SIGEVNT_NB)
        {
            Ret_e = RC_OK;
            Ret_e = FMKIO_Set_InEvntSigCfg( f_endStopCfg_ps->EndStopSignal_e,
                                            f_endStopCfg_ps->PullMode_e,
                                            f_endStopCfg_ps->triggerEvnt_e,
                                            (t_uint32)f_endStopCfg_ps->debuncValue_u16,
                                            s_CL42T_EvntEndStopCallback,
                                            NULL_FUNCTION);
        }
        else
        {
            Ret_e = RC_WARNING_NO_OPERATION;
        }
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_EvntEndStopCallback
 *********************************/
static void s_CL42T_EvntEndStopCallback(t_eFMKIO_InEvntSig f_evntSig_e)
{
    t_eReturnCode Ret_e;
    t_uint8 idxMotor_u8;
    t_sCL42T_MotorInfo * motorInfo_ps;
    t_bool motorFound_b = (t_bool)FALSE;
    
    //---- find out reach a end stop ----//
    for(idxMotor_u8 = (t_uint8)0 ; 
    (idxMotor_u8 < CL42T_MOTOR_NB) 
    && (motorFound_b == (t_bool)FALSE) ; 
    idxMotor_u8++)
    {
        motorInfo_ps = (t_sCL42T_MotorInfo *)(&g_MotorInfo_as[idxMotor_u8]);
        if(f_evntSig_e == 
            motorInfo_ps->signalId_au8[CL42T_SIGTYPE_ENDSTOP_CW])
        {
            motorFound_b = (t_bool)TRUE;
            SETBIT_16B(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CW);
            motorInfo_ps->endStoptrigger_e = CL42T_MOTOR_DIRECTION_CW;
        }
        else if(f_evntSig_e == 
            motorInfo_ps->signalId_au8[CL42T_SIGTYPE_ENDSTOP_CCW])
        {
            motorFound_b = (t_bool)TRUE;
            SETBIT_16B(motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CCW);
            motorInfo_ps->endStoptrigger_e = CL42T_MOTOR_DIRECTION_CCW;
        }
    }
    if(motorFound_b == (t_bool)TRUE)
    {
        CL42T_LOG("[CL42T] Interruption from sig %d,  shut down motor...!!!\r\n", (t_uint32)f_evntSig_e);
        Ret_e = FMKIO_Set_OutPwmSigPulses(  motorInfo_ps->signalId_au8[CL42T_SIGTYPE_PULSE],
                                            CL42T_NOMINATIVE_FREQUENCY,
                                            CL42T_NOMINATIVE_DUTYCYCLE,
                                            (t_uint16)0);
        if(Ret_e == RC_OK)
        {
            RESETBIT_16B(motorInfo_ps->maskInfo_u16,CL42T_BITFIELD_MOTOR_ON);
        }
        else
        {
            motorInfo_ps->Health_e = CL42T_DIAGNOSTIC_PULSE_INFINITE;
            motorInfo_ps->flagErrorDetected_b = (t_bool)TRUE;
        }
    }
    else
    {
        ASSERT((t_uint16)f_evntSig_e);
    }    

    return;
}

/*********************************
 * s_CL42T_MotorCommandMngmt
 *********************************/
static t_eReturnCode s_CL42T_MotorCommandMngmt(t_sCL42T_MotorInfo * f_MotorInfo_ps)
{
    t_eReturnCode Ret_e;
    t_sCL42T_HwSignalCmd hwSigCmd_s = {
        .direction_e = CL42T_MOTOR_DIRECTION_NB,
        .frequency_f32 = 0.0f,
        .nbPulses_u32 = (t_uint16)0,
        .state_e = CL42T_MOTOR_STATE_NB
    };
    t_uint32 currentTime_u32 = (t_uint32)0;
    t_uint8 safeCnt_u8 = (t_uint8)0;
    t_bool isCmdSend_b = (t_bool)FALSE;
    t_sint32 deltaTime_s32 = 0;

    if(f_MotorInfo_ps == (t_sCL42T_MotorInfo *)NULL)
    {
        ASSERT((t_uint16)0);
        Ret_e = RC_ERROR_PTR_NULL;
    }
    else 
    {
        Ret_e = RC_OK;
        FMKCPU_GetTick(&currentTime_u32);
        //---- basically, do something only if motor is OFF ----//
        if((GETBIT(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ON) == BIT_IS_RESET_16B)
        && (f_MotorInfo_ps->Health_e == CL42T_DIAGNOSTIC_OK))
        {
            //---- deadtime managment ---//
            if(GETBIT(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_IN_DEAD_TIME) == BIT_IS_SET_16B)
            {
                if((currentTime_u32 - f_MotorInfo_ps->InhibTime_u32) > CL42T_DEAD_TIME_TRANSITION)
                {
                    //---- erase bit status and try to make the command ----//
                    RESETBIT_16B(f_MotorInfo_ps->maskInfo_u16,CL42T_BITFIELD_IN_DEAD_TIME);
                }
                else 
                {
                    Ret_e = RC_WARNING_BUSY;    
                }
            }
            if(Ret_e == RC_OK)
            {
                //---- we set the safe cnt to Queue Size because if less, 
                //      there is possibility that user set all pulses to the same direction
                //      and when the bit enstop CW or CCW are ON, every command are dropped     
                //      so if we dropped all the queue, it's normal' but more than Queue it's not normal ----//
                while((safeCnt_u8 < (t_uint8)CL42T_CMD_QUEUE_SIZE) 
                && (Ret_e == RC_OK) 
                && isCmdSend_b == (t_bool)FALSE)
                {
                    safeCnt_u8++;
                    //---- get an element from the queue, Lib will return 
                    //          NO_OPERATION if Queue is empty which is fine
                    //      We pop an element and the queue and delete it if we aplly the command ----//
                    Ret_e = LIBQUEUE_PopElement(&f_MotorInfo_ps->HwCmdFifo_s, 
                                                &hwSigCmd_s,  
                                                sizeof(hwSigCmd_s));
                    if(Ret_e == RC_OK)
                    {
                        //---- here we check that we haven't a collision with a Enstop
                        //      and the pulse commanded are in the direction that trigger the collision ----//
                        if(((GETBIT(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CW) == BIT_IS_SET_16B)
                        && ((hwSigCmd_s.direction_e == f_MotorInfo_ps->endStoptrigger_e)))
                        || ((GETBIT(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CCW) == BIT_IS_SET_16B)
                        && ((hwSigCmd_s.direction_e == f_MotorInfo_ps->endStoptrigger_e))))
                        {
                            CL42T_LOG("[CL42T] for motor %d, Trigger End Stop ON, and pulse in this sens, abort cmd\r\n", (t_uint16)f_MotorInfo_ps->selfId_e);
                            //---- we dropp that sequence and call user with the number of pulse dropped ----//
                            if(f_MotorInfo_ps->pulseDroppCallback_pcb != NULL_FUNCTION)
                            {
                                f_MotorInfo_ps->pulseDroppCallback_pcb( f_MotorInfo_ps->selfId_e, 
                                                                        hwSigCmd_s.nbPulses_u32,
                                                                        hwSigCmd_s.direction_e);
                            }
                            //---- we dropped the command and will not be executed ----//
                            //---- Erase the Queue Element ----//
                            (void)LIBQUEUE_ReadElement(&f_MotorInfo_ps->HwCmdFifo_s, &hwSigCmd_s, sizeof(hwSigCmd_s));
                        }
                        else
                        {
                            //---- check any changement of direction ----//
                            if((f_MotorInfo_ps->enableDeadtime_b == (t_bool)TRUE)
                            &&(hwSigCmd_s.direction_e != 
                                    (t_eCL42T_MotorDirection)GETBIT(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFILED_MOTOR_DIR)))
                            {
                                //--- maybe check because the deadtime may have passed if there was a collision or something ----//
                                if((currentTime_u32 - f_MotorInfo_ps->InhibTime_u32) > CL42T_DEAD_TIME_TRANSITION)
                                {
                                    Ret_e = RC_OK;
                                    //--- it is not ON but just making sure ----//
                                    RESETBIT_16B(f_MotorInfo_ps->maskInfo_u16,CL42T_BITFIELD_IN_DEAD_TIME);
                                }
                                else 
                                {
                                    SETBIT_16B(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_IN_DEAD_TIME);
                                    //---- to get out of while loop 'cause we have to wait ----//
                                    Ret_e = RC_WARNING_BUSY;
                                }
                                //--- not erase the fifo element 'cause this one is valid 
                                //      we just cant' set the command, we will retry later ----//
                            }
                            //---- check any planning schedule for this pulse ----//
                            if(Ret_e == RC_OK)
                            {
                                deltaTime_s32 = (t_sint32)(currentTime_u32 - hwSigCmd_s.triggerTimer_u32);
                                //---- < 0 means not yet ready to send ----//
                                if(deltaTime_s32 < (t_sint32)0)
                                {
                                    Ret_e = RC_WARNING_PENDING;
                                }
                                //else > 0 means go for sending 

                            }
                            //---- finally apply the command ^^ ----//
                            if(Ret_e == RC_OK) 
                            {
                                //---- send command ----//
                                Ret_e = s_CL42T_SendHwCommand(f_MotorInfo_ps, &hwSigCmd_s);                               
                                
                                if((Ret_e == RC_OK)
                                || (Ret_e == RC_WARNING_ALREADY_CONFIGURED))
                                {
                                    CL42T_LOG("[CL42T] Send cmd to Motor %d, freq %d, pulses %d, dir %d\r\n",
                                                (t_uint32)f_MotorInfo_ps->selfId_e,
                                                (t_uint32)hwSigCmd_s.frequency_f32,
                                                hwSigCmd_s.nbPulses_u32,
                                                (t_uint32)hwSigCmd_s.direction_e);
                                                
                                    //---- update flag ----//
                                    isCmdSend_b = (t_bool)TRUE;
                                    f_MotorInfo_ps->startPulseTime_u32 = currentTime_u32;

                                    if(hwSigCmd_s.nbPulses_u32 != (t_sint32)0)
                                    {
                                        if(hwSigCmd_s.nbPulses_u32 == CL42T_SEND_INFINITE_PULSE)
                                        {
                                            f_MotorInfo_ps->estimPulseTime_f32 = (t_float32)(CST_MAX_UINT_32BIT);
                                        }
                                        else
                                        {
                                            f_MotorInfo_ps->estimPulseTime_f32 = (t_float32)hwSigCmd_s.nbPulses_u32 / hwSigCmd_s.frequency_f32;
                                            f_MotorInfo_ps->estimPulseTime_f32 *= (t_float32)1000.0f; // let in ms
                                        }

                                        if(hwSigCmd_s.direction_e == CL42T_MOTOR_DIRECTION_CW)
                                        {
                                            RESETBIT_16B(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFILED_MOTOR_DIR);    
                                        }
                                        else // CL42T_MOTOR_DIRECTION_CCW
                                        {
                                            SETBIT_16B(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFILED_MOTOR_DIR);    
                                        }

                                        SETBIT_16B(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ON);
                                        RESETBIT_16B(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CW);
                                        RESETBIT_16B(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CCW);
                                        f_MotorInfo_ps->endStoptrigger_e = CL42T_MOTOR_DIRECTION_NB;
                                    }
                                    else // nb pulse = 0
                                    {
                                        RESETBIT_16B(f_MotorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ON);
                                    }
                                    
                                    //---- deleted the Command in Fifo ----//
                                    (void)LIBQUEUE_ReadElement(&f_MotorInfo_ps->HwCmdFifo_s, NULL, sizeof(hwSigCmd_s));
                                }
                                else if(Ret_e != RC_WARNING_BUSY)
                                {
                                    ASSERT((t_uint16)Ret_e);
                                }
                            }
                        }
                    }
                }
                //--- this condition should never happened 
                //      cause either we set the cmd or lib queue return WARNNING NO OPE----//
                if((Ret_e < RC_OK)
                || (safeCnt_u8 >= (t_uint8)CL42T_CMD_QUEUE_SIZE))
                {
                    ASSERT((t_uint16)Ret_e);
                } 
            }
        }
        //--- else the motor is still turning so we do nothing ----//
        else if(f_MotorInfo_ps->Health_e == CL42T_DIAGNOSTIC_OK)
        {
            //---- pulse controle managment ----//
            if((currentTime_u32 - f_MotorInfo_ps->startPulseTime_u32) > (t_uint32)(f_MotorInfo_ps->estimPulseTime_f32 + 10.0f))
            {
                ASSERT((t_uint16)(currentTime_u32 - f_MotorInfo_ps->startPulseTime_u32));
                CL42T_LOG(  "[CL42T] Motor %d, Overflow of pulse detected, expected to last %d but %d ms has passed\r\n",
                            f_MotorInfo_ps->selfId_e,
                            (t_uint32)f_MotorInfo_ps->estimPulseTime_f32,
                            (currentTime_u32 - f_MotorInfo_ps->startPulseTime_u32));
                f_MotorInfo_ps->flagErrorDetected_b = (t_bool)TRUE;
                f_MotorInfo_ps->Health_e = CL42T_DIAGNOSTIC_PULSE_INFINITE;
            }

            Ret_e = RC_WARNING_NO_OPERATION;
        }
    }    

    return Ret_e;
}

/*********************************
 * s_CL42T_GetStateSignal
 *********************************/
static t_eReturnCode s_CL42T_GetStateSignal(   t_sCL42T_MotorInfo * f_motorInfo_ps,
                                        t_eCL42T_MotorState *f_state_pe)
{
    t_eReturnCode Ret_e = RC_OK;
    t_sCL42T_MotorSignalInfo * SigInfo_ps;
    t_eFMKIO_DigValue digValue_e;

    if(f_motorInfo_ps == (t_sCL42T_MotorInfo *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        SigInfo_ps = (t_sCL42T_MotorSignalInfo *)(&f_motorInfo_ps->signalId_au8[CL42T_SIGTYPE_STATE]);

        Ret_e = FMKIO_Get_OutDigSigValue(SigInfo_ps->signal_u8, &digValue_e);

        if(Ret_e == RC_OK)
        {
            switch(digValue_e)
            {
                case FMKIO_DIG_VALUE_LOW:
                    *f_state_pe = CL42T_MOTOR_STATE_ON;
                    break;
                case FMKIO_DIG_VALUE_HIGH:
                    *f_state_pe = CL42T_MOTOR_STATE_OFF;
                    break;
                case FMKIO_DIG_VALUE_NB:
                default:
                    *f_state_pe = CL42T_MOTOR_STATE_ON;
                    Ret_e = RC_ERROR_PARAM_NOT_SUPPORTED;
                    break;

            }
        }
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_CounterDiagMngmt
 *********************************/
static t_eReturnCode s_CL42T_CounterDiagMngmt(t_eCL42T_MotorId f_motorId_e, t_uint8 * f_counter_pu8)
{
    t_eReturnCode Ret_e = RC_OK;
    t_float32 counter_f32 = 0.0f;
    t_uint32 counter_u32;
    t_uint32 currentTime_u32;
    t_bool endReception_b = (t_bool)False;

    // verif 
    if(f_motorId_e >= CL42T_MOTOR_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)0);
    }
    else if(f_counter_pu8 == (t_uint8 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)0);
    }
    else 
    {
        Ret_e = FMKIO_Get_InFreqSigValue(   g_MotorInfo_as[f_motorId_e].signalId_au8[CL42T_SIGTYPE_DIAG],
                                            &counter_f32);
        if(Ret_e == RC_OK)
        {
            counter_u32 = (t_uint32)(counter_f32 + 0.5f);

            if(g_diagMngmt_as[f_motorId_e].flagReception_b == (t_bool)TRUE)
            {
                    if(counter_u32 > (t_uint32)0)
                    {
                        if(g_diagMngmt_as[f_motorId_e].startTime_u32 > (t_uint32)0)
                        {
                            g_diagMngmt_as[f_motorId_e].startTime_u32 = (t_uint32)0;
                        }

                        g_diagMngmt_as[f_motorId_e].counterDiag_u16 += (t_uint16)counter_u32;
                    }
                    else 
                    {
                        FMKCPU_GetTick(&currentTime_u32);

                        if((currentTime_u32 - g_diagMngmt_as[f_motorId_e].startTime_u32) > (t_uint32)1500)
                        {
                            endReception_b = (t_bool)True;
                            *f_counter_pu8 = (t_uint8)g_diagMngmt_as[f_motorId_e].counterDiag_u16;
                            g_diagMngmt_as[f_motorId_e].counterDiag_u16 = (t_uint16)0;
                            g_diagMngmt_as[f_motorId_e].startTime_u32 = (t_uint32)0;
                            g_diagMngmt_as[f_motorId_e].flagReception_b = (t_bool)False;
                        }
                    }
            }
            else
            {
                if(counter_u32 > (t_uint32)0)
                {
                    g_diagMngmt_as[f_motorId_e].counterDiag_u16 += (t_uint16)counter_u32;
                    FMKCPU_GetTick(&g_diagMngmt_as[f_motorId_e].startTime_u32);
                    g_diagMngmt_as[f_motorId_e].flagReception_b = (t_bool)True;

                }
            }
            if(endReception_b == (t_bool)False)
            {
                *f_counter_pu8 = (t_uint8)0;
            }
        }
    }

    return Ret_e;
}


/*********************************
 * s_CL42T_DebugUpdateSignal
 *********************************/
static t_eReturnCode s_CL42T_DebugUpdateSignal(t_sCL42T_MotorInfo * f_motorInfo_ps)
{
    t_eReturnCode Ret_e;
    const t_sCL42T_MtrDebugInfo * mtrSigInfo_ps;

    if(f_motorInfo_ps == (t_sCL42T_MotorInfo *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)0);
    }
    else 
    {
        mtrSigInfo_ps = &c_CL42T_SigMtrDebug[f_motorInfo_ps->selfId_e];

        Ret_e = APPSIG_SetSignalValue(  mtrSigInfo_ps->mtrState_e,
                                        (t_float32)GETBIT(f_motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_MOTOR_ON));

        Ret_e |= APPSIG_SetSignalValue(  mtrSigInfo_ps->cwEndStop_e,
                                        (t_float32)GETBIT(f_motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CW));

        Ret_e |= APPSIG_SetSignalValue(  mtrSigInfo_ps->ccwEndStop_e,
                                        (t_float32)GETBIT(f_motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_TRIG_ENDSTOP_CCW));

        Ret_e |= APPSIG_SetSignalValue(  mtrSigInfo_ps->deadTime_e,
                                        (t_float32)GETBIT(f_motorInfo_ps->maskInfo_u16, CL42T_BITFIELD_IN_DEAD_TIME));

        Ret_e |= APPSIG_SetSignalValue(  mtrSigInfo_ps->direction_e,
                                        (t_float32)GETBIT(f_motorInfo_ps->maskInfo_u16, CL42T_BITFILED_MOTOR_DIR));

        Ret_e |= APPSIG_SetSignalValue(  mtrSigInfo_ps->health_e,
                                        (t_float32)f_motorInfo_ps->Health_e);
    }

    return Ret_e;
}

/*********************************
 * s_CL42T_DroppAllPulses
 *********************************/
static t_eReturnCode s_CL42T_DroppAllPulses(t_sCL42T_MotorInfo * f_motorInfo_ps)
{
    t_eReturnCode Ret_e;
    t_uint8 LLI_u8;
    t_sCL42T_HwSignalCmd hwSigCmd_s = {
        .direction_e = CL42T_MOTOR_DIRECTION_NB,
        .frequency_f32 = 0.0f,
        .nbPulses_u32 = (t_uint16)0,
        .state_e = CL42T_MOTOR_STATE_NB
    };

    if(f_motorInfo_ps == NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)0);
    }
    else
    {
        LLI_u8 = 0;
        Ret_e = RC_OK;
        while((Ret_e == RC_OK)
        &&    (LLI_u8 < CL42T_CMD_QUEUE_SIZE))
        {
            LLI_u8++;
            Ret_e = LIBQUEUE_ReadElement(&f_motorInfo_ps->HwCmdFifo_s, 
                                        &hwSigCmd_s,  
                                        sizeof(hwSigCmd_s));
            
            if(Ret_e == RC_OK)
            {
                CL42T_LOG("[CL42T] Motor Id %d, dropp %d pulse in %d direction\r\n",
                            f_motorInfo_ps->selfId_e,
                            hwSigCmd_s.nbPulses_u32,
                            hwSigCmd_s.direction_e);

                if(f_motorInfo_ps->pulseDroppCallback_pcb != NULL_FUNCTION)
                {
                    f_motorInfo_ps->pulseDroppCallback_pcb( f_motorInfo_ps->selfId_e,
                                                            (t_uint32)hwSigCmd_s.nbPulses_u32,
                                                            hwSigCmd_s.direction_e);
                }
            }
            //---- no pulse to dropp -> OK ----//
            else if(Ret_e == RC_WARNING_NO_OPERATION)
            {
                Ret_e = RC_OK;
            }
        }
    }

    return Ret_e;
}
//************************************************************************************
// End of File
//************************************************************************************

/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */
