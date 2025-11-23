select * from dba_users;

--Using FIA_PDB and a user named FIA


alter pluggable database FIA_PDB open;

--1: Granting privilidges to a user

grant create session, create any table,
drop any table, create cluster, 
create synonym, create public SYNONYM,
create view, create materialized view,
create SEQUENCE to FIA;

select * from dba_sys_privs where grantee = 'FIA';

--2:create S1 sequence with 1000 start, increment by 10 and so on

create sequence S1 start with 1000 increment by 10
nominvalue nomaxvalue nocycle nocache noorder;

select S1.nextval from dual;
select S1.nextval from dual;

select S1.currval from dual;

drop sequence S1;


--3: create S2 sequence with 10 start, increment 10 and so on

create sequence S2 start with 10 increment by 10
maxvalue 100 nocycle ;

select S2.nextval from dual;
drop sequence S2;


--4:create S3 sequence with 10 start, decrement 10 and so on

create sequence S3 start with 10 increment by -10
minvalue -100 maxvalue 100 nocycle order;

select S3.nextval from dual;
drop sequence S3;

--5: create S4 sequence with 1 start, increment by 1 and so on

create sequence S4 start with 1 increment by 1
maxvalue 10 cycle cache 5 noorder;

select S4.nextval from dual;
drop sequence S4;

--6: select all sequences for current user

select * from user_sequences;


--7: create a table, locate in KEEP and form using sequences

create table T1(
N1 number(20),
N2 number(20),
N3 number(20),
N4 number(20)
) cache storage (buffer_pool keep);

drop table T1;

insert into T1 values(S1.nextval, S2.nextval, S3.nextval,S4.nextval);

select * from T1;

--8: create a cluster ABC that has a hash-type 200 

create cluster ABC(
X number(10),
V varchar(12)
)hashkeys 200;

drop cluster ABC;


--9: create a table A that belongs to ABC cluster

create table A(
XA number(10),
VA varchar(12),
YA number(10)
) cluster ABC(XA,VA);

drop table A;

--10: create a table B that belongs to ABC cluster

create table B(
XB number(10),
VB varchar(12),
YB number(10)
) cluster ABC(XB,VB);

drop table B;

--11: create table C that belongs to ABC cluster

create table C(
XC number(10),
VC varchar(12),
YC number(10)
) cluster ABC(XC,VC);

drop table C;

--12: find previously created cluster and tables

select * from user_clusters;
select * from user_tables;


--13: create a private synonym for C table

create synonym SC for FIA.C;
select * from SC;
drop synonym SC;

--14: create a public synonym for table B

create public synonym SB for FIA.B;
select * from SB;

--15: create A and B tables with a view

create table A1(
XA number(10) primary key,
VA varchar(12),
YA number(10)
);

create table B1(
XB number(10) ,
VB varchar(12),
YB number(10),
constraint fk_b1 foreign key (XB) references A1(XA)
);

insert into A1 values(1,'1',1);
insert into A1 values(2,'2',2);
insert into A1 values(3,'3',3);
insert into A1 values(4,'4',4);

insert into B1 values(1,'1',1);
insert into B1 values(4,'4',4);

create view V1 as
Select * from A1 inner join B1 on XA = XB;

select * from V1;


--16: create a materialized view

create materialized view MV
refresh force on demand 
next sysdate + 2/1440
as
Select * from A1 inner join B1 on XA = XB;

select * from MV;
