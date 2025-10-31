if "%1" == "" if "%2" == "" (echo "Help: set first param to work mode (create, delete), second to a file name." 
goto :eof) 

if "%1" == "" (echo "Error: work mode param is missing" 
goto :eof)

if not "%1" == "create" if not "%1" == "delete" (echo "Help: wrong work mode input(try create/delete)" 
goto :eof)

if "%2" == "" (echo "Error: file name is missing"
goto :eof)
if "%1" == "create" if exist "%2" (echo "Error: file already exists"
goto :eof) else (echo. > "%2"
goto :eof)
if "%1" == "delete" if not exist "%2" (echo "Error: no file was found"
goto :eof) else (del "%2")

