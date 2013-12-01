<?php
$ws = new WebSocket();
$ws->bind("0.0.0.0", "8080");
$ws->onopen = function($client) {
  echo "New client connection: " . $client->sockfd . "\n";
  echo "Sending back 'word'\n";
  $client->sendText("word");
};
$ws->onclose = function($client) {
  echo "Client close: " . $client->sockfd . "\n";
};
$ws->onmessage = function($client, $msg) {
  echo "Received message from client (" . $client->sockfd . "): " . $msg->payload . "\n";
  //$client->sendText($msg->payload);
};
$ws->run();
var_dump($ws);
?>
