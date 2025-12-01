ALTER PLUGGABLE DATABASE FIA_PDB OPEN;

--1: anonymous block with no operators
declare 
    begin
        null;
    end;

--2: anonymous block to print HelloWorld. Execute in SQL Dev and SQL Plus

declare 
    begin
        dbms_output.put_line('Hello World!');
    end;
    
--3: exceptions, sqlerrm, sqlcode

declare 
    x number(3) := 3;
    y number(3) := 0;
    z number(10,2);
begin
    dbms_output.put_line('x = ' ||x||
                        ', y = '||y||
                        ', z = '||z);
    z :=x/y;
    dbms_output.put_line('z = '||z); 
        
    exception
        when others
        then dbms_output.put_line('error: code='||sqlcode||' message: '||sqlerrm); 
end;

--4: exception handling in nested blocks

declare 
    x number(3) := 3;
    y number(3) := 0;
    z number(10,2);
begin
    dbms_output.put_line('x = ' ||x||
                        ', y = '||y||
                        ', z = '||z);
    begin
    z :=x/y;
    dbms_output.put_line('inner z = '||z); 
        
    exception
        when others
        then dbms_output.put_line('error: code='||sqlcode||' message: '||sqlerrm); 
    end;
    dbms_output.put_line('outer z = '||z); 
end;

--5: currently supported compiler warning types
select * from v$parameter where name = 'plsql_warnings';

--6: all PL/SQL special symbols
select keyword from v$reserved_words where length=1 and keyword !='A' order by keyword;

--7: all PL/SQL keywords

select keyword from v$reserved_words where length>1 and keyword!='A' order by keyword;

--8: all Oracle Server parameters connected to PL/SQL . Also view via sql plus

select * from v$parameter where name like '%plsql%';
show parameter plsql;

--9: 

--10: initialize 2 number variables

declare 
    x number := 4;
    y number := 10;
    begin
        dbms_output.put_line('Task 10');
        dbms_output.put_line(x);
        dbms_output.put_line(y);    
    end;

--11: arythmetic operations with 2 number variables including division with remaining digits

declare 
    x number := 4;
    y number := 10;
    begin
        dbms_output.put_line('Task 11');
        dbms_output.put_line(x+y);
        dbms_output.put_line(x-y);    
        dbms_output.put_line(x*y);
        dbms_output.put_line(x/y);
        dbms_output.put_line(x mod y);
    end;

--12: initialize number variables with a fixed point

declare 
    x number := 123.123;
    y number(10,1) := 123.123;
    begin
        dbms_output.put_line('Task 12');
        dbms_output.put_line(x);
        dbms_output.put_line(y);
    end;


--13: initialize number variables with a fixed point and negative round

declare 
    x number(10,-2) := 1234567.127;
    y number(10,-3) := 123.123;
    begin
        dbms_output.put_line('Task 13');
        dbms_output.put_line(x);
        dbms_output.put_line(y);
    end;


--14: initialize BINARY_FLOAT variable

declare 
    x binary_float := 12345.12345;
    begin
        dbms_output.put_line('Task 14');
        dbms_output.put_line(x);
    
    end;

--15: initialize BINARY_DOUBLE variable

declare 
    x binary_double := 12345.12345;
    begin
        dbms_output.put_line('Task 15');
        dbms_output.put_line(x);
    end;
    
--16: declare number variables and use E in initialization

declare
    x number(20, 10) := 12345.123456E+005;
    begin
        dbms_output.put_line('Task 16');
        dbms_output.put_line(x);
    end;


--17: initialize BOOLEAN variables

declare
  x boolean := true;
begin
    dbms_output.put_line('Task 17');
  if x then
   dbms_output.put_line('true');
   ELSE
   dbms_output.put_line('false');
    end if;
end;


--18: anonymous PL/SQL block for nested operations demonstration(exception will be unhandled 'cause it happens during the compilation phase)

declare
    x18 constant number := 10;
    begin
    dbms_output.put_line('Task 18');
    x18:= 25;
    exception
        when others
        then dbms_output.put_line('error: code='||sqlcode||' message: '||sqlerrm); 
    end;

--19: using %TYPE options in AB
declare
    x number (10, 3):= 123.123;
    y x%type := 123.123456;
    begin
        dbms_output.put_line('Task 19');
        dbms_output.put_line(x);
        dbms_output.put_line(y);
    end;

--20: using %ROWTYPE option in AB

create table demo(
    firstname varchar(20) primary key,
    secondname varchar(20)
);

declare 
    x DEMO%rowtype;
    begin
        x.firstname := 'IVAN';
        x.secondname := 'IVANOV';
        dbms_output.put_line('Task 20');
        dbms_output.put_line(x.firstname||' has '||x.secondname||' surename');
    end;


--21 and 22: all possible IF operator constructions

declare
    x number := 1;
    begin
    dbms_output.put_line('Task 21 and 22');
    if x = 1 then
        dbms_output.put_line(x);
    elsif x = 2 then 
        dbms_output.put_line(x);
    else
        dbms_output.put_line(x);
    end if;
    end;

--23: CASE operator work demonstration

declare
    x number := 1;
    begin
    dbms_output.put_line('Task 23');
    case x
        when 1 then
          dbms_output.put_line(x);
       when 2 then
            dbms_output.put_line(x);
        else
            dbms_output.put_line(x);
    end case;
    end;
    
--24: LOOP operator work demonstration

declare 
    x number := 1;
    begin
        dbms_output.put_line('Task 24');
        loop
            dbms_output.put_line(x);
            x:=x+1;
            exit when x > 10;
        end loop;
    end;


--25: FOR operator work demonstration

declare 
    x number := 1;
    begin
        dbms_output.put_line('Task 25');
        while x <=10 loop
            dbms_output.put_line(x);
            x:=x+1;
        end loop;
    end;
    


ALTER PLUGGABLE DATABASE FIA_PDB CLOSE IMMEDIATE;



