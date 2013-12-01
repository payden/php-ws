<?php
$ws = new WebSocket();
$ws->bind("0.0.0.0", "8080");
$ws->run();
var_dump($ws);
?>
