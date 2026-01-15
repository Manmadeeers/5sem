ALTER PLUGGABLE DATABASE FIA_PDB OPEN;

ALTER PLUGGABLE DATABASE FIA_PDB CLOSE IMMEDIATE;

grant create tablespace to FIA;
grant alter tablespace to FIA;
grant drop tablespace  to FIA;

create tablespace TS1
    datafile 'TS1_FIA.dbf'
    size 7 m
    autoextend on
    maxsize unlimited
    extent management local;

create tablespace TS2
    datafile 'TS2_FIA.dbf'
    size 7 m
    autoextend on
    maxsize unlimited
    extent management local;
    

create tablespace TS3
    datafile 'TS3_FIA.dbf'
    size 7 m
    autoextend on
    maxsize unlimited
    extent management local;
    

create tablespace TS4
    datafile 'TS4_FIA.dbf'
    size 7 m
    autoextend on
    maxsize unlimited
    extent management local;   
    
    
   
alter user FIA quota unlimited on TS1;
alter user FIA quota unlimited on TS2;
alter user FIA quota unlimited on TS3;
alter user FIA quota unlimited on TS4;

drop tablespace TS1 including contents and datafiles;
drop tablespace TS2 including contents and datafiles;
drop tablespace TS3 including contents and datafiles;
drop tablespace TS4 including contents and datafiles;

--1: create T_RANGE table with range sectioning. Use NUMBER sectioning key

create table T_RANGE
(
    id  number,
    TIME_ID date
)
partition by range (id) 
(
    partition p0 values less than (100) tablespace TS1,
    partition p1 values less than (200) tablespace TS2,
    partition PMAX values less than (maxvalue) tablespace TS4
);

drop table T_RANGE purge;

--2: create T_INTERVAL table with insterval sectioning. Use DATE sectioning key

create table T_INTERVAL
(
    id      number,
    TIME_ID date
)
tablespace TS1
partition by range (TIME_ID)
interval (numtoyminterval(1,'MONTH'))
(
    partition p1 values less than (date '2024-01-01')
);

drop table T_INTERVAL purge;
    
    
    
--3: create T_HASH table with hash sectioning. Use VARCHAR2 sectioning key

create table T_HASH
(
    code varchar2(10),
    data varchar2(20)
)
tablespace TS2
partition by hash (code)
partitions 4;

drop table T_HASH purge;

--4: create T_LIST table with list sectioning. Use CHAR sectioning key

create table T_LIST
(
    status char(1),
    descr  varchar2(20)
)
partition by list (status)
(
    partition p_a values ('A') tablespace TS1,
    partition p_b values ('B') tablespace TS2,
    partition p_other values (default) tablespace TS4
);

drop table T_LIST purge;

--5: Insert data(they should be located in all section) in previously created tables and demonstrate sections allocation using SELECT

insert into T_RANGE values (1, sysdate);
insert into T_RANGE values (101, sysdate);
insert into T_RANGE values (201, sysdate);


insert into T_INTERVAL values (1, '01-12-2023');
insert into T_INTERVAL values (2, '01-02-2024');
insert into T_INTERVAL values (3, '01-03-2025');



insert into T_HASH values ('A1', 'one');
insert into T_HASH values ('B2', 'two');
insert into T_HASH values ('C3', 'three');



insert into T_LIST values ('A', 'alpha');
insert into T_LIST values ('B', 'beta');
insert into T_LIST values ('C', 'other');

commit;

select table_name, partition_name, num_rows, high_value
from user_tab_partitions
where table_name like 'T_%'
order by table_name;


select * from T_RANGE partition(p1);

--6: Demonstrate row-to-row data transition using UPDATE operator

alter table T_RANGE enable row movement;
alter table T_INTERVAL enable row movement;
alter table T_HASH enable row movement;
alter table T_LIST enable row movement;

update T_RANGE set id = 250 where id = 1;
update T_INTERVAL set TIME_ID = '02-06-2024' where TIME_ID = '01-12-2023';
update T_HASH set code = 'Z9' where code = 'A1';
update T_LIST set status = 'A' where descr ='other';
commit;

--we can look at transmitted rows in a partition
select *
from T_RANGE partition (p0);

-- or we can look at statistics for all partitions
-- if the statistic would not update - run exec
exec dbms_stats.gather_table_stats(user, 'T_INTERVAL');
exec dbms_stats.gather_table_stats(user, 'T_RANGE');
exec dbms_stats.gather_table_stats(user, 'T_HASH');
exec dbms_stats.gather_table_stats(user, 'T_LIST');

select table_name, partition_name, num_rows, high_value
from user_tab_partitions
where table_name like 'T_%'
order by table_name;


--7: Demonstrate usage of ALTER TABLE MERGE for one of previously created tables

alter table T_RANGE
merge partitions p0, p1 into partition p1
tablespace TS1;

select partition_name, num_rows, high_value
from user_tab_partitions
where table_name = 'T_RANGE'
order by partition_position;

--8:Demonstrate usage of ALTER TABLE SPLIT for one of previously created tables
alter table T_RANGE
split partition p1
at (150)
into
(
    partition p0,
    partition p1
);

select partition_name, num_rows, high_value
from user_tab_partitions
where table_name = 'T_RANGE'
order by partition_position;


--9: Demonstrate usage of ALTER TABLE EXCHANGE for one of previously created tables

drop table T_EXCHANGE purge;

create table T_EXCHANGE
(
    id number,
    TIME_ID date
);


alter table T_RANGE
exchange partition p0
with table T_EXCHANGE
without validation;

select * from T_EXCHANGE;
delete T_EXCHANGE;


select * from T_RANGE partition (p0);


    
    
    
    
    
    
    