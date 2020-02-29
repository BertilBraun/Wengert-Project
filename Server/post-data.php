<?php

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}

$api_key_value = "tPmAT5Ab3j7F9";

if (test_input($_POST["api_key"]) <> $api_key_value)
	die("Wrong API Key provided.");
	
$humidity 			= test_input($_POST["humidity"]);
$ground_humidity	= test_input($_POST["ground_humidity"]);
$temperature 		= test_input($_POST["temperature"]);
$pressure 			= test_input($_POST["pressure"]);
$timeToWater 		= test_input($_POST["timeToWater"]);


$servername = "rdbms.strato.de";

$dbname 	= "DB3920559";
$username 	= "U3920559";
$password 	= "(L3mb3rg3rLand!)";


$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error)
	die("Connection failed: " . $conn->connect_error);

$sql = "INSERT INTO SensorData (humidity, ground_humidity, temperature, pressure, timeToWater)
VALUES ('" . $humidity . "', '" . $ground_humidity . "', '" . $temperature . "', '" . $pressure . "', '" . $timeToWater . "');";

if ($conn->query($sql) === TRUE)
	echo "New record created successfully";
else 
	echo "Error: " . $sql . "<br>" . $conn->error;

$conn->close();

?>