
#include "tos_k.h"
#include "tos_at_gps.h"
#include "tos_hal.h"


#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"
#include "gps.h"
#include "mymath.h"

const char ATGM336H_GPS[]="$PCAS04,1*18\r\n";
const char ATGM336H_BDS[]="$PCAS04,2*1B\r\n";
const char ATGM336H_GPS_BDS[]="$PCAS04,3*1A\r\n";
const char ATGM336H_GLONASS[]="$PCAS04,4*1D\r\n";
const char ATGM336H_GPS_GLONASS[]="$PCAS04,5*1C\r\n";
const char ATGM336H_BDS_GLONASS[]="$PCAS04,6*1F\r\n";
const char ATGM336H_GPS_BDS_GLONASS[]="$PCAS04,7*1E\r\n";
const char ATGM336H_FREQ[]="$PCAS03,0,0,0,0,2,0,0,0,0,0,0,0,0,0*00\r\n";//"$PCAS03,2,0,0,2,2,0,0,0,0,0,0,0,0,0*00\r\n";


char* get_gpsrmc (char* buf)
{
    char const *buff=buf;
    char *p=NULL;	

	if(p= strstr(buff,"GNRMC"))			//联合
	{

	}
	else if(p= strstr(buff,"GBRMC")){	//BDS

	}
	else if(p= strstr(buff,"GPRMC")){	//GPS

	}
	else if(p= strstr(buff,"BDRMC")){	//BDS

	}
	return p;
}

char* get_gpsgga(char* buf)
{
    char const *buff=buf;
    char *p=NULL;	

	if(p= strstr(buff,"GNGGA")){
	}
	else if(p= strstr(buff,"GBGGA")){
	}
	else if(p= strstr(buff,"GPGGA")){
	}
	else if(p= strstr(buff,"BDGGA")){
	}
	return p;
}

char* get_gpsgsv(char* buf)
{
    char const *buff=buf;
    char *p=NULL;	

	if(p= strstr(buff,"GNGSV")){
	}
	else if(p= strstr(buff,"GBGSV")){
	}
	else if(p= strstr(buff,"GPGSV")){
	}
	else if(p= strstr(buff,"BDGSV")){
	}
	return p;
}


//GBRMC,065730.00,V,2231.89755,N,11356.42786,E,0.206,,100919,,,N,V*08
void ParseGpsRmc(char *gpsdata)
{
	uint16_t  i,j,numtotal= 0;
	uint8_t zero,first,second,third,fourth,fifth,sixth,sevevth;
	char gpstestbuf[40];
	char coordinate[25];
	char rmcdatabuf[60];
	char *buff_gprmc=NULL;
	char tmp[10],tmp1[10],GPSChecksum[2],result;
	char *test;
	int len;
	test = gpsdata;
	j =0;
	first =0; second=0;third=0;fourth=0;fifth=0;sixth=0;sevevth=0,result=0;
//			printf("\r\n%s \n",test);			
			buff_gprmc = get_gpsrmc(test);
			len = strlen(test);
			if(buff_gprmc ==0){
//				gpsFlag.Havetime2calitNb=0;
				return;
			}else{
//				printf("len= %d\r\n",len);
//				printf("%s",buff_gprmc);
//				tos_kprintln("rmc\n");
				for(i =0;i<len-2;i++){
//					printf("%c",buff_gprmc[i]);
					if(buff_gprmc[i] == '\n')break;
					if(buff_gprmc[i] == ','){
						numtotal++;
						if(numtotal == 1){
							zero = i;
						} else if(numtotal == 3){
							first = i;
						} else if(numtotal == 4){
							second = i;
						} else if(numtotal == 5){
							third = i;
						} else if(numtotal == 6){
							fourth = i;
						} else if(numtotal == 7){
							fifth = i;
						} else if(numtotal == 8){
							sixth = i;
						} else if(numtotal == 9){
							sevevth = i;
						}
					}
				}
				if(numtotal <12)
				{
//					gpsFlag.Havetime2calitNb=0;
					return ;
				}			
				if(buff_gprmc[first-1] != 65)		//V 
				{	
//					gpsFlag.Havetime2calitNb=0;	
					return ;
				}				
			}
			if(((second-first)<2)||((third-second)<2)){
				return;
			}	
			
			for(result=buff_gprmc[0],i=1;buff_gprmc[i]!='*';i++)
			{
				result^=buff_gprmc[i];				
			}
			memset(GPSChecksum,0,2);
			ByteToHexStr(&result,GPSChecksum,2);
			if((buff_gprmc[i+1]!=GPSChecksum[0]) || (buff_gprmc[i+2]!=GPSChecksum[1]))
			{
				return;
			}

			//--get gps time ---------------------------------------------------
			for(i=zero+1,j=0;i<zero+7;i++,j++)
			{				
				gpstestbuf[j] = buff_gprmc[i];
			}
			tos_at_gps_channel_write(0, gpstestbuf, 13);
//			memset(gpsData.time,0,sizeof(gpsData.time));
//			memcpy(gpsData.time,gpstestbuf,6);
			#ifdef DEBUG_GPS_DATA
//			printf("\r\nGps time:%.6s \r\n",gpsData.time);
			#endif
			//-------------------------------------------------------------------
			
			memset(gpstestbuf,0,40);
			memset(coordinate,0,25);
			for(j =0,i = first+1;i<second;i++){				
				if((buff_gprmc[i] < 46) || (buff_gprmc[i] > 57) )
				{
					return;
				}
				gpstestbuf[j++] = buff_gprmc[i];
			}
			if((i-first-1)<10)	//如果位数少了1位
				gpstestbuf[j++] = '0';
			gpstestbuf[j++] = ',';
			for(i = third+1;i<fourth;i++){				
				if((buff_gprmc[i] < 46) || (buff_gprmc[i] > 57) )
				{
					return;
				}
				gpstestbuf[j++] = buff_gprmc[i];
			}
			if((i-third-1)<11)	//如果位数少了1位
				gpstestbuf[j++] = '0';
			
			#ifdef DEBUG_GPS_DATA
			printf("\r\nN AND E is \r\n");
			#endif
//			tos_kprintln(gpstestbuf);
			tos_at_gps_channel_write(1, gpstestbuf, strlen(gpstestbuf));
									
			j =0;
			#ifdef DEBUG_GPS_DATA
			printf("\r\nfindchar5 is \r\n");
			#endif
			memset(tmp,0,10);
			for(i = fifth+1;i<sixth;i++){
				if(buff_gprmc[i] =='.'){
					break;
				}
				tmp[j++]= buff_gprmc[i];
			}
			
			i = atoi(tmp);
			if(i>1)
			{
//				gpsFlag.gps_speed_non_zero=1;				
			}
			else{
//				gpsFlag.gps_speed_non_zero=0;			
			}			
			#ifdef DEBUG_GPS_DATA
			printf("\r\nspeed is:%d\r\n",i);
			#endif			
			j =0;
			memset(tmp,0,10);
			for(i = sixth+1;i<sevevth;i++){
				if(buff_gprmc[i] =='.'){
					break;
				}
				tmp[j++]= buff_gprmc[i];
			}
			i = atoi(tmp);
			#ifdef DEBUG_GPS_DATA
			printf("\r\ndirection is:%d\r\n",i);
			#endif		
			#ifdef DEBUG_GPS_DATA
			printf("\r\nparse data end \r\n");
			#endif
}



//$GBGGA,032112.00,3924.40647,N,07556.84250,E,1,03,4.20,1333.7,M,-49.1,M,,*5B
void ParseGpsGga(char *gpsdata)
{
	uint16_t  i,j,numtotal= 0;
	uint8_t sixth,sevevth;
	char *buff_gpgga=NULL;
	char tmp[10];
	char *test;
	int len;
	test = gpsdata;
	j =0;sixth=0;sevevth=0;		
	buff_gpgga = get_gpsgga(test);
	len = strlen(test);
			if(buff_gpgga ==0){
//				gpsFlag.Havetime2calitNb=0;
				return;
			}
			else
			{
				for(i =0;i<len-2;i++)
				{
					if(buff_gpgga[i] == '\n')break;
					if(buff_gpgga[i] == ',')
					{
						numtotal++;
						if(numtotal == 7)
							sixth = i;
						else if(numtotal == 8)
							sevevth = i;
					}
				}
				if(numtotal <14)				
					return ;						
			}
			if(((sevevth-sixth)<2))
				return;
			for(j=0,i = sixth+1;i<sevevth;i++,j++)
			{				
				if((buff_gpgga[i] < 48) || (buff_gpgga[i] > 57) )
				{
					printf("Data error \r\n");
					return;
				}
				tmp[j]= buff_gpgga[i];
			}
//			printf("i:%d \r\n",ava_satellite);
}

//$BDGSV,4,1,15,01,47,123,42,02,47,235,36,03,64,189,44,07,74,092,41*66
GSV_ gsv;
void ParseGpsGsv(char *gsvdata)
{
	char *buff_gpgga=NULL;
//	char *test;
	int i,len;
	int num,num1,index,line,total,db;
	char *revbuf[23] = {0};
//	test = gpsdata;
//	buff_gpgga = get_gpsgsv(gsvdata);
	if(gsvdata ==NULL){
		return;
	}
	else{
		printf("%s\n",gsvdata);
		split(gsvdata,",*",revbuf,&num);
//		split(revbuf[num-1],"*",NULL,NULL);

		
		for(i=1;i<num; i++)
		{
			if(revbuf[i]!=NULL)
				printf("revbufval =%d\n",decstr_to_int(revbuf[i]));
			else
				printf("revbufval =NULL%d\n",i);
		}
		
		
		if(num>4)
		{
			line = decstr_to_int(revbuf[2]);//当前是第几条
			total = decstr_to_int(revbuf[3]);//总星数
			if(line==1)	//如果是收到第一条，把之前记录清零，重取
	  		{
				gsv.db1= 0;
				gsv.db2= 0;
				gsv.db3= 0;
				gsv.db4= 0;
//				gpsData.gsv = gsv;
			}

			index = (line-1)*4;	//算出卫星信号起始个数,因为每一条最多4个卫星信息
			
			for(i=0;i<4&&i<total-index; i++)
			{
				db = decstr_to_int(revbuf[7+4*i]);
				if(db!=-1)
				{
					if(db>=40)
					{
						gsv.db4+=1;
					}
					else if(db>=30)
					{
						gsv.db3+=1;
					}
					else if(db>=20)
					{
						gsv.db2+=1;
					}
					else if(db>0)
					{
						gsv.db1+=1;
					}
				}
			}
			
			
			if(line==decstr_to_int(revbuf[1]))	//收到的最后一条
			{
//				gpsData.gsv = gsv;
				
//				printf("gsv:%d,%d,%d,%d\n",gpsData.gsv.db1,gpsData.gsv.db2,gpsData.gsv.db3,gpsData.gsv.db4);
			}
			
		}
		
	}
}



//配置卫星联合工作模式
// 1GPS 2BDS 3GPS+BDS 4GLONASS 5GPS+GLONASS 6BDS+GLONASS 7GPS+BDS+GLONASS
void atgm336h_Unite(Atgm336h_Unite unite)
{
	switch(unite)
	{
	case ATGM336H_UNITE_GPS:
		tos_at_gps_send(ATGM336H_GPS);
		break;
	case ATGM336H_UNITE_BDS:
		tos_at_gps_send(ATGM336H_BDS);
		break;
	case ATGM336H_UNITE_GPS_BDS:
		tos_at_gps_send(ATGM336H_GPS_BDS);
		break;
	case ATGM336H_UNITE_GLONASS:
		tos_at_gps_send(ATGM336H_GLONASS);
		break;
	case ATGM336H_UNITE_GPS_GLONASS:
		tos_at_gps_send(ATGM336H_GPS_GLONASS);
		break;
	case ATGM336H_UNITE_BDS_GLONASS:
		tos_at_gps_send(ATGM336H_BDS_GLONASS);
		break;
	case ATGM336H_UNITE_GPS_BDS_GLONASS:
		tos_at_gps_send(ATGM336H_GPS_BDS_GLONASS);
		break;
	}
}

int gps_read(int channel_id, void *buf, size_t len)
{
    return tos_at_gps_channel_read(channel_id, buf, len);
}

__STATIC__ void atgm336h_init(void)
{
	printf("Init atgm336h ...\n" );
	tos_task_delay(1000);	
	atgm336h_Unite(ATGM336H_UNITE_BDS);
	tos_task_delay(200);	
	tos_at_gps_send(ATGM336H_FREQ);
}


__STATIC__ void gps_incoming_rmc(char * str)
{
//	printf("gp\n" );
//	printf(str);
	ParseGpsRmc(str);
}

at_event_t gps_at_event[] = {
//    { "$GPTXT,01,01,02,HW=ATGM336H",    atgm336h_init },
	{ "$BDRMC",    gps_incoming_rmc },
};

int gps_init(hal_uart_port_t uart_port)
{
	HAL_GPIO_WritePin(GPS_RST_GPIO_Port, GPS_RST_Pin, GPIO_PIN_RESET);
	tos_task_delay(200);	
	HAL_GPIO_WritePin(GPS_RST_GPIO_Port, GPS_RST_Pin, GPIO_PIN_SET);
	
	if (tos_at_gps_init(uart_port, gps_at_event,sizeof(gps_at_event) / sizeof(gps_at_event[0])) != 0) {
		return -1;
	}

	atgm336h_init();

	return 0;
}

