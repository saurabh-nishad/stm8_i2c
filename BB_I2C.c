#include <BB_I2C.h>

void BB_I2C_Init ( void )
{
  I2C_INIT();
  BB_I2C_CLOCK_H();
  BB_I2C_DATA_H();
  BB_I2C_DATA_OUT();
}

BB_I2C_Result BB_I2C_Write ( uint8_t* pBuffer, uint8_t NumByteToWrite, uint8_t flag, uint8_t SlaveAddressshiftedByOne )
{
	bool rc;
  disableInterrupts();
			
  rc = BB_I2C_WriteNoStop(SlaveAddressshiftedByOne, pBuffer, NumByteToWrite);

  if((rc)&&(flag!=0))
    {
      BB_I2C_TxStop();   // send STOP
    }
	  
	enableInterrupts();
  
	return rc ? BB_I2C_OK : BB_I2C_ERR;
}

BB_I2C_Result BB_I2C_Read ( uint8_t* pBuffer, uint8_t NumByteToRead, uint8_t SlaveAddressshiftedByOne )
{
	bool ack = FALSE;
  uint8_t dummy;
	
  disableInterrupts();
    
  BB_I2C_TxStart ();

  /* send READ request to slave - READ_BIT is set to indicate a READ op */
  ack = BB_I2C_TxData (SlaveAddressshiftedByOne | 0x01);

  if (ack)
    {
      if (pBuffer != (uint8_t*)0)
        {
          while (NumByteToRead--)
            {
              /* read the data - master-receiver acks slave-transmitter, except last byte */
              BB_I2C_RxData (pBuffer++, NumByteToRead != 0u);
            }
        }
    else
        {
         // Poll acknowledge, clock out 1 byte from slave, don't send ack
          BB_I2C_RxData (&dummy, FALSE);
        }
    }

    /* always transmit the stop bit, regardless of ack status */
	BB_I2C_TxStop ();
	enableInterrupts();
    
	return ack ? BB_I2C_OK : BB_I2C_ERR;;
}

BB_I2C_Result BB_I2C_WriteReg (unsigned char regAddress, uint16_t value, uint8_t SlaveAddressshiftedByOne)
{
	unsigned char buffer[3];

  buffer[0] = regAddress;
	// might need to modify the order of writting: 
  buffer[2] = (value & 0xff00) >> 8; // buffer[2] = (value & 0xff00) >> 8;
  buffer[1] = (value & 0x00ff); // buffer[1] = (value & 0x00ff);

  if	(BB_I2C_Write(&buffer[0], 3, 1, SlaveAddressshiftedByOne))
      return BB_I2C_ERR;

  return BB_I2C_OK;
}

BB_I2C_Result BB_I2C_ReadReg (uint8_t regAddress, uint16_t* pValue, uint8_t SlaveAddressshiftedByOne)
{
  uint8_t buffer[3];

  if (BB_I2C_Write(&regAddress, 1, 0, SlaveAddressshiftedByOne))
			return BB_I2C_ERR;
    
  if (BB_I2C_Read((uint8_t *)&buffer[0],3, SlaveAddressshiftedByOne))
			return BB_I2C_ERR;
  
  *pValue = (buffer[1] <<8) + buffer[0]; 

  return BB_I2C_OK;
}
/*
da cancellare
*/
void Init_SigmaDelta ( void )
{}

/*
da cancellare
*/
void Acquire_from_Sigmadelta ( void )
{}