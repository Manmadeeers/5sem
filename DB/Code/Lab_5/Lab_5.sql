--1:Total SGA size--

select sum(value) from v$sga;

--2:Current main SGA pools size--

select component, current_size, max_size 
from v$sga_dynamic_components
where current_size > 0;

--3:Determine granule size for each pool-
select component, granule_size 
from v$sga_dynamic_components
where current_size>0;

--4:Determine the size of SGA free memory--

select pool, name, bytes 
from v$sgastat 
where name='free memory' 
order by pool;

--5:Determine the max goal size of SGA area--
show parameter sga;

--6:Determine the size of KEEP, RECYCLE and DEFAULT pools--
select component, current_size, min_size, max_size
from v$sga_dynamic_components
where component in ('KEEP buffer cache','DEFAULT buffer cache','RECYCLE buffer cache');

--7:Create table that will locate in KEEP pool. Show table segments

alter system set db_keep_cache_size = 100m scope=spfile;

create table TEST_TABLE_KEEP(
key int primary key,
value varchar(20) not null
) storage (buffer_pool keep);

insert into TEST_TABLE_KEEP 
values
(1,'test1'),(2,'test2'),(3,'test3');

select * from TEST_TABLE_KEEP;
commit;


select segment_name, segment_type, tablespace_name, buffer_pool
from user_segments where SEGMENT_NAME like '%TEST_TABLE_KEEP%';

select * from user_segments where segment_name = 'TEST_TABLE_KEEP'; 

drop table test_table_keep purge;

--8:Create table that will locate in DEFAULT pool. Show table segments

create table TEST_TABLE_DEFAULT(
key int primary key,
value varchar(20) not null
) storage (buffer_pool keep);

insert into TEST_TABLE_DEFAULT
values
(1,'test1'),(2,'test2'),(3,'test3');

select * from TEST_TABLE_DEFAULT;
commit;
select segment_name, segment_type, tablespace_name, buffer_pool
from user_segments where SEGMENT_NAME like '%TEST_TABLE_DEFAULT%';

select * from user_segments where segment_name = 'TEST_TABLE_DEFAULT'; 

drop table TEST_TABLE_DEFAULT purge;


--9:Determine buffer log size

show parameter log_buffer;

--10:Determine the size of free memory in large pool

select * from v$sgastat where pool='large pool' and name='free memory';

--11:Determine current instance connection types (dedicated/shared)

select username, service_name, server
from v$session
where username is not null;

--12:Get all currently working background processes

select * from v$bgprocess;

--13:Get all currently working server processes

select * from v$process where background is null;

--14:Determine the number of currently working DBWn processes

select count(*) from v$bgprocess where name like '%DBW%';

--15: Determine all services (instance connection points)

select * from v$services;

--16:Get known dispatch parameters

select * from v$dispatcher;

--17:


