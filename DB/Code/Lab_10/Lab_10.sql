ALTER PLUGGABLE DATABASE FIA_PDB OPEN;

ALTER PLUGGABLE DATABASE FIA_PDB CLOSE IMMEDIATE;

--1:Adding BDAY and SALARY columns to TEACHERS table

ALTER TABLE TEACHER
ADD (BIRTHDAY date, SALARY number);

UPDATE TEACHER
SET SALARY= FLOOR(dbms_random.value(2000, 4000)),
BITHDAT = trunc(sysdate) - floor(dbms_random.value(365*23, 365*50));


SELECT * FROM TEACHER;

ALTER TABLE TEACHER DROP COLUMN SALARY;
ALTER TABLE TEACHER DROP COLUMN BIRTHDAY;


--2:Select all teachers like Surename N.P

SELECT SUBSTR(TEACHER_NAME, 0, instr(TEACHER_NAME,' ')+1)||'.'
|| SUBSTR(TEACHER_NAME, instr(TEACHER_NAME, ' ',1,2)+1,1)||'.' as FIO
FROM TEACHER;

--3: Select all teachcers born on monday


SELECT * from TEACHER WHERE to_char(BIRTHDAY,'D') = 1;


--4: Create a view that will print all teachers that have birthday n the next month

GRANT CREATE ANY VIEW, DROP ANY VIEW to FIA;

select * from dba_users;


CREATE OR REPLACE VIEW next_month_bday
AS
SELECT * FROM TEACHER
WHERE to_char(BIRTHDAY,'MM') = mod(to_char(sysdate,'MM')+1,12);


SELECT * from next_month_bday;



--5: View printing teachers for each month
GRANT CREATE VIEW TO FIA;

create or replace view month_amount
as 
select to_char(birthday, 'MONTH') as month, count(*) as amount
from teacher group by to_char(birthday, 'MONTH');

select * from month_amount;


--6: Create a cursor and print all teacher that will celebrate a jubily within the next year


declare
    cTeach  SYS_REFCURSOR;
     vTeacher_name teacher.teacher_name%type;
    vBirthday     teacher.birthday%type;
begin
    open cTeach for 
    select teacher_name, birthday  from teacher
    where mod(to_char(sysdate, 'YYYY') + 1 - (to_char(birthday, 'YYYY')), 5) = 0;
    
    loop
        fetch cTeach into vTeacher_name, vBirthday;
        exit when cTeach%notfound;

        dbms_output.put_line(vTeacher_name || ' ' || vBirthday);
    end loop;
    

end;
/


--7: Create a cursor and print all pulpits average salary flooring them down

declare 
cSalary SYS_REFCURSOR;
aFaculty pulpit.pulpit%type;
aSalary teacher.salary%type;
aPulpit pulpit.pulpit%type;

begin
open cSalary for
select faculty.faculty, pulpit.pulpit, floor(avg(salary)) 
from teacher inner join pulpit on pulpit.pulpit = teacher.pulpit
inner join faculty on faculty.faculty = pulpit.faculty
group by rollup( faculty.faculty, pulpit.pulpit);


loop 
    fetch cSalary into aFaculty,aPulpit, aSalary;
    exit when cSalary%notfound;
    dbms_output.put_line('Средняя ' || trim(aFaculty) || ' ' || trim(aPulpit) || ': ' || aSalary);

    
end loop;

end;
/



--8: Create a PL/SQL record type and demonstrate it's usage. Demonstrate nested operations

declare
    type tTeacher is record
    (
        full_name       teacher.teacher_name%type,
        birth_date      teacher.birthday%type,
        salary_amount   teacher.salary%type,
        random_number   number
    );

    type tTeacherOuter is record
    (
        teacher_data    tTeacher,
        extra_text      varchar2(50)
    );

    teacher1     tTeacher;
    teacher2     tTeacher;
    teacherOuter tTeacherOuter;

begin
    teacher1.full_name     := 'fullname var1';
    teacher1.birth_date    := sysdate;
    teacher1.salary_amount := 1356;
    teacher1.random_number := 6.2;

    dbms_output.put_line('teacher1: '||
        teacher1.full_name||' '||
        teacher1.salary_amount||' '||
        teacher1.random_number);

    teacher2 := teacher1;

    dbms_output.put_line('teacher2: '||
        teacher2.full_name||' '||
        teacher2.salary_amount||' '||
        teacher2.random_number);

    teacherOuter.teacher_data := teacher2;
    teacherOuter.extra_text   := '13:56';

    dbms_output.put_line('Outer salary: '||
        teacherOuter.teacher_data.salary_amount);

end;
/











