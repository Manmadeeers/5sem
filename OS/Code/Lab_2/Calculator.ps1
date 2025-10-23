function Read-Number([string]$prompt) {

    while ($true) {

        $user_input = Read-Host $prompt

        if ($user_input -match '^\s*-?\d+([.,]\d+)?\s*$') {

            $norm = $input -replace ',', '.'#replace , with . and cast to double
            return [double]$norm

        } else {

            Write-Host "Incorrect number. Try again" -ForegroundColor Yellow

        }

    }

}
function Read-Int([string]$prompt){
    while($true){
        $user_input = Read-Host $prompt
        if($user_input -match '^\s*-?\d+\s*$'){
            return [int]$user_input
        }
        else{
            Write-Host "Incorrect number. Try again" -ForegroundColor Yellow
        }
    }
}

function To-Radians([double]$value, [string]$unit){
    if($unit -eq 'd'){
        return $value*[System.Math]::PI/180.0
    }
    return $value
}

function From-Radians([double]$value,[string]$unit){
    if($unit -eq 'd'){
        return $value*180.0/[System.Math]::PI
    }
    return $value
}

function Show-Menu{
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
    Write-Host "8) Trigonometric functions (sin cos tan ctg acos atan)"
    Write-Host "9) Logorythms (ln, log10)"
    Write-Host "0) Exit"
}