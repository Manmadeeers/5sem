
--1--find config files and inspect ther contents--------------------------------------------------------------


-- cat oradata/dbconfig/FREE/sqlnet.ora

-- cat oradata/dbconfig/FREE/tnsnames.ora


--2---connect via sqlplus as SYSTEM and get parameters of oracle instance-----------------------------------------------------------------

-- sqlplus
-- show parameter

--3---------------------------------------------------------------------

-- sqlplus

-- select tablespace_name from dba_tablespaces;
-- select file_name, tablespace_name from dba_data_files;
-- select role from dba_roles;
-- select username from dba_users;

--4-- exemine HKEY_LOCAL_MACHINE oralce parameters----


--na dockere ne rabotaet:(       


--5 --------------------------------------------------------

--sqlplus 

-- echo "Common_CDB_Connection =" > oradata/dbconfig/FREE/tnsnames.ora
-- echo "  (DESCRIPTION =" >> oradata/dbconfig/FREE/tnsnames.ora
-- echo "    (ADDRESS = (PROTOCOL = TCP)(HOST = localhost)(PORT = 1521))" >> oradata/dbconfig/FREE/tnsnames.ora
-- echo "    (CONNECT_DATA = (SERVICE_NAME = FREEPDB1))" >> oradata/dbconfig/FREE/tnsnames.ora
-- echo "  )" >> oradata/dbconfig/FREE/tnsnames.ora

-- cat  oradata/dbconfig/FREE/tnsnames.ora

--6----------------------------------------------------------------------
-- юзер с лабы 3

-- sqlplus

-- connect C##FIA/1111@Common_CDB_Connection;


--7----------------------------------------------------------------------

-- sqlplus
-- проверить все таблицы пользователя select * from user_tables;
-- или создать новую

-- select * from BANK;

--8---------------------------------------------------------------------

-- sqlplus

-- timing start 
-- select * from BANK;
-- timing stop;

--9----------------------------------------------------------------------

-- sqlplus 

-- DESCRIBE BANK;

--10---------------------------------------------------------------------

-- sqlplus

-- select segment_name, segment_type, tablespace_name from user_segments;

9
--11--------------------------------------------------------------------


create or replace view segment_summary as
select
    owner,
    segment_type,
    count(*) as segment_count,
    sum(extents) as total_extents,
    sum(blocks) as total_blocks,
    sum(bytes) / 1024 as total_size_kb
from
    dba_segments
group by
    owner,
    segment_type;


select * from segment_summary;