/*
**********************************************************************************************************
    *@  file:007_elmo.c
    *@  author: 007
    *@  date: 3/20/2015
    *@  version: v2.0
    *@  brief: ���ӵ������õĿ��������Ϳ���ɲ������
    *@         ����������Ԥ��������
    *@         ������note1�еĺ���ע��
    *@         ������note2�е�ע������
    *...............................................................................
    *@ Notes1: (1)��ʼ��������       
    *@            name      : Elmo_Init(CAN_TypeDef* CANx, uint8_t PPr, uint8_t SPr)
    *@            function  : ���ó�ʼ����������ʹ��CANͨ��ģ�飬���ΪCAN1��CAN2������ѡ��
    *@			                  CAN�ڣ�ȷ������ͨ�Ų���ȷ��ͨ���ٶ�Ϊ1Mbps��ͬʱѡ��TIM7���ж�
    *@	     		              ���ȼ���Ϊ�ײ㱨�ķ����ṩʱ��	
    *@            input     : CANx      CAN1 or CAN2
    *@						            PPr       TIM7����ռ���ȼ�
    *@                        SPr       TIM7�Ĵ����ȼ�
    *@            output    : 0	          ��ʼ���ɹ�
    *@                        0x80000000  ����û������CAN����
    *@ 							          ����         ��Ӧ��ELMO��IDλû����1
    *@	
    *@         (2)����ģʽ������        		
    *@            name      : Elmo_PTM(u8 elmoID, float torque);
    *@            function  : �������ؿ��ƺ�����ά�ֵ�����غ㶨,֧��ģʽ�л�  
    *@            input     : elmoID    ȡelmo�ڵ�ID     
    *@			    			        torque     Ŀ��ת��(A)
    *@            output    : 0         �������óɹ�
    *@                        1         ��������ʧ��
    *@ 	
    *@         (3)�ٶ�ģʽ������  
    *@            name      : Elmo_PVM(u8 elmoID, s32 speed);      
    *@            function  : �����ٶȿ��ƺ���,֧��ģʽ�л� ��֧���������ã�elmo 
    *@                        ���������ٶȻ���ٶ�ʹ�� ��ת�ٴﵽ�趨ֵ
    *@            input     : elmoID    ȡelmo�ڵ�ID      
    *@                        speed     Ŀ���ٶ�(cnt/s)
    *@            output    : 0         �������óɹ�
    *@                        1         ��������ʧ��
    *@
    *@         (4)λ��ģʽ������   
    *@            name      : Elmo_PPM(u8 elmoID, u32 speed, s32 position, u8 PPMmode);
    *@            function  : �����ٶȿ��ƺ�����֧��ģʽ�л���֧���������ã�����POS_ABSģʽ
    *@ 						            ����ָ���յ����λ��������ϵ��ĵ����λ�ã�Ҳ���ǵ�� �У�Ŀ    
    *@ 						            ��λ��ʼ�ľ���λ�á�����POS_RELģʽ�У����յ����λ��������elmo
    *@ 				                ����λ�üĴ����еľ���λ�ü���Ŀ��λ�ã�����λ�üĴ�����CAN_init 
    *@                        ��Ĭ��Ϊ0
    *@						  ����1��
    *@						  Elmo_PPM(1, 50000, 500000, POS_ABS);
    *@						  while(position != 500000);
    *@					   	  Elmo_PPM(1, 50000, 250000, POS_REL);		
    *@                             ���λ����750000��		
    *@						            ����2��
    *@						            Elmo_PPM(1, 50000, 500000, POS_ABS);
    *@						            while(position != 500000);
    *@						            Elmo_PPM(1, 50000, 250000, POS_ABS);			
    *@						            ���λ����250000��		
    *@						            ����3��
    *@						            Elmo_PPM(1, 50000, 500000, POS_ABS);
    *@						            while(position <= 250000);
    *@						            Elmo_PPM(1, 50000, 250000, POS_REL);			
    *@					              ���λ����750000��		
    *@            input     : elmoID      ȡelmo�ڵ�ID,�����ڵ��̿����е���1--4        
    *@ 						            speed       �ٶ�(cnt/s)
    *@ 					              position    Ŀ��λ��(cnt)
    *@ 						            PPMmode     ����ģʽ
    *@ 						            POS_ABS    // PPM���з�ʽ:����λ��
    *@ 		   			            POS_REL    // PPM���з�ʽ:���λ��
    *@  			    output    : 0         �������óɹ�
    *@                        1         ��������ʧ��
    *@
    *@         (5)����ͷź�����   
    *@            name      : Elmo_Close(u8 elmoID);
    *@            function  : ��������رգ���������Լ�����ʻ,֧���������ã�֧�����ϵ��ã���
    *@						            ���ڸ���elmo����ǰ����ã�������Ӱ�����������Ĺ���ʵ�ֺ�elmo��
    *@						            ����														
    *@            input     : elmoID      ȡelmo�ڵ�ID     
    *@  			    output    : 0         �������óɹ�
    *@                        1         ��������ʧ��
    *@         
    *@         (6)�������������
    *@            name      : Elmo_Stop(u8 elmoID);
    *@            function  : ���������ά�ֵ����ǰλ�ò��䣬֧���������ã�֧�����ϵ��ã���
    *@ 						            ���ڸ���elmo����ǰ����ã�������Ӱ�����������Ĺ���ʵ�ֺ�elmo��
    *@ 						            ����	
    *@            input     : elmoID      ȡelmo�ڵ�ID     
    *@  			    output    : 0         �������óɹ�
    *@                        1         ��������ʧ��
    *@	
    *@         (7)���ٶ����ú�����	
    *@  			    name      : Elmo_SetAcc(u8 elmoID, u32 acc, u32 dec)
    *@            function  : �����ٶ�ģʽ��λ��ģʽ�ĵ���Ӽ��٣����û���ں����е��ã���ʼ��
    *@	                      ����ٶ�Ĭ��Ϊ1000000000�����ٶ�Ĭ��Ϊ1000000000
    *@			      input     : elmoID      ȡelmo�Ľڵ�ID
    *@                        acc         ���ٶ�,���ٶ�����ܳ���1000000000,ͬʱӦ���ǵ������
    *@                        dec         ���ٶ�,���ٶ�����ܳ���1000000000,ͬʱӦ���ǵ������
    *@  			    output    : 0         �������óɹ�
    *@         (8)λ�ö�ȡ������	
    *@  			    name      : Elmo_Read_POS(u8 elmoID)
    *@            function  : ��ȡ��������������ݣ��������ж��ж�ȡ���洢��ȫ�ֱ���Encoder_Data��
    *@			      input     : elmoID      ȡelmo�Ľڵ�ID
    *@  			    output    : NONE
    *@         (9)λ�����ú�����	
    *@  			    name      : Elmo_Set_POS(u8 elmoID,s32 POS)
    *@            function  : ���õ��������������
    *@			      input     : elmoID      ȡelmo�Ľڵ�ID
    *@                        POS         Ҫ��elmo���õĵ�ַ
    *@  			    output    : NONE
    *@         (10)������ȡ������	
    *@  			     name      : Elmo_Read_ACT_CUR(u8 elmoID)
    *@             function  : ��ȡ����й����������ݣ��������ж��ж�ȡ���洢��ȫ�ֱ���IQ��
    *@			       input     : elmoID      ȡelmo�Ľڵ�ID
    *@  			     output    : NONE
    *...............................................................................
    *@ Notes2: (1)��CAN������ 
    *@			     CAN1 RX -> PD0     CAN1 TX -> PD1
    *@               CAN2 RX -> PB5     CAN2 TX -> PB6
    *@         (2)���޸ĺ궨��CAN_BUF_NUM��ֵ�޸Ļ�����Ŀ
    *@         (3)���޸�TIM7_Configuration()��TIM7�����ж����ȼ�
    *@         (4)��ע��CANʱ��Ϊ42MHz,TIM7ʱ��Ϊ42MHz
    *@ 	       (5)��ע��д���ȼ����麯��
    *@         (6)��ע������ж��еĵ��ID
    *@         (7)����Elmo_Read_POS��Elmo_Read_ACT_CUR��������֮ǰҪ��ʱ1ms�����Է�ֹ��ϵ�ǰ������˶�״̬
    *@         (8)����Four_Elmo_PVM��������֮ǰҪ����Elmo_Pre_PVM��Ԥ����
    *...............................................................................
    *@ Notes3: (1)��û��д�ڵ�ID����ID�ķ��亯��
    *@	       (2)��û�мӷ������Ļ��� 
    *@		   (3)��û�м�ƽ���������ú���	    
    *@		   (4)��Elmo_Stop()��Elmo_PTM()����Ӧʱ��̫������Ҫ�Ż�			
    *@         (5)��û�мӽڵ㱣��������������������				 
    *@    	   (6)��Elmo_Delay100us()���ԸĽ������Ч��    
    *@         (7)��GroupID��ʱ��������	
**********************************************************************************************************
*/
#include "007_elmo.h"
#include "Rsing_Can.h"
#include "FreeRTOS.h"
#include "task.h"

static Elmo elmo[ELMO_NUM + 1];
static Elmo chassisGroup, slidewayGroup;
const T_CanFrame MainHeartFrame = {(uint32_t)(0x700+9) << 21,0,{0,0,0,0,0,0,0,0}};  //���ط���������Elmo���

u32 ElmoHeartbeat = 0;   //������¼ELMO����
#define DEF_ElmoHeartbeat_VALUE     (0xfF)  //ֻ����elmo�յ���ֵ��ʾ�ĸ�elmo������Ż��ͳ������ź���

extern xSemaphoreHandle ElmoSem;
extern xSemaphoreHandle PosRacketMainSem;      //�յ�Elmo��������ֵ
extern xSemaphoreHandle PosRacketServeSem;    //�յ�Elmo��������ֵ

#define Elmo_Delay100us(time_100us)     vTaskDelay(time_100us)

/**
  ******************************************************************************
  * @name     : Elmo_Init
  * @function : 
  * @input    : 
  * @output   : 
  ******************************************************************************
  */
u32 Elmo_Init(CAN_TypeDef* CANx, uint8_t PPr, uint8_t SPr)
{
    u8 i = 0;
    int temp = 0;
    /* Ӳ����ȫ�ֱ�����ʼ�� */ 
    CAN_Config(CANx);

    /*���������ʼ��*/
    Variate_init();
    
    /* �رյ�� */
    RSDO(&elmo[0], 0x6040, 0x00, 0x00);
    Elmo_Delay100us(200);
    
    /* ��elmo����ڵ�ID */
    for(i=0; i <= ELMO_NUM; i++)
    {
        elmo[i].NodeID = i;			
    }

    /* ��elmo������ID */
    chassisGroup.NodeID = 64;
    slidewayGroup.NodeID = 65;

    /* ��Elmo5~8 & 1~2����GroupID */
    for(i = 5; i <= 8; i++)
    {
        RSDO(&elmo[i], 0x2040, 0x00, chassisGroup.NodeID);
        Elmo_Delay100us(10);
    }
    for(i = 1; i <= 2; i++)
    {
        RSDO(&elmo[i], 0x2040, 0x00, slidewayGroup.NodeID);
        Elmo_Delay100us(10);
    }

    /* ��ȫ��ڵ����ͨ�Ÿ�λ */
    Elmo_Delay100us(500);
    NMTCmd(&elmo[0], NMT_RESET_COMMUNICATION);
    Elmo_Delay100us(500);

    /* Elmo�������Start------------------------------------------------------*/
    RSDO(&elmo[0], 0x1017, 0, (100&0xffff));//��������Ϊ100ms
    Elmo_Delay100us(100);
    
    RSDO(&elmo[0], 0x6007, 0, 1);//������ϣ�MO=0
    Elmo_Delay100us(100);

    RSDO(&elmo[0], 0x1029, 1, 2);//ͨ�Ŵ�����Ϊ��ֹͣ
    Elmo_Delay100us(100);
    
    RSDO(&elmo[0], 0x1016, 1, (0x09<<16) | (150&0xffff));//����������ʱ���� ID��0x09 T��150ms
    Elmo_Delay100us(2000);

    /* Elmo�������End--------------------------------------------------------*/
    
    //Elmo_software_delay_ms(100);
    //	/* �ȴ�Elmo�������,�����յ�Boot up���� */
    //	Elmo_Delay100us(50);

    /* CANOpenͨ�Ų�����ʼ�� */
    /* RPDO1->0x6040,ָ����,2�ֽ�,�첽���� */
    // ������Ĭ��ӳ��,����Ҫ�޸� //

    /* RPDO2->0x2012,�����Ʊ�������,4�ֽ�,�첽���� */
    // ������Ĭ��ӳ��,����Ҫ�޸� //

    /* ����TPDO,Debugʱ����,������ʱ��ùر� */
    RSDO(&elmo[0], 0x1A00, 0x00, 0);
    Elmo_Delay100us(150);
    RSDO(&elmo[0], 0x1A01, 0x00, 0);
    Elmo_Delay100us(150);
    
    /* ����NMT����״̬ */
    Elmo_Delay100us(40);
    NMTCmd(&elmo[0], NMT_ENTER_OPERATIONAL);
    Elmo_Delay100us(80);

    /*�˺��������ݻ��Ϊ0x05������״̬��Ϊ����̬*/

    /* �ر����� */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(80);

    /* ��ʼ�����ٶ� */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"PM", 0, TYPE_INTEGER, 0x01);
    Elmo_Delay100us(40);	
    RPDO2_Cmd_data(&elmo[0], (u8 *)"AC", 0, TYPE_INTEGER, 1000000);
    Elmo_Delay100us(40);
    RPDO2_Cmd_data(&elmo[0], (u8 *)"DC", 0, TYPE_INTEGER, 1000000);
    Elmo_Delay100us(40);

    /* Enter in SCM */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
    Elmo_Delay100us(40);

    for(i=0;i<=ELMO_NUM;i++)
    {
        elmo[i].CurOPMode = UM_SCM;			
    }

    /* ʹ������ */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
    Elmo_Delay100us(250);
    Elmo_Delay100us(250);
    Elmo_Delay100us(50);
    return temp;
}

void Elmo_Reinit(void)
{
    uint32_t i = 0;
    ElmoHeartbeat = 0;
    
    RSDO(&elmo[0], 0x6040, 0x00, 0x80);  //��������־λ
    Elmo_Delay100us(40);
    
    /* �رյ�� */
    RSDO(&elmo[0], 0x6040, 0x00, 0x00);
    Elmo_Delay100us(200);
    
    /* ��Elmo5~8 & 1~2����GroupID */
    for(i = 5; i <= 8; i++)
    {
        RSDO(&elmo[i], 0x2040, 0x00, chassisGroup.NodeID);
        Elmo_Delay100us(10);
    }
    for(i = 1; i <= 2; i++)
    {
        RSDO(&elmo[i], 0x2040, 0x00, slidewayGroup.NodeID);
        Elmo_Delay100us(10);
    }
    
    /* ��ȫ��ڵ����ͨ�Ÿ�λ */
    Elmo_Delay100us(500);
    NMTCmd(&elmo[0], NMT_RESET_COMMUNICATION);
    Elmo_Delay100us(1000);

    /* Elmo�������Start------------------------------------------------------*/
    RSDO(&elmo[0], 0x1017, 0, (100&0xffff));//��������Ϊ100ms
    Elmo_Delay100us(100);
    
    RSDO(&elmo[0], 0x6007, 0, 1);//������ϣ�MO=0
    Elmo_Delay100us(100);

    RSDO(&elmo[0], 0x1029, 1, 2);//ͨ�Ŵ�����Ϊ��ֹͣ
    Elmo_Delay100us(100);
    
    RSDO(&elmo[0], 0x1016, 1, (0x09<<16) | (150&0xffff));//����������ʱ���� ID��0x09 T��150ms
    Elmo_Delay100us(2500);
    /* Elmo�������End--------------------------------------------------------*/
    
    
    /* ����TPDO,Debugʱ����,������ʱ��ùر� */
    RSDO(&elmo[0], 0x1A00, 0x00, 0);
    Elmo_Delay100us(150);
    RSDO(&elmo[0], 0x1A01, 0x00, 0);
    Elmo_Delay100us(150);
    
    /* ����NMT����״̬ */
    Elmo_Delay100us(40);
    NMTCmd(&elmo[0], NMT_ENTER_OPERATIONAL);
    Elmo_Delay100us(80);

    /*�˺��������ݻ��Ϊ0x05������״̬��Ϊ����̬*/

    /* �ر����� */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(80);

    /* ��ʼ�����ٶ� */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"PM", 0, TYPE_INTEGER, 0x01);
    Elmo_Delay100us(40);	
    RPDO2_Cmd_data(&elmo[0], (u8 *)"AC", 0, TYPE_INTEGER, 1000000);
    Elmo_Delay100us(40);
    RPDO2_Cmd_data(&elmo[0], (u8 *)"DC", 0, TYPE_INTEGER, 1000000);
    Elmo_Delay100us(40);

    /* Enter in SCM */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
    Elmo_Delay100us(40);

    for(i=0;i<=ELMO_NUM;i++)
    {
        elmo[i].CurOPMode = UM_SCM;			
    }

    /* ʹ������ */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
    Elmo_Delay100us(550);
}

/**
  ******************************************************************************
  * @name     : Elmo_PTM
  * @function : ��������ģʽ����
  * @input    : elmoID  ȡelmo�ڵ�ID
  *             torque  Ŀ��ת��(A)
  * @output   : 0       �������óɹ�
  *             1       ��������ʧ��
  ******************************************************************************
  */
u8 Elmo_PTM(u8 elmoID, float torque)
{
    u8 i = 0;
    
    /* ���ʹ�ù㲥����, ��ǰģʽ��Ϊ����ģʽ,�л�Ϊ����ģʽ*/
    if(elmoID == 0 && elmo[elmoID].CurOPMode != UM_TCM)  
    {
        if( elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }		
        
        /* ������״̬������ֵΪTCM */		
        for(i = 0; i <= ELMO_NUM; i++)
        {
            elmo[i].CurOPMode = UM_TCM;			
        }
        
        RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_TCM);
        Elmo_Delay100us(10);
        
        RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
    }  
    /* ʹ�õ���ELMO����,��ǰģʽ��Ϊ����ģʽ,�л�Ϊ����ģʽ */
    else if( elmo[elmoID].CurOPMode != UM_TCM)         
    {
        if( elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        elmo[elmoID].CurOPMode = UM_TCM;
        RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_TCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
        
        /* ���ELMOֻ������1����������ô�����ģʽ��elmo[0]��ģʽ����һ�� */		
        if(ELMO_NUM == 1)
        {
            elmo[0].CurOPMode = UM_TCM;
        }	
        /* �����ж��Ƿ����е��ģʽ��ͬ���Ӷ�ȷ��elmo[0]��ģʽ */
        else
        {
            for(i = 1; i < ELMO_NUM; i++)  
            {
                if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
                {
                    elmo[0].CurOPMode = UM_TCM;
                }
                else
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }			
        }
    }
        
    /* ����Ŀ��ת�ز����е���ģʽ */
    RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"TC", 0, TYPE_FLOAT, f2h(torque));
    Elmo_Delay100us(10);
    return 0;
}

/**
  ******************************************************************************
  * @name     : Elmo_PVM
  * @function : �����ٶ�ģʽ����
  * @input    : elmoID      ȡelmo�ڵ�ID
  *             speed       �ٶ�(cnt/s)
  * @output   : 0       �������óɹ�
  *             1       ��������ʧ��
  ******************************************************************************
  */
u8 Elmo_PVM(u8 elmoID, s32 speed)
{
    u8 i = 0;    
    if(elmo[elmoID].CurOPMode != UM_SCM)        /* ��ǰģʽ��Ϊ�ٶ�ģʽ,�л�Ϊ�ٶ�ģʽ */
    {
        if(elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
        
        
        if(elmoID == 0) //���ʹ�ù㲥����
        {
            for(i = 0; i <= ELMO_NUM; i++)
            {
                elmo[i].CurOPMode = UM_SCM;
            }
        }
        else            //�ǹ㲥����
        {
            elmo[elmoID].CurOPMode = UM_SCM;
            elmo[0].CurOPMode = UM_SCM;
            for(i = 1; i <= ELMO_NUM; i++) 
            {
                if( elmo[i].CurOPMode != UM_SCM )
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }
        }
    }
    
    /* ����Ŀ���ٶ� */
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"JV", 0, TYPE_INTEGER, speed);
    Elmo_Delay100us(10);

    /* �����ٶ�ģʽ */
    RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"BG");
    Elmo_Delay100us(20);
    
    return 0;
}


/**
  ******************************************************************************
  * @name     : Elmo_PPM
  * @function : ����ƽ���ٶȵ�λ��ģʽ
  * @input    : elmoID      ȡelmo�ڵ�ID
  *             speed       �ٶ�(cnt/s)
  *             position    Ŀ��λ��(cnt)
  *             PPMmode     ����ģʽ
  *             POS_ABS     PPM���з�ʽ:����λ��
  *             POS_REL     PPM���з�ʽ:���λ��
  * @output   : 0       �������óɹ�
  *             1       ��������ʧ��
  ******************************************************************************
  */
u8 Elmo_PPM(u8 elmoID, u32 speed, s32 position, u8 PPMmode)
{
    u8 i = 0;

    /* ���ʹ�ù㲥����,��ǰģʽ��Ϊλ��ģʽ,�л�Ϊλ��ģʽ */
    if(elmoID == 0 && elmo[elmoID].CurOPMode != UM_PCM)
    {
        if(elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(90);
        }	
        /* ������״̬������ֵΪPCM */					
        for(i = 0; i <= ELMO_NUM; i++)
        {
            elmo[i].CurOPMode = UM_PCM;			
        }

        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_PCM);
        Elmo_Delay100us(20);
        
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
    }  
    /* ʹ�õ���ELMO����,��ǰģʽ��Ϊλ��ģʽ,�л�Ϊλ��ģʽ */
    else if(elmo[elmoID].CurOPMode != UM_PCM)
    {
        if(elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(90);
        }

        elmo[elmoID].CurOPMode = UM_PCM;
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_PCM);
        Elmo_Delay100us(200);

        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);


        /* ���ELMOֻ������1����������ô�����ģʽ��elmo[0]��ģʽ����һ�� */				
        if(ELMO_NUM == 1)      
        {
            elmo[0].CurOPMode = UM_PCM;
        }		
        /* �����ж��Ƿ����е��ģʽ��ͬ���Ӷ�ȷ��elmo[0]��ģʽ */
        else 
        {
            for(i = 1; i < ELMO_NUM; i++) 
            {
                if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
                {
                    elmo[0].CurOPMode = UM_PCM;
                }
                else
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }			
        }
    }

    /* ����Ŀ���ٶ� */
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"SP", 0, TYPE_INTEGER, speed);
    Elmo_Delay100us(20);

    /* ��������ģʽ����λ�� */
    if(PPMmode)
    {
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"PA", 0, TYPE_INTEGER, position);
        Elmo_Delay100us(20);
    }
    else
    {
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"PR", 0, TYPE_INTEGER, position);
        Elmo_Delay100us(20);
    }
    
    /* ����λ��ģʽ */
    RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"BG");
    Elmo_Delay100us(10);
    return 0;
}

/**
  ******************************************************************************
  * @name     : Elmo_Close
  * @function : ��������رգ���������Լ�����ʻ
  * @input    : elmoID  ȡelmo�Ľڵ�ID
  * @output   : 0       �������óɹ�
  *             1       ��������ʧ��
  ******************************************************************************
  */
u8 Elmo_Close(u8 elmoID)
{
    u8 i = 0;
    
    /* ���ʹ�ù㲥����,������ELMOʧ�� */
    if(elmoID == 0)
    {
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
        Elmo_Delay100us(100);
        
        /* ������״̬������ֵΪIDLE */
        for(i = 0; i <= ELMO_NUM; i++)
        {
            elmo[i].CurOPMode = UM_IDLE;			
        }
    }
    /* ʹ�õ���ELMO���� */
    else 
    {
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
        Elmo_Delay100us(100);
        elmo[elmoID].CurOPMode = UM_IDLE;		

        /* ���ELMOֻ������1����������ô�����ģʽ��elmo[0]��ģʽ����һ�� */			
        if(ELMO_NUM == 1)       
        {
            elmo[0].CurOPMode = UM_IDLE;
        }		
        /* �����ж��Ƿ����е��ģʽ��ͬ���Ӷ�ȷ��elmo[0]��ģʽ */
        else 
        {
            for(i = 1; i < ELMO_NUM; i++)  
            {
                if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
                {
                    elmo[0].CurOPMode = UM_IDLE;
                }
                else
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }
        }
    }
    return 0;
}


/**
  ******************************************************************************
  * @name     : Elmo_Stop
  * @function : ɲ�����������
  * @input    : elmoID  ȡelmo�Ľڵ�ID
  * @output   : 0       �������óɹ�
  *             1       ��������ʧ��
  ******************************************************************************
  */
u8 Elmo_Stop(u8 elmoID)
{
    u8 i = 0;
    
    /* ���CAN������������Ŀ���ģʽ����ͬ���ҷ����㲥ָ����ιر� */
    if(elmoID == 0 && elmo[elmoID].CurOPMode == UM_UNC)  
    {
        /* ѭ���жϸ��������״̬�����ر�ÿһ����� */
        for(i = 1; i <= ELMO_NUM; i++)            
        {		
            /* ��ǰģʽΪ�ͷŵ��,�ȴ򿪵�����ٱ��� */		
            if (elmo[i].CurOPMode == UM_IDLE) 
            {
                RPDO2_Cmd_data(&elmo[i], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
                Elmo_Delay100us(30);				
            }
            /* ��ǰģʽΪ����ģʽ,���л�Ϊ�ٶ�ģʽ���ٹر� */
            else if(elmo[i].CurOPMode == UM_TCM)
            {
                RPDO2_Cmd_data(&elmo[i], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);			
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);			
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);			

                elmo[i].CurOPMode = UM_SCM;			

                RPDO2_Cmd_data(&elmo[i], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
                Elmo_Delay100us(10);

                RPDO2_Cmd_data(&elmo[i], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
                Elmo_Delay100us(30);

                /* ����Ŀ���ٶ� */
                RPDO2_Cmd_data(&elmo[i], (u8 *)"JV", 0, TYPE_INTEGER, 0);
                Elmo_Delay100us(10);

                /* �����ٶ�ģʽ */
                RPDO2_Cmd_string(&elmo[i], (u8 *)"BG");
                Elmo_Delay100us(10);
            }	
                
            RPDO2_Cmd_string(&elmo[i], (u8 *)"ST");
            Elmo_Delay100us(10);
        }	
        
        /* �����ж��Ƿ����е��ģʽ��ͬ���Ӷ�ȷ��elmo[0]��ģʽ,�ܹ��������if�Ĳ���������CAN����ֻ�е��� */
        for(i = 1; i < ELMO_NUM; i++)  
        {
            if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
            {
                elmo[0].CurOPMode = UM_SCM;         //�����STOPģʽ������ģʽͳһ�ˣ�ֻ�п�����SCMģʽ
            }
            else
            {
                elmo[0].CurOPMode = UM_UNC;
                break;
            }
        }											
    }
    /* �����㲥ָ����������϶���Ŀ���ģʽ����ͬ����ʱ�����һ��ر� */
    else if(elmoID == 0 && elmo[elmoID].CurOPMode != UM_UNC)  
    {
        /* ��ǰģʽΪ�ͷŵ��,�ȴ򿪵�����ٱ��� */		
        if (elmo[elmoID].CurOPMode == UM_IDLE) 
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
            Elmo_Delay100us(30);				
        }
        /* ��ǰģʽΪ����ģʽ,���л�Ϊ�ٶ�ģʽ���ٹر� */
        else if(elmo[elmoID].CurOPMode == UM_TCM)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			

            /* �㲥ģʽ�£������ٶ�ģʽ����ΪSCM */
            for(i=0; i <= ELMO_NUM; i++) 
            {
                elmo[i].CurOPMode = UM_SCM;			
            }
        
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
            Elmo_Delay100us(10);

            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
            Elmo_Delay100us(30);

            /* ����Ŀ���ٶ� */
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"JV", 0, TYPE_INTEGER, 0);
            Elmo_Delay100us(10);

            /* �����ٶ�ģʽ */
            RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"BG");
            Elmo_Delay100us(10);
        }	
            
        RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"ST");
        Elmo_Delay100us(10);	
        
    }
    /* ʹ�õ���ELMO���� */
    else    
    {
        /* ��ǰģʽΪ�ͷŵ��,�ȴ򿪵�����ٱ��� */		
        if (elmo[elmoID].CurOPMode == UM_IDLE) 
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
            Elmo_Delay100us(30);				
        }
        /* ��ǰģʽΪ����ģʽ,���л�Ϊ�ٶ�ģʽ���ٹر� */
        else if(elmo[elmoID].CurOPMode == UM_TCM)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			

            elmo[elmoID].CurOPMode = UM_SCM;			

            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
            Elmo_Delay100us(10);

            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
            Elmo_Delay100us(30);

            /* ����Ŀ���ٶ� */
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"JV", 0, TYPE_INTEGER, 0);
            Elmo_Delay100us(10);

            /* �����ٶ�ģʽ */
            RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"BG");
            Elmo_Delay100us(10);

            /* ���ELMOֻ������1����������ô�����ģʽ��elmo[0]��ģʽ����һ�� */			
            for(i=1;i<ELMO_NUM;i++)  //�����ж��Ƿ����е��ģʽ��ͬ���Ӷ�ȷ��elmo[0]��ģʽ
            {
                if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
                {
                    elmo[0].CurOPMode = UM_SCM;    //�����STOPģʽ������ģʽͳһ�ˣ�ֻ�п�����SCMģʽ
                }
                else
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }				
        }	
            
        RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"ST");
        Elmo_Delay100us(10);		
    }
    return 0;		
}

/**
  ******************************************************************************
  * @name     : Elmo_SetAcc(u8 elmoID, u32 acc, u32 dec)
  * @function : �����ٶ�ģʽ��λ��ģʽ�ĵ���Ӽ���
  * @input    : elmoID  ȡelmo�Ľڵ�ID
  *             acc     ���ٶ�,���ٶ�����ܳ���1000000000,ͬʱӦ���ǵ������
  *             dec     ���ٶ�,���ٶ�����ܳ���1000000000,ͬʱӦ���ǵ������
  * @output   : 0       �������óɹ�
  *             1       ��������ʧ��
  ******************************************************************************
  */
u8 Elmo_SetAcc(u8 elmoID, u32 acc, u32 dec)
{ 
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(30);	
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"PM", 0, TYPE_INTEGER, 0x01);
    Elmo_Delay100us(10);	
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"AC", 0, TYPE_INTEGER, acc);
    Elmo_Delay100us(10);
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"DC", 0, TYPE_INTEGER, dec);
    Elmo_Delay100us(10);
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
    Elmo_Delay100us(100);
    return 0;
}
/*============================================================================*/


/** @ElmoCanOpen �ײ㺯�� ----------------------------------------------------*/
/**
  ******************************************************************************
  * @name     : NMTCmd
  * @function : ����CANOPEN��NMT״̬����
  * @input    : elmo    ȡelmo�Ľڵ�ID
  *             MNTCmd  NMTָ��,NMT_xxx
  * @output   : None
  ******************************************************************************
  */
static void NMTCmd(Elmo *elmo, u8 MNTCmd)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = COBID_NMT_SERVICE << 21;    //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 2;                          //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u08[0] = MNTCmd;
    CanFrame.Data.u08[1] = elmo->NodeID;
    
    CAN_SendFrame(&CanFrame);
}

/**
  ******************************************************************************
  * @name     : RSDO
  * @function : ʹ������SDO����ָ���
  * @input    : elmo      ȡelmo�ڵ�ID
  *             Index     ����
  *             SubIndex  ������
  *             Data      ����
  * @output   : None
  ******************************************************************************
  */
static void RSDO(Elmo *elmo, u16 Index, u8 SubIndex, u32 Data)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = (COBID_RSDO + elmo->NodeID) << 21;  //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 8;                                  //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u32[0] = ((uint32_t)SubIndex) << 24 | ((uint32_t)Index<<8) | 0x22;
    CanFrame.Data.u32[1] = Data;
    
    CAN_SendFrame(&CanFrame);
}

static void RSDORead(Elmo *elmo, u16 Index, u8 SubIndex, u32 Data)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = (COBID_RSDO + elmo->NodeID) << 21;  //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 8;                                  //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u32[0] = ((uint32_t)SubIndex) << 24 | ((uint32_t)Index<<8) | 0x40;
    CanFrame.Data.u32[1] = Data;
    
    CAN_SendFrame(&CanFrame);
}

/**
  ******************************************************************************
  * @name     : RPDO2_Cmd_data
  * @function : ʹ�ö����Ʊ����ELMO��������ָ��
  *             CANopen RPDO2 -> 0x2012 ����������-���ù���
  * @input    : elmo    ȡelmo�ڵ�ID
  *             Cmd     ����,���ַ�����ʽ����
  *             Index   ������±�           
  *             Type    ��������
  *             Data    ����
  * @output   : None
  ******************************************************************************
  */
static void RPDO2_Cmd_data(Elmo *elmo, u8 *Cmd, u8 Index, u8 Type, u32 Data)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = (COBID_RPDO2 + elmo->NodeID) << 21;  //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 8;                                  //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u08[0] = (*Cmd++);
    CanFrame.Data.u08[1] = (*Cmd);
    CanFrame.Data.u08[2] = Index;
    CanFrame.Data.u08[3] = Type<<7;
    CanFrame.Data.u32[1] = Data;
    
    CAN_SendFrame(&CanFrame);
}

/**
  ******************************************************************************
  * @name     : RPDO2_Cmd_string
  * @function : ʹ�ö����Ʊ����ELMO�����ַ���ָ��
  *             CANopen RPDO2 -> 0x2012 ����������-ִ�й���
  * @input    : elmo    ȡelmo�ڵ�ID
                cmd     ����,���ַ�����ʽ����
  * @output   : None
  ******************************************************************************
  */
static void RPDO2_Cmd_string(Elmo *elmo, u8 *Cmd)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = (COBID_RPDO2 + elmo->NodeID) << 21;  //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 4;                                  //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u08[0] = (*Cmd++);
    CanFrame.Data.u08[1] = (*Cmd);
    CanFrame.Data.u08[2] = 0;
    CanFrame.Data.u08[3] = 0;
    
    CAN_SendFrame(&CanFrame);
}

/**
  ******************************************************************************
  * @name     : SendHeart2Elmo
  * @function : ���ط�������֡��elmo
  * @input    : None
  * @output   : None
  ******************************************************************************
  */
void SendHeart2Elmo(void)
{
    CAN_SendFrame((T_CanFrame*)&MainHeartFrame);
}
/*============================================================================*/


/**
  ******************************************************************************
  * @name     : Variate_init
  * @function : ������ʼ��
  * @input    : None
  * @output   : None
  ******************************************************************************
  */
static void Variate_init(void)
{
    ElmoHeartbeat = 0;
    memset (elmo, 0, (ELMO_NUM + 1)*sizeof(Elmo));
    memset (&chassisGroup, 0, sizeof(Elmo));
    memset (&slidewayGroup, 0, sizeof(Elmo));
}

/**
  ******************************************************************************
  * @name     : Elmo_software_delay_ms
  * @function : Elmo������ʱ
  * @input    : None
  * @output   : None
  ******************************************************************************
  */
void Elmo_software_delay_ms(unsigned int t)
{
    int i;
    for( i=0;i<t;i++)
    {
        int a = 41580; //at 168MHz 41580 is ok
        while(a--);
    }
}

/**
  ******************************************************************************
  * @name     : f2h
  * @function : ��������ת��Ϊ8�ֽ�ʮ��������(IEEE754)
  * @input    : x   ������ 
  * @output   : None
  ******************************************************************************
  */
static u32 f2h(float x)
{
    u32 *p = (u32 *)&x;
    return ((u32)*p);
}

/**
  ******************************************************************************
  * @name     : CAN1_RX0_IRQHandler
  * @function : CAN�жϴ�������
  * @input    : None
  * @output   : None
  ******************************************************************************
  */
int32_t Encoder_Data_Main = 0, Encoder_Data_Serve = 0;
void CAN1_RX0_IRQHandler()
{
    static CanRxMsg RxMsg;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMsg);

    if(RxMsg.StdId > COBID_HEARTBEAT && RxMsg.StdId < (COBID_HEARTBEAT+0xf) && RxMsg.Data[0] == 0x05)
    {
        ElmoHeartbeat |= (1<<((RxMsg.StdId&0x0F)-1));
        if(ElmoHeartbeat == DEF_ElmoHeartbeat_VALUE)
        {
            portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
            ElmoHeartbeat = 0;
            xSemaphoreGiveFromISR( ElmoSem, &xHigherPriorityTaskWoken );
            portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
        }
    }
    
    else if(RxMsg.StdId == COBID_TSDO + MOTOR_ID_HIT_MAIN)
    {
        if ((RxMsg.Data[2]<<8 | RxMsg.Data[1]) == 0x6064)
        {
            portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
            memcpy(&Encoder_Data_Main, &RxMsg.Data[4],sizeof(int32_t));
            xSemaphoreGiveFromISR( PosRacketMainSem, &xHigherPriorityTaskWoken );
            portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
        }
    }
    else if(RxMsg.StdId == COBID_TSDO + MOTOR_ID_HIT_SERVE)
    {
        if ((RxMsg.Data[2]<<8 | RxMsg.Data[1]) == 0x6064)
        {
            portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
            memcpy(&Encoder_Data_Serve, &RxMsg.Data[4],sizeof(int32_t));
            xSemaphoreGiveFromISR( PosRacketServeSem, &xHigherPriorityTaskWoken );
            portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
        }
    }
}

/**
  * @brief  Elmo_Read_POS,��ȡ��Ӧ��elmo�ı�����������
  * @param  elmoID   �޷���8λ����
  *         pEncData int32_t ��ָ�룬ָ�򷵻�����
  * @retval 0   ���ɹ�
  *         1   ��ʧ��
  * @time   RSDORead
  *         -   1ms�����������1000�Σ�����ȡ(���������������յ�����)ʱ��δ���� 80000/84us = 952.38us
  *         -   �˴��ȴ�1.5ms
  * @note   �ж��ж�ȡ
  *         RxMsg.StdId == COBID_TSDO + ID;
  *         if ((RxMsg.Data[2]<<8 | RxMsg.Data[1]) == 0x6064)
  *             memcpy(&Encoder_Data, &RxMsg.Data[4],sizeof(int32_t));
  *         Ŀǰ��֧�� ID��MOTOR_ID_HIT_MAIN��MOTOR_ID_HIT_SERVE ������
  */
uint32_t Elmo_Read_POS(u8 elmoID, int32_t *pEncData)
{
    uint32_t err = 1;
    /* ����ʼ�տ���CAN1�����жϣ��ʲ����ٿ���CAN1�����ж� */
    //CAN1->IER |= CAN_IT_FMP0;

    /* ��ȡelmo����������ֵ */
    RSDORead(&elmo[elmoID], 0x6064, 0x00, 0);
    
    if(elmoID == MOTOR_ID_HIT_MAIN)
    {
        if(xSemaphoreTake(PosRacketMainSem, 3*portTICK_MS/2) == pdTRUE)   //1.5ms
        {
            *pEncData = Encoder_Data_Main;
            err = 0;
        }
        else
            err = 1;
    }
    else if(elmoID == MOTOR_ID_HIT_SERVE)
    {
        if(xSemaphoreTake(PosRacketServeSem, 3*portTICK_MS/2) == pdTRUE)   //1.5ms
        {
            *pEncData = Encoder_Data_Serve;
            err = 0;
        }
        else
            err = 1;
    }
    
    /* �ر�CAN1�����ж� */
    //CAN1->IER &= ~CAN_IT_FMP0;
    return err;
}

/**
  ******************************************************************************
  * @name     : Elmo_Set_POS
  * @function : ��ȡ��Ӧ��elmo�ı�����������
  * @input    : elmoID   �޷���8λ���� 
  * @output   : None
  ******************************************************************************
  */
void Elmo_Set_POS(u8 elmoID,s32 POS)
{
    //�رյ������������λ��ֵ���Ⱦ�����
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(90);

    //�Ա�������ֵ��������
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"PX", 0, TYPE_INTEGER, POS);
    Elmo_Delay100us(90);

    //�������
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
    Elmo_Delay100us(90);
    //���µ�ǰ���״̬
    elmo[elmoID].CurOPMode = UM_UNC;
}

/**
  ******************************************************************************
  * @name     : Elmo_Read_ACT_CUR
  * @function : ��ȡ��Ӧ��elmo����Ч����
  * @input    : elmoID   �޷���8λ���� 
  * @output   : None
  ******************************************************************************
  */
void Elmo_Read_ACT_CUR(u8 elmoID)
{
    //����TPDO2
    RSDO(&elmo[0], 0x1A01, 0x00,1);
    Elmo_Delay100us(50);
    //��ȡelmo����������ֵ
    RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"IQ");
    Elmo_Delay100us(20);

    //�ر�TPDO2
    RSDO(&elmo[0], 0x1A01, 0x00,0);
    Elmo_Delay100us(50);
}
/*============================================================================*/


/** @GroupOperation �����ģʽ -----------------------------------------------*/
// PVM ---------------------------------------------------------------
void Chassis_Elmo_PVM(s32 speed1, s32 speed2, s32 speed3, s32 speed4)
{
    u8 i;
    for(i = 5; i < 8; i++)
    {
        if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
        {
            chassisGroup.CurOPMode = elmo[i].CurOPMode;
        }
        else
        {
            chassisGroup.CurOPMode = UM_UNC;
            break;
        }
    }
    
    if(chassisGroup.CurOPMode != UM_SCM)        /* ��ǰģʽ��Ϊ�ٶ�ģʽ,�л�Ϊ�ٶ�ģʽ */
    {
        if(chassisGroup.CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&chassisGroup, (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
    }
    
    /* ����Ŀ���ٶ� */
    RPDO2_Cmd_data(&elmo[MOTOR_ID_CHASSIS_1], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_CHASSIS_1*speed1);
    RPDO2_Cmd_data(&elmo[MOTOR_ID_CHASSIS_2], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_CHASSIS_2*speed2);
    RPDO2_Cmd_data(&elmo[MOTOR_ID_CHASSIS_3], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_CHASSIS_3*speed3);
    RPDO2_Cmd_data(&elmo[MOTOR_ID_CHASSIS_4], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_CHASSIS_4*speed4);
    Elmo_Delay100us(10);

    /* �����ٶ�ģʽ */
    RPDO2_Cmd_string(&chassisGroup, (u8 *)"BG");
    Elmo_Delay100us(20);
    
    for(i = 5; i <= 8; i++)
    {
        elmo[i].CurOPMode = UM_SCM;
    }
}

void Slideway_Elmo_PVM(s32 speed1,s32 speed2)
{
    u8 i;
    
    for(i = 1; i < 2; i++)
    {
        if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
        {
            slidewayGroup.CurOPMode = elmo[i].CurOPMode;
        }
        else
        {
            slidewayGroup.CurOPMode = UM_UNC;
            break;
        }
    }
    
    if(slidewayGroup.CurOPMode != UM_SCM)        /* ��ǰģʽ��Ϊ�ٶ�ģʽ,�л�Ϊ�ٶ�ģʽ */
    {
        if(slidewayGroup.CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&slidewayGroup, (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
    }
    
    /* ����Ŀ���ٶ� */
    RPDO2_Cmd_data(&elmo[MOTOR_ID_SLIDEWAY_X], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_SLIDEWAY_X*speed1);
    RPDO2_Cmd_data(&elmo[MOTOR_ID_SLIDEWAY_Y], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_SLIDEWAY_Y*speed2);
    Elmo_Delay100us(10);

    /* �����ٶ�ģʽ */
    RPDO2_Cmd_string(&slidewayGroup, (u8 *)"BG");
    Elmo_Delay100us(20);
    
    for(i = 1; i <= 2; i++)
    {
        elmo[i].CurOPMode = UM_SCM;
    }
}

// Stop --------------------------------------------------------------------------
void Chassis_Elmo_Stop()
{
    u8 i;
    for(i = 5; i < 8; i++)
    {
        if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
        {
            chassisGroup.CurOPMode = elmo[i].CurOPMode;
        }
        else
        {
            chassisGroup.CurOPMode = UM_UNC;
            break;
        }
    }
    
    if(chassisGroup.CurOPMode != UM_SCM)        /* ��ǰģʽ��Ϊ�ٶ�ģʽ,�л�Ϊ�ٶ�ģʽ */
    {
        if(chassisGroup.CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&chassisGroup, (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);

        for(i = 5; i <= 8; i++)
        {
            elmo[i].CurOPMode = UM_SCM;
        }
    }

    RPDO2_Cmd_string(&chassisGroup, (u8 *)"ST");
    Elmo_Delay100us(10);
}

void Slideway_Elmo_Stop()
{
    u8 i;
    for(i = 1; i < 2; i++)
    {
        if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
        {
            slidewayGroup.CurOPMode = elmo[i].CurOPMode;
        }
        else
        {
            slidewayGroup.CurOPMode = UM_UNC;
            break;
        }
    }
    
    if(slidewayGroup.CurOPMode != UM_SCM)        /* ��ǰģʽ��Ϊ�ٶ�ģʽ,�л�Ϊ�ٶ�ģʽ */
    {
        if(slidewayGroup.CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&slidewayGroup, (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
        for(i = 1; i <= 2; i++)
        {
            elmo[i].CurOPMode = UM_SCM;
        }
    }
    
    RPDO2_Cmd_string(&slidewayGroup, (u8 *)"ST");
    Elmo_Delay100us(10);
}


// Close ----------------------------------------------------------------------
void Chassis_Elmo_Close()
{
    u8 i;
    
    RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(100);

    for(i = 5; i <= 8; i++)
    {
        elmo[i].CurOPMode = UM_IDLE;
    }
}

void Slideway_Elmo_Close()
{
    u8 i;
    
    RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(100);

    for(i = 1; i <= 2; i++)
    {
        elmo[i].CurOPMode = UM_IDLE;
    }
}