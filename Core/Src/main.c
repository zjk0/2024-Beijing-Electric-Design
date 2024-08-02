/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dac.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ILI9488.h"
#include "AD9959.h"
#include "AD9959_Outset.h"
#include "X9C103.h"
#include "PE4302.H"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int ModulationDegree_index = 0; // The index of array "DCvoltage_Modulation" and array "ModulationDegreeData"
int Amplitude_index = 0;        // The index of array "DCvoltage_CW"
int Frequency_index = 0;        // The index of array "Frequency", array "DCvoltage_CW" and array "DelayTime"
int DelayTime_index = 0;        // The index of array "DelayTime"
int AMwave_index = 0;           // The index of array "DCvoltage_AM"

int CWorAM = CW_MODE;

enum BottomType bottom = NoWork;                // The work type of bottom
enum BottomMode bottom_mode = BottomCWorAMMode; // The mode of bottom
// enum BottomMode bottom_mode = BottomAmplitudeMode;
uint32_t DCvoltage_AM[10];        // The array to store the data of AM mode
uint32_t DCvoltage_CW[11][10];    // The array to store the data of CW mode in different frequency
uint32_t DCvoltage_Modulation[7]; // The array to store the data which is used to calculate modulation degree
uint32_t MultiPath_Modulation[7]; // The array to store the data which is used to calculate the modulation degree of Multipath wave
uint32_t DelayPhase[7];

// uint32_t DCvoltage[2][10];

double Frequency[11]; // The array of different frequency
int DelayTime[11][7]; // The array of the delay time in different frequency

int ModulationDegreeData[7] = {30, 40, 50, 60, 70, 80, 90};
int DelayTimeData[7] = {0, 50, 80, 110, 140, 170, 200};
int AmplitudeValidValueData[10] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

// Test used
int phase = 0;
int amp = 150;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Initialize data
void DCvoltageInit(void)
{
    // DCvoltage[AMPLITUDE_INDEX] is the same with DCvoltage_CW[0]
    //   DCvoltage[AMPLITUDE_INDEX][0] = 170;//130;
    //   DCvoltage[AMPLITUDE_INDEX][1] = 360;//262;
    //   DCvoltage[AMPLITUDE_INDEX][2] = 545;//400;
    //   DCvoltage[AMPLITUDE_INDEX][3] = 760;//542;
    //   DCvoltage[AMPLITUDE_INDEX][4] = 960;//699;
    //   DCvoltage[AMPLITUDE_INDEX][5] = 1160;//865
    //   DCvoltage[AMPLITUDE_INDEX][6] = 1360;//999
    //   DCvoltage[AMPLITUDE_INDEX][7] = 1550;//1183
    //   DCvoltage[AMPLITUDE_INDEX][8] = 1750;//1345
    //   DCvoltage[AMPLITUDE_INDEX][9] = 1950;//1459

    DCvoltage_AM[0] = 148;
    DCvoltage_AM[1] = 310;
    DCvoltage_AM[2] = 491;
    DCvoltage_AM[3] = 640;
    DCvoltage_AM[4] = 820;
    DCvoltage_AM[5] = 1000;
    DCvoltage_AM[6] = 1200;
    DCvoltage_AM[7] = 1365;
    DCvoltage_AM[8] = 1575;
    DCvoltage_AM[9] = 1745;

    // 30M
    DCvoltage_CW[0][0] = 170;
    DCvoltage_CW[0][1] = 350;
    DCvoltage_CW[0][2] = 535;
    DCvoltage_CW[0][3] = 734;
    DCvoltage_CW[0][4] = 930;
    DCvoltage_CW[0][5] = 1135;
    DCvoltage_CW[0][6] = 1330;
    DCvoltage_CW[0][7] = 1530;
    DCvoltage_CW[0][8] = 1730;
    DCvoltage_CW[0][9] = 1925;

    // 31M
    DCvoltage_CW[1][0] = 152;
    DCvoltage_CW[1][1] = 315;
    DCvoltage_CW[1][2] = 489;
    DCvoltage_CW[1][3] = 680;
    DCvoltage_CW[1][4] = 877;
    DCvoltage_CW[1][5] = 1051;
    DCvoltage_CW[1][6] = 1242;
    DCvoltage_CW[1][7] = 1420;
    DCvoltage_CW[1][8] = 1620;
    DCvoltage_CW[1][9] = 1810;

    // 32M
    DCvoltage_CW[2][0] = 137;
    DCvoltage_CW[2][1] = 287;
    DCvoltage_CW[2][2] = 451;
    DCvoltage_CW[2][3] = 625;
    DCvoltage_CW[2][4] = 801;
    DCvoltage_CW[2][5] = 970;
    DCvoltage_CW[2][6] = 1145;
    DCvoltage_CW[2][7] = 1300;
    DCvoltage_CW[2][8] = 1489;
    DCvoltage_CW[2][9] = 1660;

    // 33M
    DCvoltage_CW[3][0] = 126;
    DCvoltage_CW[3][1] = 271;
    DCvoltage_CW[3][2] = 419;
    DCvoltage_CW[3][3] = 590;
    DCvoltage_CW[3][4] = 745;
    DCvoltage_CW[3][5] = 910;
    DCvoltage_CW[3][6] = 1070;
    DCvoltage_CW[3][7] = 1235;
    DCvoltage_CW[3][8] = 1385;
    DCvoltage_CW[3][9] = 1550;

    // 34M
    DCvoltage_CW[4][0] = 126;
    DCvoltage_CW[4][1] = 265;//271;
    DCvoltage_CW[4][2] = 395;//421;
    DCvoltage_CW[4][3] = 560;
    DCvoltage_CW[4][4] = 715;
    DCvoltage_CW[4][5] = 870;
    DCvoltage_CW[4][6] = 1020;
    DCvoltage_CW[4][7] = 1160;
    DCvoltage_CW[4][8] = 1338;
    DCvoltage_CW[4][9] = 1500;

    // 35M
    DCvoltage_CW[5][0] = 126;
    DCvoltage_CW[5][1] = 260;
    DCvoltage_CW[5][2] = 390;//421;
    DCvoltage_CW[5][3] = 530;
    DCvoltage_CW[5][4] = 705;
    DCvoltage_CW[5][5] = 860;
    DCvoltage_CW[5][6] = 1010;
    DCvoltage_CW[5][7] = 1160;
    DCvoltage_CW[5][8] = 1315;
    DCvoltage_CW[5][9] = 1455;

    // 36M
    DCvoltage_CW[6][0] = 120;
    DCvoltage_CW[6][1] = 260;//283;
    DCvoltage_CW[6][2] = 400;//446;
    DCvoltage_CW[6][3] = 550;
    DCvoltage_CW[6][4] = 717;
    DCvoltage_CW[6][5] = 870;
    DCvoltage_CW[6][6] = 1015;
    DCvoltage_CW[6][7] = 1170;
    DCvoltage_CW[6][8] = 1320;
    DCvoltage_CW[6][9] = 1470;

    // 37M
    DCvoltage_CW[7][0] = 125;
    DCvoltage_CW[7][1] = 280;//311;
    DCvoltage_CW[7][2] = 423;//488;
    DCvoltage_CW[7][3] = 580;
    DCvoltage_CW[7][4] = 750;
    DCvoltage_CW[7][5] = 920;
    DCvoltage_CW[7][6] = 1080;
    DCvoltage_CW[7][7] = 1230;
    DCvoltage_CW[7][8] = 1390;
    DCvoltage_CW[7][9] = 1555;

    // 38M
    DCvoltage_CW[8][0] = 142;//167;
    DCvoltage_CW[8][1] = 300;//348;
    DCvoltage_CW[8][2] = 460;//552;
    DCvoltage_CW[8][3] = 635;
    DCvoltage_CW[8][4] = 820;
    DCvoltage_CW[8][5] = 985;
    DCvoltage_CW[8][6] = 1150;
    DCvoltage_CW[8][7] = 1320;
    DCvoltage_CW[8][8] = 1480;
    DCvoltage_CW[8][9] = 1630;

    // 39M
    DCvoltage_CW[9][0] = 142;//188;
    DCvoltage_CW[9][1] = 325;//386;
    DCvoltage_CW[9][2] = 495;//614;
    DCvoltage_CW[9][3] = 680;
    DCvoltage_CW[9][4] = 870;
    DCvoltage_CW[9][5] = 1070;
    DCvoltage_CW[9][6] = 1240;
    DCvoltage_CW[9][7] = 1425;
    DCvoltage_CW[9][8] = 1600;
    DCvoltage_CW[9][9] = 1750;

    // 40M
    DCvoltage_CW[10][0] = 155;//214;
    DCvoltage_CW[10][1] = 355;//436;
    DCvoltage_CW[10][2] = 540;//675;
    DCvoltage_CW[10][3] = 750;
    DCvoltage_CW[10][4] = 960;
    DCvoltage_CW[10][5] = 1160;
    DCvoltage_CW[10][6] = 1340;
    DCvoltage_CW[10][7] = 1525;
    DCvoltage_CW[10][8] = 1730;
    DCvoltage_CW[10][9] = 1930;

    DelayPhase[0] = 0;
    DelayPhase[1] = 40;
    DelayPhase[2] = 60;
    DelayPhase[3] = 80;
    DelayPhase[4] = 100;
    DelayPhase[5] = 120;
    DelayPhase[6] = 140;

    //   DCvoltage[MODULATION_DEGREE_INDEX][0] = 700;
    //   DCvoltage[MODULATION_DEGREE_INDEX][1] = 570;
    //   DCvoltage[MODULATION_DEGREE_INDEX][2] = 440;
    //   DCvoltage[MODULATION_DEGREE_INDEX][3] = 353;
    //   DCvoltage[MODULATION_DEGREE_INDEX][4] = 310;
    //   DCvoltage[MODULATION_DEGREE_INDEX][5] = 285;
    //   DCvoltage[MODULATION_DEGREE_INDEX][6] = 240;
    //   DCvoltage[MODULATION_DEGREE_INDEX][7] = 240;
    //   DCvoltage[MODULATION_DEGREE_INDEX][8] = 240;
    //   DCvoltage[MODULATION_DEGREE_INDEX][9] = 240;

    DCvoltage_Modulation[0] = 670;
    DCvoltage_Modulation[1] = 488 + 20 + 20;
    DCvoltage_Modulation[2] = 413 + 20 + 20;
    DCvoltage_Modulation[3] = 353 + 20 + 20;
    DCvoltage_Modulation[4] = 312 + 20 + 20;
    DCvoltage_Modulation[5] = 275 + 20 + 20;
    DCvoltage_Modulation[6] = 245 + 20 + 20;

    MultiPath_Modulation[0] = 44;
    MultiPath_Modulation[1] = 41;
    MultiPath_Modulation[2] = 40;
    MultiPath_Modulation[3] = 38;
    MultiPath_Modulation[4] = 36;
    MultiPath_Modulation[5] = 34;
    MultiPath_Modulation[6] = 32;

    Frequency[0] = 30000000;
    Frequency[1] = 31000000;
    Frequency[2] = 32000000;
    Frequency[3] = 33000000;
    Frequency[4] = 34000000;
    Frequency[5] = 35000000;
    Frequency[6] = 36000000;
    Frequency[7] = 37000000;
    Frequency[8] = 38000000;
    Frequency[9] = 39000000;
    Frequency[10] = 40000000;

    // 30M
    DelayTime[0][0] = 0;
    DelayTime[0][1] = 530;
    DelayTime[0][2] = 855;
    DelayTime[0][3] = 1180;
    DelayTime[0][4] = 1505;
    DelayTime[0][5] = 1830;
    DelayTime[0][6] = 2155;

    // 31M
    DelayTime[1][0] = 0;
    DelayTime[1][1] = 550;
    DelayTime[1][2] = 890;
    DelayTime[1][3] = 1220;
    DelayTime[1][4] = 1555;
    DelayTime[1][5] = 1890;
    DelayTime[1][6] = 2225;

    // 32M
    DelayTime[2][0] = 0;
    DelayTime[2][1] = 565;
    DelayTime[2][2] = 905;
    DelayTime[2][3] = 1250;
    DelayTime[2][4] = 1605;
    DelayTime[2][5] = 1955;
    DelayTime[2][6] = 2305;

    // 33M
    DelayTime[3][0] = 0;
    DelayTime[3][1] = 595;
    DelayTime[3][2] = 945;
    DelayTime[3][3] = 1295;
    DelayTime[3][4] = 1645;
    DelayTime[3][5] = 1995;
    DelayTime[3][6] = 2345;

    // 34M
    DelayTime[4][0] = 0;
    DelayTime[4][1] = 605;
    DelayTime[4][2] = 970;
    DelayTime[4][3] = 1335;
    DelayTime[4][4] = 1715;
    DelayTime[4][5] = 2085;
    DelayTime[4][6] = 2455;

    // 35M
    DelayTime[5][0] = 0;
    DelayTime[5][1] = 630;
    DelayTime[5][2] = 1000;
    DelayTime[5][3] = 1375;
    DelayTime[5][4] = 1747;
    DelayTime[5][5] = 2119;
    DelayTime[5][6] = 2491;

    // 36M
    DelayTime[6][0] = 0;
    DelayTime[6][1] = 640;
    DelayTime[6][2] = 1030;
    DelayTime[6][3] = 1415;
    DelayTime[6][4] = 1802;
    DelayTime[6][5] = 2189;
    DelayTime[6][6] = 2576;

    // 37M
    DelayTime[7][0] = 0;
    DelayTime[7][1] = 660;
    DelayTime[7][2] = 1065;
    DelayTime[7][3] = 1460;
    DelayTime[7][4] = 1860;
    DelayTime[7][5] = 2260;
    DelayTime[7][6] = 2660;

    // 38M
    DelayTime[8][0] = 0;
    DelayTime[8][1] = 680;
    DelayTime[8][2] = 1090;
    DelayTime[8][3] = 1505;
    DelayTime[8][4] = 1917;
    DelayTime[8][5] = 2329;
    DelayTime[8][6] = 2741;

    // 39M
    DelayTime[9][0] = 0;
    DelayTime[9][1] = 700;
    DelayTime[9][2] = 1120;
    DelayTime[9][3] = 1540;
    DelayTime[9][4] = 1960;
    DelayTime[9][5] = 2380;
    DelayTime[9][6] = 2800;

    // 40M
    DelayTime[10][0] = 0;
    DelayTime[10][1] = 720;
    DelayTime[10][2] = 1140;
    DelayTime[10][3] = 1560;
    DelayTime[10][4] = 1980;
    DelayTime[10][5] = 2400;
    DelayTime[10][6] = 2820;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

    delay_init(180);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_DAC_Init();
  /* USER CODE BEGIN 2 */
    DCvoltageInit();

    // Initialize DAC
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    //   HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DCvoltage[MODULATION_DEGREE_INDEX][0]);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DCvoltage_Modulation[ModulationDegree_index]);
    // HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 245);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, DCvoltage_CW[Frequency_index][Amplitude_index]);
    //   HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, DCvoltage[AMPLITUDE_INDEX][0]);

    //   HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, amp);

    // Initialize AD9959
    AD9959_Init();
    AD9959_enablechannel0();
    AD9959_SetFreAmpPha(30000000, 400, 0);
    AD9959_enablechannel1();
    AD9959_SetFreAmpPha(2000000, 400, 0);
    AD9959_enablechannel2();
    AD9959_SetFreAmpPha(2000000, 400, DelayTime[0][0]);
    AD9959_enablechannel3();
    AD9959_SetFreAmpPha(30000000, 400, DelayTime[0][0]);

    // Initialize LCD
    lcd_init_spi();
    fill_rectangle(0, 0, WIDTH, HEIGHT, COLOR_DARKBLUE);
    ShowInformation(CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);

    // Initialize X9C103
    X9C103_Init();
    X9C103_Inc_N_Step(MultiPath_Modulation[ModulationDegree_index]);
//    X9C103_Init_2();
//    X9C103_Inc_N_Step_2(10);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
        //     if (bottom != NoWork) {
        //         if (bottom == ClockwiseRotate) {
        //             // if (bottom_mode == BottomFrequencyMode) {
        //             //     Frequency_index++;
        //             //     if (Frequency_index > 10) {
        //             //         Frequency_index = 10;
        //             //     }
        //             // }
        //             // else if (bottom_mode == BottomAmplitudeMode) {
        //             //     amp++;
        //             //     if (amp > 4095) {
        //             //         amp = 4095;
        //             //     }
        //             //     HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, amp);
        //             // }
        //             phase += 30;

        //         }
        // 		else if (bottom == AnticlockwiseRotate) {
        //             // if (bottom_mode == BottomFrequencyMode) {
        //             //     Frequency_index--;
        //             //     if (Frequency_index < 0) {
        //             //         Frequency_index = 0;
        //             //     }
        //             // }
        //             // else if (bottom_mode == BottomAmplitudeMode) {
        //             //     amp--;
        //             //     if (amp < 0) {
        //             //         amp = 0;
        //             //     }
        //             //     HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, amp);
        //             // }
        //             phase -= 30;
        //         }
        //         else if (bottom == PushDown) {
        //             // if (bottom_mode == BottomFrequencyMode) {
        //             //     bottom_mode = BottomAmplitudeMode;
        //             // }
        //             // else if (bottom_mode == BottomAmplitudeMode) {
        //             //     bottom_mode = BottomFrequencyMode;
        //             // }
        //         }
        // 		bottom = NoWork;

        // //		AD9959_enablechannel3();
        // //        AD9959_SetFreAmpPha(Frequency[Frequency_index], 400, phase);
        //         AD9959_enablechannel2();
        //         AD9959_SetFreAmpPha(2000000, 400, phase);
        //     }

        if (bottom != NoWork)
        {
            if (bottom == ClockwiseRotate)
            {
                if (bottom_mode == BottomAmplitudeMode)
                {
                    if (CWorAM == CW_MODE)
                    {
                        Amplitude_index++;
                        if (Amplitude_index > 9)
                        {
                            Amplitude_index = 9;
                            LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                           2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                           AmplitudeValidValueData[Amplitude_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                        }
                        else
                        {
                            LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                           2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                           AmplitudeValidValueData[Amplitude_index - 1], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                        }
                        LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                       2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                       AmplitudeValidValueData[Amplitude_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                    }
                    else if (CWorAM == AM_MODE)
                    {
                        AMwave_index++;
                        if (AMwave_index > 9)
                        {
                            AMwave_index = 9;
                            LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                           2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                           AmplitudeValidValueData[AMwave_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                        }
                        else
                        {
                            LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                           2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                           AmplitudeValidValueData[AMwave_index - 1], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                        }
                        LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                       2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                       AmplitudeValidValueData[AMwave_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                    }
                }
                else if (bottom_mode == BottomModulationDegreeMode)
                {
                    ModulationDegree_index++;
                    if (ModulationDegree_index > 6)
                    {
                        ModulationDegree_index = 6;
                    }
                    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(DCvoltage_Modulation[ModulationDegree_index]));
                    X9C103_Init();
                    X9C103_Inc_N_Step(MultiPath_Modulation[ModulationDegree_index]);
                    LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 3 + 16,
                                   ModulationDegreeData[ModulationDegree_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
                else if (bottom_mode == BottomFrequencyMode)
                {
                    Frequency_index++;
                    if (Frequency_index > 10)
                    {
                        Frequency_index = 10;
                    }
                    LCD_ShowNumber(2 + CHINESE_WIDTH * 5 + 2,
                                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT + 6,
                                   Frequency[Frequency_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
                else if (bottom_mode == BottomDelayMode)
                {
                    DelayTime_index++;
                    if (DelayTime_index > 6)
                    {
                        DelayTime_index = 6;
                        LCD_ShowNumber(2 + CHINESE_WIDTH * 7 + 2,
                                       2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 2 + 8,
                                       DelayTimeData[DelayTime_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    }
                    else
                    {
                        LCD_ShowNumber(2 + CHINESE_WIDTH * 7 + 2,
                                       2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 2 + 8,
                                       DelayTimeData[DelayTime_index - 1], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    }
                    LCD_ShowNumber(2 + CHINESE_WIDTH * 7 + 2,
                                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 2 + 8,
                                   DelayTimeData[DelayTime_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                    //phase += 10;
                }
                else if (bottom_mode == BottomCWorAMMode)
                {
                    CWorAM = AM_MODE;
                    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, DCvoltage_AM[AMwave_index]);
                    // LCD_DrawString(2, HEIGHT - CHAR_HEIGHT - 2, "CW", 2, CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    // LCD_DrawString(2, HEIGHT - CHAR_HEIGHT - 2, "AM", 2, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
            }
            else if (bottom == AnticlockwiseRotate)
            {
                if (bottom_mode == BottomAmplitudeMode)
                {
                    if (CWorAM == CW_MODE)
                    {
                        Amplitude_index--;
                        if (Amplitude_index < 0)
                        {
                            Amplitude_index = 0;
                            LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                           2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                           AmplitudeValidValueData[Amplitude_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                        }
                        else
                        {
                            LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                           2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                           AmplitudeValidValueData[Amplitude_index + 1], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                        }
                        LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                       2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                       AmplitudeValidValueData[Amplitude_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                    }
                    else if (CWorAM == AM_MODE)
                    {
                        AMwave_index--;
                        if (AMwave_index < 0)
                        {
                            AMwave_index = 0;
                            LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                           2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                           AmplitudeValidValueData[AMwave_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                        }
                        else
                        {
                            LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                           2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                           AmplitudeValidValueData[AMwave_index + 1], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                        }
                        LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                       2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20,
                                       AmplitudeValidValueData[AMwave_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                    }
                }
                else if (bottom_mode == BottomModulationDegreeMode)
                {
                    ModulationDegree_index--;
                    if (ModulationDegree_index < 0)
                    {
                        ModulationDegree_index = 0;
                    }
                    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(DCvoltage_Modulation[ModulationDegree_index]));
                    X9C103_Init();
                    X9C103_Inc_N_Step(MultiPath_Modulation[ModulationDegree_index]);
                    LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
                                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 3 + 16,
                                   ModulationDegreeData[ModulationDegree_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
                else if (bottom_mode == BottomFrequencyMode)
                {
                    Frequency_index--;
                    if (Frequency_index < 0)
                    {
                        Frequency_index = 0;
                    }
                    LCD_ShowNumber(2 + CHINESE_WIDTH * 5 + 2,
                                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT + 6,
                                   Frequency[Frequency_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
                else if (bottom_mode == BottomDelayMode)
                {
                    DelayTime_index--;
                    if (DelayTime_index < 0)
                    {
                        DelayTime_index = 0;
                        LCD_ShowNumber(2 + CHINESE_WIDTH * 7 + 2,
                                       2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 2 + 8,
                                       DelayTimeData[DelayTime_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    }
                    else
                    {
                        LCD_ShowNumber(2 + CHINESE_WIDTH * 7 + 2,
                                       2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 2 + 8,
                                       DelayTimeData[DelayTime_index + 1], CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    }
                    LCD_ShowNumber(2 + CHINESE_WIDTH * 7 + 2,
                                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 2 + 8,
                                   DelayTimeData[DelayTime_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                      //phase -= 10;
                }
                else if (bottom_mode == BottomCWorAMMode)
                {
                    CWorAM = CW_MODE;
                    // HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, DCvoltage[AMPLITUDE_INDEX][Amplitude_index]);
                    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, DCvoltage_CW[Frequency_index][Amplitude_index]);
                    // LCD_DrawString(2, HEIGHT - CHAR_HEIGHT - 2, "AM", 2, CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    // LCD_DrawString(2, HEIGHT - CHAR_HEIGHT - 2, "CW", 2, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
            }
            else if (bottom == PushDown)
            {
                if (bottom_mode == BottomAmplitudeMode)
                {
                    bottom_mode = BottomModulationDegreeMode;
                    LCD_DrawChinese(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 2, Amplitude, 2, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    LCD_DrawChinese(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 2, ModulationDegree, 3, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
                else if (bottom_mode == BottomModulationDegreeMode)
                {
                    bottom_mode = BottomFrequencyMode;
                    LCD_DrawChinese(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 2, ModulationDegree, 3, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    LCD_DrawChinese(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 2, FrequencyData, 2, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
                else if (bottom_mode == BottomFrequencyMode)
                {
                    bottom_mode = BottomDelayMode;
                    LCD_DrawChinese(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 2, FrequencyData, 2, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    LCD_DrawChinese(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 2, Delay, 2, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
                else if (bottom_mode == BottomDelayMode)
                {
                    bottom_mode = BottomCWorAMMode;
                    LCD_DrawChinese(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 2, Delay, 2, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    // LCD_DrawString(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 6, "CW or AM", 8, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
                else if (bottom_mode == BottomCWorAMMode)
                {
                    bottom_mode = BottomAmplitudeMode;
                    LCD_DrawString(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 6, "CW or AM", 8, CHAR_WIDTH, CHAR_HEIGHT, COLOR_DARKBLUE, COLOR_DARKBLUE);
                    LCD_DrawChinese(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 2, Amplitude, 2, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
                }
            }

            bottom = NoWork;

            //   LCD_ShowNumber(2 + CHINESE_WIDTH * 5 + 2,
            //                  2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT + 6,
            //                  Frequency[Frequency_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);

            //   LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2,
            //                  2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 6 + 26,
            //                  ModulationDegreeData[ModulationDegree_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
        }
        AD9959_enablechannel0();
        AD9959_SetFreAmpPha(Frequency[Frequency_index], 400, 0);
        AD9959_enablechannel3();
        AD9959_SetFreAmpPha(Frequency[Frequency_index], 400, 0);
        AD9959_enablechannel2();
        //AD9959_SetFreAmpPha(2000000, 400, phase);
        AD9959_SetFreAmpPha(2000000, 400, DelayPhase[DelayTime_index]);
        if (CWorAM == CW_MODE)
        {
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t)(DCvoltage_CW[Frequency_index][Amplitude_index]));
        }
        else if (CWorAM == AM_MODE)
        {
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t)(DCvoltage_AM[AMwave_index]));
        }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
