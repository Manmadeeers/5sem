ALTER PLUGGABLE DATABASE FIA_PDB OPEN;

ALTER PLUGGABLE DATABASE FIA_PDB CLOSE IMMEDIATE;

alter system set job_queue_processes = 5;

grant create job to FIA;

--1:
create table BANK(
user_id number,
name nvarchar2(50) not null, 
balance number(10,2) default 0 check(balance >=0)
);

create table RICH_BANK_USERS(
user_id number,
name nvarchar2(50) not null, 
balance number(10,2) default 0 check(balance >=0)
);


create table bank_log(
status varchar2(20), 
operation_date date,
error_code number,
error_message varchar2(500)

);


select * from rich_bank_users;
create or replace procedure GET_RICH_USERS
is
    errm varchar2(500);
    errcode number;
begin
for user_ in (select * from BANK where balance > 10000)
loop
    insert into rich_bank_users values (user_.user_id, user_.name, user_.balance);
end loop;
    delete bank where balance > 10000;
    insert into bank_log values('SUCCESS', sysdate, null, null);  --3--
exception 
    when others then
        errcode := sqlcode;
        errm := sqlerrm;
        insert into bank_log values('FAILURE', sysdate, errcode, errm);--3--
end;

declare job_number user_jobs.job%type;
begin
    dbms_job.submit(job_number, 'begin get_rich_users(); end;', sysdate, 'sysdate + 7');
    commit;

end;


--2: checks

select job, last_date, last_sec, this_date, this_sec, next_date, next_sec, interval, broken, what from user_jobs;
    

select * from bank_log;

--3:

begin
    dbms_job.run(1);
end;

begin 
    dbms_job.broken(1, true);
end;

begin 
    dbms_job.broken(1, false);
end;


begin 
    dbms_job.remove(1);
end;


begin
    dbms_job.change(
        job       => 1,
        what      =>'begin get_rich_users(); end;',
        next_date => trunc(sysdate) + 1/1440,
        interval  => 'sysdate + 1/1440'
    );
    commit;
end;
/



--4:
begin
    dbms_scheduler.disable('jb_1');
end;
/

begin
    dbms_scheduler.run_job('jb_1');
end;
/



begin
dbms_scheduler.create_schedule(
    schedule_name => 'sch_1',
    start_date => to_date('15/12/2025 18:33:00', 'DD.MM.YYYY HH24:MI:SS'),
    repeat_interval => 'FREQ=MINUTELY',
    comments => 'sch_1 minutly'
);
end;


begin
dbms_scheduler.create_program(
    program_name => 'pr_1',
    program_type => 'STORED_PROCEDURE',
    program_action => 'GET_RICH_USERS',
    number_of_arguments => 0,
    enabled => true,
    comments => 'get_rich_users_job'
);
end;


begin 
dbms_scheduler.create_job(
job_name => 'jb_1',
program_name => 'pr_1',
schedule_name => 'sch_1',
enabled => true
);
end;


select program_name, enabled
from user_scheduler_programs;


select schedule_name, repeat_interval
from user_scheduler_schedules;


select job_name, state, next_run_date
from user_scheduler_jobs;


select job_name, status, error#, run_duration
from user_scheduler_job_run_details;


select * from user_scheduler_job_run_details;









