/**************************************************************************
 * �ļ���: mshmpool.h
 * ����      : �ù����ڴ�ʵ�ֵĻ����  ,�����ɲ�ͬ����
 			  ʵ�������ߺ�������
 *************************************************************************/
#ifndef MSHMPOOL_H
#define MSHMPOOL_H
#ifndef _WIN32

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#else
#define size_t unsigned long
#define key_t  unsigned long
#endif

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//���޸Ĺ����������е�����ʱ��ע�⣬�޸ĺ����
//���豸������ʹ���µ�gtipc��Ӧ�ó����п��ܻ����
//�δ�������������Ҫ��������gtvs1000/3000�豸����
//������һ��ֵΪkey�Ĺ��������������ǽṹ��һ��
//�ˣ����Ի���ֶδ��������gtvs3000�豸�Ͽ�����
//ipcrm  �� ipcs �ر��Ѿ�����Ĺ�����������Ȼ��������
//�µ�Ӧ�ó�������gtvs1000��û��������Ӧ�ó���
//�����������豸��ȷ���״������°��Ӧ�ó���.
//zw-add 2011-11-08
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#include "mshm.h"
#define 	SHPOOL_BLOCK_SIZE		512		//������еĿ�����
#define	SHPOOL_MAX_USERS		8		//�û����֧�ֵ�����û���(�ź������������ź�����Ŀ)
										//Ҳ���Ǳ�����ؿ����֧�ֵ��û���
//#define SHPOOL_MAX_ELE			400	//100//����ؿ������ɵ����Ԫ�ظ���
//SHPOOL_MAX_ELE ���ֵ��ô��ڹ�����������ܴ�ŵ����Ԫ�� �ĸ�����
//�������õĺô��ǿ��Ա����ڻ�������ظ����ֶ����ͬԪ�ص����
#define SHPOOL_MAX_ELE				5000	//����Ԫ�ظ�����Ϊ�˱�������֡������¼��ط�ʹ�� zw-modified-20110720
								
typedef struct{
//��������Ԫ�صĽṹ
	int num;					//��� 0~0xffffffffѭ��
	long userflag;			//�û��Զ���ı��
	long datalen;				//Ԫ����Ч���ݳ���
	int blocks;				//Ԫ������ռ�õ��ܿ���
	int bblock;				//Ԫ������ռ�õ���ʼ��
	int eblock;				//Ԫ������ռ�õĽ�����
}SHPOOL_ELE;

typedef union{
	unsigned char data[SHPOOL_BLOCK_SIZE];
}SHPOOL_BLOCK;

#define  	MSHMPOOL_LOCAL_USR	0	//�����û�
#define	MSHMPOOL_NET_USR		1	//�����û�
typedef struct{
	//������û���Ϣ�ṹ
	int	 	valid;			//�û���Ч��־0��������1������ʹ��
	int		curele;			//��ǰ���Է��ʵ�Ԫ�����
	pid_t 	upid;			//�û��Ľ��̺�
	time_t	stime;			//�û����ӿ�ʼʱ��//��Ϊ��λ
	char 	name[32];		//�û������31�ֽ�(ʹ�û���صĽ�������)
	int		type;			//�û����� MSHMPOOL_LOCAL_USR:MSHMPOOL_NET_USR
	char  	info[128];		//�û�������Ϣ,ÿ���û���Ϣ�64�ֽ�
}SHPOOL_USR;

typedef struct{
	int maxsem;								//������������û������ź�����
											//����ź����������ֵ����Ϊ
											//�û������Ѿ������˳�
	int num;									//�����������Ԫ�ص����
	int tblocks;								//������е��ܿ���
	int fblocks;								//��ǰ��ʣ�����
	int fbstart;								//���п����ʼ���
//	int maxele;								//�Զ�����ɵ�Ԫ�ظ���
	int boffset;								//Ԫ����������ڶ�ph�׵�ַ��ƫ��
	int count;								//����ص�ǰ��ЧԪ�ظ���
	int head;									//��һ����ЧԪ��(���ϵ�Ԫ��)
	int tail;									//�������Ԫ�صĿհ׿ռ�
	key_t semkey;							//����ͨѶ���ź�����key,���key�ǹ����ڴ�
											//�õ����ź�����key|0x50000000
	int semid;								//����ͨѶ���ź���
	int last_usr_no;							//���һ���û�����Ÿ�ֵ��ʾ������û�й��û�
//����ͳ�����������õ��ı���
	unsigned long		send_bytes;						//�ܹ����͵��ֽ���
///////
	char	pinfo[512];							//������õ���������Ϣ������ṹ�ṩ���ϲ�Ӧ���Լ�����
											//������ṩ��Ӧ�ó�������Ϣ�Ŀռ�
	
	SHPOOL_ELE	eles[SHPOOL_MAX_ELE];		//Ԫ��ͷ�洢��
	SHPOOL_USR	users[SHPOOL_MAX_USERS];	//����û���Ϣ
}SHPOOL_HEAD;

#define	MSHM_POOL_NORMAL_USR	0	//��ͨ�û�
#define	MSHM_POOL_CREATER		1	//������
#define	MSHM_POOL_LOOKER			2	//�쿴��
typedef struct{
	int				userflag;	//�û���־ 0:��ͨ�û�  1:������ 2:�쿴��(���ܴӳ��н����ź���)
	int				userno;		//�û����
	MEM_CORE		*mc;		//��Ż���ص��ڴ�
	SHPOOL_HEAD 	*ph;		//��Ż���ص�ͷ�ĵ�ַ
	SHPOOL_BLOCK 	*eledata;	//���Ԫ�����ݵĵ�ַ
}MSHM_POOL;
/**************************************************************************
  *������	:MShmPoolMvUsrPlace
  *����	:���û���ǰ��ȡ���ݵ�ָ�밴Ҫ���ƶ�
  *����	: pool:ָ�򻺳�ص�ָ�� 
  			  place:Ҫ�ƶ�������
  			  	��ֵ��ʾ��ǰ�ƶ�|place|��
  			  	��ֵ��ʾ����ƶ�place��
  			  	0��ʾ�����ƶ�
  *����ֵ	:����0��ʾ�ɹ� ��ֵ��ʾʧ�ܴ����Ǹ���errno
  *************************************************************************/
int MShmPoolMvUsrPlace(MSHM_POOL *pool,int place);

/**************************************************************************
  *������	:MShmPoolGetResource
  *����	:�ӻ������ȡ��һ��Ԫ�ط��뻺����
  *����	: pool:ָ�򻺳�ص�ָ�� 
  			  buf:Ŀ�����ݻ��������׵�ַ
  			  buflen:�������ĳ���
	���   :eleseq:Ԫ�ص����
			 flag:Ԫ�صı�־
  *����ֵ	:�ɹ��򷵻ؿ������ֽ���(Ԫ�ص���Ч����)
  			 ���û���������С�����򷵻� -ENOMEM
  *************************************************************************/
int MShmPoolGetResource(MSHM_POOL *pool,void *buf,int buflen,int *eleseq,int *flag);

/**************************************************************************
  *������	:MShmPoolAddResource
  *����	:��ָ����������������ΪԪ�ط��뻺���β��
  *����	: pool:ָ�򻺳�ص�ָ�� 
  			  buf:Դ���ݻ��������׵�ַ
  			  buflen:����������Ч���ݳ���
			 flag:Ҫ�����Ԫ�صı�־
  *����ֵ	:����0��ʾ�ɹ� ��ֵ��ʾ����
  *
  *************************************************************************/
 int MShmPoolAddResource(MSHM_POOL *pool,void *buf,int buflen,int flag);

/**************************************************************************
  *������	:MShmPoolCreate
  *����	:���ո�����pkey����ָ����С�Ĺ��������
  *����	: name:����ص�����
  			  pkey: ����ص�key(�������������key���������ڴ�
  			           �������ź���,����(key|0x50000000)������Դ�ź���)
  			  bytes:ϣ������ �Ļ������е�ʵ����������
  *���	:����������ʼ��pool�ṹ�е��ֶ�
  *����ֵ	:����0��ʾ�ɹ� ��ֵ��ʾʧ�ܴ����Ǹ���errno
  *
  *************************************************************************/
int MShmPoolCreate(char *name,MSHM_POOL *pool,key_t pkey,int bytes);

/**************************************************************************
  *������	:MShmPoolSetMaxNum
  *����	:�����źŻ�������ж��û���ʱ��Ԫ����
  *����	: pool:ָ�򻺳�ص�ָ�� 
  			  num: �жϳ�ʱ��Ԫ�ظ���
  *����ֵ	:����0��ʾ�ɹ� ��ֵ��ʾʧ�ܴ����Ǹ���errno
  *                   �������ɵ���MShmPoolCreate�Ľ��̵���,���û��г���
  *                   num��Ԫ��û�ж�ȡʱ������Ϊ�û��Ѿ��˳����߳��û�
  *************************************************************************/
int MShmPoolSetMaxNum(MSHM_POOL *pool,int num);


/**************************************************************************
  *������	:MShmPoolSetMaxNum_t
  *����	:�����źŻ�������ж��û���ʱ��Ԫ����
  *����	: num: �жϳ�ʱ��Ԫ�ظ���
  *����ֵ	:����0��ʾ�ɹ� ��ֵ��ʾʧ�ܴ����Ǹ���errno
  *                   �������ɵ���MShmPoolCreate�Ľ��̵���,���û��г���
  *                   num��Ԫ��û�ж�ȡʱ������Ϊ�û��Ѿ��˳����߳��û�
  *************************************************************************/
void MSHmPoolSetMaxNum_t(int num);

/**************************************************************************
  *������	:MSHmPoolAttach
  *����	:���ո�����pkey���ӵ������
  *����	: pkey:Ҫ���ӵ� ����ص�key
  *���	:����������ʼ��pool�ṹ�е��ֶ�
  *����ֵ	:����0��ʾ�ɹ� ��ֵ��ʾʧ�ܴ����Ǹ���errno
  *ע		:��������Ҫ���ڼ�ػ���ص�����״̬�ĳ���
  *************************************************************************/
int MSHmPoolAttach(MSHM_POOL *pool,key_t pkey);

/**************************************************************************
  *������	:MShmPoolReq
  *����	:���ո�����pkey����һ�������,�������䵽pool
  *����	: pkey:Ҫ����� ����ص�key
  			  name:�����ڴ�ص��û���
  			  type:�û��������ϲ㺯��������
  *���	:����������ʼ��pool�ṹ�е��ֶ�
  *����ֵ	:����0��ʾ�ɹ� ��ֵ��ʾʧ�ܴ����Ǹ���errno
  *************************************************************************/
int MShmPoolReq(MSHM_POOL *pool,key_t pkey,char *name,int type);

/**************************************************************************
  *������	:MShmPoolReqFree
  *����	:�ͷ��Ѿ����빲�������
  *����	: pool:����ؽṹָ��
  *����ֵ	:����0��ʾ�ɹ� ��ֵ��ʾʧ�ܴ����Ǹ���errno
  *************************************************************************/
int MShmPoolReqFree(MSHM_POOL *pool);

/**************************************************************************
  *������	:MShmPoolGetUsrInfo
  *����	:��ȡָ���û��ŵ��û���Ϣ������ַ����info��
  *����	: pool:����ؽṹָ��
  			: userno �û���
  *���	: info ����ʱ��info�����뻺�����ĵ�ַ
  *����ֵ	:���ظ�ֵ��ʾ��������ֵ��ʾinfo�������Ĵ�С
  *ע		:info�������е���Ϣ���ݼ��ṹ�����ϲ�Ӧ�ó�
  			 �����
  			 ����Ҫʱ���ϲ㺯���������ڴ�
  *************************************************************************/
int MShmPoolGetUsrInfo(MSHM_POOL *pool,int userno,void **info);

/**************************************************************************
  *������	:MshmPoolGetTotalUser
  *����	:��ȡ��ǰʹ�û���ص��û�����
  *����	: pool:����ؽṹָ��
  *����ֵ	:���ػ���ص��û�����,��ֵ��ʾ����
  *************************************************************************/
int MShmPoolGetTotalUser(MSHM_POOL *pool);
/**************************************************************************
  *������	:MShmPoolGetInfo
  *����	:��ȡ��ǰʹ�û���ص�Ӧ���Զ�����Ϣ
  *����	: pool:����ؽṹָ��
  *����ֵ	:����Ӧ���Զ������Ϣ��ַ
  *			�����Ҫ���ϲ����Ӧ�����ڴ����
  *************************************************************************/
void *MShmPoolGetInfo(MSHM_POOL *pool);
/**************************************************************************
  *������	:GetUsrValid
  *����	:��ȡ��ǰ�û�������Ƿ���Ȼ��Ч��־
  *����	: pool:����ؽṹָ��
  *����ֵ	:���ظ�ֵ��ʾ������0��ʾ��Ч 1��ʾ��Ч

  *************************************************************************/
int GetUsrValid(MSHM_POOL *pool);
/**************************************************************************
  *������	:MShmPoolReActiveUsr
  *����	:���¼����û�,����û���ʱ�䲻�ܽ�������
  			:�򻺳�ط��ͳ�������Ϊ���˳���,���Խ�����Ӧ
  			 ���ڵ��ô˺���,���¼����û�(��ֹ����)
  *����	: pool:����ؽṹָ��
  *����ֵ	:0��ʾ��ȷ ��ֵ��ʾ���� 1��ʾ�û��Ѿ���ɾ����
  			�������¼���
  *ע		:�����������������ڴ�			
  *************************************************************************/
int	MShmPoolReActiveUsr(MSHM_POOL *pool);

/**************************************************************************
  *������	:MShmEleRemain
  *����	:���ػ��ж���Ԫ��û�ж�ȡ
  *����	: pool:ָ�򻺳�ص�ָ�� 
  *����ֵ	: ��ǰ�û����ж�������û�ж�ȡ
  *ע		:pool�ĺϷ����ɵ����߱�֤
  *************************************************************************/
static __inline__ int MShmEleRemain(MSHM_POOL *pool)
{
	SHPOOL_HEAD     	*ph=pool->ph;
	SHPOOL_USR	 	*usr=&ph->users[pool->userno];
	SHPOOL_ELE	      	*ele=&ph->eles[usr->curele];
	if(usr->curele==ph->tail)
		return 0;		//FIXME ��num����unsigned long ��ʾ�ķ�ΧʱӦ���⴦��
        if(ph->num>=ele->num)
            return (ph->num-ele->num);	
        else
            return (ph->num);//FIXME
}


#endif




