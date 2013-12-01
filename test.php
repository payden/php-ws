<?php
$ws = new WebSocketServer();
$ws->bind("0.0.0.0", "8080");
$ws->onopen = function($client) {
  echo "New client connection: " . $client->sockfd . "\n";
};
$ws->onclose = function($client) {
  echo "Client closed connection: " . $client->sockfd . "\n";
};
$ws->onmessage = function($client, $msg) {
  echo "Received message from client (" . $client->sockfd . "): " . $msg->payload . "\n";
  $client->sendText($msg->payload);
};
$ws->run();
?>
