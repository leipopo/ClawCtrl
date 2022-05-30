
#include "main.h"
/*-------------------------------------
* ���������û�ָ��toצ�ӿ�����Ϣ
* ����  �������û�ָ�����צ�ӿ�����Ϣ
* ����  ��usercommand��clawinfo
* ���  ����
���ߣ�LPGUAIA
���ڣ�2022.5
-----------------------------------------*/ 
clawinfo cinfo;




void command2clawinfo(usercommand *uc,clawinfo *ci,MotorInfo m[4])
{
	//usercommandctoclawinfo/״̬ת��1
	switch(uc->modeset)
	{
		case 0x00:
		{
			if(ci->clawmode==0x05||ci->clawmode==0x02)
			{
				ci->clawmode=uc->modeset;
				uc->modeset=0x05;
				for(int8_t i=0;i<4;i++)
				{			
					ci->motorstate[i]=0x11;
				}	
			}
		}break;
		case 0x01:
		{
			if(ci->clawmode==0x05)
			{
				ci->clawmode=uc->modeset;
				uc->modeset=0x05;
				for(int8_t i=0;i<4;i++)
				{			
					ci->motorstate[i]=0x11;
				}	
			}
		}break;
		case 0x02:
		{
			if(ci->clawmode!=0x04)
			{
				ci->clawmode=uc->modeset;
				uc->modeset=0x02;
				for(int8_t i=0;i<4;i++)
				{			
					ci->motorstate[i]=0x11;
				}	
			}
		}break;
		case 0x03:
		{
				ci->clawmode=uc->modeset;
				uc->modeset=0x03;
				for(int8_t i=0;i<4;i++)
				{			
					ci->motorstate[i]=0x00;
				}	
		}break;
		case 0x04:
		{
			if(ci->clawmode!=0x04)
			{
				for(int8_t i=0;i<4;i++)
				{			
					ci->motorstate[i]=0x01;
					m[i].tempdata.angle=0.f;
					minfo[0].parameter.angle_limit[1]=maxfa;
					minfo[1].parameter.angle_limit[1]=maxfa;
					minfo[2].parameter.angle_limit[1]=maxpa;
					minfo[3].parameter.angle_limit[0]=minpa;
				}	
			}
			ci->clawmode=uc->modeset;
			uc->modeset=0x04;
			
		}break;
		case 0x05:
		{
				
		}break;
		
	}
	
	
	//clawinfotomotorinfo/״̬ת��2
	switch(ci->clawmode)
	{
		case 0x00:
		{
			

			for(int8_t i=0;i<4;i++)
			{
				ci->tarmotorinfo.angle[i]=ci->tarmotorinfo.angle[i]+uc->motoranglechange[i];
				uc->motoranglechange[i]=0;
			}
			ci->clawmode=0x06;
			
		}break;
		
		case 0x01:
		{			
			
			for(int8_t i=0;i<4;i++)
			{				
				ci->tarmotorinfo.angle[i]=uc->motorangle[i];		
				uc->motorangle[i]=0;
			}
			ci->clawmode=0x06;
		}break;
		
		
		case 0x02:
		{
			for(int8_t i=0;i<4;i++)
			{
				ci->tarmotorinfo.torque[i]=ctcoe*currentlimit(0.1f*uc->motortorque[i]*tccoe,safecurrentcoe*m[i].parameter.current_limit[1]);
				ci->tarmotorinfo.angle[i]=ci->curmotorinfo.angle[i];
				
			}
			ci->clawmode=0x02;
		}break;
		
		case 0x03:
		{
			for(int8_t i=0;i<4;i++)
			{
				ci->tarmotorinfo.angle[i]=ci->curmotorinfo.angle[i];
				uc->motortorque[i]=0;
			}
			ci->clawmode=0x03;
		}break;
		
		case 0x04:
		{
			uc->modeset=0x05;
			if((ci->motorstate[0]>>4)==0)
			{
				ci->tarmotorinfo.angle[0]=ci->curmotorinfo.angle[0]+m[0].parameter.zerodir*zas*360.f/60.f/(1000.f/tp);
			}
			else 
			{
				ci->tarmotorinfo.angle[0]=0;
			}
			
			if((ci->motorstate[0]>>4)!=0)
			{
				if((ci->motorstate[1]>>4)==0)
				{
					ci->tarmotorinfo.angle[1]=ci->curmotorinfo.angle[1]+m[1].parameter.zerodir*zas*360.f/60.f/(1000.f/tp);
				}
				else 
				{
					ci->tarmotorinfo.angle[1]=0;
				}
			}
			
			if((ci->motorstate[0]>>4)!=0&&(ci->motorstate[1]>>4)!=0)
			{
				for(int8_t i=2;i<4;i++)
				{
					if((ci->motorstate[i]>>4)==0)
					{
						ci->tarmotorinfo.angle[i]=ci->curmotorinfo.angle[i]+m[i].parameter.zerodir*zas*360.f/60.f/(1000.f/tp);
					}
					else 
					{
						ci->tarmotorinfo.angle[i]=0;
					}
				}
			}
			if((ci->motorstate[2]>>4)!=0&&(ci->motorstate[3]>>4)!=0)
			{
				ci->clawmode=0x05;
			}
		}break;
		
		case 0x05:
		{
			
		}break;
		
		case 0x06:
		{
				
		}break;
	}
	
	
	for(int8_t i=0;i<4;i++)
	{
		ci->tarmotorinfo.speed[i]=uc->motorspeed[i];
	}
	
}


/*-------------------------------------
* �������������ǰ��Ϣtoצ�ӿ�����Ϣ
* ����  ����������ǰ��Ϣ������צ�ӿ�����Ϣ
* ����  ��usercommand��clawinfo
* ���  ����
���ߣ�LPGUAIA
���ڣ�2022.5
-----------------------------------------*/ 
void motor2clawinfo(clawinfo *ci,MotorInfo m[4])
{
	for(int8_t i=0;i<4;i++)
	{
		
		//����
		ci->curmotorinfo.angle[i]=m[i].curmotorinfo.angle;
		ci->curmotorinfo.speed[i]=m[i].curmotorinfo.speed;
		ci->curmotorinfo.torque[i]=m[i].curmotorinfo.current[4]*ctcoe;	
		
		
		//�����λ���
		if(ABS(ci->tarmotorinfo.angle[i]-ci->curmotorinfo.angle[i])<=0.5f&&\
			ci->motorstate[i]!=0x00&&ci->motorstate[i]!=0x01&&ci->clawmode!=0x02)
		{
			ci->motorstate[i]=0x10;
			m[i].tempdata.angle=0.f;
		}
		
		
		
		//������
		if(ci->motorstate[i]==0x01)
		{
			if(m[i].tempdata.angle!=0.f)
			{
				ci->motorstate[i]=0x10;
				m[i].curmotorinfo.angle=0.f;
				m[i].tarmotorinfo.angle=0.f;
				ci->curmotorinfo.angle[i]=0.f;
				ci->tarmotorinfo.angle[i]=0.f;
				m[i].tempdata.angle=0.f;
				PID_clear(&mapid[i]);
				PID_clear(&mspid[i]);
				
			}
		}
		
		//צ�Ӿ�λ���
		if((ci->motorstate[0]==0x10&&ci->motorstate[1]==0x10&&ci->motorstate[2]==0x10&&ci->motorstate[3]==0x10)&&ci->clawmode!=0x02)
		{
			ci->clawmode=0x05;
			minfo[0].parameter.angle_limit[1]=0;
			minfo[1].parameter.angle_limit[1]=0;
			minfo[2].parameter.angle_limit[1]=0;
			minfo[3].parameter.angle_limit[0]=0;
		}

	}
	
}

/*-------------------------------------
* �������������ǰ��Ϣtoצ�ӿ�����Ϣ
* ����  ����������ǰ��Ϣ������צ�ӿ�����Ϣ
* ����  ��usercommand��clawinfo
* ���  ����
���ߣ�LPGUAIA
���ڣ�2022.5
-----------------------------------------*/ 
void claw2motorinfo(clawinfo *ci,MotorInfo m[4])
{
	for(int8_t i=0;i<4;i++)
	{
		m[i].tarmotorinfo.angle=anglelimit(ci->tarmotorinfo.angle[i],m[i].parameter.angle_limit[1],m[i].parameter.angle_limit[0]);
		m[i].tarmotorinfo.speed=speedlimit(ci->tarmotorinfo.speed[i],m[i].parameter.speed_limit);
	}
	
}


/*-------------------------------------
* ������������λ������Ϣ
* ����  �����צ����Ϣ������
* ����  ����
* ���  ����
���ߣ�LPGUAIA
���ڣ�2022.5
-----------------------------------------*/ 
void sendmestopc(void)
{
	uint8_t mestopc[12];

	HAL_UART_DMAResume(&huart3);
	
	//������״̬
	mestopc[0]=0xf0;
	for(int8_t i=2;i<10;i=i+2)
	{
		mestopc[i]=cinfo.motorstate[i/2-1];
	}
	for(int8_t i=1;i<9;i=i+2)
	{
		mestopc[i]=0x00;
	}
	mestopc[9]=0x00;
	mestopc[10]=cinfo.clawmode;
	mestopc[11]=0x0a;
	HAL_UART_Transmit_DMA(&huart3,mestopc,sizeof(mestopc));
	
	
	//�����ǰ���ת��
	mestopc[0]=0xf1;
	//�߰�λ�Ƚ� ֵΪ���ת�ص�һ�ٱ�
	int16_t sumtorque=0;
	for(int8_t i=2;i<10;i=i+2)
	{
		mestopc[i]=(int16_t)(cinfo.curmotorinfo.torque[i/2-1]*100.f);
	}
	for(int8_t i=1;i<9;i=i+2)
	{
		mestopc[i]=((int16_t)(cinfo.curmotorinfo.torque[(i-1)/2]*100.f))>>8;
		sumtorque+=(int16_t)(cinfo.curmotorinfo.torque[(i-1)/2]*100.f);
	}
	mestopc[9]=sumtorque>>8;
	mestopc[10]=sumtorque;
	mestopc[11]=0x0a;
	HAL_UART_Transmit_DMA(&huart3,mestopc,sizeof(mestopc));
	
	//�����ǰ����Ƕ�
	mestopc[0]=0xf2;
	//�߰�λ�Ƚ� ֵΪ����Ƕ�ֵ
	int16_t sumangle=0;
	for(int8_t i=2;i<10;i=i+2)
	{
		mestopc[i]=(int16_t)(cinfo.curmotorinfo.angle[i/2-1]);
	}
	for(int8_t i=1;i<9;i=i+2)
	{
		mestopc[i]=((int16_t)(cinfo.curmotorinfo.angle[(i-1)/2]))>>8;
		sumangle+=(int16_t)(cinfo.curmotorinfo.angle[(i-1)/2]);
	}
	mestopc[9]=sumangle>>8;
	mestopc[10]=sumangle;
	mestopc[11]=0x0a;
	HAL_UART_Transmit_DMA(&huart3,mestopc,sizeof(mestopc));
	
	//�����ǰ����ٶ�
	mestopc[0]=0xf3;
	//�߰�λ�Ƚ� ֵΪ����ٶ�ֵ
	int16_t sumspeed=0;
	for(int8_t i=2;i<10;i=i+2)
	{
		mestopc[i]=(int16_t)(cinfo.curmotorinfo.speed[i/2-1]);
	}
	for(int8_t i=1;i<9;i=i+2)
	{
		mestopc[i]=((int16_t)(cinfo.curmotorinfo.speed[(i-1)/2]))>>8;
		sumspeed+=(int16_t)(cinfo.curmotorinfo.speed[(i-1)/2]);
	}
	mestopc[9]=sumspeed>>8;
	mestopc[10]=sumspeed;
	mestopc[11]=0x0a;
	HAL_UART_Transmit_DMA(&huart3,mestopc,sizeof(mestopc));
	
	HAL_UART_DMAPause(&huart3);
}


void infoproc()
{
	
	cinfo.clawmode=0x03;
	for(int8_t i=0;i<4;i++)
	{
		
		ucomd.motorspeed[i]=zeromotoranglespeed;
	}
	for(;;)
	{
		motor2clawinfo(&cinfo,minfo);
		command2clawinfo(&ucomd,&cinfo,minfo);
		claw2motorinfo(&cinfo,minfo);
		sendmestopc();
		osDelayUntil(tp);
	}
}
