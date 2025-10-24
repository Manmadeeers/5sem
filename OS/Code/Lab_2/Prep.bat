@echo off
chcp 65001
rem Temp dir
md tasks_tmp
echo txt1 > .\tasks_tmp\txt1.txt
echo txt2 > .\tasks_tmp\txt2.txt
echo txt3 > .\tasks_tmp\txt3.txt
echo hidden1 > .\tasks_tmp\hidden1.txt
echo system1 > .\tasks_tmp\system1.txt
type NUL > .\tasks_tmp\bmp1.bmp
type NUL > .\tasks_tmp\bmp2.bmp
type NUL > .\tasks_tmp\exe1.exe
type NUL > .\tasks_tmp\exe2.exe
attrib +h .\tasks_tmp\hidden1.txt
attrib +s .\tasks_tmp\system1.txt
rem Task 1a
md task_01a
rem Task 1b
md task_01b
md task_01b\subdir
md task_01b\subdir\empty_subdir
type NUL > task_01b\subdir\txt1.txt
type NUL > task_01b\subdir\txt2.txt
md task_01b\hidden_dir
attrib +h .\task_01b\hidden_dir
md task_01b\hidden_dir\empty_subdir
md task_01b\hidden_dir\subdir
type NUL > task_01b\hidden_dir\subdir\txt1.txt
type NUL > task_01b\hidden_dir\subdir\txt2.txt
type NUL > task_01b\hidden_dir\txt1.txt
type NUL > task_01b\hidden_dir\system1.txt
attrib +s task_01b\hidden_dir\system1.txt
type NUL > task_01b\readonly.txt
attrib +r task_01b\readonly.txt
type NUL > task_01b\nonarchieve.txt
attrib -a task_01b\nonarchieve.txt
type NUL > task_01b\nonarchieve-readonly.txt
attrib -a +r task_01b\nonarchieve-readonly.txt
type NUL > task_01b\system-nonarchieve-readonly.txt
attrib -a +r +s task_01b\system-nonarchieve-readonly.txt
type NUL > task_01b\system-readonly.txt
attrib +r +s task_01b\system-readonly.txt
rem Task 1c
md task_01c_src
copy .\tasks_tmp\txt1.txt .\task_01c_src\txt1.txt
copy .\tasks_tmp\txt1.txt .\task_01c_src\txt2.txt
copy .\tasks_tmp\txt1.txt .\task_01c_src\txt3.txt
copy .\tasks_tmp\txt1.txt .\task_01c_src\txt4.txt
copy .\tasks_tmp\txt1.txt .\task_01c_src\txt5.txt
md task_01c_dst
rem Task 1d
md task_01d_1
md task_01d_2
copy .\tasks_tmp\txt1.txt .\task_01d_1\
copy .\tasks_tmp\txt2.txt .\task_01d_1\
copy .\tasks_tmp\txt3.txt .\task_01d_1\
copy .\tasks_tmp\txt1.txt .\task_01d_2\
copy .\tasks_tmp\txt2.txt .\task_01d_2\
copy .\tasks_tmp\txt3.txt .\task_01d_2\
rem Task 1e
md task_01e
copy .\tasks_tmp\txt1.txt .\task_01e\txt1.txt
copy .\tasks_tmp\txt1.txt .\task_01e\txt2.txt
copy .\tasks_tmp\txt1.txt .\task_01e\txt3.txt
copy .\tasks_tmp\txt1.txt .\task_01e\txt4.txt
copy .\tasks_tmp\txt1.txt .\task_01e\txt5.txt
rem Task 1f
md task_01f
echo "Start" > .\task_01f\txt1.txt
echo "Hello World" >> .\task_01f\txt1.txt
echo "End" >> .\task_01f\txt1.txt
echo "Start" > .\task_01f\txt2.txt
echo "Hello, World!" >> .\task_01f\txt2.txt
echo "End" >> .\task_01f\txt2.txt
rem Task 1g
md task_01g
md task_01g\subdir
md task_01g\subdir\empty_subdir
type NUL > task_01g\subdir\txt1.txt
type NUL > task_01g\subdir\txt2.txt
md task_01g\hidden_dir
attrib +h .\task_01g\hidden_dir
md task_01g\hidden_dir\empty_subdir
md task_01g\hidden_dir\subdir
type NUL > task_01g\hidden_dir\subdir\txt1.txt
type NUL > task_01g\hidden_dir\subdir\txt2.txt
type NUL > task_01g\hidden_dir\txt1.txt
type NUL > task_01g\hidden_dir\system1.txt
attrib +s task_01g\hidden_dir\system1.txt
type NUL > task_01g\readonly.txt
attrib +r task_01g\readonly.txt
type NUL > task_01g\nonarchieve.txt
attrib -a task_01g\nonarchieve.txt
type NUL > task_01g\nonarchieve-readonly.txt
attrib -a +r task_01g\nonarchieve-readonly.txt
type NUL > task_01g\system-nonarchieve-readonly.txt
attrib -a +r +s task_01g\system-nonarchieve-readonly.txt
type NUL > task_01g\system-readonly.txt
attrib +r +s task_01g\system-readonly.txt
type NUL > task_01g\system.txt
attrib +s task_01g\system.txt
type NUL > task_01g\system-nonarchieve.txt
attrib -a +s task_01g\system-nonarchieve.txt
rem Task 1h
md task_01h
rem Task 1i
md task_01i
md task_01i\Русский
md task_01i\English
rem Task 1j
md task_01j
rem Task 1k
md task_01k
md task_01k\subdir
md task_01k\empty_subdir
rem Task 2a