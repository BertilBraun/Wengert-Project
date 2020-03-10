<?php

$hour = intval(date('H'));

if ($hour < 7 || $hour > 21)
	echo '0';
else
	echo '1';
?>
