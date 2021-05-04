<?php

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
function log_to_file($msg, $file) {
    $result = $msg . "\n";
    if (!file_exists($file)) {
        file_put_contents($file, "");
    }
    file_put_contents($file, $result . file_get_contents($file));
}
function err($msg) {
    log_to_file($msg . " " . implode(error_get_last()), "error.txt");
    die($msg);
}

$size = (int) $_SERVER['CONTENT_LENGTH'];
// log_to_file(print_r($_POST, true), "data.txt");
log_to_file("New Entry: " . $_POST["dateutc"] . " size: " . $size, "log.txt");

$pass_key_value = "58B6E6B64C7A088FA18CAB6A84668F9E";

if (test_input($_POST["PASSKEY"]) <> $pass_key_value)
    err("Wrong API Key provided.");
	
$servername = "rdbms.strato.de";

$dbname 	= "dbs1773217";
$username 	= "dbu1299844";
$password 	= "(L3mb3rg3rLand!)";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error)
    err("Connection failed: " . $conn->connect_error);

$sql = "INSERT INTO `Weather Data`
    (baromrelin,
    baromabsin,
    tempf,
    humidity,
    winddir,
    windspeedmph,
    windgustmph,
    rainratein,
    eventrainin,
    dailyrainin,
    weeklyrainin,
    monthlyrainin,
    yearlyrainin,
    totalrainin,
    solarradiation,
    uv)
    VALUES 
    (" . test_input($_POST["baromrelin"]) . 
    ", " . test_input($_POST["baromabsin"]) . 
    ", " . test_input($_POST["tempf"]) . 
    ", " . test_input($_POST["humidity"]) . 
    ", " . test_input($_POST["winddir"]) . 
    ", " . test_input($_POST["windspeedmph"]) . 
    ", " . test_input($_POST["windgustmph"]) . 
    ", " . test_input($_POST["rainratein"]) . 
    ", " . test_input($_POST["eventrainin"]) . 
    ", " . test_input($_POST["dailyrainin"]) . 
    ", " . test_input($_POST["weeklyrainin"]) . 
    ", " . test_input($_POST["monthlyrainin"]) . 
    ", " . test_input($_POST["yearlyrainin"]) . 
    ", " . test_input($_POST["totalrainin"]) . 
    ", " . test_input($_POST["solarradiation"]) . 
    ", " . test_input($_POST["uv"]) .  ");";

if ($conn->query($sql) === TRUE) {
    log_to_file("New record created successfully", "log.txt");
	echo "New record created successfully";
} else {
    log_to_file("Error: " . $sql . "<br>" . $conn->error, "error.txt");
	echo "Error: " . $sql . "<br>" . $conn->error;
}

$conn->close();

?>
