/**
 * @file        CL42T.h
 * @brief       Driver Module for Driver CL42T.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      sde
 * @date        23/01/2025
 * @version     1.0
 */
  
#ifndef CL42T_H_INCLUDED
#define CL42T_H_INCLUDED

    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "TypeCommon.h"
    #include "APP_CFG/ConfigFiles/CL42T_ConfigPublic.h"
    #include "FMK_HAL/FMK_CPU/Src/FMK_CPU.h"
    #include "1_FMK/FMK_HAL/FMK_IO/Src/FMK_IO.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************

    // ********************************************************************
    // *                      Types
    // ********************************************************************

    //-----------------------------ENUM TYPES-----------------------------//
    typedef enum 
    {
        CL42T_MOTOR_DIRECTION_CW,
        CL42T_MOTOR_DIRECTION_CCW,
        
        CL42T_MOTOR_DIRECTION_NB
    }t_eCL42T_MotorDirection;

    typedef enum 
    {
        CL42T_MOTOR_STATE_ON,
        CL42T_MOTOR_STATE_OFF,

        CL42T_MOTOR_STATE_NB
    }t_eCL42T_MotorState;

    typedef enum 
    {
        CL42T_MOTOR_PULSE_OFF = 0x00,
        CL42T_MOTOR_PULSE_ON,

        CL42T_MOTOR_PULSE_NB
    } t_eCL42T_MotorPulseState;

    typedef enum 
    {
        CL42T_DIAGNOSTIC_OK,
        CL42T_DIAGNOSTIC_PRESENTS,
        CL42T_DIAGNOSTIC_OVER_CURRENT,
        CL42T_DIAGNOSTIC_OVER_VOLTAGE,
        CL42T_DIAGNOSTIC_CHIP_ERROR,
        CL42T_DIAGNOSTIC_LOCK_MOTOR_SHAFT,
        CL42T_DIAGNOSTIC_AUTO_TUNNING,
        CL42T_DIAGNOSTIC_EEPROM,
        CL42T_DIAGNOSTIC_POSITION,
        CL42T_DIAGNOSTIC_PCB_BOARD,
        CL42T_DIAGNOSTIC_SIGNAL_PULSE,
        CL42T_DIAGNOSTIC_SIGNAL_FREQ,
        CL42T_DIAGNOSTIC_PULSE_INFINITE,

        CL42T_DIAGNOSTIC_NB
    }t_eCL42T_DiagError;

    typedef enum 
    {
        CL42T_PULSE_CHANGE_DIR_OPE_CANCEL = 0x00,
        CL42T_PULSE_CHANGE_DIR_OPE_ADD,

        CL42T_PULSE_CHANGE_DIR_OPE_NB
    } t_eCL42T_PulseChgDirOpe;
    /* CAUTION : Automatic generated code section for Enum: Start */

    /* CAUTION : Automatic generated code section for Enum: End */
   
    //-----------------------------STRUCT TYPES---------------------------//

    /* CAUTION : Automatic generated code section for Structure: Start */

    /* CAUTION : Automatic generated code section for Structure: End */
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
    typedef void (t_cbCL42T_Diagnostic)(t_eCL42T_MotorId f_MotorID_e, t_eCL42T_DiagError f_DefeultInfo_e);

    typedef struct 
    {
        t_sint32 nbPulses_s32;
        t_uint32 frequency_u32;
        t_eCL42T_MotorState state_e;
        t_bool stopPulse_b;

    } t_sCL42T_SetMotorValue;

    typedef struct 
    {
        t_eCL42T_MotorPulseState pulseState_e;
        t_uint32 frequency_u32;
        t_eCL42T_MotorDirection dir_e;
        t_eCL42T_MotorState state_e;
    } t_sCL42T_GetMotorValue;
    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************

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
    t_eReturnCode CL42T_Init(   void);

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
    t_eReturnCode CL42T_Cyclic( void);

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
    t_eReturnCode CL42T_GetState(t_eCyclicModState *f_State_pe);

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
    t_eReturnCode CL42T_SetState(t_eCyclicModState f_State_e);

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
    t_eReturnCode CL42T_AddMotorConfiguration(  t_eCL42T_MotorId f_motorId_e,
                                                t_sCL42T_MotorSigCfg f_MotorCfg_s,
                                                t_eCL42T_PulseChgDirOpe f_PulseOpe_e,
                                                t_cbCL42T_Diagnostic *f_diagEvnt_pcb);

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
    t_eReturnCode CL42T_SetMotorSigValue(   t_eCL42T_MotorId f_motorId_e,
                                            t_sCL42T_SetMotorValue f_MotorValue_s);
    
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
    t_eReturnCode CL42T_GetMotorSigValue(   t_eCL42T_MotorId f_motorId_e,
                                            t_sCL42T_GetMotorValue * f_MotorValue_ps);

                                                                                                                                           
    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************
    
#endif // LIBRAMP_H_INCLUDED
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
