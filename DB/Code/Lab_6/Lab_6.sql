--1-----------------------------------------------------------------------

-- sqlplus

-- cat oradata/dbconfig/FREE/sqlnet.ora

-- cat oradata/dbconfig/FREE/tnsnames.ora


--2----------------------------------------------------------------------

-- sqlplus
-- show parameter

--3---------------------------------------------------------------------

-- sqlplus

-- select tablespace_name from dba_tablespaces;
-- select file_name, tablespace_name from dba_data_files;
-- select role from dba_roles;
-- select username from dba_users;


--5-----------------------------------------------------------------------

--sqlplus 

-- echo "C##KEO_CONECTION =" > oradata/dbconfig/FREE/tnsnames.ora
-- echo "  (DESCRIPTION =" >> oradata/dbconfig/FREE/tnsnames.ora
-- echo "    (ADDRESS = (PROTOCOL = TCP)(HOST = localhost)(PORT = 1521))" >> oradata/dbconfig/FREE/tnsnames.ora
-- echo "    (CONNECT_DATA = (SERVICE_NAME = FREEPDB1))" >> oradata/dbconfig/FREE/tnsnames.ora
-- echo "  )" >> oradata/dbconfig/FREE/tnsnames.ora

-- cat  oradata/dbconfig/FREE/tnsnames.ora

--6----------------------------------------------------------------------
-- юзер с лабы 3

-- sqlplus

-- connect C##KEO/1111@C##KEO_CONECTION;


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