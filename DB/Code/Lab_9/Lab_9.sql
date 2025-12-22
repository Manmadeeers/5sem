--needed to set the user and do some additional creations
alter pluggable database FIA_PDB close immediate;
alter pluggable database FIA_PDB open;
SELECT username, account_status, authentication_type
FROM dba_users
WHERE username = 'FIA';

select * from dba_tablespaces;

--1: select operator with percise choise

declare
    auditorium_types auditorium_type%rowtype;
    begin
        select *into auditorium_types from auditorium_type where auditorium_types = 'ЛК';
    end;
/

select * from auditorium_type where auditorium_type='ЛК';

--2: select operator with not percise choise using WHEN OTHERS and exception handling

declare
    auditorium_types auditorium_type%rowtype;
    begin
    dbms_output.put_line('Task 2');
    select *  into auditorium_types from auditorium_type where auditorium_type like '%ЛБ%';
    
    exception
        when others 
        then dbms_output.put_line('error = '||sqlcode||' message = '||sqlerrm);
    
    end;
/

--3: demonstrate WHEN_TO_MANY_ROWS

declare
    auditorium_types auditorium_type%rowtype;
    begin
    dbms_output.put_line('Task 3');
    select *  into auditorium_types from auditorium_type where auditorium_type like '%ЛБ%';
    
    exception
        when TOO_MANY_ROWS
        then dbms_output.put_line('error = '||sqlcode||' message = too many rows');
    
        when others
        then dbms_output.put_line('error = '||sqlcode||' message = '||sqlerrm);
    
    end;
/


--4: Handle NO_DATA_FOUND exception


declare
    auditorium_types auditorium_type%rowtype;
    begin
    dbms_output.put_line('Task 4');
    select *  into auditorium_types from auditorium_type where auditorium_type like '%ЛД%';
    
    exception
        when NO_DATA_FOUND
        then dbms_output.put_line('error = '||sqlcode||' message = no data found');
    
        when others
        then dbms_output.put_line('error = '||sqlcode||' message = '||sqlerrm);
    
    end;
/

--5: UPDATE operator usage with COMMIT/ROLLBACK

select * from auditorium;

update auditorium set auditorium_capacity = 150 where auditorium='408-2';

declare 
    new_capacity auditorium.auditorium_capacity%type;
    old_capacity auditorium.auditorium_capacity%type;
    begin
    select auditorium_capacity into old_capacity
     from auditorium where auditorium = '408-2';
    
    dbms_output.put_line('Старая вместимость = '||old_capacity);
    
    
    update auditorium set auditorium_capacity = 90 where auditorium='408-2';
    
     select auditorium_capacity into new_capacity
      from auditorium where auditorium = '408-2'; 
     
     dbms_output.put_line('Новая вместимость = '||new_capacity);
     
    commit;
    
    select auditorium_capacity into new_capacity
     from auditorium where auditorium = '408-2';
    dbms_output.put_line('Новая вместимость после commit = '||new_capacity);
     
     
     update auditorium set auditorium_capacity = 200 where auditorium='408-2';
    
     select auditorium_capacity into new_capacity
      from auditorium where auditorium = '408-2'; 
     
     dbms_output.put_line('Новая новая вместимость = '||new_capacity);
    
    rollback;
     select auditorium_capacity into new_capacity
      from auditorium where auditorium = '408-2'; 
     
     dbms_output.put_line('прошлая новая вместимость = '||new_capacity);
    
    exception
        when others then
        dbms_output.put_line('error = '||sqlerrm);   
        rollback;
    end;
/


--7: INSERT operator with COMMIT/ROLLBACK

delete from faculty where faculty = 'ФИТ';

select * from faculty;

declare
    faculty_short faculty.faculty%type;
    faculty_full_name faculty.faculty_name%type; 
    begin
    insert into faculty(faculty, faculty_name) values ('ФИТ', 'Факультет информационных технологий')
    returning faculty, faculty_name into faculty_short, faculty_full_name;
    
    dbms_output.put_line('faculty: '||faculty_short||'faculty_name: '||faculty_full_name);

    
    commit;
    
    insert into faculty(faculty, faculty_name) values ('ФИБ', 'Факультет информационной безопасности')
    returning faculty, faculty_name into faculty_short, faculty_full_name;
     dbms_output.put_line('faculty: '||faculty_short||'faculty_name: '||faculty_full_name);
    rollback;
    exception
        when others then 
            dbms_output.put_line('error = '|| sqlerrm);
            rollback;
    end;
/
    select * from faculty;

--8: INSERT operator corrupting data

select * from auditorium; 
declare 
    begin
        insert into auditorium values('200-3a', '200-3a', 'ЛК' ,'300');
    
    exception
        when others then 
            dbms_output.put_line('error = '|| sqlerrm);
            rollback;
    end;
    /
    
--9: DELETE operator with COMMIT/ROLLBACK

select count(*) from auditorium;

declare 
    old_count number;
    new_count number;
    begin
        select count(*) into old_count from auditorium;
        dbms_output.put_line('Строк до удаления: '||old_count);
        delete from auditorium where auditorium.auditorium_type = 'ЛБ-К';
        commit;
         select count(*) into new_count from auditorium;
        dbms_output.put_line('Строк после удаления: '||new_count);
         delete from auditorium where auditorium.auditorium_type = 'ЛК';
         select count(*) into new_count from auditorium;
        dbms_output.put_line('Строк до rollback: '||new_count);
        rollback;
        select count(*) into new_count from auditorium;
        dbms_output.put_line('Строк rollback: '||new_count);
     exception
        when others then 
            dbms_output.put_line('error = '|| sqlerrm);
            rollback;
    end;
/

--10: DELETE operator corrupting data

select * from auditorium_type;
declare
    begin
    
    delete from auditorium_type where auditorium_type.auditorium_type like '%ЛК%';
    
     exception
        when others then 
            dbms_output.put_line('error = '|| sqlerrm);
            rollback;
    end;
/

--11: print TEACHER table using explicit LOOP coursor

declare 
    sTeacher teacher.teacher%type;
    sTeacher_name teacher.teacher_name%type;
    sPulpit teacher.pulpit%type;
    cursor cTeacher is select teacher, teacher_name, pulpit from teacher;
    begin
    dbms_output.put_line('TASK 11');
    open cTeacher;
        loop 
        fetch cTeacher into sTeacher, sTeacher_name, sPulpit;
        exit when cTeacher%notfound;
        dbms_output.put_line(sTEacher||' '||sTeacher_name||' '||sPulpit);
        end loop;
        
        close cTeacher;
    exception
        when others then 
            dbms_output.put_line('error = '|| sqlerrm);
            rollback;
    end;
    /

--12: print SUBJECT table using explicit WHILE coursor

select * from subject;
    
declare 
    rSubject subject%rowtype; 
    cursor cSubject is
     select subject, subject_name, pulpit from subject;
    begin
        dbms_output.put_line('TASK 12');
        open cSubject;
        fetch cSubject into rSubject;
        while (cSubject%found) loop
            dbms_output.put_line(rSubject.subject||' '||rSubject.subject_name||' ' ||rSubject.pulpit);
            fetch cSubject into rSubject;
        
        end loop;
        close cSubject;
    exception
        when others then 
            dbms_output.put_line('error = '|| sqlerrm);
            rollback;
    end;
    /


--13: print PULPIT JOIN TEACHER using explicit coursor and FOR loop

select * from pulpit;

declare 
    type myRecType is record
    (
        teacher_name teacher.teacher_name%type,
        pulpit_name pulpit.pulpit_name%type
    );
    myRec myRecType;
    cursor cTeacher is 
    select teacher_name, pulpit_name from teacher inner join pulpit on teacher.pulpit = pulpit.pulpit;
    begin
        dbms_output.put_line('TASK 13');
        for rec in cTeacher
        loop
            
        myRec.teacher_name := rec.teacher_name;
        myRec.pulpit_name := rec.pulpit_name;
        dbms_output.put_line(myRec.teacher_name||' '||myRec.pulpit_name);
        end loop;
    
     exception
        when others then 
            dbms_output.put_line('error = '|| sqlerrm);
            rollback;
    end;
    /
    
    
--14: print all AUDITORIUMS with specific capacity using coursor with parameters and 3 ways to organize a loop 

declare

    cursor cAud(lower_limit number, upper_limit number) is
        select auditorium, auditorium_name, auditorium_capacity, auditorium_type
        from auditorium
        where auditorium_capacity >= lower_limit 
          and (upper_limit is null or auditorium_capacity <= upper_limit)
        order by auditorium_capacity;
    vAud auditorium%rowtype;

begin
    dbms_output.put_line('TASK 14');
    dbms_output.put_line('СПИСОК АУДИТОРИЙ (вместимость < 20)');
    dbms_output.put_line('-------------------------------------');

    open cAud(0, 19);
    loop
        fetch cAud into vAud;
        exit when cAud%notfound;
        dbms_output.put_line(vAud.auditorium||' '||vAud.auditorium_name||
                             ' '||vAud.auditorium_capacity||' '||vAud.auditorium_type);
    end loop;
    close cAud;


    dbms_output.put_line('СПИСОК АУДИТОРИЙ (вместимость 21–30)');
    dbms_output.put_line('-------------------------------------');


    open cAud(21, 30);
    fetch cAud into vAud;
    while cAud%found loop
        dbms_output.put_line(vAud.auditorium||' '||vAud.auditorium_name||
                             ' '||vAud.auditorium_capacity||' '||vAud.auditorium_type);
        fetch cAud into vAud;
    end loop;
    close cAud;

    dbms_output.put_line('СПИСОК АУДИТОРИЙ (вместимость 31–60)');
    dbms_output.put_line('-------------------------------------');

    for rec in cAud(31, 60)
    loop
        dbms_output.put_line(rec.auditorium||' '||rec.auditorium_name||
                             ' '||rec.auditorium_capacity||' '||rec.auditorium_type);
    end loop;

    dbms_output.put_line('СПИСОК АУДИТОРИЙ (вместимость 61–80)');
    dbms_output.put_line('-------------------------------------');

    for rec in cAud(61, 80)
    loop
        dbms_output.put_line(rec.auditorium||' '||rec.auditorium_name||
                             ' '||rec.auditorium_capacity||' '||rec.auditorium_type);
    end loop;

    dbms_output.put_line('СПИСОК АУДИТОРИЙ (вместимость >= 81)');
    dbms_output.put_line('-------------------------------------');

    for rec in cAud(81, null)
    loop
        dbms_output.put_line(rec.auditorium||' '||rec.auditorium_name||
                             ' '||rec.auditorium_capacity||' '||rec.auditorium_type);
    end loop;


exception
    when others then 
        dbms_output.put_line('error = '|| sqlerrm);
        rollback;
end;
/

--15: delcaring a cursor variable using refcursor

declare
    cAud SYS_REFCURSOR;
    vAud auditorium%rowtype;
   
    lowerLimit number := 90;
    upperLimit number := 150;
   
    begin
        dbms_output.put_line('TASK 15');
        open cAud for
            select auditorium, auditorium_name, auditorium_capacity, auditorium_type
            from auditorium
            where auditorium_capacity >= lowerLimit and auditorium_capacity <= upperLimit;
    
    
    loop
        fetch cAud into vAud;
        exit when cAud%notfound;

        dbms_output.put_line(
            vAud.auditorium||' '||vAud.auditorium_name||
            ' '||vAud.auditorium_capacity||
            ' '||vAud.auditorium_type
        );
    end loop;

    close cAud;
    
    exception
        when others then
         dbms_output.put_line('error = '|| sqlerrm);
        rollback;
    end;
    /

--16: demonstrate a cursor subquery

declare
    cursor cPulpit is
        select pulpit, pulpit_name,
            CURSOR(
                select teacher_name 
                from teacher 
                where teacher.pulpit = p.pulpit
            ) as teachers
        from pulpit p;

    vPulpit pulpit.pulpit%type;
    vPulpitName pulpit.pulpit_name%type;

    cTeach  SYS_REFCURSOR;
    vTeacherName teacher.teacher_name%type;

begin
    dbms_output.put_line('TASK 16');
    open cPulpit;
    loop
        fetch cPulpit into vPulpit, vPulpitName, cTeach;
        exit when cPulpit%notfound;

        dbms_output.put_line('Кафедра: '||vPulpit||' '||vPulpitName);
        dbms_output.put_line('  Преподаватели:');

        loop
            fetch cTeach into vTeacherName;
            exit when cTeach%notfound;

            dbms_output.put_line('     '||vTeacherName);
        end loop;

        close cTeach;
    end loop;

    close cPulpit;

exception
    when others then
        dbms_output.put_line('error = '||sqlerrm);
        rollback;
end;
/

--17: Reduce capacities of all auditoriums using explicit cursors

declare
    vlowerLimit number := 40;
    vUpperLimit number := 80;

    cursor cAud is 
        select auditorium, auditorium_name, auditorium_capacity, auditorium_type
            from auditorium
            where auditorium_capacity >= vlowerLimit and auditorium_capacity <= vupperLimit
            for update; 
    
    vAud auditorium%rowtype;
    begin
    dbms_output.put_line('TASK 17');
    open cAud;
        fetch cAud into vAud;
    
    while cAud%found
    loop
        
        vAud.auditorium_capacity := vAud.auditorium_capacity * 0.9;
        
        update auditorium 
        set auditorium_capacity = vAud.auditorium_capacity
        where current of cAud;
        
        dbms_output.put_line(' '|| vAud.auditorium||' '||vAud.auditorium_capacity);
    
     fetch cAud into vAud;
    end loop;
    
    close cAud;
    
   exception
    when others then
        dbms_output.put_line('error = '||sqlerrm);
        rollback; 
    end;
/


--18: delete all auditoriums with capacity less then 20 using an explicit cursor

select count(*) from auditorium;

declare
    vUpperLimit number := 20;

     cursor cAud is 
        select auditorium, auditorium_name, auditorium_capacity, auditorium_type
            from auditorium
            where  auditorium_capacity <= vupperLimit
            for update; 
    
    vAud auditorium%rowtype;
            
    begin
    dbms_output.put_line('TASK 18');
    open cAud;
    
    fetch cAud into vAud;
    
    while cAud%found
    loop
    
        delete from auditorium
        where current of cAud;
        
        fetch cAud into vAud;        
    
    end loop;
    
    close cAud;


  exception
    when others then
        dbms_output.put_line('error = '||sqlerrm);
        rollback; 
    end;
/


--19: demonstrate pseudo column ROWID usage

declare
    cursor cAud is
        select auditorium, auditorium_capacity, rowid
            from auditorium;
    cpy auditorium.auditorium_capacity%type;
    begin
    dbms_output.put_line('TASK 19');
    for i in cAud
    loop
    cpy := i.auditorium_capacity;
    if (cpy <=20) then
        delete from auditorium
        where rowid = i.rowid;
        dbms_output.put_line('Удалена аудитория '||i.auditorium);
    else
        cpy := cpy*1.11;
        update auditorium 
        set auditorium_capacity = cpy
        where rowid = i.rowid;
    dbms_output.put_line(i.auditorium||' '||i.auditorium_capacity);
    end if;
    
    end loop;
    
    exception
    when others then
        dbms_output.put_line('error = '||sqlerrm);
        rollback; 
    end;
/

--20: print all TEACHERS in one looop and devide groups with ---- line

declare
  cursor cTeach is
    select teacher, teacher_name, pulpit
    from teacher;
  v_teacher     teacher.teacher%TYPE;         -- or appropriate column types
  v_teacher_name teacher.teacher_name%TYPE;
  v_pulpit      teacher.pulpit%TYPE;
begin
  dbms_output.put_line('TASK 20');
  open cTeach;
  loop
    fetch cTeach into v_teacher, v_teacher_name, v_pulpit;
    exit when cTeach%notfound;
    dbms_output.put_line(v_teacher||' '||v_teacher_name||' '||v_pulpit);
    if mod(cTeach%rowcount,3) = 0 then
      dbms_output.put_line('--------------------------');
    end if;
  end loop;
  close cTeach;
exception
  when others then
    dbms_output.put_line('error = '||sqlerrm);
    rollback;
end;
/
    











