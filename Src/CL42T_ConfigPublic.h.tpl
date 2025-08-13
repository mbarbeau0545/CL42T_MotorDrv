/**
 * @file        CL42T_ConfigPublic.h
 * @brief       Driver Module for Driver CL42T.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      mba
 * @date        21/01/2025
 * @version     1.0
 */
  
#ifndef CL42T_CONFIGPUBLIC_H_INCLUDED
#define CL42T_CONFIGPUBLIC_H_INCLUDED

    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "FMK_HAL/FMK_IO/Src/FMK_IO.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************

    // ********************************************************************
    // *                      Types
    // ********************************************************************

    //-----------------------------ENUM TYPES-----------------------------//
    typedef enum 
    {
        CL42T_MOTOR_1,
        CL42T_MOTOR_2,
        CL42T_MOTOR_3,
        CL42T_MOTOR_4,

        CL42T_MOTOR_NB
    } t_eCL42T_MotorId;
    
    typedef enum 
    {
        CL42T_SIGTYPE_PULSE = 0x00,
        CL42T_SIGTYPE_DIR,
        CL42T_SIGTYPE_STATE,
        CL42T_SIGTYPE_DIAG,
        CL42T_SIGTYPE_ENDSTOP_CW,
        CL42T_SIGTYPE_ENDSTOP_CCW,

        CL42T_SIGTYPE_NB
    } t_eCL42T_MotorSignalType;
    /* CAUTION : Automatic generated code section for Enum: Start */

    /* CAUTION : Automatic generated code section for Enum: End */
   
    //-----------------------------STRUCT TYPES---------------------------//
    /// @brief Signal configuration
    typedef struct
    {
        t_eFMKIO_InEvntSig EndStopSignal_e;
        t_eFMKIO_PullMode PullMode_e;
        t_eFMKIO_SigTrigCptr triggerEvnt_e;
    } t_sCL42T_EndStopignalCfg;

    /// @brief Pulse Signal Configuration
    typedef struct 
    {
        t_eFMKIO_OutPwmSig PulseSignal_e;
        t_sFMKIO_PwmControlPrm pwmCtrlPrm_s;
        t_sFMKIO_PwmWaveformCfg pwmWaveForm_s;
    } t_sCL42T_PwmSignalCfg;

    typedef struct 
    {
        t_sCL42T_PwmSignalCfg       PulseSigCfg_s;      //---- Pulse Signal Configuration ----//
        t_eFMKIO_OutDigSig          DirSignal_e;        //---- Direction Signal Configuration ----//
        t_eFMKIO_OutDigSig          StateSignal_e;      //---- State of the motor Signal Configuration ----//
        t_eFMKIO_InFreqSig          DiagSignal_e;       //---- Diagnotic Frequency Signal Configuration ----//
        t_sCL42T_EndStopignalCfg    EndStopSigCW_s;     //---- End Stop Cfg for ClockWise, put FMKIO_INPUT_FREQ_NB if not used ----//
        t_sCL42T_EndStopignalCfg    EndStopSigCCW_s;    //---- End Stop Cfg for Counter Clockwise, put FMKIO_INPUT_FREQ_NB if not used ----//

    } t_sCL42T_MotorSigCfg;

    /* CAUTION : Automatic generated code section for Structure: Start */

    /* CAUTION : Automatic generated code section for Structure: End */

    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************

    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************
    
#endif // CL42T_CONFIGPUBLIC_H_INCLUDED
//************************************************************************************
// End of File
//************************************************************************************

/**
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[in]
 *	 
 *
 *
 */
