<?php

$servername = "rdbms.strato.de";

$dbname 	= "dbs1773217";
$username 	= "dbu1299844";
$password 	= "(L3mb3rg3rLand!)";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) 
	die("Connection failed: " . $conn->connect_error);

$sql = "SELECT id FROM `Weather Data` WHERE id = (SELECT MAX(id) FROM `Weather Data`);";
$path = '../Images/'.$conn->query($sql)->fetch_object()->id.'.jpg';

file_put_contents($path, "");
file_put_contents("next_image_path.txt", $path);
file_put_contents("log.txt", "Image Upload\n", FILE_APPEND);
echo "File created";
	
$conn->close();

?>