function Read-Number([string]$prompt) {

    while ($true) {

        $input = Read-Host $prompt

        if ($input -match '^\s*-?\d+([.,]\d+)?\s*$') {

            $norm = $input -replace ',', '.'

            return [double]$norm

        }
        else {

            Write-Host "Incorrect number. Try again" -ForegroundColor Yellow

        }

    }

}
function Read-Int([string]$prompt) {
    while ($true) {
        $user_input = Read-Host $prompt
        if ($user_input -match '^\s*-?\d+\s*$') {
            return [int]$user_input
        }
        else {
            Write-Host "Incorrect number. Try again" -ForegroundColor Yellow
        }
    }
}

function To-Radians([double]$value, [string]$unit) {
    if ($unit -eq 'd') {
        return $value * [System.Math]::PI / 180.0
    }
    return $value
}

function From-Radians([double]$value, [string]$unit) {
    if ($unit -eq 'd') {
        return $value * 180.0 / [System.Math]::PI
    }
    return $value
}

function Show-Menu {
    Clear-Host
    Write-Host "---Interactive Calculator---"
    Write-Host "Select an operation: "
    Write-Host "1) Addition (a + b)"
    Write-Host "2) Substraction (a - b)"
    Write-Host "3) Multiplication (a * b)"
    Write-Host "4) Division (a / b)"
    Write-Host "5) Exponentiation (a ^ b)"
    Write-Host "6) N-power root extraction (n√a)"
    Write-Host "7) Remainder from division (a mod b)"
    Write-Host "8) Trigonometric functions (sin, cos, tan, ctg)"
    Write-Host "9) Logorythms (ln, log10)"
    Write-Host "0) Exit"
}

function Do-Trigonometry {
    $unit = ''
    while ($unit -notin @('r', 'd')) {
        $unit = Read-Host "Enter values for trygonometric functions: (r) radiand (d) degrees"
        $unit = $unit.ToLower()
    }

    $ops = @('sin', 'cos', 'tan', 'ctg')
    $op = ''
    while ($op -notin $ops) {
        $op = Read-Host "Function (${($ops -join ', ')})"
        $op.ToLower()
    }


    $x = Read-Number "Enter the argument"
    $rad = To-Radians $x $unit
    switch ($op) {
        'sin' { $res = [System.Math]::Sin($rad) }
        'cos' { $res = [System.Math]::Cos($rad) }
        'tan' { $res = [System.Math]::Tan($rad) }
        'ctg' { $res = 1 / [System.Math]::Tan($rad) }
    }
    Write-Host "Result: $res"
}


function Do-Logarythm {
    $ops = @('ln', 'log10')
    $op = ''

    while ($op -notin $ops) {
        $op = Read-Host "Choose the logorythmic operation (ln,log10)"
        $op.ToLower() 
    }

    $x = Read-Number "Enter the POSITIVE number"
    if ($x -le 0) {
        Write-Host "Logorythm is defined for positive number only." -ForegroundColor Yellow
        return
    }

    switch ($op) {
        'ln' { $res = [System.Math]::Log($x) }
        'log10' { $res = [System.Math]::Log10($x) }
    }

    Write-Host "Result: $x"
}

$exited = $false

while ($true) {

    if($exited){
        break
    }

    Show-Menu
    $choise = Read-Host "Select an operation"


    switch ($choise) {
        '1' {
            $a = Read-Number "a="
            $b = Read-Number "b="
            Write-Host "Addition result: $([Math]::Round(($a+$b),12))"
        }
        '2' {
            $a = Read-Number "a="
            $b = Read-Number "b="
            Write-Host "Substraction result: $($a-$b)"
        }
        '3' {
            $a = Read-Number "a="
            $b = Read-Number "b="
            Write-Host "Multiplication result: $($a*$b)"
        }
        '4' {
            $a = Read-Number "a="
            $b = Read-Number "b="
            if ($b -eq 0) {
                Write-Host "Unable to devide by zero" -ForegroundColor Red
            }
            else {
                Write-Host "Division result: $($a/$b)"
            }
        }
        '5' {
            $a = Read-Number "Basis="
            $b = Read-Number "Power="
            try {
                $res = [System.Math]::Pow($a, $b)
                Write-Host "Exponentiation result: $res"
            }
            catch {
                Write-Host "Error $_" -ForegroundColor Red
            }   
        }
        '6' {
            $N = Read-Number "Root power="
            if ($N -eq 0) {
                Write-Host "Root power must be more then 0" -ForegroundColor Yellow
            }
            else {
                $a = Read-Host "a="
                if ($a -lt 0 -and ($N % 2 -eq 0)) {
                    Write-Host "Unable to extract even root from a negative number" -ForegroundColor Yellow
                }
                else {
                    try {
                        $res = [System.Math]::Pow($a, 1.0 / $n)
                        Write-Host "N-power root extraction result: $res"
                    }
                    catch {
                        Write-Host "Error: $_" -ForegroundColor Red
                    }
                }
            }
        }
        '7' {
            $a = Read-Number "a="
            $b = Read-Number "b="
            if ($b -eq 0) {
                Write-Host "Error: Unable to devide by zero" -ForegroundColor Red
            }
            else {
                $quot = [System.Math]::Floor($a / $b)
                $res = $a - ($quot * $b)
                Write-Host "Remainder from division result: $res"
            }
        }
        '8' {
            Do-Trigonometry
        }
        '9' {
            Do-Logarythm
        }
        '0' {
            Write-Host "Exiting the application..."-ForegroundColor Green
            $exited = $true
            break
        }
        default {
            Write-Host "Invalid choise. Try again" -ForegroundColor Yellow
        }
        
    }
    
    Write-Host ""
    Read-Host "Press any key to continue..."
}