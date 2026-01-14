ALTER PLUGGABLE DATABASE FIA_PDB OPEN;

ALTER PLUGGABLE DATABASE FIA_PDB CLOSE IMMEDIATE;

grant create procedure to FIA;


--1: Create a local procedure that will print a list of TEACHERS working on a pulpit passed as a parameter. Develop anon block for demo

select * from TEACHER;


declare
    procedure GET_TEACHERS(pcode in teacher.pulpit%TYPE) is
    begin
        dbms_output.put_line('Task 1');
        for rec in (select teacher_name from teacher where pulpit = pcode)
        loop
            dbms_output.put_line(rec.teacher_name);
        end loop;
    end GET_TEACHERS;
begin
    GET_TEACHERS('ИСиТ');
end;
/


--2: Develop a local function that will print the amount of teachers working on a specified pulpit
create or replace function GET_NUM_TEACHERS(pcode in teacher.pulpit%TYPE)
return number is
counter number;
cur SYS_REFCURSOR;
begin
    open cur for
    select count(*) from teacher where pulpit = pcode;
    fetch cur into counter;
    return counter;
end GET_NUM_TEACHERS;
begin
    dbms_output.put_line('Task 2');
    dbms_output.put_line('There are '||GET_NUM_TEACHERS('ИСиТ')||' teachers woeking on ИСиТ pulpit');
end;

--3:First procedure prints all teachers for a specified faculty and the second procedure prints all the subjects that are being tought on a pulpit

create or replace procedure GET_TEACHERS(FCODE FACULTY.FACULTY%TYPE)
is
begin
    for teacher in 
    (select * from teacher where pulpit in (select pulpit from faculty where faculty = FCODE))
    loop
        dbms_output.put_line(teacher.teacher_name);
    
    end loop;

end;


create or replace procedure GET_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE)
is
begin
    for subject in (select * from subject where pulpit = pcode)
    loop
        dbms_output.put_line(subject.subject);
    end loop;

end;



begin 
    GET_TEACHERS('ИДиП');
    dbms_output.put_line('---------------------------------------');
    GET_SUBJECTS('ИСиТ');
end;

--4: Develop a function that will functionaly copy the previous task

declare
function GET_NUM_TEACHERS(FCODE FACULTY.FACULTY%TYPE)
return number
is
counter number;
cCur SYS_REFCURSOR;
begin
    open cCur for 
    Select count(*) from teacher where pulpit in
    (select pulpit from faculty where faculty = FCODE);
    
    fetch cCur into counter;
    
    return counter;
end GET_NUM_TEACHERS;
    
begin
    dbms_output.put_line('Teachers for pulpit ИДиП: ' || get_num_teachers('ИДиП'));
end;


declare
function GET_NUM_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE)
return number
is
counter number;
cCur SYS_REFCURSOR;
begin
    open cCur for 
    Select count(*) from subject where pulpit = PCODE;
    
    fetch cCur into counter;
    
    return counter;
end GET_NUM_SUBJECTS;
    
begin
    dbms_output.put_line('Subjects for pulpits: ' || get_num_subjects('ИСиТ'));
end;

--5: Develop a package that will contain procedures and functions
create or replace package TEACHERS 
is
procedure GET_TEACHERS(FCODE FACULTY.FACULTY%TYPE);
procedure GET_SUBJECTS (PCODE SUBJECT.PULPIT%TYPE);
function GET_NUM_TEACHERS(FCODE FACULTY.FACULTY%TYPE) return number;
function GET_NUM_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE) return number;

end TEACHERS;

--6: Create an anonymous block to test the previous task
create or replace package body TEACHERS is
  procedure GET_TEACHERS(FCODE FACULTY.FACULTY%TYPE) is
  begin
    for teacher in (select * from TEACHER where PULPIT in (select PULPIT from PULPIT where FACULTY = FCODE))
      loop
        dbms_output.put_line(teacher.TEACHER_NAME);
      end loop;
  end;

  procedure GET_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE) is
  begin
    for subject in (select * from SUBJECT where PULPIT = PCODE)
      loop
        dbms_output.put_line(subject.SUBJECT_NAME);
      end loop;
  end;

  function GET_NUM_TEACHERS(FCODE FACULTY.FACULTY%TYPE) return number
    is
    num number;
    cCur SYS_REFCURSOR;
  begin
    open cCur for  select count(*) from TEACHER where PULPIT in (select PULPIT from PULPIT where FACULTY = FCODE);
    fetch cCur into num;
    return num;
  end;

  function GET_NUM_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE) return number
    is
    num number;
  begin
    select count(*) into num from SUBJECT where PULPIT = PCODE;
    return num;
  end;
end TEACHERS;


begin
    TEACHERS.get_teachers('ИДиП');
    dbms_output.put_line('-------------------------------------------');
    TEACHERS.get_subjects('ИСиТ');
    dbms_output.put_line('-------------------------------------------');
    dbms_output.put_line('Teachers for pulpit ИДиП: ' || TEACHERS.get_num_teachers('ИДиП'));
    dbms_output.put_line('-------------------------------------------');
    dbms_output.put_line('Subjects for faculty: ' || TEACHERS.get_num_subjects('ИСиТ'));
    
end;
















