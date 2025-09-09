create table FIA_t(
x number(3) primary key,
s varchar(50)
);

insert into FIA_t(x,s) values
(1,'first'),
(2,'second'),
(3,'third')

commit;

update FIA_t
set s = 'first_updated'
where x=1;

update FIA_t
set s='second_updated'
where x=2;

commit;

select * from FIA_t
select * from FIA_t where x=3

delete from FIA_t where x=2;
commit;

select * from FIA_t

create table FIA_t1(
x1 number(3) primary key,
x_fia_t number(3),
s1 varchar(50),
foreign key (x_fia_t) references FIA_t(x)
);

select * from FIA_t1

insert into FIA_t1 (x1,x_fia_t,s1) values
(3,3,'duplicate_3')


select FIA_t.x as Original_X, FIA_t.s as Original_S, FIA_t1.x1 as Duplicate_X, FIA_t1.s1 as Duplicate_S from FIA_t inner join FIA_t1 on x=x1

select FIA_t.x as Original_X, FIA_t.s as Original_S, FIA_t1.x1 as Duplicate_X, FIA_t1.s1 as Duplicate_S from FIA_t left join FIA_t1 on x=x1

select FIA_t.x as Original_X, FIA_t.s as Original_S, FIA_t1.x1 as Duplicate_X, FIA_t1.s1 as Duplicate_S from FIA_t right join FIA_t1 on x=x1

drop table FIA_t;
drop table FIA_t1;

