set /a a=%1
set /a b=%2
set /a c=%3

set /a res1=a-b
echo %a% - %b% = %res1%

set /a res2=a+b
echo %a% + %b% = %res2%

if %b% equ 0 (
   echo "Error: attempt to divide by zero!"
) else (
   set /a res3=c/b
   echo %c% / %b% = %res3%
)

set /a res4=c*b
   echo %c% * %b% = %res4%

set /a res5=(a-b)*(b-a)
echo (%a% - %b%) * (%b% - %a%) = %res5%

