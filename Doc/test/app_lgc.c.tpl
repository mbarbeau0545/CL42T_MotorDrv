/*********************************************************************
 * @file        APP_LGC.c
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */






// ********************************************************************
// *                      Includes
// ********************************************************************
#include "stdio.h"
#include "string.h"

#include "./APP_LGC.h"
#include "./APP_CTRL/APP_SYS/Src/APP_SYS.h"
#include "APP_CFG/ConfigFiles/APPLGC_ConfigPrivate.h"
#include "3_APP/APP_CTRL/APP_SPM/Src/APP_SPM.h"
#include "FMK_HAL/FMK_IO/Src/FMK_IO.h"
#include "FMK_HAL/FMK_HRT/Src/FMK_HRT.h"
#include "FMK_HAL/FMK_CAN/Src/FMK_FDCAN.h"
#include "APP_CTRL/APP_SIG/Src/APP_SIG.h"

#include "Library/SafeMem/SafeMem.h"
#include "CL42T/Src/CL42T.h"
// ********************************************************************
// *                      Defines
// ********************************************************************

// ********************************************************************
// *                      Types
// ********************************************************************
/* CAUTION : Automatic generated code section for Enum: Start */

/* CAUTION : Automatic generated code section for Enum: End */
//-----------------------------ENUM TYPES-----------------------------//


/* CAUTION : Automatic generated code section for Structure: Start */

/* CAUTION : Automatic generated code section for Structure: End */
//-----------------------------STRUCT TYPES---------------------------//

/* CAUTION : Automatic generated code section : Start */

/* CAUTION : Automatic generated code section : End */
//-----------------------------TYPEDEF TYPES---------------------------//
// ********************************************************************
// *                      Prototypes
// ********************************************************************

// ********************************************************************
// *                      Variables
// ********************************************************************
RTC_HandleTypeDef g_rtcHandle_s;
/**
* @brief App Logic Module State
*/
static t_eCyclicModState g_AppLgc_ModState_e = STATE_CYCLIC_CFG;
/**
* @brief Structure for Service Information 
*/
static t_sAPPLGC_ServiceInfo g_srvFuncInfo_as[APPLGC_SRV_NB];
/**
* @brief Container for Sensors Values
*/
//static t_float32 g_snsValues_af32[APPSNS_SNS_NB];
static t_float32 g_snsValues_af32[2];

/**
* @brief Flag to Reset Service State
*/
static t_bool  g_resetSrvState_b = (t_bool)TRUE; 

t_sSafeMem_BlockInfo g_SecBlockSnsValue_as[2];
/* CAUTION : Automatic generated code section for Variable: Start */
/* CAUTION : Automatic generated code section for Variable: End */
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
static t_eReturnCode s_APPLGC_PreOperational(void);
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
static t_eReturnCode s_APPLGC_Operational(void);
/**
*
*	@brief
*	@note
*	 
*
*
*/
static t_eReturnCode s_APPLGC_ConfigurationState(void);
/**
*
*	@brief      Get Sensors Values.\n
*
*/

static t_eReturnCode s_APPLGC_GetSnsValues(void);
/**
*
*	@brief      Set Actuators Values Depending on g_srvFuncInfo_as
*/
static t_eReturnCode s_APPLGC_SetActValues(void);
/**
*
*	@brief
*	@note   
*
*
*/
static t_eReturnCode s_APPLGC_ResetSrvState(void);
/**
*
*	@brief
*	@note   
*
*
*/
static t_eReturnCode s_APPLGC_SetActValues(void);
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
static void s_APPLGC_DiagnosticEvent(   t_eAPPSDM_DiagnosticItem f_item_e,
                                        t_eAPPSDM_DiagnosticReport f_reportState_e,
                                        t_uint16 f_debugInfo1_u16,
                                        t_uint16 f_debugInfo2_u16);

static void s_APPLGC_CanCallback(   t_eFMKFDCAN_NodeList f_Node_e,
                                    t_sFMKFDCAN_RxItemEvent f_RxItem_s, 
                                    t_eFMKFDCAN_NodeStatus f_NodeStatus_e);

static void s_APPLGC_CanCallback_2(   t_eFMKFDCAN_NodeList f_Node_e,
                                    t_sFMKFDCAN_RxItemEvent f_RxItem_s, 
                                    t_eFMKFDCAN_NodeStatus f_NodeStatus_e);

static void s_APPLGC_MotorDiag(t_eCL42T_MotorId f_MotorID_e, t_eCL42T_DiagError f_defaultInfo_e);
static void s_APPLGC_PulseDropped(t_eCL42T_MotorId f_MotorID_e, 
                                    t_uint16 f_pulseDropped_u16, 
                                    t_eCL42T_MotorDirection f_direction_e);
static void s_APPLGC_PulseFinished(t_eFMKIO_OutPwmSig f_pwmSig_e);
static void s_APPLGC_FastTask(void);
//****************************************************************************
//                      Public functions - Implementation
//********************************************************************************

// ********************************************************************
// *                      Variables
// ********************************************************************

/*********************************
 * APPLGC_Init
 *********************************/
t_eReturnCode APPLGC_Init(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxAgent_u8 = (t_uint8)0; 
    t_uint8 idxSrv_u8 = (t_uint8)0;
    Ret_e = CL42T_Init();
    

    /* CAUTION : Automatic generated code section for Actuators Containers/Service: Start */
    /* CAUTION : Automatic generated code section for Actuators Containers/Service: End */

    //----- Set Service Init -----//
    for(idxSrv_u8 = (t_uint8)0 ; idxSrv_u8 < APPLGC_SRV_NB ; idxSrv_u8++)
    {

        g_srvFuncInfo_as[idxSrv_u8].health_e = APPLGC_SRV_HEALTH_OK;
        g_srvFuncInfo_as[idxSrv_u8].state_e = APPLGC_SRV_STATE_NB;
        
    }

    //---- Set Agent Init -----//
    for(idxAgent_u8 = (t_uint8)0 ; (idxAgent_u8 < APPLGC_AGENT_NB) && (Ret_e == RC_OK) ; idxAgent_u8++)
    {
        Ret_e = c_AppLGc_AgentInfo_as[idxAgent_u8].init_pcb();
    }

    //Ret_e = APPSDM_AddCallbackEvnt(s_APPLGC_DiagnosticEvent);

    return Ret_e;
}

/*********************************
 * APPLGC_Cyclic
 *********************************/
t_eReturnCode APPLGC_Cyclic(void)
{
    t_eReturnCode Ret_e = RC_OK;
    // code to run every x milliseconds, config in APPSYS_ConfigPrivate.h

    switch (g_AppLgc_ModState_e)
    {
    case STATE_CYCLIC_CFG:
    {
        Ret_e = s_APPLGC_ConfigurationState();
        if(Ret_e == RC_OK)
        {
            g_AppLgc_ModState_e = STATE_CYCLIC_WAITING;
        }
        break;
    }

    case STATE_CYCLIC_WAITING:
    {
        // nothing to do, just wait all module are Ope
        break;
    }
    case STATE_CYCLIC_PREOPE:
    {
        Ret_e = s_APPLGC_PreOperational();

        if(Ret_e == RC_OK)
        {
            g_AppLgc_ModState_e = STATE_CYCLIC_OPE;
        }
    
        break;
    }
    case STATE_CYCLIC_OPE:
    {
        Ret_e = s_APPLGC_Operational();
        break;
    }
    case STATE_CYCLIC_ERROR:
    {
        break;
    }
    case STATE_CYCLIC_BUSY:
    default:
        Ret_e = RC_OK;
        break;
    }
    return Ret_e;
}

/*********************************
 * APPLGC_GetState
 *********************************/
t_eReturnCode APPLGC_GetState(t_eCyclicModState *f_State_pe)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_State_pe == (t_eCyclicModState *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        *f_State_pe = g_AppLgc_ModState_e;
    }

    return Ret_e;
}

/*********************************
 * APPLGC_SetState
 *********************************/
t_eReturnCode APPLGC_SetState(t_eCyclicModState f_State_e)
{

    g_AppLgc_ModState_e = f_State_e;

    return RC_OK;
}

/*********************************
 * APPLGC_SetServiceHealth
 *********************************/
t_eReturnCode APPLGC_SetServiceHealth(t_eAPPLGC_SrvList f_service_e, t_eAPPLGC_SrvHealth f_srvHealth_e)
{
    t_eReturnCode Ret_e = RC_OK;

    if((f_service_e >= APPLGC_SRV_NB)
    || (f_srvHealth_e >= APPLGC_SRV_HEALTH_NB))
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(Ret_e == RC_OK)
    {
        //----- set Health state -----//
        g_srvFuncInfo_as[f_service_e].health_e = f_srvHealth_e; 
    }

    return Ret_e;
}

/*********************************
 * APPLGC_SetServiceHealth
 *********************************/
t_eReturnCode APPLGC_GetServiceHealth(t_eAPPLGC_SrvList f_service_e, t_eAPPLGC_SrvHealth * f_srvHealth_pe)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_service_e >= APPLGC_SRV_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_srvHealth_pe == (t_eAPPLGC_SrvHealth *)NULL)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(Ret_e == RC_OK)
    {
        *f_srvHealth_pe = g_srvFuncInfo_as[f_service_e].health_e;
    }

    return Ret_e;
}

/*********************************
 * APPLGC_GetSnsValue
 *********************************/
t_eReturnCode APPLGC_GetSnsValue(t_eAPPSNS_SnsInterface f_sensors_e, t_sint32 * f_snsValue_ps32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_sensors_e >= APPSNS_SNS_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)f_sensors_e);
    }
    if(f_snsValue_ps32 == (t_sint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)0);
    }
    if(Ret_e == RC_OK)
    {
        *f_snsValue_ps32 = (t_sint32)g_snsValues_af32[f_sensors_e];
    }

    return Ret_e;
}
//********************************************************************************
//                      Local functions - Implementation
//********************************************************************************
static void s_APPLGC_AppEvntCallback(   t_uint8 * f_rxData_pu8, 
                                        t_uint16 f_dataSize_u16, 
                                        t_eFMKSRL_RxCallbackInfo f_InfoCb_e);

static void s_APPLGC_AppEvntCallback(   t_uint8 * f_rxData_pu8, 
                    t_uint16 f_dataSize_u16, 
                    t_eFMKSRL_RxCallbackInfo f_InfoCb_e)
{
    return;
}
/*********************************
 * s_APPLGC_ConfigurationState
 *********************************/
static t_eReturnCode s_APPLGC_ConfigurationState(void)
{

    t_eReturnCode Ret_e;
    HAL_StatusTypeDef bspRet_e;
    // t_sFMKIO_PwmWaveformCfg pwmWaveForm = {
    //     .deadTime_u32 = 0,
    //     .frequency_f32 = 2000,
    //     .polarity_e = FMKIO_SIGPWM_POLARITY_LOW,
    //     .pullMode_e = FMKIO_PULL_MODE_DISABLE,
    //     .spdMode_e = FMKIO_SPD_MODE_HIGH,
    // };
    // t_sFMKIO_PwmControlPrm pwmCtrl_s = {
    //     .ctrlType_e = FMKIO_PWM_CTRL_TYPE_UNUSED,
    //             .rampCfg_ps = NULL,
    //             .enablePulseSyncOpe_b = TRUE,
    // };

    // Ret_e = FMKIO_Set_OutPwmSigCfg(FMKIO_OUTPUT_SIGPWM_8, pwmWaveForm, pwmCtrl_s, s_APPLGC_PulseFinished, NULL_FUNCTION);
    // if(Ret_e == RC_OK)
    // {
    //     Ret_e = FMKIO_Set_OutPwmSigCfg(FMKIO_OUTPUT_SIGPWM_7, pwmWaveForm, pwmCtrl_s, s_APPLGC_PulseFinished, NULL_FUNCTION);
    // }
    t_sCL42T_MotorSigCfg motor_config = {
        .DiagSignal_e = FMKIO_INPUT_SIGFREQ_1,
        .DirSignal_e = FMKIO_OUTPUT_SIGDIG_1,
        .StateSignal_e = FMKIO_OUTPUT_SIGDIG_2,
        .EndStopSigCW_s = {
            .EndStopSignal_e = FMKIO_INPUT_SIGEVNT_1,
            .PullMode_e = FMKIO_PULL_MODE_UP,
            .triggerEvnt_e = FMKIO_STC_RISING_EDGE
        },
        .EndStopSigCCW_s = {
            .EndStopSignal_e = FMKIO_INPUT_SIGEVNT_2,
            .PullMode_e = FMKIO_PULL_MODE_UP,
            .triggerEvnt_e = FMKIO_STC_RISING_EDGE
        },
        .PulseSigCfg_s = {
            .PulseSignal_e = FMKIO_OUTPUT_SIGPWM_7,
            .pwmWaveForm_s = {
                .deadTime_u32 = 0,
                .frequency_f32 = 1000,
                .polarity_e = FMKIO_SIGPWM_POLARITY_LOW,
                .pullMode_e = FMKIO_PULL_MODE_DISABLE,
                .spdMode_e = FMKIO_SPD_MODE_HIGH,
            },
            .pwmCtrlPrm_s = {
                .ctrlType_e = FMKIO_PWM_CTRL_TYPE_UNUSED,
                .rampCfg_ps = NULL,
                .enablePulseSyncOpe_b = TRUE
            }
        }

    };

        t_sCL42T_MotorSigCfg motor2_config = {
        .DiagSignal_e = FMKIO_INPUT_SIGFREQ_2,
        .DirSignal_e = FMKIO_OUTPUT_SIGDIG_3,
        .StateSignal_e = FMKIO_OUTPUT_SIGDIG_4,
        .EndStopSigCW_s = {
            .EndStopSignal_e = FMKIO_INPUT_SIGEVNT_3,
            .PullMode_e = FMKIO_PULL_MODE_DOWN,
            .triggerEvnt_e = FMKIO_STC_RISING_EDGE,
            .debuncValue_u16 = 0xFFFF,
        },
        .EndStopSigCCW_s = {
            .EndStopSignal_e = FMKIO_INPUT_SIGEVNT_4,
            .PullMode_e = FMKIO_PULL_MODE_DOWN,
            .triggerEvnt_e = FMKIO_STC_RISING_EDGE,
            .debuncValue_u16 = 0xFFFF,
        },
        .PulseSigCfg_s = {
            .PulseSignal_e = FMKIO_OUTPUT_SIGPWM_8,
            .pwmWaveForm_s = {
                .deadTime_u32 = 0,
                .frequency_f32 = 1000,
                .polarity_e = FMKIO_SIGPWM_POLARITY_LOW,
                .pullMode_e = FMKIO_PULL_MODE_DISABLE,
                .spdMode_e = FMKIO_SPD_MODE_HIGH,
            },
            .pwmCtrlPrm_s = {
                .ctrlType_e = FMKIO_PWM_CTRL_TYPE_UNUSED,
                .rampCfg_ps = NULL,
                .enablePulseSyncOpe_b = TRUE
            }
        }
    };
    
    Ret_e = CL42T_AddMotorConfiguration(CL42T_MOTOR_1,motor_config, TRUE,
                                        s_APPLGC_MotorDiag,
                                        s_APPLGC_PulseDropped);
    Ret_e |= CL42T_AddMotorConfiguration(CL42T_MOTOR_2,motor2_config, TRUE,
                                        s_APPLGC_MotorDiag,
                                        s_APPLGC_PulseDropped);
    Ret_e |= APPSYS_AddFastTask(APPSYS_MODULE_APP_LGC, s_APPLGC_FastTask);
    
    

    return Ret_e;
}

/*********************************
 * s_APPLGC_ConfigurationState
 *********************************/
static t_eReturnCode s_APPLGC_PreOperational(void)
{
    t_eReturnCode Ret_e = RC_OK;

    Ret_e = APPSYS_SetFastTaskState(APPSYS_MODULE_APP_LGC, APPSYS_FAST_TASK_ENABLE);
    if(Ret_e < RC_OK)
    {
        ASSERT((t_uint16)Ret_e);
    }

    return Ret_e;
}
/*********************************
 * s_APPLGC_Operational
 *********************************/
typedef enum 
{
    APPLGC_MOTOR_STATE_SET_CMD,
    APPLGC_MOTOR_STATE_WAIT,
} t_eAPPLGC_MotorSts;

t_sint16 factorPulses = 1;
t_sint16 g_factdir_u8 = 1;
static t_eReturnCode s_APPLGC_Operational(void)
{
    static t_uint8 idxPulses_u8 = 0;
    static t_bool setPerturb_b = FALSE;
    t_eReturnCode Ret_e = RC_OK; 
    
    // if(g_resetSrvState_b == TRUE)
    // {
    //     g_resetSrvState_b = FALSE;
    //     Ret_e = FMKIO_Set_OutPwmSigPulses(FMKIO_OUTPUT_SIGPWM_7, 1000, 500, 500);  
    //     Ret_e = FMKIO_Set_OutPwmSigPulses(FMKIO_OUTPUT_SIGPWM_8, 1000, 500, 500);  
    // }
    //Ret_e = FMKIO_Set_OutPwmSigDutyCycle(FMKIO_OUTPUT_SIGPWM_15, 500);
    t_uint16 mototbitSts_u16;
    t_uint16 nbcmd_ua16[6] = {200,500,100,400,200,1500};
    t_uint16 nbcfreq_ua16[6] = {2000,5000,1000,4000,2000,850};

    
    t_sCL42T_SetMotorValue motorValue_s = {
        .frequency_u32 = 1000,
        .nbPulses_s32  = 600,
    };
    t_float32 anaMeasure_f32;
    static t_eAPPLGC_MotorSts motorSs_e = APPLGC_MOTOR_STATE_SET_CMD;

    switch (motorSs_e)
    {
        case APPLGC_MOTOR_STATE_SET_CMD:
            setPerturb_b = FALSE;
            for ( ; (idxPulses_u8 < 6) && (Ret_e == RC_OK) ; )
            {
                motorValue_s.nbPulses_s32 = factorPulses * nbcmd_ua16[idxPulses_u8] * g_factdir_u8;
                motorValue_s.frequency_u32 = nbcfreq_ua16[idxPulses_u8];
                Ret_e = CL42T_SetMotorSigValue(CL42T_MOTOR_1, motorValue_s);
                Ret_e = CL42T_SetMotorSigValue(CL42T_MOTOR_2, motorValue_s);

                if(Ret_e == RC_OK)
                {
                    idxPulses_u8++;
                }

            }

            if((Ret_e == RC_OK) || (RC_WARNING_BUSY) || (Ret_e == RC_WARNING_LIMIT_REACHED))
            {
                FMKSRL_LOG("[LGC] CHange state waiting\r\n");
                motorSs_e = APPLGC_MOTOR_STATE_WAIT;
            }

            if(idxPulses_u8 >= 6)
            {
                idxPulses_u8 = 0;
            }
            FMKSRL_LOG("[LGC] Set command, retcode : %d\r\n", Ret_e);
            break;
        case APPLGC_MOTOR_STATE_WAIT:
            Ret_e = CL42T_GetMotorInfo(CL42T_MOTOR_1, &mototbitSts_u16);

            if(Ret_e == RC_OK)
            {
                if(GETBIT(mototbitSts_u16, CL42T_BITFIELD_MOTOR_ON) == BIT_IS_RESET_16B)
                {
                    FMKSRL_LOG("[LGC] Motor OFF, change state to setcmd\r\n");
                    factorPulses *= (t_sint8)-1;
                    motorSs_e = APPLGC_MOTOR_STATE_SET_CMD;
                }
                else 
                {
                    if(setPerturb_b == FALSE)
                    {
                        CL42T_Test_SetPerturb(CL42T_MOTOR_1, FALSE);
                        CL42T_Test_SetPerturb(CL42T_MOTOR_2, FALSE);
                        FMKSRL_LOG("[LGC] : Set Perturbation\r\n");
                        setPerturb_b = TRUE;
                    }
                }
            }
        break;
        default:
        break;

    }
           /*t_uint8 idxAgent_u8;
        
        if(g_resetSrvState_b == (t_bool)True)
        {
            Ret_e = s_APPLGC_ResetSrvState();
            if(Ret_e == RC_OK)
        {
            g_resetSrvState_b = (t_bool)False;
        }
    }
    if(Ret_e == RC_OK)
    {
        //------ Get Sensors Values for this cyclic -----//
        Ret_e = s_APPLGC_GetSnsValues();
    }

    //----- Call Agent Periodic Task Depending on Coordinator -----//
    if(Ret_e == RC_OK)
    {   
        for(idxAgent_u8 = (t_uint8)0 ; (idxAgent_u8 < APPLGC_AGENT_NB) &&  (Ret_e >= RC_OK) ; idxAgent_u8++)
        {
            Ret_e = c_AppLGc_AgentInfo_as[idxAgent_u8].PeriodTask_pcb( (t_float32 *)g_snsValues_af32,
                                                                        (t_sAPPLGC_ServiceInfo *)g_srvFuncInfo_as);
        }
    }

    if(Ret_e >= RC_OK)
    { 
        Ret_e = s_APPLGC_SetActValues();
    }*/

}
/*********************************
 * s_APPLGC_GetSnsValues
 *********************************/
static t_eReturnCode s_APPLGC_GetSnsValues(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_sAPPSNS_SnsValueInfo snsInfo_s;
    t_uint8 idxSns_u8 = (t_uint8)0;

    for(idxSns_u8 = (t_uint8)0 ; (idxSns_u8 < APPSNS_SNS_NB) && (Ret_e == RC_OK) ; idxSns_u8++)
    {
        //----- Reset Container values -----//
        snsInfo_s.isValueOK_b = (t_bool)False;
        snsInfo_s.rawValue_f32 = (t_float32)0.0;
        snsInfo_s.SnsValue_f32 = (t_float32)0.0;

        Ret_e = APPSNS_Get_SnsValue((t_eAPPSNS_SnsInterface)idxSns_u8, &snsInfo_s);

        if((Ret_e == RC_OK)
        && (snsInfo_s.isValueOK_b == (t_bool)true))
        {
            g_snsValues_af32[idxSns_u8] = snsInfo_s.SnsValue_f32;
        }
    }
    
    return Ret_e;
}


/*********************************
 * s_APPLGC_SetActValues
 *********************************/
static t_eReturnCode s_APPLGC_SetActValues(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxSrv_u8 = (t_uint8)0;
    t_uint8 idxAct_u8 = (t_uint8)0;
    t_eAPPACT_ActInterface actuatorLabel_e;

    //----- Loop on every Service -----//
    for(idxSrv_u8 = (t_uint8)0 ; (idxSrv_u8 < APPLGC_SRV_NB) && (Ret_e == RC_OK) ; idxSrv_u8++)
    {
        //----- Loop on every Actuators For this Service -----//
        for(idxAct_u8 = (t_uint8)0 ; idxAct_u8 < c_AppLGc_SrvActuatorsMax_ua8[idxSrv_u8] ; idxAct_u8++)
        {
            actuatorLabel_e = c_AppLGc_SrvDepedencies_pae[idxSrv_u8][idxAct_u8];

            //Ret_e = APPACT_Set_ActValue(actuatorLabel_e, (t_uAPPACT_SetValue)g_srvFuncInfo_as[idxSrv_u8].actVal_pau[idxAct_u8]);

        }
    }

    return Ret_e;
}

/*********************************
 * s_APPLGC_ResetSrvState
 *********************************/
static t_eReturnCode s_APPLGC_ResetSrvState(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxSrv_u8;

    Ret_e = APPSDM_ResetDiagEvnt();

    if(Ret_e == RC_OK)
    {
        for(idxSrv_u8 = (t_uint8)0 ; 
            (idxSrv_u8 < APPLGC_SRV_NB)
        &&  (Ret_e == RC_OK) ; 
        idxSrv_u8++)
        {
            Ret_e = APPLGC_SetServiceHealth((t_eAPPLGC_SrvList)idxSrv_u8, 
                                            APPLGC_SRV_HEALTH_OK);
        }
    }

    return Ret_e;
}

/*********************************
 * s_APPLGC_DiagnosticEvent
 *********************************/
static void s_APPLGC_DiagnosticEvent(   t_eAPPSDM_DiagnosticItem f_item_e,
                                        t_eAPPSDM_DiagnosticReport f_reportState_e,
                                        t_uint16 f_debugInfo1_u16,
                                        t_uint16 f_debugInfo2_u16)
{
    t_eReturnCode Ret_e = RC_OK;

    // choose a way to communicate error

    return;
}

/*********************************
 * s_APPLGC_DiagnosticEvent
 *********************************/
static void s_APPLGC_CanCallback(   t_eFMKFDCAN_NodeList f_Node_e,
                                    t_sFMKFDCAN_RxItemEvent f_RxItem_s, 
                                    t_eFMKFDCAN_NodeStatus f_NodeStatus_e)
{
    t_uint8 data_u8[8] = {0};

    if(f_Node_e == FMKFDCAN_NODE_1)
    {
        if(f_RxItem_s.ItemId_s.Identifier_u32 == 0x18FF9087)
        {
            memcpy(data_u8, f_RxItem_s.CanMsg_s.data_pu8, 8);

            if(data_u8[1] > 8)
            {
                g_AppLgc_ModState_e = STATE_CYCLIC_OPE;
            }
        }
    }

    return;
}

static void s_APPLGC_CanCallback_2(   t_eFMKFDCAN_NodeList f_Node_e,
                                    t_sFMKFDCAN_RxItemEvent f_RxItem_s, 
                                    t_eFMKFDCAN_NodeStatus f_NodeStatus_e)
{
    t_uint8 data_u8[8] = {0};

    if(f_Node_e == FMKFDCAN_NODE_1)
    {
        if(f_RxItem_s.ItemId_s.Identifier_u32 == 0x18FF9088)
        {
            memcpy(data_u8, f_RxItem_s.CanMsg_s.data_pu8, 8);

            if(data_u8[1] > 8)
            {
                g_AppLgc_ModState_e = STATE_CYCLIC_OPE;
            }
        }
    }

    return;
}

static void s_APPLGC_MotorDiag(t_eCL42T_MotorId f_MotorID_e, t_eCL42T_DiagError f_defaultInfo_e)
{
    FMKSRL_LOG("Motor Id %d, Error %d\r\n", f_MotorID_e, f_defaultInfo_e);
}

static void s_APPLGC_PulseDropped(t_eCL42T_MotorId f_MotorID_e, 
                                    t_uint16 f_pulseDropped_u16, 
                                    t_eCL42T_MotorDirection f_direction_e)
{
    g_factdir_u8 *= -1;
    FMKSRL_LOG("Motor Id : %d, pulse dropped %d in dir %d\r\n", f_MotorID_e, f_pulseDropped_u16, f_direction_e);
}

static void s_APPLGC_PulseFinished(t_eFMKIO_OutPwmSig f_pwmSig_e)
{
    if (f_pwmSig_e < FMKIO_OUTPUT_SIGPWM_NB)
    {
        g_resetSrvState_b = TRUE;
    }
}

static void s_APPLGC_FastTask(void)
{
    t_eReturnCode Ret_e = RC_OK;


    return;
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
 *	@params[in] 
 *	@params[out]
 *	 
 *
 *
 */

