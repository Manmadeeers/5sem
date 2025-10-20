--1: select all tablespaces files
select file_name, tablespace_name from dba_data_files
union
select file_name, tablespace_name from dba_temp_files;

--2: create a tablespace FIA_QDATA
create pluggable database FIA_PDB
ADMIN USER root identified by 1111
roles = (connect)
CREATE_FILE_DEST = '/opt/oracle/oradata/FIA';

alter pluggable database FIA_PDB open;

drop tablespace FIA_QDATA including contents and datafiles;

create tablespace FIA_QDATA
datafile 'FIA_DATA.dbf'
size 10m
autoextend on next 1m
maxsize 20m
offline


alter tablespace FIA_QDATA online;

drop user FIA;


create user FIA identified by 1234
default tablespace FIA_QDATA quota 2m on FIA_QDATA
account unlock;
grant create session, create table to FIA;

drop table FIA_T1;

create table FIA_T1
(id int primary key,
name varchar(20)
);

insert into FIA_T1 values (1,'1');
insert into FIA_T1 values (2,'2');
insert into FIA_T1 values (3,'3');


select * from FIA_T1;

--3: Get list of segments of FIA_QDATA and a segment for FIA_T1

select * from dba_segments where TABLESPACE_NAME='FIA_QDATA';
select * from user_recyclebin;

--4: Drop FIA_T1 

drop table FIA_T1;

select * from dba_segments where TABLESPACE_NAME='FIA_QDATA';
select * from user_recyclebin;

--5: Flashback previously deleted table

flashback table FIA_T1 to before drop;

select * from dba_segments where TABLESPACE_NAME='FIA_QDATA';
select * from user_recyclebin;

--6:PL/SQL script to fill table with a 1000 rows

declare i number :=4;

begin
    while i<1001 loop
        insert into FIA_T1 values
        (i,TO_CHAR(i));
        i:=i+1;
    end loop;
end;


commit;

select * from FIA_T1;


--7: Search for extents in FIA_T1 

select segment_name,extents, blocks, bytes from dba_segments
where SEGMENT_NAME like '%FIA%';

select * from dba_extents;


--8: Drop tablespace

alter tablespace FIA_QDATA offline;
drop tablespace FIA_QDATA including contents and datafiles;

--9: select all v$logs

select * from v$log;

--10: select all v$logs for instance

select * from v$logfile;

--11: full switch loop EX
alter system switch logfile;
select * from v$log;

--12: Create an additional logfile group EX

alter database add logfile group 3 'REDO3.LOG'
size 50m blocksize 512;

alter database add logfile member 'REDO31.LOG'
to group 3;

alter database add logfile member 'REDO32.LOG'
to group 3;

select * from v$log;

alter system switch logfile;


--13: Delete previously created logfile group EX


alter database drop logfile member '/opt/oracle/product/23ai/dbhomeFree/dbs/REDO31.LOG';

alter database drop logfile member '/opt/oracle/product/23ai/dbhomeFree/dbs/REDO32.LOG';

alter database drop logfile group 3;

select * from v$logfile;
select * from v$log;


--14: Determine whether logfiles are being archived or not EX

select * from v$database;

select * from v$instance;

--15: Determine the number of the last archive EX

select * from gv$archived_log;

--16: Turn archiving ON  EX

--To the console:
--SHUTDOWN IMMEDIATE;
--STARTUP MOUNT;
--ALTER DATABASE ARCHIVELOG;
--ALTER DATABASE OPEN;

select NAME, log_mode from v$database;
select instance_name,archiver, active_state from v$instance;

--17: Create an archive file manually EX

select * from v$log;

alter system switch logfile;

select * from v$archived_log;

--18: Turn archiving OFF EX

--To the console:
--SHUTDOWN IMMEDIATE;
--STARTUP MOUNT;
--ALTER DATABASE NOARCHIVELOG;
--ALTER DATABASE OPEN;

select * from v$database;
select * from v$instance;

--19: Select all controlfiles 

select * from v$controlfile;

--20: Get and inspect the contents of a controlfile 

alter database backup controlfile to trace;

select value from v$diag_info where name='Default Trace File';

show parameter control;

select * from v$controlfile_record_section;

select * from v$parameter;

--21: Determine the location of instance parameter file 

show parameter spfile;

--22: Create PFILE 

create pfile='FIA_OPEN.ora' from spfile;

--23: Determine the location of instance password file

show parameter remote_login_passwordfile;


--24: Get list of directories for logfiles and diagnostics

select * from v$diag_info;

--25: Find and inspect instance work protocol (LOG.XML) EX

--To the console:
--SHOW PARAMETER log_archive_dest
--purge RECYCLEBIN;






