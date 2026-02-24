/**
 * PWM Generated Driver Types Header File
 * 
 * @file     pwm_hs_types.h
 * 
 * @ingroup  pwmhsdriver
 * 
 * @brief    This is the generated driver types header file for the PWM driver
 *
 * @skipline @version   PLIB Version 1.2.0
 *
 * @skipline Device : 
*/

/*disclaimer*/

#ifndef PWM_HS_TYPES_H
#define PWM_HS_TYPES_H

// Section: Included Files

// Section: Enum Declarations

/**
 @ingroup  pwmhsdriver
 @enum     PWM_GENERATOR
 @brief    Defines the PWM generators that are 
           selected from the MCC Melody User Interface for the PWM output controls.
 Note: The enum list in the Help document might be just an illustration to show the selected PWM channel list
 Generated enum list is based on the configuration done by user in the MCC Melody user interface
*/
enum PWM_GENERATOR
{
    PWM_GENERATOR_1 =  1,       /**< Custom name of PWM generator 1 */
    PWM_GENERATOR_2 =  2,       /**< Custom name of PWM generator 2 */
    PWM_GENERATOR_3 =  3,       /**< Custom name of PWM generator 3 */
    PWM_MAX_GENERATOR
};

/**
 @ingroup  pwmhsdriver
 @enum     PWM_GENERATOR_INTERRUPT
 @brief    Defines the PWM generator interrupts that are available for the module to use.
*/
enum PWM_GENERATOR_INTERRUPT
{
    PWM_GENERATOR_INTERRUPT_FAULT =  1,       /**< PWM Generator fault Interrupt*/
    PWM_GENERATOR_INTERRUPT_CURRENT_LIMIT =  2,       /**< PWM Generator current limit Interrupt*/
    PWM_GENERATOR_INTERRUPT_FEED_FORWARD =  3,       /**< PWM Generator feed forward Interrupt*/
    PWM_GENERATOR_INTERRUPT_SYNC =  4,       /**< PWM Generator sync Interrupt*/
};

/**
 @ingroup  pwmhsdriver
 @enum     PWM_COMMON_EVENT
 @brief    Defines the PWM generator Common Events that are 
           available for the module to use.
*/
enum PWM_COMMON_EVENT
{
    PWM_EVENT_A = 1,         /**< PWM EVENT_A Output */
    PWM_EVENT_B = 2,         /**< PWM EVENT_B Output */
    PWM_EVENT_C = 3,         /**< PWM EVENT_C Output */
    PWM_EVENT_D = 4,         /**< PWM EVENT_D Output */
    PWM_EVENT_E = 5,         /**< PWM EVENT_E Output */
    PWM_EVENT_F = 6          /**< PWM EVENT_F Output */
};

/**
 @ingroup  pwmhsdriver
 @enum     PWM_TRIGGER_COMPARE
 @brief    Defines the PWM generator Trigger Compare registers 
           that are available for the module to use.
*/
enum PWM_TRIGGER_COMPARE
{
    PWM_TRIGGER_COMPARE_A =  1,       /**< PWM TriggerA Compare Register */
    PWM_TRIGGER_COMPARE_B =  2,       /**< PWM TriggerB Compare Register */
    PWM_TRIGGER_COMPARE_C =  3,       /**< PWM TriggerC Compare Register */
};

/**
 @ingroup  pwmhsdriver
 @enum     PWM_DAC_TRIGGER_COMPARE
 @brief    Defines the PWM generator Trigger Compare registers 
           that are available for triggering CMP_DAC module.
*/
enum PWM_DAC_TRIGGER_COMPARE
{
    PWM_TRIGGER_COMPARE_D =  4,       /**< PWM TriggerD Compare Register */
    PWM_TRIGGER_COMPARE_E =  5,       /**< PWM TriggerE Compare Register */
};


/**
 @ingroup  pwmhsdriver
 @enum     PWM_MODES
 @brief    Defines the PWM generator operating modes that are available.
*/
enum PWM_MODES
{
    PWM_MODE_INDEPENDENT_EDGE =  0x0,       /**< Independent Edge mode*/
    PWM_MODE_VARIABLE_PHASE =  0x1,       /**< Variable Phase mode*/
    PWM_MODE_INDEPENDENT_EDGE_DUAL_OUTPUT =  0x2,       /**< Independent Edge, dual output mode*/
    PWM_MODE_CENTER_ALIGNED =  0x4,       /**< Center-Aligned mode*/
    PWM_MODE_DOUBLE_UPDATE_CENTER_ALIGNED =  0x5,       /**< Double-Update Center-Aligned mode*/
    PWM_MODE_DUAL_EDGE_CENTER_ALIGNED_ONE_UPDATE_CYCLE =  0x6,       /**< Dual Edge Center-Aligned;one update/cycle mode*/
    PWM_MODE_DUAL_EDGE_CENTER_ALIGNED_TWO_UPDATES_CYCLE =  0x7,       /**< Dual Edge Center-Aligned;two updates/cycle mode*/
};

/**
 @ingroup  pwmhsdriver
 @enum     PWM_HS_OUTPUT_MODES
 @brief    Defines the PWM generator output modes that are available.
*/
enum PWM_HS_OUTPUT_MODES
{
    COMPLEMENTARY_OUTPUT_MODE = 0x0,         /**< Complementary output mode*/
    INDEPENDENT_OUTPUT_MODE = 0x1,         /**< Independent output mode*/
    PUSH_PULL_OUTPUT_MODE = 0x2,         /**< Push-Pull output mode*/
};

/**
 @ingroup  pwmhsdriver
 @enum     PWM_SOURCE_SELECT    
 @brief    Defines the PWM generator Master or Independent source selection.
*/
enum PWM_SOURCE_SELECT
{
    PWM_SOURCE_SELECT_INDEPENDENT = 0,         /**< PWM select Independent PWM as source */
    PWM_SOURCE_SELECT_MASTER = 1,             /**< PWM select Master as source */
};

#endif  //PWM_HS_TYPES_H
/**
 End of File
*/
