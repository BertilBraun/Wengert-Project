<?php

$path = file_get_contents("next_image_path.txt");
file_put_contents($path, $_REQUEST["data"], FILE_APPEND);
echo "Data appended  " . strlen($_REQUEST["data"]) . "  " . $path;

?>