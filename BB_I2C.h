/*********************************
Notes:
- Minimum ADS1113 clock speed is 10 KHz.
- Good clock speed to accomplish emulated I2C.
*********************************/
#include "stm8s.h"
//#include "stm8l15x.h"
//#include <global.h>

#define BB_I2C_PORT 	GPIOB
#define BB_I2C_SDA	GPIO_PIN_5
#define BB_I2C_SCL	GPIO_PIN_4

#define SLAVE_ADDRESS 0x68 // I2C address of the STUSB device

static uint8_t Delay_Counter;
static uint8_t SlaveAddressshiftedByOne = SLAVE_ADDRESS << 1;

/*
DELAY da sistemare per gestire timing del protocollo

*/

#define DELAY()	do {} while(0)

typedef enum
{
  BB_I2C_OK = 0,
  BB_I2C_ERR
} BB_I2C_Result;

// Init
#define I2C_INIT() do {GPIO_Init(BB_I2C_PORT, BB_I2C_SDA, GPIO_MODE_OUT_OD_HIZ_SLOW);\
GPIO_Init(BB_I2C_PORT, BB_I2C_SCL, GPIO_MODE_OUT_OD_LOW_FAST); /* with interrupt capability in input direction*/\
BB_I2C_PORT->DDR &= ~BB_I2C_SCL;   /*change clock pin to input (became High Z, therefore High Level )*/ \
                      } while(0) 

// DATA mng
#define BB_I2C_DATA_OUT() do {BB_I2C_PORT->DDR|=BB_I2C_SDA;} while(0)
#define BB_I2C_DATA_IN() do {BB_I2C_PORT->DDR&=~BB_I2C_SDA;} while(0)

#define BB_I2C_DATA_H() do {BB_I2C_PORT->ODR|=BB_I2C_SDA; DELAY();} while(0)
#define BB_I2C_DATA_L() do {BB_I2C_PORT->ODR&=~BB_I2C_SDA; DELAY();} while(0)
#define BB_I2C_DATA_TEST() (!(!(BB_I2C_PORT->IDR&BB_I2C_SDA)))

// CLOCK mng
#define BB_I2C_CLOCK_H() do {BB_I2C_PORT->DDR &= ~BB_I2C_SCL; DELAY();} while(0)
#define BB_I2C_CLOCK_L() do {BB_I2C_PORT->DDR |= BB_I2C_SCL; DELAY();} while(0)
#define BB_I2C_CLOCK_TEST() (!(!(BB_I2C_PORT->IDR & BB_I2C_SCL)))

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

static void BB_I2C_DelayLoop ( void )
{
	nop()
	nop()
}

static void BB_I2C_TxStop ( void )
{
  //BB_I2C_CLOCK_L();
  BB_I2C_DATA_OUT();
  BB_I2C_DATA_L ();
  BB_I2C_DelayLoop();

  BB_I2C_CLOCK_H ();
  BB_I2C_DelayLoop();

  BB_I2C_DATA_H (); /* data line goes from LOW to HIGH whilst clock is high */
  BB_I2C_DATA_IN();  /* at the end of a bus cycle return in input mode */
}

static void BB_I2C_TxStart ( void )
{
  //BB_I2C_DATA_IN();
  BB_I2C_DATA_H();
  BB_I2C_CLOCK_H();
  BB_I2C_DATA_OUT();
  BB_I2C_DelayLoop();

  BB_I2C_DATA_L ();
  BB_I2C_DelayLoop();
}

static bool BB_I2C_RxAck(void)
{
  bool status = TRUE; /* default status */

  BB_I2C_CLOCK_L ();
  BB_I2C_DATA_IN ();   /* prime data line as high input - slave ack is active low */
  BB_I2C_DelayLoop();
  BB_I2C_CLOCK_H();  /* clock in the acknowledge bit */
  BB_I2C_DelayLoop();

  if (BB_I2C_DATA_TEST())
  {
    status = FALSE; /* failed to received ACK - data line remained high */
  }
  BB_I2C_DATA_H();
  BB_I2C_DATA_OUT ();
  BB_I2C_CLOCK_L ();
  return (status);
}

static bool BB_I2C_TxData (uint8_t data)
{
  bool status;
  uint8_t Mask;
  /*
  * clock out 8 bit, MSB first
  */
  BB_I2C_DATA_OUT (); 
  for (Mask = 0x80; Mask >= 1u; Mask >>= 1)
  {
    BB_I2C_CLOCK_L ();  /* clock it out */
  
    if (data & Mask)
    {
	BB_I2C_DATA_H (); /* set at 1 */
    }
    else
    {
	BB_I2C_DATA_L ();  /* set at 0 */
    }
    //I2C_delayLoop();
    nop()
    nop()
    nop()
    nop()
    BB_I2C_CLOCK_H ();   /* clock it out */
    //I2C_delayLoop();				
    nop()
    nop()
    nop()
    nop()				
  }
  status = BB_I2C_RxAck(); /* clock in the slave ack bit */

  return status;
}

static void BB_I2C_RxData ( uint8_t* data, bool ack )
{
  uint8_t Mask;
  uint8_t result = 0;
		
  /*
  * clock in 8 bits, MSB first
  */
  for (Mask = 0x80; Mask >= 1u; Mask >>= 1)
  {
    BB_I2C_CLOCK_L ();
    BB_I2C_DATA_IN (); /* we should set only one time! */
    //I2C_delayLoop();
    nop()
    nop()
    nop()
    nop()

    BB_I2C_CLOCK_H ();
  
    /* handle hold condition, wait for hold 
     condition to be released (CLOCK going high) */

//        while(BB_I2C_CLOCK_TEST()==0)	
//        {
//            BSP_GO_IN_LOW_POWER_MODE2();
//		}
				
  if (BB_I2C_DATA_TEST())
  {
    /* got a 1 bit */
    result |= Mask;
  }
  else
  {
    /* got a zero bit */
  }
  //I2C_delayLoop();
    nop()
    nop()
    nop()
    nop()
  }

    *data = result;  

    /*
    * send master ack during sequential read, except on the last byte
    */
    BB_I2C_DATA_OUT ();
    BB_I2C_CLOCK_L ();
    if (ack)
    {
      BB_I2C_DATA_L ();     /* ack slave by pulling data line low */
    }
    else
    {
      BB_I2C_DATA_H ();     /* nack slave by pulling data line high */
    }
    BB_I2C_DelayLoop();
    BB_I2C_CLOCK_H ();  /* clock the ack signal out */
    BB_I2C_DelayLoop();
}

static bool BB_I2C_WriteNoStop( uint8_t address, const uint8_t* buffer, uint8_t length )
{	uint8_t i;
  bool status;
		
	disableInterrupts();
    
	BB_I2C_TxStart();  // initiate START condition
  status = BB_I2C_TxData( address & ~0x01 );  // write 1 byte - R/W bit should be 0
  if	(!status)
				return status;
  for	(i=0; i<length; i++)
    {
      status = BB_I2C_TxData( buffer[i] );           // write a byte
      if	(!status)
          return status;
    }

    // BB_I2C_DATA_IN();
		
	enableInterrupts();
  return TRUE;
}
/*
forse la cancello
*/
void Init_SigmaDelta ( void );

/*
da cancellare
*/
void Acquire_from_Sigmadelta ( void );

/*
Bit Banging I2C implements communication 
from/to Sigma-Delta
*/
void BB_I2C_Init ( void );

/*
Low level Write
*/
BB_I2C_Result BB_I2C_Write ( uint8_t* pBuffer, uint8_t NumByteToWrite, uint8_t flag, uint8_t SlaveAddressshiftedByOne );

/*
Low level Read
*/
BB_I2C_Result BB_I2C_Read ( uint8_t* pBuffer, uint8_t NumByteToRead, uint8_t SlaveAddressshiftedByOne );

/*
High level Write (write registers)
*/
BB_I2C_Result BB_I2C_WriteReg ( unsigned char regAddress, uint16_t value, uint8_t SlaveAddressshiftedByOne );

/*
High level Read (read registers)
*/
BB_I2C_Result BB_I2C_ReadReg ( uint8_t regAddress, uint16_t* pValue, uint8_t SlaveAddressshiftedByOne );

