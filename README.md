# distributed-algorithms-for-noise-reduction-on-large-scale-picture
This repository contains the code to perform filters to the image and implemented by C and MPICH. It is implemented based on SIMD on a shared memory system.

مراحل نصب MPICH در محیط Linux

•	کپی کردن فايل mpich-1.2.7 در دایرکتوری مورد نظر (مثلا /home ).
 gunzip –c mpich.tar.gz 
 tar –xvf mpich.tar
•	در دایرکتوری mpich:

 ./configure –prefix=/usr/local/mpich-1.2.7 | tee c.log
•	اضافه کردن دستور زیر در فایل .bashrc 
export  PATH=/home/mpich-1.2.7/bin:$PATH
•	نصب برنامه
 make
 make install
•	بررسی وضعیت نصب rsh
 rpm –q rsh rsh-server
	Rsh-0.17-25.3
	Rsh-server-0.17.25.3
•	فعال کردن سرویس rsh
 su –
 chkconfig rsh on
 chkconfig rlogin on
 xinetd –restart
•	اصلاح فایل /etc/hosts به صورت زیر
192.168.60.1  station0
192.168.60.2  station1
192.168.60.3  station2
…..

•	اصلاح فایل /etc/hosts.equiv به صورت زیر

Station1
Station2
….
•	ایجاد فایل /user/.rhosts.txt و تنظیمات آن

touch /user/.rhosts.txt
chmod 600 /user/.rhosts.txt

•	اصلاح فایل /user/.rhosts.txt به صورت زیر
Station0
Station1
Station2
….

•	بررسی عملکرد rsh: با rsh از هر نود به همه نودها متصل می شویم تا از عملکرد آن اطمینان حاصل شود.(البته باید سیستم ها دارای user name و password های یکسان برای اجرای برنامه موازی باشند.)

rsh station0

برای اجرای یک برنامه ابتدا باید آنرا به کمک کمپایلر mpicc کامپایل کرده و فایل a.out را ایجاد می کنیم. این فایل باید در تمامی کامپیوترها در یک مسیر مشخص و ثابت کپی شود سپس با استفاده از دستور mpirun که در کامپیوتر اصلی اجرا می گردد برنامه به صورت همزمان بر روی کلیه کامپیوترها اجرا می گردد.

Mpicc pmedian.c

Mpirun –np 4 pmedian.out

Pmedian.c نام برنامه موازی شده و عدد 4 تعدد پردازنده های مورد استفاده است.

