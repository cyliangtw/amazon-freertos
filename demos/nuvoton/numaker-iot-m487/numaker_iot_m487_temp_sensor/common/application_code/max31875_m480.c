#include "max31875_m480.h"
#include "max31875_c.h"
#include "max31875.h"

#define MAX31875_I2C_PORT		I2C1
#define MAX31875_I2C_FREQ		100000
#define MAX31875_TMR_PORT		TIMER1

extern I2C sI2C_m480;

int m480_i2c_read(int address, const char* reg, int reg_length, char *data, int length)
{
		int ret =0;
		if (length > 1) {
			
			switch(reg_length)
			{
				case 2:
					ret = (I2C_ReadMultiBytesTwoRegs(MAX31875_I2C_PORT, address, *((uint16_t*)reg), (uint8_t*)data, length) == length )? 0 : -1;
					break;
				case 1:
					ret = (I2C_ReadMultiBytesOneReg(MAX31875_I2C_PORT, address, *((uint8_t*)reg), (uint8_t*)data, length) == length)? 0: -1;		
					break;
				default:
					ret = -1;
					break;
			} //switch
				
		} else {
			
			uint8_t u8RByte = 0u;
			
			switch(reg_length)
			{
				case 2:
					u8RByte = I2C_ReadByteTwoRegs(MAX31875_I2C_PORT, address, *((uint16_t*)reg));
					break;
				case 1:
					u8RByte = I2C_ReadByteOneReg(MAX31875_I2C_PORT, address, *((uint8_t*)reg));
					break;
				default:
					ret = -1;
					break;
			} //switch
			
			if ( u8RByte == 0u )
				ret = -1;
			else
			{
				data[0] = u8RByte;
				ret = 0;
			}
			
		} //else

		return ret;
}

int m480_i2c_write(int address, const char *data, int length)
{
	int ret =0;
	if (length > 1)
		ret = (I2C_WriteMultiBytesOneReg(MAX31875_I2C_PORT, address, data[0], (uint8_t*)&data[1], length-1)==(length-1)) ? 0:-1 ;
	else
		ret = (I2C_WriteByteOneReg(MAX31875_I2C_PORT, address, data[0], data[1]) == 1 )?0:-1;
	return ret;
}

int8_t m480_max31875_init(uint8_t slave_id)
{	
    SYS_UnlockReg();
	
		/* Enable & Set the timer module for BME680 */
		CLK_EnableModuleClock(TMR1_MODULE);
		CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HXT, NULL);

#if 0 // MAX31875_I2C_PORT == I2C0"
	/* Enable M480 I2C module Clock */
    CLK_EnableModuleClock(I2C0_MODULE);
	
	/* Set PG.0 for I2C0_SCL, PG.1 for I2C0_SDA */
    SYS->GPG_MFPL &= ~(SYS_GPG_MFPL_PG0MFP_Msk | SYS_GPG_MFPL_PG1MFP_Msk);
    SYS->GPG_MFPL |= (SYS_GPG_MFPL_PG0MFP_I2C0_SCL | SYS_GPG_MFPL_PG1MFP_I2C0_SDA);
	
    /* I2C0_SCL(PG.0) enable schmitt trigger */
    PG->SMTEN |= GPIO_SMTEN_SMTEN0_Msk;
#elif 1 // MAX31875_I2C_PORT == I2C1
	/* Enable M480 I2C2 module Clock */
    CLK_EnableModuleClock(I2C1_MODULE);

    /* Set PG.3 for I2C1_SDA, PG.2 for I2C1_SCL */
    SYS->GPG_MFPL &= ~(SYS_GPG_MFPL_PG2MFP_Msk | SYS_GPG_MFPL_PG3MFP_Msk);
    SYS->GPG_MFPL |= (SYS_GPG_MFPL_PG3MFP_I2C1_SDA | SYS_GPG_MFPL_PG2MFP_I2C1_SCL);

    /* I2C1_SCL(PD.1) enable schmitt trigger */
    PG->SMTEN |= GPIO_SMTEN_SMTEN1_Msk;
#elif 1 // MAX31875_I2C_PORT == I2C2
	/* Enable M480 I2C2 module Clock */
    CLK_EnableModuleClock(I2C2_MODULE);

    /* Set PD.0 for I2C2_SDA, PD.1 for I2C2_SCL */
    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD0MFP_Msk | SYS_GPD_MFPL_PD1MFP_Msk);
    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD0MFP_I2C2_SDA | SYS_GPD_MFPL_PD1MFP_I2C2_SCL);

    /* I2C2_SCL(PD.1) enable schmitt trigger */
    PD->SMTEN |= GPIO_SMTEN_SMTEN1_Msk;
#endif

    /* Open I2C module and set bus clock */
    I2C_Open(MAX31875_I2C_PORT, MAX31875_I2C_FREQ);
	
		SYS_LockReg();

		sI2C_m480.read = m480_i2c_read;
		sI2C_m480.write = m480_i2c_write;
		
		return max31875_init(slave_id);
}
