select * from dba_pdbs;


--------------------get all pdbs-----------------------------------------

select * from v$instance;

-----------------------get instances list---------------------------------------

select *  from dba_registry;

------------------------get installed components list--------------------------------------

create pluggable database FIA_PDB 
    ADMIN USER root identified by 1111  
    roles=(CONNECT)
    CREATE_FILE_DEST='/opt/oracle/oradata/FIA/';
    
    
    alter pluggable database FIA_PDB open;
    
    
------------------create a new PDB --------------------------------------------

select * from dba_pdbs;

------------------------Get all pdbs --------------------------------------

drop tablespace ts_FIA including contents and datafiles;

select * from dba_data_files;


create tablespace ts_FIA
datafile 'ts_FIA.dbf'
size 10m
autoextend on next 5m
maxsize 20m;

create temporary tablespace TS_FIA_TEMP
tempfile 'TS_FIA_TEMP.dbf'
size 5m
autoextend on next 3m
maxsize 30m;

alter tablespace TS_FIA_TEMP
drop datafile '/opt/oracle/oradata/FIA/TS_FIA_TEMP.dbf';
SELECT name, bytes FROM v$tempfile;



create profile PF_FIACORE limit
PASSWORD_LIFE_TIME 180
SESSIONS_PER_USER 3
FAILED_LOGIN_ATTEMPTS 7
PASSWORD_LOCK_TIME 1
PASSWORD_REUSE_TIME 10
password_grace_time default
connect_time 180
idle_time 30;

create role RL_FIACORE;

grant 
    create session,
    create table,
    create view,
    create procedure,
    drop any table,
    drop any view,
    drop any procedure
    to RL_FIACORE;
    
    
    create user U1_FIA_PDB identified by 12345
    default tablespace ts_FIA quota unlimited on ts_FIA
    temporary tablespace TS_FIA_TEMP
    account unlock;
    
    
    grant RL_FIACORE to U1_FIA_PDB;
--------connect to FIA_PDB using sqldev, create tablespaces , roles, profiles, tables ---------------------------------------------------

create table FIA_table
(
username varchar2(20),
balance int
);


insert into FIA_table 
values ('John', 1000);
insert into FIA_table
values ('Nick', 200);
insert into FIA_table
values ('Ian', 400);

commit;

select * from FIA_table;


-------connect to user U1 using sqldev and creare a FIA_table, add lines into it----------------------------------------------------

select * from dba_tablespaces;
select file_name, tablespace_name from dba_data_files
union 
select file_name, tablespace_name from dba_temp_files;

select * from dba_roles;
select * from dba_role_privs;

select * from dba_profiles;


select USERNAME, GRANTED_ROLE from dba_users left outer join dba_role_privs on USERNAME=GRANTEE;


--get all tablespaces,all files(premanent and temporary), all roles(and granted priviliges),security profiles, all users for FIA_PDB and asigned roles---------------------------------------------------------


create user c##FIA identified by 1111;

grant create session to c##FIA;
grant create session to c##FIA container=all

grant SYSDBA to c##FIA;

drop user c##FIA;

--connect to CDB and create a common user, grant it create session, create 2 connections in SQL dev for C##FIA to CDB and PDB---------------------------------------------------------

alter pluggable DATABASE FIA_PDB close;
drop pluggable database FIA_PDB including datafiles; 

select file_name, tablespace_name from dba_data_files
union 
select file_name, tablespace_name from dba_temp_files;

--delete created PDB, check if all data are deleted, delete C## user, delete all PDB connections
