<?php

$servername = "rdbms.strato.de";

$dbname = "DB3920559";
$username = "U3920559";
$password = "(L3mb3rg3rLand!)";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) 
	die("Connection failed: " . $conn->connect_error);

$sql = "SELECT readingTime FROM SensorData WHERE id = (SELECT MAX(id) FROM SensorData);";
$path = 'Images/'.$conn->query($sql)->fetch_object()->readingTime.'.jpg';


if(move_uploaded_file($_FILES['data']['tmp_name'], $path))
    echo "The file ". $path . " has been uploaded";
else
    echo "There was an error uploading the file, please try again!";

?>