#include "AD9959_Outset.h"
#include "AD9959.h"

// enum StepMode step_mode = FrequencyMode;

// double WaveParameter[3] = {30000000, 51, 0};
// double WaveParameterStep[3] = {1000000, 48, 30};
// double WaveParameterMax[3] = {40000000, 487, 180};
// double WaveParameterMin[3] = {30000000, 51, 0};

// WaveInformation Wave = {
// 	.Parameter[0] = 30000000, .ParameterStep[0] = 1000000, .ParameterMax[0] = 40000000, .ParameterMin[0] = 30000000,  // Frequency
// 	.Parameter[1] = 51,       .ParameterStep[1] = 48,      .ParameterMax[1] = 487,      .ParameterMin[1] = 51,        // Amplitude
// 	.Parameter[2] = 0,        .ParameterStep[2] = 30,      .ParameterMax[2] = 180,      .ParameterMin[2] = 0          // Phase
// };

// double Frequency = 30000000;
// int Amplitude = 100;
// int Phase = 0;

//--------通道寄存器地址宏定义---------------//
#define CSR 0x00   // 通道选择寄存器
#define FR1 0x01   // 功能寄存器1
#define FR2 0x02   // 功能寄存器2
#define CFR 0x03   // 通道功能寄存器
#define CFTW0 0x04 // 32位通道频率转换字寄存器
#define CPOW0 0x05 // 14位通道相位转换字寄存器
#define ACR 0x06   // 幅度控制寄存器
#define SRR 0x07   // 线性扫描定时器
#define RDW 0x08   // 线性向上扫描定时器
#define FDW 0x09   // 线性向下扫描定时器

#define uchar unsigned char
#define uint unsigned int

void AD9959_enablechannel0(void) // 通道0使能
{
	uchar ChannelSelectRegisterdata0[1] = {0x10}; // 通道选择寄存器，3线传输，数据高位优先

	WriteToAD9959ViaSpi(CSR, 1, ChannelSelectRegisterdata0, 0);
}

void AD9959_enablechannel1(void) // 通道1使能
{
	uchar ChannelSelectRegisterdata1[1] = {0x20}; // 通道选择寄存器，3线传输，数据高位优先
												  // SDIO0 数据输入 and SDIO2 数据输出
	WriteToAD9959ViaSpi(CSR, 1, ChannelSelectRegisterdata1, 0);
}
void AD9959_enablechannel2(void) // 通道1使能
{
	uchar ChannelSelectRegisterdata2[1] = {0x40}; // 通道选择寄存器，3线传输，数据高位优先
												  // SDIO0 数据输入 and SDIO2 数据输出
	WriteToAD9959ViaSpi(CSR, 1, ChannelSelectRegisterdata2, 0);
}
void AD9959_enablechannel3(void) // 通道1使能
{
	uchar ChannelSelectRegisterdata3[1] = {0x80}; // 通道选择寄存器，3线传输，数据高位优先
												  // SDIO0 数据输入 and SDIO2 数据输出
	WriteToAD9959ViaSpi(CSR, 1, ChannelSelectRegisterdata3, 0);
}

void AD9959_Setwavefrequency(double f) // 输出单个频率
{
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35}; // 单频模式时选择，启用正弦功能（sine）
	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，目的是为了实现正余弦功能
																 // （没有要求时可以不设置该寄存器同样也可以正常输出）
																 // 寄存器8、9位控制输出幅度，分四个档位
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord1data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord1data, 1); // 写入频率控制字
}

void AD9959_Setwavephase(double f, int p) // 输出相位程序可调  // -512 ~ 512 cycle
{
	uchar ChannelPhaseOffsetTuningWorddata[2];
	uchar ChannelFrequencyTuningWorddata[4];

	uchar ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35}; // 单频模式时选择，启用正弦功能（sine）
	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};
	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0); // 设置功能寄存器

	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，目的是为了实现正余弦功能

	WrPhaseOffsetTuningWorddata(p, ChannelPhaseOffsetTuningWorddata);	// 设置相位转换字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWorddata, 0); // 写入相位控制字

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWorddata);	  // 设置频率转换字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWorddata, 1); // 写入频率控制字
}

void AD9959_Setwaveamplitute(double f, int a) // 输出幅度可自定义调节
{
	uchar ChannelFrequencyTuningWorddata[4];
	uchar ASRAmplituteWordata[3];
	uchar AmplitudeControldata[3] = {0xff, 0x17, 0xff};		   // 手动控制输出幅度
	uchar ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35}; // 单频模式时选择，启用正弦功能（sine）

	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};
	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，目的是为了实现正余弦功能
	WrAmplitudeTuningWorddata1(a, AmplitudeControldata, ASRAmplituteWordata);
	WriteToAD9959ViaSpi(ACR, 3, ASRAmplituteWordata, 0); // 设置输出幅度控制模式

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWorddata);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWorddata, 1); // 写入频率转换字
}

void AD9959_SetFreAmpPha (double fre, int amp, int pha) {
	uchar ChannelFrequencyTuningWorddata[4];
	uchar ASRAmplituteWordata[3];
	uchar ChannelPhaseOffsetTuningWorddata[2];
	uchar AmplitudeControldata[3] = {0xff, 0x17, 0xff};
	uchar ChannelFunctionRegisterdata[3] = {0x00, 0x23, 0x35};
	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0);

	WrPhaseOffsetTuningWorddata(pha, ChannelPhaseOffsetTuningWorddata);
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWorddata, 0);

	WrAmplitudeTuningWorddata1(amp, AmplitudeControldata, ASRAmplituteWordata);
	WriteToAD9959ViaSpi(ACR, 3, ASRAmplituteWordata, 0);

	WrFrequencyTuningWorddata(fre, ChannelFrequencyTuningWorddata);
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWorddata, 1);
}

// 设置二阶调制
//////////////////////////////////////////////////////
void AD9959_SetFremodulation2(double f1, double f2)
{
	// 	 u8 a2 = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	// 设置起始频率S0（0x0A）

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	// 设置终止频率E0（0x04）

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord1data, 1); // 写入频率控制字
}

void AD9959_SetPhamodulation2(double f, int p1, int p2) // p1起始相位，p2终止相位
{
	// 	 uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord0data[2];
	uchar ChannelPhaseOffsetTuningWord1data[2];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x34}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 二级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};
	// 	 uchar FunctionRegister1data[3] = {0xD0,0x54,0x00};    						//二级调制时需进行幅度RU/RD时选择
	// PO进行调频，P2进行幅度RU/RD
	// 	 uchar AmplitudeControldata[3] = {0xff,0x1f,0xff}; 								//开启RU/RD

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0); // 设置功能寄存器

	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动
	//    WriteToAD9959ViaSpi(ACR,3,AmplitudeControldata,0);    					//需输出幅度RU/RD模式才选择,且要修改功能寄存器FR1
	// 设置地址0x05（CPOW0） 的相位

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 0); // 写入相位控制字
	// 设置地址0x0a（CW1） 的相位

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 0); // 写入相位控制字
	// f=2000000;    //设置输出频率
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord1data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord1data, 1); // 写入频率控制字
}

void AD9959_SetAM2(double f)
{
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelAmplitudeTuningWorddata[4] = {0x1f, 0x30, 0x00, 0x00}; // 72mV
	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff}; // 500mV
	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWorddata, 0); // 设置幅度大小（S2）
																	 // 设置输出频率大小
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	 // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}

/////////////////////////////////////////////////////////////////////

// 四阶调制   CH0/1
//////////////////////////////////////////////////////////////////////
void AD9959_SetFremodulation4(double f1, double f2, double f3, double f4)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0x01, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation4(double f, int p1, int p2, int p3, int p4)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 四级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0x01, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字
	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM4(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0x8f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0x5f, 0x30, 0x00, 0x00};

	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0x01, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}

/////////////////////////////////////////////////////////////////////////
//  四阶调制 CH2/3
/////////////////////////////////////////////////////////////////////////

void AD9959_SetFremodulation42(double f1, double f2, double f3, double f4)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0x51, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation42(double f, int p1, int p2, int p3, int p4)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 四级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0x51, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字
	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM42(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0x8f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0x5f, 0x30, 0x00, 0x00};

	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0x51, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}

/////////////////////////////////////////////////////////////////////////
//  八阶调制 CH0
/////////////////////////////////////////////////////////////////////////

void AD9959_SetFremodulation80(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFrequencyTuningWord4data[4];
	uchar ChannelFrequencyTuningWord5data[4];
	uchar ChannelFrequencyTuningWord6data[4];
	uchar ChannelFrequencyTuningWord7data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0xc2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0D, 4, ChannelFrequencyTuningWord4data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0E, 4, ChannelFrequencyTuningWord5data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0F, 4, ChannelFrequencyTuningWord6data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x10, 4, ChannelFrequencyTuningWord7data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation80(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];
	uchar ChannelPhaseOffsetTuningWord4data[4];
	uchar ChannelPhaseOffsetTuningWord5data[4];
	uchar ChannelPhaseOffsetTuningWord6data[4];
	uchar ChannelPhaseOffsetTuningWord7data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 八级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0xc2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1); // 写入相位控制字
	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM80(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];

	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0xbf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0x9f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord4data[4] = {0x7f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord5data[4] = {0x5f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord6data[4] = {0x3f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord7data[4] = {0x1f, 0x30, 0x00, 0x00};

	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0xc2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WriteToAD9959ViaSpi(0x0d, 4, ChannelAmplitudeTuningWord4data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

	WriteToAD9959ViaSpi(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}
/////////////////////////////////////////////////////////////////////////
//  八阶调制 CH1
/////////////////////////////////////////////////////////////////////////

void AD9959_SetFremodulation81(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFrequencyTuningWord4data[4];
	uchar ChannelFrequencyTuningWord5data[4];
	uchar ChannelFrequencyTuningWord6data[4];
	uchar ChannelFrequencyTuningWord7data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0xd2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0D, 4, ChannelFrequencyTuningWord4data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0E, 4, ChannelFrequencyTuningWord5data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0F, 4, ChannelFrequencyTuningWord6data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x10, 4, ChannelFrequencyTuningWord7data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation81(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];
	uchar ChannelPhaseOffsetTuningWord4data[4];
	uchar ChannelPhaseOffsetTuningWord5data[4];
	uchar ChannelPhaseOffsetTuningWord6data[4];
	uchar ChannelPhaseOffsetTuningWord7data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 八级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0xd2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1); // 写入相位控制字
	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM81(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];

	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0xbf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0x9f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord4data[4] = {0x7f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord5data[4] = {0x5f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord6data[4] = {0x3f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord7data[4] = {0x1f, 0x30, 0x00, 0x00};

	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0xd2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WriteToAD9959ViaSpi(0x0d, 4, ChannelAmplitudeTuningWord4data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

	WriteToAD9959ViaSpi(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}
/////////////////////////////////////////////////////////////////////////
//  八阶调制 CH2
/////////////////////////////////////////////////////////////////////////

void AD9959_SetFremodulation82(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFrequencyTuningWord4data[4];
	uchar ChannelFrequencyTuningWord5data[4];
	uchar ChannelFrequencyTuningWord6data[4];
	uchar ChannelFrequencyTuningWord7data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0xe2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0D, 4, ChannelFrequencyTuningWord4data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0E, 4, ChannelFrequencyTuningWord5data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0F, 4, ChannelFrequencyTuningWord6data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x10, 4, ChannelFrequencyTuningWord7data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation82(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];
	uchar ChannelPhaseOffsetTuningWord4data[4];
	uchar ChannelPhaseOffsetTuningWord5data[4];
	uchar ChannelPhaseOffsetTuningWord6data[4];
	uchar ChannelPhaseOffsetTuningWord7data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 八级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0xe2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1); // 写入相位控制字
	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM82(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];

	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0xbf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0x9f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord4data[4] = {0x7f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord5data[4] = {0x5f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord6data[4] = {0x3f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord7data[4] = {0x1f, 0x30, 0x00, 0x00};

	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0xe2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WriteToAD9959ViaSpi(0x0d, 4, ChannelAmplitudeTuningWord4data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

	WriteToAD9959ViaSpi(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}

/////////////////////////////////////////////////////////////////////////
//  八阶调制 CH3
/////////////////////////////////////////////////////////////////////////

void AD9959_SetFremodulation83(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFrequencyTuningWord4data[4];
	uchar ChannelFrequencyTuningWord5data[4];
	uchar ChannelFrequencyTuningWord6data[4];
	uchar ChannelFrequencyTuningWord7data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0xf2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0D, 4, ChannelFrequencyTuningWord4data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0E, 4, ChannelFrequencyTuningWord5data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0F, 4, ChannelFrequencyTuningWord6data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x10, 4, ChannelFrequencyTuningWord7data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation83(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];
	uchar ChannelPhaseOffsetTuningWord4data[4];
	uchar ChannelPhaseOffsetTuningWord5data[4];
	uchar ChannelPhaseOffsetTuningWord6data[4];
	uchar ChannelPhaseOffsetTuningWord7data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 八级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0xf2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1); // 写入相位控制字
	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM83(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];

	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0xbf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0x9f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord4data[4] = {0x7f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord5data[4] = {0x5f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord6data[4] = {0x3f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord7data[4] = {0x1f, 0x30, 0x00, 0x00};

	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0xf2, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WriteToAD9959ViaSpi(0x0d, 4, ChannelAmplitudeTuningWord4data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

	WriteToAD9959ViaSpi(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}

/////////////////////////////////////////////////////////////////////////
//  十六阶调制 CH0
/////////////////////////////////////////////////////////////////////////
void AD9959_SetFremodulation160(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8, double f9, double f10, double f11, double f12, double f13, double f14, double f15, double f16)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFrequencyTuningWord4data[4];
	uchar ChannelFrequencyTuningWord5data[4];
	uchar ChannelFrequencyTuningWord6data[4];
	uchar ChannelFrequencyTuningWord7data[4];
	uchar ChannelFrequencyTuningWord8data[4];
	uchar ChannelFrequencyTuningWord9data[4];
	uchar ChannelFrequencyTuningWord10data[4];
	uchar ChannelFrequencyTuningWord11data[4];
	uchar ChannelFrequencyTuningWord12data[4];
	uchar ChannelFrequencyTuningWord13data[4];
	uchar ChannelFrequencyTuningWord14data[4];
	uchar ChannelFrequencyTuningWord15data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0xc3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0D, 4, ChannelFrequencyTuningWord4data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0E, 4, ChannelFrequencyTuningWord5data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0F, 4, ChannelFrequencyTuningWord6data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x10, 4, ChannelFrequencyTuningWord7data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x11, 4, ChannelFrequencyTuningWord8data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x12, 4, ChannelFrequencyTuningWord9data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x13, 4, ChannelFrequencyTuningWord10data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x14, 4, ChannelFrequencyTuningWord11data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x15, 4, ChannelFrequencyTuningWord12data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x16, 4, ChannelFrequencyTuningWord13data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);	// 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x017, 4, ChannelFrequencyTuningWord14data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x18, 4, ChannelFrequencyTuningWord15data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation160(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];
	uchar ChannelPhaseOffsetTuningWord4data[4];
	uchar ChannelPhaseOffsetTuningWord5data[4];
	uchar ChannelPhaseOffsetTuningWord6data[4];
	uchar ChannelPhaseOffsetTuningWord7data[4];
	uchar ChannelPhaseOffsetTuningWord8data[4];
	uchar ChannelPhaseOffsetTuningWord9data[4];
	uchar ChannelPhaseOffsetTuningWord10data[4];
	uchar ChannelPhaseOffsetTuningWord11data[4];
	uchar ChannelPhaseOffsetTuningWord12data[4];
	uchar ChannelPhaseOffsetTuningWord13data[4];
	uchar ChannelPhaseOffsetTuningWord14data[4];
	uchar ChannelPhaseOffsetTuningWord15data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 十六级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0xc3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);	 // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);  // 写入相位控制字

	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM160(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];

	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xef, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0xcf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord4data[4] = {0xbf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord5data[4] = {0xaf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord6data[4] = {0x9f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord7data[4] = {0x8f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord8data[4] = {0x7f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord9data[4] = {0x6f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord10data[4] = {0x5f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord11data[4] = {0x4f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord12data[4] = {0x3f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord13data[4] = {0x2f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord14data[4] = {0x1f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord15data[4] = {0x0f, 0x30, 0x00, 0x00};
	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0xc3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WriteToAD9959ViaSpi(0x0d, 4, ChannelAmplitudeTuningWord4data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

	WriteToAD9959ViaSpi(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

	WriteToAD9959ViaSpi(0x11, 4, ChannelAmplitudeTuningWord8data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x12, 4, ChannelAmplitudeTuningWord9data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

	WriteToAD9959ViaSpi(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

	WriteToAD9959ViaSpi(0x15, 4, ChannelAmplitudeTuningWord12data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x16, 4, ChannelAmplitudeTuningWord13data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

	WriteToAD9959ViaSpi(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}
/////////////////////////////////////////////////////////////////////////
//  十六阶调制 CH1
/////////////////////////////////////////////////////////////////////////
void AD9959_SetFremodulation161(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8, double f9, double f10, double f11, double f12, double f13, double f14, double f15, double f16)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFrequencyTuningWord4data[4];
	uchar ChannelFrequencyTuningWord5data[4];
	uchar ChannelFrequencyTuningWord6data[4];
	uchar ChannelFrequencyTuningWord7data[4];
	uchar ChannelFrequencyTuningWord8data[4];
	uchar ChannelFrequencyTuningWord9data[4];
	uchar ChannelFrequencyTuningWord10data[4];
	uchar ChannelFrequencyTuningWord11data[4];
	uchar ChannelFrequencyTuningWord12data[4];
	uchar ChannelFrequencyTuningWord13data[4];
	uchar ChannelFrequencyTuningWord14data[4];
	uchar ChannelFrequencyTuningWord15data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0xd3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0D, 4, ChannelFrequencyTuningWord4data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0E, 4, ChannelFrequencyTuningWord5data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0F, 4, ChannelFrequencyTuningWord6data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x10, 4, ChannelFrequencyTuningWord7data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x11, 4, ChannelFrequencyTuningWord8data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x12, 4, ChannelFrequencyTuningWord9data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x13, 4, ChannelFrequencyTuningWord10data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x14, 4, ChannelFrequencyTuningWord11data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x15, 4, ChannelFrequencyTuningWord12data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x16, 4, ChannelFrequencyTuningWord13data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);	// 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x017, 4, ChannelFrequencyTuningWord14data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x18, 4, ChannelFrequencyTuningWord15data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation161(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];
	uchar ChannelPhaseOffsetTuningWord4data[4];
	uchar ChannelPhaseOffsetTuningWord5data[4];
	uchar ChannelPhaseOffsetTuningWord6data[4];
	uchar ChannelPhaseOffsetTuningWord7data[4];
	uchar ChannelPhaseOffsetTuningWord8data[4];
	uchar ChannelPhaseOffsetTuningWord9data[4];
	uchar ChannelPhaseOffsetTuningWord10data[4];
	uchar ChannelPhaseOffsetTuningWord11data[4];
	uchar ChannelPhaseOffsetTuningWord12data[4];
	uchar ChannelPhaseOffsetTuningWord13data[4];
	uchar ChannelPhaseOffsetTuningWord14data[4];
	uchar ChannelPhaseOffsetTuningWord15data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 十六级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0xd3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);	 // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);  // 写入相位控制字

	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM161(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];

	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xef, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0xcf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord4data[4] = {0xbf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord5data[4] = {0xaf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord6data[4] = {0x9f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord7data[4] = {0x8f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord8data[4] = {0x7f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord9data[4] = {0x6f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord10data[4] = {0x5f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord11data[4] = {0x4f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord12data[4] = {0x3f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord13data[4] = {0x2f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord14data[4] = {0x1f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord15data[4] = {0x0f, 0x30, 0x00, 0x00};
	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0xd3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WriteToAD9959ViaSpi(0x0d, 4, ChannelAmplitudeTuningWord4data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

	WriteToAD9959ViaSpi(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

	WriteToAD9959ViaSpi(0x11, 4, ChannelAmplitudeTuningWord8data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x12, 4, ChannelAmplitudeTuningWord9data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

	WriteToAD9959ViaSpi(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

	WriteToAD9959ViaSpi(0x15, 4, ChannelAmplitudeTuningWord12data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x16, 4, ChannelAmplitudeTuningWord13data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

	WriteToAD9959ViaSpi(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}

/////////////////////////////////////////////////////////////////////////
//  十六阶调制 CH2
/////////////////////////////////////////////////////////////////////////
void AD9959_SetFremodulation162(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8, double f9, double f10, double f11, double f12, double f13, double f14, double f15, double f16)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFrequencyTuningWord4data[4];
	uchar ChannelFrequencyTuningWord5data[4];
	uchar ChannelFrequencyTuningWord6data[4];
	uchar ChannelFrequencyTuningWord7data[4];
	uchar ChannelFrequencyTuningWord8data[4];
	uchar ChannelFrequencyTuningWord9data[4];
	uchar ChannelFrequencyTuningWord10data[4];
	uchar ChannelFrequencyTuningWord11data[4];
	uchar ChannelFrequencyTuningWord12data[4];
	uchar ChannelFrequencyTuningWord13data[4];
	uchar ChannelFrequencyTuningWord14data[4];
	uchar ChannelFrequencyTuningWord15data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0xe3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0D, 4, ChannelFrequencyTuningWord4data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0E, 4, ChannelFrequencyTuningWord5data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0F, 4, ChannelFrequencyTuningWord6data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x10, 4, ChannelFrequencyTuningWord7data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x11, 4, ChannelFrequencyTuningWord8data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x12, 4, ChannelFrequencyTuningWord9data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x13, 4, ChannelFrequencyTuningWord10data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x14, 4, ChannelFrequencyTuningWord11data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x15, 4, ChannelFrequencyTuningWord12data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x16, 4, ChannelFrequencyTuningWord13data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);	// 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x017, 4, ChannelFrequencyTuningWord14data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x18, 4, ChannelFrequencyTuningWord15data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation162(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];
	uchar ChannelPhaseOffsetTuningWord4data[4];
	uchar ChannelPhaseOffsetTuningWord5data[4];
	uchar ChannelPhaseOffsetTuningWord6data[4];
	uchar ChannelPhaseOffsetTuningWord7data[4];
	uchar ChannelPhaseOffsetTuningWord8data[4];
	uchar ChannelPhaseOffsetTuningWord9data[4];
	uchar ChannelPhaseOffsetTuningWord10data[4];
	uchar ChannelPhaseOffsetTuningWord11data[4];
	uchar ChannelPhaseOffsetTuningWord12data[4];
	uchar ChannelPhaseOffsetTuningWord13data[4];
	uchar ChannelPhaseOffsetTuningWord14data[4];
	uchar ChannelPhaseOffsetTuningWord15data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 十六级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0xe3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);	 // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);  // 写入相位控制字

	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM162(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];

	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xef, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0xcf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord4data[4] = {0xbf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord5data[4] = {0xaf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord6data[4] = {0x9f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord7data[4] = {0x8f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord8data[4] = {0x7f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord9data[4] = {0x6f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord10data[4] = {0x5f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord11data[4] = {0x4f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord12data[4] = {0x3f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord13data[4] = {0x2f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord14data[4] = {0x1f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord15data[4] = {0x0f, 0x30, 0x00, 0x00};
	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0xe3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WriteToAD9959ViaSpi(0x0d, 4, ChannelAmplitudeTuningWord4data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

	WriteToAD9959ViaSpi(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

	WriteToAD9959ViaSpi(0x11, 4, ChannelAmplitudeTuningWord8data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x12, 4, ChannelAmplitudeTuningWord9data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

	WriteToAD9959ViaSpi(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

	WriteToAD9959ViaSpi(0x15, 4, ChannelAmplitudeTuningWord12data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x16, 4, ChannelAmplitudeTuningWord13data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

	WriteToAD9959ViaSpi(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}

/////////////////////////////////////////////////////////////////////////
//  十六阶调制 CH3
/////////////////////////////////////////////////////////////////////////

void AD9959_SetFremodulation163(double f1, double f2, double f3, double f4, double f5, double f6, double f7, double f8, double f9, double f10, double f11, double f12, double f13, double f14, double f15, double f16)
{
	// 	 u8 a = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelFrequencyTuningWord1data[4];
	uchar ChannelFrequencyTuningWord2data[4];
	uchar ChannelFrequencyTuningWord3data[4];
	uchar ChannelFrequencyTuningWord4data[4];
	uchar ChannelFrequencyTuningWord5data[4];
	uchar ChannelFrequencyTuningWord6data[4];
	uchar ChannelFrequencyTuningWord7data[4];
	uchar ChannelFrequencyTuningWord8data[4];
	uchar ChannelFrequencyTuningWord9data[4];
	uchar ChannelFrequencyTuningWord10data[4];
	uchar ChannelFrequencyTuningWord11data[4];
	uchar ChannelFrequencyTuningWord12data[4];
	uchar ChannelFrequencyTuningWord13data[4];
	uchar ChannelFrequencyTuningWord14data[4];
	uchar ChannelFrequencyTuningWord15data[4];
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x23, 0x30}; // 无RU/RD
	uchar FunctionRegister1data[3] = {0xD0, 0xf3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWord0data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x04, 4, ChannelFrequencyTuningWord0data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWord1data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWord1data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f3, ChannelFrequencyTuningWord2data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0B, 4, ChannelFrequencyTuningWord2data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f4, ChannelFrequencyTuningWord3data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0C, 4, ChannelFrequencyTuningWord3data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f5, ChannelFrequencyTuningWord4data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0D, 4, ChannelFrequencyTuningWord4data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f6, ChannelFrequencyTuningWord5data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0E, 4, ChannelFrequencyTuningWord5data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f7, ChannelFrequencyTuningWord6data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x0F, 4, ChannelFrequencyTuningWord6data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f8, ChannelFrequencyTuningWord7data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x10, 4, ChannelFrequencyTuningWord7data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f9, ChannelFrequencyTuningWord8data);	  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x11, 4, ChannelFrequencyTuningWord8data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f10, ChannelFrequencyTuningWord9data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x12, 4, ChannelFrequencyTuningWord9data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f11, ChannelFrequencyTuningWord10data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x13, 4, ChannelFrequencyTuningWord10data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f12, ChannelFrequencyTuningWord11data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x14, 4, ChannelFrequencyTuningWord11data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f13, ChannelFrequencyTuningWord12data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x15, 4, ChannelFrequencyTuningWord12data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f14, ChannelFrequencyTuningWord13data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x16, 4, ChannelFrequencyTuningWord13data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f15, ChannelFrequencyTuningWord14data);	// 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x017, 4, ChannelFrequencyTuningWord14data, 0); // 写入频率控制字

	WrFrequencyTuningWorddata(f16, ChannelFrequencyTuningWord15data);  // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(0x18, 4, ChannelFrequencyTuningWord15data, 1); // 写入频率控制字
}
void AD9959_SetPhamodulation163(double f, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16)
{
	// 	 u8 b = 0;
	uchar ChannelFrequencyTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord0data[4];
	uchar ChannelPhaseOffsetTuningWord1data[4];
	uchar ChannelPhaseOffsetTuningWord2data[4];
	uchar ChannelPhaseOffsetTuningWord3data[4];
	uchar ChannelPhaseOffsetTuningWord4data[4];
	uchar ChannelPhaseOffsetTuningWord5data[4];
	uchar ChannelPhaseOffsetTuningWord6data[4];
	uchar ChannelPhaseOffsetTuningWord7data[4];
	uchar ChannelPhaseOffsetTuningWord8data[4];
	uchar ChannelPhaseOffsetTuningWord9data[4];
	uchar ChannelPhaseOffsetTuningWord10data[4];
	uchar ChannelPhaseOffsetTuningWord11data[4];
	uchar ChannelPhaseOffsetTuningWord12data[4];
	uchar ChannelPhaseOffsetTuningWord13data[4];
	uchar ChannelPhaseOffsetTuningWord14data[4];
	uchar ChannelPhaseOffsetTuningWord15data[4];

	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0x03, 0x30}; // 相位调制模式启用（连续扫描可能开启）
	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};		   // 十六级调制时启用，默认输出最大
	uchar FunctionRegister1data[3] = {0xD0, 0xf3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器，即相位调制启动

	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWord0data);	 // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWord0data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWord1data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWord1data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p3, ChannelPhaseOffsetTuningWord2data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0b, 4, ChannelPhaseOffsetTuningWord2data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p4, ChannelPhaseOffsetTuningWord3data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0c, 4, ChannelPhaseOffsetTuningWord3data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p5, ChannelPhaseOffsetTuningWord4data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0d, 4, ChannelPhaseOffsetTuningWord4data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p6, ChannelPhaseOffsetTuningWord5data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x0e, 4, ChannelPhaseOffsetTuningWord5data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p7, ChannelPhaseOffsetTuningWord6data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x0f, 4, ChannelPhaseOffsetTuningWord6data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p8, ChannelPhaseOffsetTuningWord7data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x10, 4, ChannelPhaseOffsetTuningWord7data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p9, ChannelPhaseOffsetTuningWord8data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x11, 4, ChannelPhaseOffsetTuningWord8data, 1); // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p10, ChannelPhaseOffsetTuningWord9data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x12, 4, ChannelPhaseOffsetTuningWord9data, 1);	 // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p11, ChannelPhaseOffsetTuningWord10data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x13, 4, ChannelPhaseOffsetTuningWord10data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p12, ChannelPhaseOffsetTuningWord11data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x14, 4, ChannelPhaseOffsetTuningWord11data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p13, ChannelPhaseOffsetTuningWord12data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x15, 4, ChannelPhaseOffsetTuningWord12data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p14, ChannelPhaseOffsetTuningWord13data); // 将十进制相位数转换为2进制相位控制字?
	WriteToAD9959ViaSpi(0x16, 4, ChannelPhaseOffsetTuningWord13data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p15, ChannelPhaseOffsetTuningWord14data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x17, 4, ChannelPhaseOffsetTuningWord14data, 1);  // 写入相位控制字

	WrPhaseOffsetTuningWorddata(p16, ChannelPhaseOffsetTuningWord15data); // 将十进制相位数转换为2进制相位控制字
	WriteToAD9959ViaSpi(0x18, 4, ChannelPhaseOffsetTuningWord15data, 1);  // 写入相位控制字

	f = 200000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data);	   // 将十进制频率数转换为2进制频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1); // 写入频率控制字
}
void AD9959_SetAM163(double f)
{
	// 	 u8 cn = 0;
	uchar ChannelFrequencyTuningWord0data[4];

	uchar AmplitudeControldata[3] = {0x00, 0x03, 0xff};
	uchar ChannelAmplitudeTuningWord1data[4] = {0xef, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord2data[4] = {0xdf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord3data[4] = {0xcf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord4data[4] = {0xbf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord5data[4] = {0xaf, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord6data[4] = {0x9f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord7data[4] = {0x8f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord8data[4] = {0x7f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord9data[4] = {0x6f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord10data[4] = {0x5f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord11data[4] = {0x4f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord12data[4] = {0x3f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord13data[4] = {0x2f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord14data[4] = {0x1f, 0x30, 0x00, 0x00};
	uchar ChannelAmplitudeTuningWord15data[4] = {0x0f, 0x30, 0x00, 0x00};
	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x03, 0x30};
	uchar FunctionRegister1data[3] = {0xD0, 0xf3, 0x00};

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); // 设置通道功能寄存器

	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWord1data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0b, 4, ChannelAmplitudeTuningWord2data, 0);

	WriteToAD9959ViaSpi(0x0c, 4, ChannelAmplitudeTuningWord3data, 0);

	WriteToAD9959ViaSpi(0x0d, 4, ChannelAmplitudeTuningWord4data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x0e, 4, ChannelAmplitudeTuningWord5data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x0f, 4, ChannelAmplitudeTuningWord6data, 0);

	WriteToAD9959ViaSpi(0x10, 4, ChannelAmplitudeTuningWord7data, 0);

	WriteToAD9959ViaSpi(0x11, 4, ChannelAmplitudeTuningWord8data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x12, 4, ChannelAmplitudeTuningWord9data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x13, 4, ChannelAmplitudeTuningWord10data, 0);

	WriteToAD9959ViaSpi(0x14, 4, ChannelAmplitudeTuningWord11data, 0);

	WriteToAD9959ViaSpi(0x15, 4, ChannelAmplitudeTuningWord12data, 0); // 设置幅度大小（S1）

	WriteToAD9959ViaSpi(0x16, 4, ChannelAmplitudeTuningWord13data, 0); // 设置幅度大小（S2）

	WriteToAD9959ViaSpi(0x17, 4, ChannelAmplitudeTuningWord14data, 0);

	WriteToAD9959ViaSpi(0x18, 4, ChannelAmplitudeTuningWord15data, 0);

	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}

/////////////////////////////////////////////////////////////////////////
void AD9959_Frequency_Sweep(double f1, double f2, double a1, double a2) // f1起始频率，f2终止频率，a1上升δ，a2下降δ
{
	uchar ChannelFrequencyTuningWorddata1[4];
	uchar ChannelFrequencyTuningWorddata2[4];
	uchar ChannelFrequencyTuningWorddata3[4];
	uchar ChannelFrequencyTuningWorddata4[4];
	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};	   // 默认情况下选择
	uchar ChannelFunctionRegisterdata[3] = {0x80, 0x43, 0x20}; // 扫频模式时选择;连续扫描没有启用，无RU/RD
	uchar SweepRampRatedata[2] = {0xff, 0xff};				   // 默认单位扫描时间最长
	// uchar FunctionRegister1data[3] = {0xD0,0x04,0x00};    							//线性扫描时需要RU/RD时选择开启
	//  uchar AmplitudeControldata[3] = {0xff,0x1f,0xff};									//开启RU/RD

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); //  设置通道功能寄存器
																 // 	WriteToAD9959ViaSpi(ACR,3,AmplitudeControldata,1); 								//需输出幅度RU/RD模式才选择,且要修改功能寄存器FR1
																 // 	f=500000;    // 设置起始频率S0（0x04）

	WrFrequencyTuningWorddata(f1, ChannelFrequencyTuningWorddata1); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWorddata1, 0);
	// 	f=5000000; //设置终止频率(0x0A)

	WrFrequencyTuningWorddata(f2, ChannelFrequencyTuningWorddata2); // 写频率控制字
	WriteToAD9959ViaSpi(0x0A, 4, ChannelFrequencyTuningWorddata2, 0);
	// 	f=100;   //设置上升δ

	WrFrequencyTuningWorddata(a1, ChannelFrequencyTuningWorddata3); // 写频率控制字
	WriteToAD9959ViaSpi(RDW, 4, ChannelFrequencyTuningWorddata3, 0);

	//   f=100;  //设置下降δ
	WrFrequencyTuningWorddata(a2, ChannelFrequencyTuningWorddata4); // 写频率控制字
	WriteToAD9959ViaSpi(FDW, 4, ChannelFrequencyTuningWorddata4, 0);

	WriteToAD9959ViaSpi(SRR, 2, SweepRampRatedata, 1); // 设置单位步进时间
}

void AD9959_Phase_Sweep(int p1, int p2, int a1, int a2, double f) // p1起始相位，p2终止相位，a1设置上升δ，a2设置下降δ
{
	uchar ChannelPhaseOffsetTuningWorddata1[2];
	uchar ChannelPhaseOffsetTuningWorddata2[2];
	uchar ChannelPhaseOffsetTuningWorddata3[2];
	uchar ChannelPhaseOffsetTuningWorddata4[2];
	uchar ChannelFrequencyTuningWord0data[4];

	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};	   // 默认情况下选择
	uchar ChannelFunctionRegisterdata[3] = {0xc0, 0xC3, 0x30}; // 扫相模式时选择，连续扫描没有启用，无RU/RD
	uchar SweepRampRatedata[2] = {0xff, 0xff};				   // 默认单位扫描时间最长
	// uchar FunctionRegister1data[3] = {0xD0,0x04,0x00};    						 //线性扫描时需要RU/RD时选择开启
	//  uchar AmplitudeControldata[3] = {0xff,0x1f,0xff};								 //开启RU/RD

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0);		 // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0); //  设置通道功能寄存器
																 // 	 WriteToAD9959ViaSpi(ACR,3,AmplitudeControldata,1); 						 //需输出幅度RU/RD模式才选择,且要修改功能寄存器FR1
																 // 	 p=0;// 设置起始相位S0（0x04）   数据类型与子函数里面不一样
	WrPhaseOffsetTuningWorddata(p1, ChannelPhaseOffsetTuningWorddata1);
	WriteToAD9959ViaSpi(CPOW0, 2, ChannelPhaseOffsetTuningWorddata1, 0);
	//    p=360;//设置终止相位E0(0x0A)
	WrPhaseOffsetTuningWorddata(p2, ChannelPhaseOffsetTuningWorddata2);
	WriteToAD9959ViaSpi(0x0a, 4, ChannelPhaseOffsetTuningWorddata2, 0);
	//    p=10;//设置上升δ
	WrPhaseOffsetTuningWorddata(a1, ChannelPhaseOffsetTuningWorddata3);
	WriteToAD9959ViaSpi(RDW, 4, ChannelPhaseOffsetTuningWorddata3, 0);
	//    p=10;//设置下降δ
	WrPhaseOffsetTuningWorddata(a2, ChannelPhaseOffsetTuningWorddata4);
	WriteToAD9959ViaSpi(FDW, 4, ChannelPhaseOffsetTuningWorddata4, 0);
	WriteToAD9959ViaSpi(SRR, 2, SweepRampRatedata, 0); // 写单位步进时间
	// 	 f=400000;    //设置输出频率大小
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWord0data); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWord0data, 1);
}

void AD9959_Amplitute_Sweep(int a, int a1, int a2, double f) // a终止幅度，a1上升δ，a2下降δ，f输出频率
{
	uchar ChannelAmplitudeTuningWorddata1[4];
	uchar ChannelAmplitudeTuningWorddata2[4];
	uchar ChannelAmplitudeTuningWorddata3[4];
	uchar ChannelFrequencyTuningWorddata4[4];

	uchar FunctionRegister1data[3] = {0xD0, 0x00, 0x00};	   // 默认情况下选择
	uchar ChannelFunctionRegisterdata[3] = {0x40, 0x43, 0x20}; // 幅度扫描位启动
	uchar AmplitudeControldata[3] = {0x00, 0x0, 0x3f};		   // 幅度扫描模式时启用
	uchar SweepRampRatedata[2] = {0xff, 0xff};				   // 默认单位扫描时间最长

	WriteToAD9959ViaSpi(FR1, 3, FunctionRegister1data, 0); // 设置功能寄存器
	WriteToAD9959ViaSpi(CFR, 3, ChannelFunctionRegisterdata, 0);
	// 写起始幅度S0
	WriteToAD9959ViaSpi(ACR, 3, AmplitudeControldata, 0);
	// 写终止幅度E0
	// 	a=1023; //写上升δ,不能写到1024，最大为1023
	WrAmplitudeTuningWorddata(a, ChannelAmplitudeTuningWorddata1);
	WriteToAD9959ViaSpi(0x0a, 4, ChannelAmplitudeTuningWorddata1, 0); // CTW0 address 0x04.输出10MHZ频率
	// 	a=10; //写上升δ
	WrAmplitudeTuningWorddata(a1, ChannelAmplitudeTuningWorddata2); // 写频率控制字
	WriteToAD9959ViaSpi(RDW, 4, ChannelAmplitudeTuningWorddata2, 0);
	//   a=10;  //写下降δ
	WrAmplitudeTuningWorddata(a2, ChannelAmplitudeTuningWorddata3); // 写频率控制字
	WriteToAD9959ViaSpi(FDW, 4, ChannelAmplitudeTuningWorddata3, 0);
	WriteToAD9959ViaSpi(SRR, 2, SweepRampRatedata, 0); // 写单位步进时间
	// 	f=400000;
	WrFrequencyTuningWorddata(f, ChannelFrequencyTuningWorddata4); // 写频率控制字
	WriteToAD9959ViaSpi(CFTW0, 4, ChannelFrequencyTuningWorddata4, 1);
}
