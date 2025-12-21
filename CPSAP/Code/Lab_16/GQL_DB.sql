drop table FACULTY;
drop table PULPIT;
drop table TEACHER;
drop table SUBJECT;




create table FACULTY
(
    FACULTY      nvarchar(10) primary key, 
    FACULTY_NAME nvarchar(100)             
);
create table PULPIT
(
    PULPIT      nvarchar(10) primary key,
    PULPIT_NAME nvarchar(100),
    FACULTY     nvarchar(10) foreign key references FACULTY (FACULTY)
);
create table TEACHER
(
    TEACHER      nvarchar(10) primary key,
    TEACHER_NAME nvarchar(100),
    PULPIT       nvarchar(10) foreign key references PULPIT (PULPIT)
);
create table SUBJECT
(
    SUBJECT      nvarchar(10) primary key,
    SUBJECT_NAME nvarchar(100),
    PULPIT       nvarchar(10) foreign key references PULPIT (PULPIT)
);
INSERT INTO FACULTY (FACULTY, FACULTY_NAME) VALUES ('ИТ', 'Информационных технологий'), ('ИЭФ', 'Инженерно-Экономический');
INSERT INTO PULPIT (PULPIT, PULPIT_NAME, FACULTY) VALUES ('ИСиТ', 'Информационных Систем и Технологий', 'ИТ'), ('ПИ', 'Программной Инженерии', 'ИТ'), ('МиК', 'Маркетинга и Коммуникаций', 'ИЭФ');
INSERT INTO TEACHER (TEACHER, TEACHER_NAME, PULPIT) VALUES ('СМВ', 'Смелов В.В.', 'ПИ'), ('НАС', 'Наркевич А.С.', 'ПИ');
INSERT INTO SUBJECT (SUBJECT, SUBJECT_NAME, PULPIT) VALUES ('ПСКП', 'Программирование Серверных Кросплатформенных Приложений', 'ПИ'), ('АиСД', 'Алгоритмы и Структуры Данных', 'ПИ'), ('ПСП', 'Программирование Сетевых Приложений', 'ПИ');