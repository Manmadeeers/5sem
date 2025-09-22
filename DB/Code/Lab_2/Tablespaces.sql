create tablespace ts_FIA
    DATAFILE  'C:\app\Tablespaces\ts_FIA.dbf'
    SIZE 7m
    AUTOEXTEND ON NEXT 5m
    MAXSIZE 20m
    EXTENT MANAGEMENT LOCAL;
    
drop tablespace ts_FIA;
    

create temporary tablespace ts_FIA_temp
    TEMPFILE 'C:\app\Tablespaces\ts_FIA_temp.dbf'
    SIZE 5m
    AUTOEXTEND ON NEXT 5m
    MAXSIZE 30m
    EXTENT MANAGEMENT LOCAL;
    
drop tablespace ts_FIA_temp


select * from dba_tablespaces;

select * from dba_data_files;

create role RL_FIACORE

GRANT CREATE SESSION,
    CREATE TABLE,
    CREATE VIEW,
    DROP ANY TABLE,
    DROP ANY PROCEDURE ,
    DROP ANY VIEW,
    CREATE PROCEDURE to RL_FIACORE


select * from dba_roles where role like 'RL_%';

select privilege from dba_sys_privs where grantee like 'RL%';

create profile PF_FIACORE LIMIT
        PASSWORD_LIFE_TIME 180
        SESSIONS_PER_USER 3
        FAILED_LOGIN_ATTEMPTS 7
        PASSWORD_LOCK_TIME 1
        PASSWORD_REUSE_TIME 10
        PASSWORD_GRACE_TIME DEFAULT
        CONNECT_TIME 180
        IDLE_TIME 30
        
        
select * from dba_profiles where profile like 'PF_FIACORE';

select * from dba_profiles where profile='DEFAULT';


create user FIACORE identified by 12345
default tablespace TS_FIA quota unlimited on TS_FIA
temporary tablespace TS_FIA_TEMP
profile PF_FIACORE
account unlock
password expire


GRANT RL_FIACORE to FIACORE

drop user FIACORE

create tablespace FIA_QDATA
    DATAFILE  'C:\app\Tablespaces\ts_FIA_QDATA.dbf'
    SIZE 10m
    OFFLINE

alter tablespace FIA_QDATA ONLINE;

alter user FIACORE quota 2m on FIA_QDATA;


    
    