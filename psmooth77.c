#include <fcntl.h> 
#include "mpi.h"
#include <stdio.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <math.h>

int main (int argc, char *argv[]) 
{ 
int myid, numprocs;
    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
int numtasks, rank, source; 
source =0;
long     h,// height
	w;//width

size_t bytes_read;
long i,j,k,z,q;// use for loop
double startwtime = 0.0, endwtime, startcommtime = 0.0, endcommtime,startcomptime = 0.0, endcomptime, totalcommtime = 0.0, totalcomptime = 0.0;

int fd,fw;
MPI_Init(&argc,&argv); 
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks); 
MPI_Comm_rank(MPI_COMM_WORLD,&myid);
MPI_Get_processor_name(processor_name,&namelen);

fprintf(stderr,"Process %d on %s\n",
	    myid, processor_name);
	    
	    
char ftype[2]; // BM type of file
unsigned char width[4];  // width
unsigned char height[4];//height

if(rank==0)
{
//open file 
fd = open ("2.bmp", O_RDONLY); // open input file for read

/* an erroe occured. print an error message and bail. */
if(fd == -1){
perror("open");
return 1;
}

//-----------------------------------------------------------------------

//open file 
fw = open ("out.bmp", O_RDWR); // open output file for write

/* an erroe occured. print an error message and bail. */
if(fw == -1){
perror("open");
return 1;
}

//-----------------------------------------------------------------------

// check type of file
bytes_read = read (fd, ftype, sizeof (ftype));// read signature of bitmap file 

printf ( "\n \n %c %c " ,ftype[0] ,ftype[1]);
// test, it's correct or no

if (ftype[0]== 66 || ftype[1]== 77 )
{
printf("type is correct \n ");
}
else
{
printf("type is not correct \n ");
return 1;
}

//-------------------------------------------------------------------------

// read width & height
lseek(fd,18,SEEK_SET);// jump width position
bytes_read = read (fd, width, sizeof (width));// read width
bytes_read = read (fd, height, sizeof (height)); // read height
	
//caculate value of width and height
h = (height[3]*(256*256*256))+(height[2]*(256*256))+(height[1]*256)+height[0];
w = (width[3]*(256*256*256))+(width[2]*(256*256))+(width[1]*256)+width[0];;

printf(" height = %d \n",h);//print value of height
printf(" width = %d \n",w);//print value of width

/* finish read */
//-----------------------------------------------------------------------------------



//---------------------------------------------------------------------------
/*
//print in order
unsigned char pixel_value[w];
if(rank==0){
printf ( "\n " );
for (j=0;j<h;j++){
lseek(fd,-((j+1)*w+2),SEEK_END);
bytes_read = read (fd, pixel_value, sizeof (pixel_value));

for (i=0;i<w;i++)
{
printf ( "%03d " , pixel_value[i]);
} 
printf ( " %03d  \n " , (j+1)); 
}
printf ( " \n\n ");
/*finished read data*/
//}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//bcast w,h of image

MPI_Bcast (&w,1,MPI_LONG,source,MPI_COMM_WORLD);
MPI_Bcast (&h,1,MPI_LONG,source,MPI_COMM_WORLD);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

unsigned char  window [25],//for window buffer
		x,sum[25],
    	       result[w];//result of filter
	       
long gap;//size of each process
gap = (h-4)/numtasks;

// initial for starting filter
source = 0; 
unsigned char array1[w],array2[w],readbuff[w];
unsigned char array3[w],array4[w],array5[w];
unsigned char send_recvbuff[numtasks][w];

//start time
if(rank==0)
{
startwtime = MPI_Wtime();
}

for(j=0;j<(gap+4);j++)//repeat untill of each process size finish
{
if(rank==0){
for (i=0;i<numtasks;i++)
{
//jumping to be ready for each process
lseek(fd,-(((gap*w)*i)+((j+1)*w)+2),SEEK_END);
bytes_read = read (fd, readbuff, sizeof (readbuff));//read

for (z=0;z<w;z++)//ready all data for sending
{
	send_recvbuff[i][z]=readbuff[z];
//	printf("%03d ",readbuff[z]);
}
	//printf("\n");
}
}


if(rank==0)
{
startcommtime = MPI_Wtime();
}

//send to all process
MPI_Scatter(send_recvbuff,w,MPI_CHAR,readbuff,w, MPI_CHAR,source,MPI_COMM_WORLD); 
if(rank==0)
{
endcommtime = MPI_Wtime();
}
totalcommtime = totalcommtime+(endcommtime - startcommtime);
startcommtime = 0.0;
endcommtime = 0.0;
/*
printf("rank = %d \n",rank);

for (z=0;z<w;z++)
{
printf(" %03d ",readbuff[z]);
}
printf("\n");
*/

//clear window
for (z=0;z<9;z++)
window[z]=0;

if(rank==0)
{
startcomptime = MPI_Wtime();
}


for (z=0;z<w;z++)
{// match data for window process 
array1[z]=array2[z];
array2[z]=array3[z];
array3[z]=array4[z];
array4[z]=array5[z];
array5[z]=readbuff[z];
}

if (j>1)//3 row of data to be complete for starting process
{

for(z=0;z<w;z++)
{
window [0]=window[1] ; 
window [5]=window[6] ;
window [10]=window[11] ;
window [15]=window[16] ;
window [20]=window[21] ;

window [1]=window[2] ; 
window [6]=window[7] ;
window [11]=window[12] ;
window [16]=window[17] ;
window [21]=window[22] ;

window [2]=window[3] ; 
window [7]=window[8] ;
window [12]=window[13] ;
window [17]=window[18] ;
window [22]=window[23] ;

window [3]=window[4] ; 
window [8]=window[9] ;
window [13]=window[14] ;
window [19]=window[19] ;
window [23]=window[24] ;

window [4]=array1[z] ; 
window [9]=array2[z] ;
window [14]=array3[z] ;
window [19]=array4[z] ;
window [24]=array5[z] ;


result[z] = (window[0]+ window[1]+ window[2]+ window[3]+ window[4]+ window[5]+ window[6]       + window[7]+ window[8]+ window[9]+ window[10]+ window[11]+ window[12]+ window[13]         + window[14]+ window[15]+ window[16]+ window[17]+ window[18]+ window[19]+ window[20]      + window[21]+ window[22]+ window[23]+ window[24]+ window[25]+ window[26]+ window[27]
     + window[28]+ window[29]+ window[30]+ window[31]+ window[32]+ window[33]+ window[34] + window[35]+ window[36]+ window[37]+ window[38]+ window[39]+ window[40]+ window[41] + window[42]+ window[43]+ window[44]+ window[45]+ window[46]+ window[47]+ window[48] 
                   )/49 ;

//calculate sum of window buffer
} 
if(rank==0)
{
endcomptime = MPI_Wtime();
}
totalcomptime = totalcomptime+(endcomptime - startcomptime);
startcomptime = 0.0;
endcomptime = 0.0;

if(rank==0)
{
startcommtime = MPI_Wtime();
}
MPI_Gather(result,w,MPI_CHAR,send_recvbuff,w, MPI_CHAR,source,MPI_COMM_WORLD); 

if(rank==0)
{
endcommtime = MPI_Wtime();
}
totalcommtime = totalcommtime+(endcommtime - startcommtime);
startcommtime = 0.0;
endcommtime = 0.0;


if(rank==0){
for (i=0;i<numtasks;i++)
{
for (z=0;z<w;z++)
{
	readbuff[z] = send_recvbuff[i][z];
//	printf("%03d ",readbuff[z]);
}

lseek(fw,-(((gap*w)*i)+(j*w)+2),SEEK_END);
write (fw, readbuff, sizeof (readbuff));

	//printf("\n");
}
}
}
}


if(rank==0)
{
printf("\n \n");


long rempixl;
rempixl = h-((gap*numtasks));

for (i=0;i<rempixl;i++)
{

lseek(fd,-(((gap*w)*numtasks)+((i+1)*w)+2),SEEK_END);
bytes_read = read (fd, readbuff, sizeof (readbuff));

for (z=0;z<9;z++)
window[z]=0;

q=0;

if(rank==0)
{
startcomptime = MPI_Wtime();
}
for (z=0;z<w;z++)
{
array1[z]=array2[z];
array2[z]=array3[z];
array3[z]=array4[z];
array4[z]=array5[z];
array5[z]=readbuff[z];
}

if (i>1)
{
window [0]=window[1] ; 
window [5]=window[6] ;
window [10]=window[11] ;
window [15]=window[16] ;
window [20]=window[21] ;

window [1]=window[2] ; 
window [6]=window[7] ;
window [11]=window[12] ;
window [16]=window[17] ;
window [21]=window[22] ;

window [2]=window[3] ; 
window [7]=window[8] ;
window [12]=window[13] ;
window [17]=window[18] ;
window [22]=window[23] ;

window [3]=window[4] ; 
window [8]=window[9] ;
window [13]=window[14] ;
window [19]=window[19] ;
window [23]=window[24] ;

window [4]=array1[z] ; 
window [9]=array2[z] ;
window [14]=array3[z] ;
window [19]=array4[z] ;
window [24]=array5[z] ;


result[z] = (window[0]+ window[1]+ window[2]+ window[3]+ window[4]+ window[5]+ window[6]       + window[7]+ window[8]+ window[9]+ window[10]+ window[11]+ window[12]+ window[13]         + window[14]+ window[15]+ window[16]+ window[17]+ window[18]+ window[19]+ window[20]      + window[21]+ window[22]+ window[23]+ window[24]+ window[25]+ window[26]+ window[27]
     + window[28]+ window[29]+ window[30]+ window[31]+ window[32]+ window[33]+ window[34] + window[35]+ window[36]+ window[37]+ window[38]+ window[39]+ window[40]+ window[41] + window[42]+ window[43]+ window[44]+ window[45]+ window[46]+ window[47]+ window[48] 
                   )/49 ;

	// caculate filter value for this pixel
	



}
if(rank==0)
{
endcomptime = MPI_Wtime();
}
totalcomptime = totalcomptime+(endcomptime - startcomptime);
startcomptime = 0.0;
endcomptime = 0.0;

lseek(fw,-(((gap*w)*numtasks)+(i*w)+2),SEEK_END);
write (fw, result, sizeof (readbuff));
/*
for (z=0;z<w;z++)
{
	
	printf("%03d ",result[z]);
}

printf("\n ");
*/
}



//finish time caculate

endwtime = MPI_Wtime();
printf("\n \n wall clock time = %f \n\n",(endwtime-startwtime));
printf("\n \n computation clock time = %f \n\n",totalcomptime);
printf("\n \n communication clock time = %f \n\n",totalcommtime);

// finish filter




//close files
close (fd); 
close (fw);
}

MPI_Finalize();
return 0; 
}
