/**
 * @file        CL42T_ConfigPrivate.h
 * @brief       Driver Module for Driver CL42T.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      mba
 * @date        21/01/2025
 * @version     1.0
 */
  
#ifndef CL42T_CONFIGPRIVATE_H_INCLUDED
#define CL42T_CONFIGPRIVATE_H_INCLUDED

    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "FMK_HAL/FMK_IO/Src/FMK_IO.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************
    #define CL42T_DEAD_TIME_TRANSITION ((t_uint16)500)

    /**
     * @brief Define for CL42T Diagnotic Pulse
     */
    #define CL42T_ONE_PULSE     ((t_uint16)1)
    #define CL42T_TWO_PULSE     ((t_uint16)2)
    #define CL42T_THREE_PULSE   ((t_uint16)3)
    #define CL42T_FOUR_PULSE    ((t_uint16)4)
    #define CL42T_FIVE_PULSE    ((t_uint16)5)
    #define CL42T_SIX_PULSE     ((t_uint16)6)
    #define CL42T_SEVEN_PULSE   ((t_uint16)7)

    /**
     * @brief Normal duty cycle
     */
    #define CL42T_NOMINATIVE_DUTYCYCLE ((t_uint16)500)

    ///@brief Nominative Frequency 
    #define CL42T_NOMINATIVE_FREQUENCY ((t_uint32)6000)

    ///@Brief Datasheet says for the first enable motor wait 200 m before setting dir/pulse
    #define CL42T_ENABLE_MOTOR_TIMEOUT  ((t_uint16)200)

    ///@brief Datasheet says frequency diagnotic period is 0,3 sec -> ~3Hz
    #define CL42T_IN_FREQ_SAMPLING      ((t_float32)0.5)

    ///@brief number of command we can get 
    #define CL42T_CMD_QUEUE_SIZE ((t_uint8)10)
    // ********************************************************************
    // *                      Types
    // ********************************************************************

    //-----------------------------ENUM TYPES-----------------------------//
 
    /* CAUTION : Automatic generated code section for Enum: Start */

    /* CAUTION : Automatic generated code section for Enum: End */
   
    //-----------------------------STRUCT TYPES---------------------------//

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
    
#endif // CL42T_CONFIGPRIVATE_H_INCLUDED
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
