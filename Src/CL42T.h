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
    #warning('change this value to not get infinite pulse by logic side')
    #define CL42T_SEND_INFINITE_PULSE ((t_uint16)32767)
    // ********************************************************************
    // *                      Types
    // ********************************************************************

    //-----------------------------ENUM TYPES-----------------------------//
    ///@brief Enumeration for the Direction CW and CCW 
    typedef enum 
    {
        CL42T_MOTOR_DIRECTION_CW = 0,
        CL42T_MOTOR_DIRECTION_CCW,
        
        CL42T_MOTOR_DIRECTION_NB
    }t_eCL42T_MotorDirection;

    /// @brief Enumeration for the motor State
    typedef enum 
    {
        CL42T_MOTOR_STATE_OFF = 0,
        CL42T_MOTOR_STATE_ON,

        CL42T_MOTOR_STATE_NB
    }t_eCL42T_MotorState;


    /// @brief Enumeration of the Motor Diagnostic Information
    typedef enum 
    {
        CL42T_DIAGNOSTIC_OK = 0,
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
        CL42T_BITFIELD_MOTOR_ENABLE = 0,     //---- Bit to 1 motor is enable and can rotate/rcv cmd, bit to 0 -> motor is not controlled by driver (can be moved by hand)
        CL42T_BITFIELD_MOTOR_ON,             //---- Bit to 1 -> motor is moving, bit to 0 -> motor is off ----//
        CL42T_BITFILED_MOTOR_DIR,            //---- Ignore bit if motor is OFF, Bit to 1 -> CL42T_MOTOR_DIRECTION_CCW, bit to 0 -> CL42T_MOTOR_DIRECTION_CCW ----//
        CL42T_BITFIELD_IN_DEAD_TIME,         //---- Bit to 1 -> motor is in deadtime state, bit to 0 motor is not to deadtime state ----//
        CL42T_BITFIELD_TRIG_ENDSTOP_CW,      //---- Bit to 1 -> motor has reach the endStop ClockWise limit, bit to 0 -> not reach ----//
        CL42T_BITFIELD_TRIG_ENDSTOP_CCW,     //---- Bit to 1 -> motor has reach the endStop1 Counter ClockWise, bit to 0 -> not reach ----//
        CL42T_BITFIELD_NB
    } t_eCL42T_BitfieldInfo;
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
    typedef void (t_cbCL42T_Diagnostic)(t_eCL42T_MotorId f_MotorID_e, t_eCL42T_DiagError f_defaultInfo_e);
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
    typedef void (t_cbCL42T_PulseDropped)(t_eCL42T_MotorId f_MotorID_e, t_uint16 f_pulseDropped_u16, t_eCL42T_MotorDirection f_direction_e);

    typedef struct 
    {
        t_sint32 nbPulses_s32;          //---- Number of pulses to send ----//
        t_float32 frequency_f32;         //---- Frequency of the PWM ----//
        t_uint32 triggerTimer_u32;      //---- Absolute timer for sending pulses in ms (Tick) ----//
    } t_sCL42T_SetMotorValue;

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
    t_eReturnCode CL42T_Init(void);

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
    t_eReturnCode CL42T_Cyclic(void);

    /**
     *
    *	@brief Function to know the module state 
    *	@param[in]  f_State_pe : store the value, value from @ref t_eCyclicModState
    *
    *   @retval RC_OK                             @ref RC_OK
    *   @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NUL
    *	 
    *
    *
    */
    t_eReturnCode CL42T_GetState(t_eCyclicModState *f_State_pe);

    /**
     *
    *	@brief Function to set the module state 
    *	@param[in]  f_State_e : the value, value from @ref t_eCyclicModState
    *
    *   @retval RC_OK                             @ref RC_OK
    *   @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NUL
    *
    *
    */
    t_eReturnCode CL42T_SetState(t_eCyclicModState f_State_e);

    /**
    *
    *	@brief      Add A motor configuration.
    *	@note       The configuration is quite exhaustive, although 
    *               the encoder was not in it on purpose. Encoder has to be 
    *               deals in logical/applcication level not in this driver.
    *  @warning     This library only works with Advanced and High Resolution Timer
    *               cause the pulses are send by package to unload the CPU using 
    *               RCR register, please see FMKTIM module SetPwmLineValue for more info.
    *
    *
    *	@param[in] f_motorId_e  : the motor Id
    *	@param[in] f_MotorCfg_s : the motor configuration
    *	@param[in] f_enableDeadtime_b : Enable/Disable the deadtime whenever a change of direction is detected
    *	@param[in] f_diagEvnt_pcb : Function to be called whenever a error happen
    *	@param[in] f_pulseDropped_pcb : Function to be called whenever a pulse are dropped
    *	 
    *   @return @ref t_eReturncode
    */
    t_eReturnCode CL42T_AddMotorConfiguration(  t_eCL42T_MotorId f_motorId_e,
                                            t_sCL42T_MotorSigCfg f_MotorCfg_s,
                                            t_bool f_enableDeadtime_b,
                                            t_cbCL42T_Diagnostic *f_diagEvnt_pcb,
                                            t_cbCL42T_PulseDropped * f_pulseDropped_pcb);

    /**
    *
    *	@brief      Set Motor Value.
    *	@note       Once the motor is initialized you can now set pulses and frequency.
    *               The motor actuators are actually ENABLE / DIRECTION / PULSE (Freq, Dc)
    *               But in order to abastract you give to the library signed pulse 
    *                   - nbPulses_s32 > 0 means will turn in CW 
    *                   - nbPulses_s32 < 0 means motor will turn in CCW
    *                   - nbPulses_s23 = 0 the motor is stopped
    *               This API will not send right away the pulse to the motor. It will depends
    *               on the state of the motor :
    *               - if the motor is OFF (not turning), the command is send from the next cyclic.
    *               - if the motor is ON, the pulse will not be sent directly, it will be when the last cmd will finish
    *                   from the pulseFinishCallback, so from this API you command will be store in queue 
    *                   that can contains CL42T_CMD_QUEUE_SIZE, this number can be increrase/decrease as you wish
    *               It means you can accumulate command and the module will send it to the motor
    *               in a asynchronous way, by controlling with triggerTimer_u32 when the pulse has to be send in absolute time (Tick).
    *               For example if you want to set a pulse and wait 500 ms to set another pulse, you have to provide triggerTimer_u32 = currentTick + 500.
    *               if something went wrong during this process you will be call. If you want to send pulses ASAP the motor is read put 0.
    * @warning      If you plan a pulse generation in 10000 ms no command will be send before that.
    *               with the f_diagEvnt_pcb callback.
    *               If the motor status not allowed command the bit CL42T_MOTOR_STS_CMD_ENABLE in f_MotorStsInfo_pu16 will be set to 0.
    * @note         A deadtime is applied whenever a change of direction is detected by software 
    *               which means every time a change of dir happened, the motor is stop CL42T_DEAD_TIME_TRANSITION millisecond
    *               before allow another command, this behaviour can be disable at the initialization
    * @warning      One command of pulse should not exceed 0xFFFF.
    *  
    *
    *	@param[in] f_motorId_e  : the motor Id
    *	@param[in] f_MotorValue_s : Frequency and Pulses to send
    *	 
    *   @return RC_WARNING_BUSY      Motor does not accept pulse at the moment
    *   @return RC_ERROR_INSTANCE_NOT_INITIALIZED      Motor not initialized
    */
    t_eReturnCode CL42T_SetMotorSigValue(   t_eCL42T_MotorId f_motorId_e,
                                            t_sCL42T_SetMotorValue f_MotorValue_s);
    
    /**
    *
    *	@brief      Get motor information 
    *	@note   
    *
    *
    *	@param[in]  f_motorId_e : the motor concern
    *	@param[in]  f_MotorStsInfo_pu16 : Container for the motor information
    *                                       which is a bit field from @ref t_eCL42T_BitfieldInfo
    */
    t_eReturnCode CL42T_GetMotorInfo(   t_eCL42T_MotorId f_motorId_e,
                                        t_uint16 * f_MotorStsInfo_pu16);
    /**
    *
    *	@brief      Get motor information 
    *	@note   
    *
    *
    *	@param[in]  f_motorId_e : the motor concern
    *	@param[in]  f_MotorStsInfo_pu16 : Container for the motor information
    *                                       which is a bit field from @ref t_eCL42T_BitfieldInfo
    */
    t_eReturnCode CL42T_GetMotorSpeed(  t_eCL42T_MotorId f_motorId_e,
                                        t_float32 * f_motorSpeed_pf32);

    /**
    *
    *	@brief      Set the motor state.
    *	@note       At the initialization, the motor will always set to ON (Enable)
    *               So no need to use this API to set that, but to Disable It, and the Enable 
    *               it you can call this API
    *
    *
    *	@param[in]  f_motorId_e : the motor concern
    *	@param[in]  f_state_e : New State of the motor
    */
    t_eReturnCode CL42T_SetMotorState(  t_eCL42T_MotorId f_motorId_e, 
                                        t_eCL42T_MotorState f_state_e,
                                        t_bool f_isEmergencyStop_b);        
    
    
    void CL42T_Test_SetPerturb(t_eCL42T_MotorId f_motor_e, t_bool isEndStopCW);
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
