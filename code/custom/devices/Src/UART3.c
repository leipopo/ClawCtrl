#include "UART3.h"

usercommand ucomd;

/*-------------------------------------
* ������������DMA��ʼ��
* ����  ����ʼ��CV�Ĵ��ڼ�DMA����
* ����  �����ڣ�����dma������0������1�����ݳ���
* ���  ����
���ߣ�LPGUAIAħ�ĵ�DJI��
���ڣ�2021.7.7
-----------------------------------------*/ 

uint8_t rx_buf[2][RX_BUF_NUM];
uint8_t Ut_ready=0;
void Ut_init(void)
{
 	DMA_RX_INIT(&huart3,&hdma_usart3_rx,rx_buf[0],rx_buf[1],RX_BUF_NUM);
}

/*-------------------------------------
* ������������������
* ����  �����봮�����յ�������
* ����  ��������buffer�����뵽�Ľṹ��
* ���  ����
���ߣ�LPGUAIAħ�ĵ�DJI��
���ڣ�2021.7.7
-----------------------------------------*/ 

void RX_Decoder(uint8_t *buf, usercommand *uc)
{
	
	
	if (buf == NULL)
  {
    return;
  }
	if (buf[0]!=0x00&&buf[0]!=0x01&&buf[0]!=0x02&&buf[0]!=0x03&&buf[0]!=0x04&&buf[0]!=0x05&&buf[0]!=0xFF) //����ͷ���ԣ������¿�ʼѰ��0x00����ͷ
	{
		return;
	}
	else
	{
		switch(buf[0])
		{
			case 0x00:
			{
				if(buf[11]==0x0A)
				{
					for (int i=0;i<4;i++)
						uc->motoranglechange[i]=(int16_t)( (buf[2*i+1] << 8)|buf[2*i+2]);
					uc->modeset=0x00;
				}
			}break;
			case 0x01:
			{
				if(buf[11]==0x0A)
				{
					for (int i=0;i<4;i++)
						uc->motorangle[i]=(int16_t)( (buf[2*i+1] << 8)|buf[2*i+2]);
					uc->modeset=0x01;
				}
			}break;
			case 0x02:
			{
				if(buf[11]==0x0A)
				{
					for (int i=0;i<4;i++)
						uc->motorspeed[i]=(int16_t)( (buf[2*i+1] << 8)|buf[2*i+2]);
				}
			}break;
			case 0x03:
			{
				if(buf[11]==0x0A)
				{
					for(int i=0;i<4;i++)
						uc->motortorque[i]=(int16_t)( (buf[2*i+1] << 8)|buf[2*i+2]);
					uc->modeset=0x02;
				}
			}break;

			case 0x04:
			{
				uc->modeset=0x03;
			}break;
			case 0x05:
			{
				uc->modeset=0x04;
			}break;
			
			case 0xFF:
			{
				if(buf[11]==0x0A)
				{
					if(buf[2]==0x00&&(buf[10]&0x01)==0x00)
					{
						HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_RESET);
					}
					if(buf[4]==0x00&&(buf[10]&0x10)==0x00)
					{
						HAL_GPIO_WritePin(GPIOH,GPIO_PIN_3,GPIO_PIN_RESET);
					}
					if(buf[6]==0x00&&(buf[9]&0x01)==0x00)
					{
						HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_RESET);
					}
					if(buf[8]==0x00&&(buf[9]&0x10)==0x00)
					{
						HAL_GPIO_WritePin(GPIOH,GPIO_PIN_5,GPIO_PIN_RESET);
					}
					
					if(buf[2]==0x01&&(buf[10]&0x01)==0x01)
					{
						HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_SET);
					}
					if(buf[4]==0x01&&(buf[10]&0x10)==0x10)
					{
						HAL_GPIO_WritePin(GPIOH,GPIO_PIN_3,GPIO_PIN_SET);
					}
					if(buf[6]==0x01&&(buf[9]&0x01)==0x01)
					{
						HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_SET);
					}
					if(buf[8]==0x01&&(buf[9]&0x10)==0x10)
					{
						HAL_GPIO_WritePin(GPIOH,GPIO_PIN_5,GPIO_PIN_SET);
					}
				}
			}break;
		}
	}
}



/*-------------------------------------
* ���������滻it.c�ļ���Ĵ��������ж����handler
* ����  ���жϴ��������մ���
* ����  ����
* ���  ����
���ߣ�LPGUAIAħ�ĵ�DJI��
���ڣ�2021.7.7

���ߣ�LPGUAIA����DJI��
���ڣ�2021.12.22
-----------------------------------------*/ 
void USART3_inIRQHandler(void)
{
	Ut_ready=0x01;
		if(huart3.Instance->SR & UART_FLAG_RXNE)
    {
      __HAL_UART_CLEAR_IDLEFLAG(&huart3);
    }
		else if(huart3.Instance->SR & UART_FLAG_IDLE) 
    {
      static uint16_t this_time_rx_len = 0;
			__HAL_UART_CLEAR_PEFLAG(&huart3);
		  
			if((hdma_usart3_rx.Instance->CR&DMA_SxCR_CT)==RESET)
			{
				__HAL_DMA_DISABLE(&hdma_usart3_rx);
				this_time_rx_len = RX_BUF_NUM - hdma_usart3_rx.Instance->NDTR;
				hdma_usart3_rx.Instance->NDTR = RX_BUF_NUM;
				hdma_usart3_rx.Instance->CR|=DMA_SxCR_CT;
				__HAL_DMA_ENABLE(&hdma_usart3_rx);
				if(this_time_rx_len == FRAME_LENGTH)
        {
					RX_Decoder(rx_buf[0],&ucomd);
        }
			}
			else
			{
				
				__HAL_DMA_DISABLE(&hdma_usart3_rx);
				this_time_rx_len = RX_BUF_NUM - hdma_usart3_rx.Instance->NDTR;
				hdma_usart3_rx.Instance->NDTR = RX_BUF_NUM;
				DMA1_Stream1->CR &= ~(DMA_SxCR_CT);
				__HAL_DMA_ENABLE(&hdma_usart3_rx);
				if(this_time_rx_len == FRAME_LENGTH)
				{
					RX_Decoder(rx_buf[1],&ucomd);
        }
			}
		}
}
