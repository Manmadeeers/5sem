function Parse-TempInput {
    param([string]$temp_input)

    if (-not $temp_input) {
        throw "Empty input"
    }

    $s = $temp_input.Trim().Replace(" ", "")

    if($s -eq "exit"){
        return -1
    }

    if ($s -match '^(?<num>-?\d+\.?\d*)(?<unit>[cCfFkK])$') {

        $num = $matches['num']

        $unit = $matches['unit'].ToUpper()

    }
    elseif ($s -match '^(?<num>-?\d+\.?\d*)$') {

        $num = $matches['num']

        $unit = 'C' 

    }
    else {

        throw "Invalid format. Use: 100K, 36.6C, 212F or just 25 for C."

    }

    return @{ Value = [double]$num; Unit = $unit }
}

function Convert-Temperature {
    param([double]$value, [string]$unit)
    switch ($unit) {

        'C' {

            $kelvin = $value + 273.15

            $fahrenheit = ($value * 9 / 5) + 32

            return @{ Celsius = $value; Kelvin = $kelvin; Fahrenheit = $fahrenheit }

        }

        'K' {

            $celsius = $value - 273.15

            $fahrenheit = ($celsius * 9 / 5) + 32

            return @{ Celsius = $celsius; Kelvin = $value; Fahrenheit = $fahrenheit }

        }

        'F' {

            $celsius = ($value - 32) * 5 / 9

            $kelvin = $celsius + 273.15

            return @{ Celsius = $celsius; Kelvin = $kelvin; Fahrenheit = $value }

        }

    }
}

while($true){
    try {

    $user_input = Read-Host "Enter temperature value (for example: 100K, 36.6C, 212F or just 25 for C)"

    $parsed = Parse-TempInput $user_input

    if($parsed -eq -1){
        break
    }

    $result = Convert-Temperature -value $parsed.Value -unit $parsed.Unit
    

    Write-Host "Results:"

    Write-Host ("Celsius: {0:N2} C" -f $result.Celsius)

    Write-Host ("Kelvin: {0:N2} K" -f $result.Kelvin)

    Write-Host ("Fahrenheit: {0:N2} F" -f $result.Fahrenheit)

}

catch {

    Write-Host "Error: $_" -ForegroundColor Red

}
}
