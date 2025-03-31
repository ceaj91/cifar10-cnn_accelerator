
#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xaxidma.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xil_cache.h"
#include "sleep.h"
#include "xdebug.h"
#include "cnn_parametars.h"


#define DMA_DEV_ID    XPAR_AXIDMA_0_DEVICE_ID     // DMA Device ID
#define DMA_BASEADDR  XPAR_AXI_DMA_0_BASEADDR     // DMA BaseAddr
#define DDR_BASE_ADDR   XPAR_PS7_DDR_0_S_AXI_BASEADDR // DDR START ADDRESS
#define MEM_BASE_ADDR (DDR_BASE_ADDR + 0x1000000)   // MEM START ADDRESS
//CNN parametars
#define CNN_DEV_ID XPAR_CNN_IP_0_DEVICE_ID
#define CNN_BASEADDR XPAR_CNN_IP_0_S00_AXI_BASEADDR

// REGISTER OFFSETS FOR DMA
// MEMORY TO STREAM REGISTER OFFSETS
#define MM2S_DMACR_OFFSET 0x00
#define MM2S_DMASR_OFFSET   0x04
#define MM2S_SA_OFFSET    0x18
#define MM2S_SA_MSB_OFFSET  0x1c
#define MM2S_LENGTH_OFFSET  0x28
// STREAM TO MEMORY REGISTER OFFSETS
#define S2MM_DMACR_OFFSET 0x30
#define S2MM_DMASR_OFFSET   0x34
#define S2MM_DA_OFFSET    0x48
#define S2MM_DA_MSB_OFFSET  0x4c
#define S2MM_LENGTH_OFFSET  0x58

// FLAG BITS INSIDE DMACR REGISTER
#define DMACR_IOC_IRQ_EN  (1 << 12) // this is IOC_IrqEn bit in DMACR register
#define DMACR_ERR_IRQ_EN  (1 << 14) // this is Err_IrqEn bit in DMACR register
#define DMACR_RESET     (1 << 2)  // this is Reset bit in DMACR register
#define DMACR_RS       1      // this is RS bit in DMACR register

#define DMASR_IOC_IRQ     (1 << 12) // this is IOC_Irq bit in DMASR register
#define IDLE_MASK   XAXIDMA_IDLE_MASK  /**< DMA channel idle */


// TRANSMIT TRANSFER (MEMORY TO STREAM) INTERRUPT ID
#define TX_INTR_ID    XPAR_FABRIC_AXI_DMA_0_MM2S_INTROUT_INTR
// TRANSMIT TRANSFER (MEMORY TO STREAM) BUFFER START ADDRESS
#define TX_BUFFER_BASE  (MEM_BASE_ADDR + 0x00001000)


// RECIEVE TRANSFER (STREAM TO MEMORY) INTERRUPT ID
#define RX_INTR_ID    XPAR_FABRIC_AXI_DMA_0_S2MM_INTROUT_INTR
// RECIEVE TRANSFER (STREAM TO MEMORY) BUFFER START ADDRESS
#define RX_BUFFER_BASE  (MEM_BASE_ADDR + 0x00010000)

//CNN INTERUPT END COMMAND
#define INTERUPT_COMMAND_DONE_ID XPAR_FABRIC_CNN_IP_0_INTERUPT_DONE_INTR


// INTERRUPT CONTROLLER DEVICE ID
#define INTC_DEVICE_ID  XPAR_PS7_SCUGIC_0_DEVICE_ID

//WTF IS THIS
#define RESET_TIMEOUT_COUNTER 10000

// AMOUNT OF BYTES IN A TRANSFER
#define XFER_LENGTH 128*2
//COMMANDS FOR CNN IP
#define IDLE_CMD 0x00000000
#define LOAD_BIAS_CMD 0x00000001
#define LOAD_WEIGHTS_0_CMD 0x00000002
#define LOAD_PICTURE_0_CMD 0x00000004
#define DO_CONV_0_CMD 0x00000008
#define LOAD_WEIGHTS_1_CMD 0x00000010
#define LOAD_PICTURE_1_CMD 0x00000020
#define DO_CONV_1_CMD 0x00000040
#define LOAD_WEIGHTS_2_CMD 0x00000080
#define LOAD_PICTURE_2_CMD 0x00000100
#define DO_CONV_2_CMD 0x00000200
#define RESET_CMD 0x00000400
#define SEND_OUTPUT_FROM_CONV_0_CMD 0x00000800
#define SEND_OUTPUT_FROM_CONV_1_CMD 0x00001000
#define SEND_OUTPUT_FROM_CONV_2_CMD 0x00002000

//System functions, define whole system
static void Disable_Interrupt_System();
static void End_Command_Interrupt_Handler(void *Callback);
u32 Setup_Interrupt(u32 DeviceId, Xil_InterruptHandler Handler, u32 interrupt_ID);
void DMA_init_interrupts();

void load_float_in_tx_buffer(float* float_array,u16* u16_array,int num_of_parametars);
u16 castFloatToBin(float val);
float castBinToFloat(u16 val);
XScuGic_Config *IntcConfig;
static XScuGic INTCInst;

XAxiDma_Config *myDmaConfig;
XAxiDma myDma;

u16 TxBuffer[18432];
u16 RxBuffer[32768];

volatile int end_command_done=0;


int main()
{
	int layer_to_print = 2;  //chose which output layer you want to print
	Xil_DCacheDisable();
	Xil_ICacheDisable();
	init_platform();
	u32 status;
	myDmaConfig = XAxiDma_LookupConfigBaseAddr(DMA_BASEADDR);
	status = XAxiDma_CfgInitialize(&myDma, myDmaConfig);
	if(status != XST_SUCCESS){
		  print("DMA initialization failed\n");
		  return -1;
	 }

	Xil_DCacheFlushRange((u32)TxBuffer,18432*sizeof(u16));
	Xil_DCacheFlushRange((u32)RxBuffer,32768*sizeof(u16));
	status=Setup_Interrupt(INTC_DEVICE_ID, (Xil_InterruptHandler)End_Command_Interrupt_Handler, INTERUPT_COMMAND_DONE_ID);
	if(status != XST_SUCCESS){
			  print("Interupt initialization failed\n");
			  return -1;
		 }
	DMA_init_interrupts();
	xil_printf("PRINTING OUTPUT FROM LAYER : %d\r\n",layer_to_print);
//----------------------------------------------- 0. CONV 0 LAYER ------------------------------------------------------
	//RESET IP
	//-----------------------------------------------------------------------------------------------------------------------
	Xil_Out32(CNN_BASEADDR ,  (UINTPTR)RESET_CMD);
    //Xil_Out32(CNN_BASEADDR ,  (UINTPTR)IDLE_CMD);
	//-----------------------------------------------------------------------------------------------------------------------
    end_command_done=0;
    load_float_in_tx_buffer(bias,TxBuffer,128);
    Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_BIAS_CMD);
    status = XAxiDma_SimpleTransfer(&myDma, (u32)TxBuffer, 128*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
    if(status != XST_SUCCESS){
      	xil_printf("Greska u slanju transakcije\r\n");
    }
    //while ((XAxiDma_Busy(&myDma,XAXIDMA_DMA_TO_DEVICE)) && !end_command_done);
    while(!end_command_done);
    end_command_done=0;
    //xil_printf("\r\nBias prosao\r\n");
	//-----------------------------------------------------------------------------------------------------------------------
    load_float_in_tx_buffer(weights0_formated,TxBuffer,864);
    Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_WEIGHTS_0_CMD);
    status = XAxiDma_SimpleTransfer(&myDma, (u32)TxBuffer, 864*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
	if(status != XST_SUCCESS){
		xil_printf("Greska u slanju transakcije\r\n");
	}
    //while ((XAxiDma_Busy(&myDma,XAXIDMA_DMA_TO_DEVICE)) && !end_command_done);
	while(!end_command_done);
	end_command_done=0;
	//end_command_done=0;
    //xil_printf("Weights prosao\r\n");

	//-----------------------------------------------------------------------------------------------------------------------
		load_float_in_tx_buffer(picture_conv0_input,TxBuffer,3468);
        Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_PICTURE_0_CMD);
        status = XAxiDma_SimpleTransfer(&myDma, (u32)TxBuffer, 3468*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
    	if(status != XST_SUCCESS){
    		xil_printf("Greska u slanju transakcije\r\n");
    	}
        //while ((XAxiDma_Busy(&myDma,XAXIDMA_DMA_TO_DEVICE)) && !end_command_done);
        while(!end_command_done);
    	end_command_done=0;
        //xil_printf("Picture prosao\r\n");

    //-----------------------------------------------------------------------------------------------------------------------
     // Xil_Out32(CNN_BASEADDR ,  (UINTPTR)RESET_CMD);
      Xil_Out32(CNN_BASEADDR ,  (UINTPTR)DO_CONV_0_CMD);
	  while(!end_command_done);
	  end_command_done=0;
	  xil_printf("CONV 0 DONE!\r\n");
	//-----------------------------------------------------------------------------------------------------------------------
	 // Xil_Out32(CNN_BASEADDR ,  (UINTPTR)RESET_CMD);
	  //Xil_Out32(CNN_BASEADDR ,  (UINTPTR)IDLE_CMD);
	  Xil_Out32(CNN_BASEADDR ,  (UINTPTR)SEND_OUTPUT_FROM_CONV_0_CMD);
	 status = XAxiDma_SimpleTransfer(&myDma, (u32)RxBuffer, 32768*sizeof(u16),XAXIDMA_DEVICE_TO_DMA);
	 while ((XAxiDma_Busy(&myDma,XAXIDMA_DEVICE_TO_DMA)) && !end_command_done);
	 end_command_done=0;
	 xil_printf("Data back from CONV0\r\n");
	 //-----------------------------------------------------------------------------------------------------------------------
	 //printing back data
	 if(layer_to_print == 0)
	 {
		 for(int i = 0 ; i<50;i++)
			 printf("RxBuffer[%d] = %f\r\n",i,castBinToFloat(RxBuffer[i]));

	 }
//----------------------------------------------- 1. CONV 1 LAYER ------------------------------------------------------
	 Xil_Out32(CNN_BASEADDR ,  (UINTPTR)RESET_CMD);
	 //-----------------------------------------------------------------------------------------------------------------------
		 	load_float_in_tx_buffer(picture_conv1_input,TxBuffer,10368);
			Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_PICTURE_1_CMD);
			status = XAxiDma_SimpleTransfer(&myDma, (u32)TxBuffer, 10368*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
			if(status != XST_SUCCESS){
				xil_printf("Greska u slanju transakcije\r\n");
			}
			//while ((XAxiDma_Busy(&myDma,XAXIDMA_DMA_TO_DEVICE)) && !end_command_done);
			while(!end_command_done);
			end_command_done=0;
	 //-----------------------------------------------------------------------------------------------------------------------
	     load_float_in_tx_buffer(weights1_formated,TxBuffer,9216);
	     Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_WEIGHTS_1_CMD);
	     status = XAxiDma_SimpleTransfer(&myDma, (u32)TxBuffer, 4608*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
		 if(status != XST_SUCCESS){
			xil_printf("Greska u slanju transakcije\r\n");
		 }
	 	while(!end_command_done);
	 	end_command_done=0;
	 //-----------------------------------------------------------------------------------------------------------------------
	 	Xil_Out32(CNN_BASEADDR ,  (UINTPTR)DO_CONV_1_CMD);
		while(!end_command_done);
		end_command_done=0;
		xil_printf("CONV 1 DONE!\r\n");
	 //-----------------------------------------------------------------------------------------------------------------------
		Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_WEIGHTS_1_CMD);
		status = XAxiDma_SimpleTransfer(&myDma, (u32)&TxBuffer[4608], 4608*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
		if(status != XST_SUCCESS){
			xil_printf("Greska u slanju transakcije\r\n");
		}
		//while ((XAxiDma_Busy(&myDma,XAXIDMA_DMA_TO_DEVICE)) && !end_command_done);
		while(!end_command_done);
		end_command_done=0;
	 //-----------------------------------------------------------------------------------------------------------------------
		Xil_Out32(CNN_BASEADDR ,  (UINTPTR)DO_CONV_1_CMD);
		while(!end_command_done);
		end_command_done=0;
		xil_printf("CONV 1 DONE!\r\n");
	 //-----------------------------------------------------------------------------------------------------------------------
		Xil_Out32(CNN_BASEADDR ,  (UINTPTR)SEND_OUTPUT_FROM_CONV_1_CMD);
		status = XAxiDma_SimpleTransfer(&myDma, (u32)RxBuffer, 8192*sizeof(u16),XAXIDMA_DEVICE_TO_DMA);
		while(!end_command_done);
		end_command_done=0;
		xil_printf("Data back from CONV1\r\n");
		//-----------------------------------------------------------------------------------------------------------------------
		 //printing back data
		 if(layer_to_print == 1)
		 {
			 for(int i = 0 ; i<50;i++)
				 printf("RxBuffer[%d] = %f\r\n",i,castBinToFloat(RxBuffer[i]));

		 }
		//----------------------------------------------- 2. CONV 2 LAYER ------------------------------------------------------
			 Xil_Out32(CNN_BASEADDR ,  (UINTPTR)RESET_CMD);
		 //-----------------------------------------------------------------------------------------------------------------------
			load_float_in_tx_buffer(picture_conv2_input,TxBuffer,3200);
			Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_PICTURE_2_CMD);
			status = XAxiDma_SimpleTransfer(&myDma, (u32)TxBuffer, 3200*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
			if(status != XST_SUCCESS){
				xil_printf("Greska u slanju transakcije\r\n");
			}
			//while ((XAxiDma_Busy(&myDma,XAXIDMA_DMA_TO_DEVICE)) && !end_command_done);
			while(!end_command_done);
			end_command_done=0;
		//-----------------------------------------------------------------------------------------------------------------------
			 load_float_in_tx_buffer(weights2_formated,TxBuffer,18432);
			 Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_WEIGHTS_2_CMD);
			 status = XAxiDma_SimpleTransfer(&myDma, (u32)TxBuffer, 4608*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
			 if(status != XST_SUCCESS){
				xil_printf("Greska u slanju transakcije\r\n");
			 }
			 while(!end_command_done);
			 end_command_done=0;
		 //-----------------------------------------------------------------------------------------------------------------------
			Xil_Out32(CNN_BASEADDR ,  (UINTPTR)DO_CONV_2_CMD);
			while(!end_command_done);
			end_command_done=0;
			xil_printf("CONV 2 DONE!\r\n");
		//-----------------------------------------------------------------------------------------------------------------------
			 Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_WEIGHTS_2_CMD);
			 status = XAxiDma_SimpleTransfer(&myDma, (u32)&TxBuffer[4608], 4608*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
			 if(status != XST_SUCCESS){
				xil_printf("Greska u slanju transakcije\r\n");
			 }
			 while(!end_command_done);
			 end_command_done=0;
		 //-----------------------------------------------------------------------------------------------------------------------
			Xil_Out32(CNN_BASEADDR ,  (UINTPTR)DO_CONV_2_CMD);
			while(!end_command_done);
			end_command_done=0;
			xil_printf("CONV 2 DONE!\r\n");
			//-----------------------------------------------------------------------------------------------------------------------
			 Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_WEIGHTS_2_CMD);
			 status = XAxiDma_SimpleTransfer(&myDma, (u32)&TxBuffer[9216], 4608*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
			 if(status != XST_SUCCESS){
				xil_printf("Greska u slanju transakcije\r\n");
			 }
			 while(!end_command_done);
			 end_command_done=0;
		 //-----------------------------------------------------------------------------------------------------------------------
			Xil_Out32(CNN_BASEADDR ,  (UINTPTR)DO_CONV_2_CMD);
			while(!end_command_done);
			end_command_done=0;
			xil_printf("CONV 2 DONE!\r\n");
		//-----------------------------------------------------------------------------------------------------------------------
			 Xil_Out32(CNN_BASEADDR ,  (UINTPTR)LOAD_WEIGHTS_2_CMD);
			 status = XAxiDma_SimpleTransfer(&myDma, (u32)&TxBuffer[13824], 4608*sizeof(u16),XAXIDMA_DMA_TO_DEVICE);
			 if(status != XST_SUCCESS){
				xil_printf("Greska u slanju transakcije\r\n");
			 }
			 while(!end_command_done);
			 end_command_done=0;
		 //-----------------------------------------------------------------------------------------------------------------------
			Xil_Out32(CNN_BASEADDR ,  (UINTPTR)DO_CONV_2_CMD);
			while(!end_command_done);
			end_command_done=0;
			xil_printf("CONV 2 DONE!\r\n");
		 //-----------------------------------------------------------------------------------------------------------------------
			Xil_Out32(CNN_BASEADDR ,  (UINTPTR)SEND_OUTPUT_FROM_CONV_2_CMD);
			status = XAxiDma_SimpleTransfer(&myDma, (u32)RxBuffer, 4096*sizeof(u16),XAXIDMA_DEVICE_TO_DMA);
			while(!end_command_done);
			end_command_done=0;
			xil_printf("Data back from CONV2\r\n");
		//-----------------------------------------------------------------------------------------------------------------------
		 //printing back data
		 if(layer_to_print == 2)
		 {
			 for(int i = 0 ; i<50;i++)
				 printf("RxBuffer[%d] = %f\r\n",i,castBinToFloat(RxBuffer[i]));
		 }
	xil_printf("End of application\r\n");
    Disable_Interrupt_System();
    cleanup_platform();

  return 0;
}

static void End_Command_Interrupt_Handler(void *Callback)
{
	end_command_done = 1;
}

u32 Setup_Interrupt(u32 DeviceId, Xil_InterruptHandler Handler, u32 interrupt_ID)
{
  //XScuGic_Config *IntcConfig;
  //XScuGic INTCInst;
  int status;
  // Extracts informations about processor core based on its ID, and they are used to setup interrupts
  IntcConfig = XScuGic_LookupConfig(DeviceId);

  // Initializes processor registers using information extracted in the previous step
  status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
  if(status != XST_SUCCESS) return XST_FAILURE;
  status = XScuGic_SelfTest(&INTCInst);
  if (status != XST_SUCCESS) return XST_FAILURE;

  // Connect Timer Handler And Enable Interrupt
  // The processor can have multiple interrupt sources, and we must setup trigger and   priority
  // for the our interrupt. For this we are using interrupt ID.
   XScuGic_SetPriorityTriggerType(&INTCInst, interrupt_ID, 0xA8, 3);

  // Connects out interrupt with the appropriate ISR (Handler)
  status = XScuGic_Connect(&INTCInst, interrupt_ID, Handler, (void *)&INTCInst);
  if(status != XST_SUCCESS) return XST_FAILURE;

  // Enable interrupt for out device
  XScuGic_Enable(&INTCInst, interrupt_ID);

  //Two lines bellow enable exeptions
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			       (Xil_ExceptionHandler)XScuGic_InterruptHandler,&INTCInst);
  Xil_ExceptionEnable();

  return XST_SUCCESS;
}

void DMA_init_interrupts()
{
  u32 MM2S_DMACR_reg;
  u32 S2MM_DMACR_reg;

  Xil_Out32(DMA_BASEADDR + MM2S_DMACR_OFFSET,  DMACR_RESET); // writing to MM2S_DMACR register
  Xil_Out32(DMA_BASEADDR + S2MM_DMACR_OFFSET,  DMACR_RESET); // writing to S2MM_DMACR register

  /* THIS HERE IS NEEDED TO CONFIGURE DMA*/
  //enable interrupts
  MM2S_DMACR_reg = Xil_In32(DMA_BASEADDR + MM2S_DMACR_OFFSET); // Reading from MM2S_DMACR register inside DMA
  Xil_Out32((DMA_BASEADDR + MM2S_DMACR_OFFSET),  (MM2S_DMACR_reg | DMACR_IOC_IRQ_EN | DMACR_ERR_IRQ_EN)); // writing to MM2S_DMACR register
  S2MM_DMACR_reg = Xil_In32(DMA_BASEADDR + S2MM_DMACR_OFFSET); // Reading from S2MM_DMACR register inside DMA
  Xil_Out32((DMA_BASEADDR + S2MM_DMACR_OFFSET),  (S2MM_DMACR_reg | DMACR_IOC_IRQ_EN | DMACR_ERR_IRQ_EN)); // writing to S2MM_DMACR register
}
static void Disable_Interrupt_System()
{
  XScuGic_Disconnect(&INTCInst, INTERUPT_COMMAND_DONE_ID);
}

void load_float_in_tx_buffer(float* float_array,u16* u16_array,int num_of_parametars)
{

  for(int i=0; i<num_of_parametars; i++)
  {
	  u16_array[i]= castFloatToBin(float_array[i]);
  }

}

u16 castFloatToBin(float val)
{
	int sign;
	int integerPart;
	int decimalPart;
	u16 binary;

	sign = ((val>=0) ? 0 : 1);

	  if (sign == 0)
	  {
		integerPart = (int) val;
		decimalPart = (int) ((val - integerPart)*4096);
		binary = (sign << 15) | ((integerPart & 0x7) << 12) | (decimalPart & 0xFFF);
	  }
	  else
	  {
		integerPart = (int) ((-1)*val);
		decimalPart = (int)(((-1)*val - integerPart)*4096);
		binary = (sign << 15) | (((~integerPart) & 0x7) << 12) | ((~decimalPart) & 0xFFF);
		binary = binary + 0b000000000001;
	  }
	  return binary;

}
float castBinToFloat(uint16_t binaryValue) {
    uint16_t binaryValue_uint = binaryValue;
    int sign = (binaryValue_uint >> 15) & 0x1;

    if (sign == 1) {
        binaryValue_uint = (~binaryValue_uint) + 1; // prebacujemo u pozitivno, posle cemo float pomnozitit sa -1
    }

    int integerPart = (binaryValue_uint >> 12) & 0x7;
    int decimalPart = binaryValue_uint & 0xFFF;

    float floatValue = (float)integerPart + ((float)decimalPart / 4096.0f);
    if (sign == 1)
        floatValue = floatValue * (-1);

    return floatValue;
}

