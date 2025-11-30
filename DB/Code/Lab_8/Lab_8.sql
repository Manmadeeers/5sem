ALTER PLUGGABLE DATABASE FIA_PDB OPEN;

--anonymous block with no operators
declare 
    begin
        null;
    end;

--anonymous block to print HelloWorld. Execute in SQL Dev and SQL Plus

declare 
    begin
        dbms_output.put_line('Hello World!');
    end;
    
-- exceptions, sqlerrm, sqlcode

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

--exception handling in nested blocks

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

--
















